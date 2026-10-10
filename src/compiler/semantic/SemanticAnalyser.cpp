#include "SemanticAnalyser.h"

#include <queue>

#include "../../include/Error.h"

compiler::SemanticAnalyser::SemanticAnalyser(SymbolTable* symbolTable, TypeRegistry* typeRegistry, const std::filesystem::path* path) :
    symbolTable(symbolTable), typeRegistry(typeRegistry), path(path) {}

void compiler::SemanticAnalyser::processProgram(const ast::Program& program) {
    Scope* globalScope = this->symbolTable->enterScope(ScopeKind::GLOBAL); // generate global scope

    // process function declarations
    for (auto& functionDecl : program.functionDecls) {
        this->processFunctionDecl(*functionDecl);
    }

    // process statements
    for (auto& stm : program.statements) {
        this->processStm(globalScope, *stm);
    }

    // process function bodies
    for (auto& functionDecl : program.functionDecls) {
        const auto semanticAnalysisResult = this->processFunctionBody(*functionDecl, functionDecl->functionSymbol);

        // check function body always reaches returnStm if return type is non-void
        if (functionDecl->returnTypeInfo->type.typeId != VOID_TYPE_ID &&
            !semanticAnalysisResult.alwaysReturns
        ) {
            // get FunctionSymbol
            throw SemanticError(
                *this->path,
                functionDecl->line,
                functionDecl->column,
                "function '" +
                    functionSignatureToString(functionDecl->identifier->name, functionDecl->getParameterTypes()) +
                    "' may not return a value on all paths"
            );
        }
    }
}

void compiler::SemanticAnalyser::processFunctionDecl(const ast::FunctionDecl& functionDecl) {
    // declare function in symbol table
    const auto parameterTypes = functionDecl.getParameterTypes();
    const auto functionSignature = functionSignatureToString(functionDecl.identifier->name, parameterTypes);
    FunctionSymbol* functionSymbol = this->symbolTable->declareFunction(functionDecl.identifier->name, functionSignature, functionDecl.returnTypeInfo->type, parameterTypes);

    if (functionSymbol == nullptr) {
        throw SemanticError(
            *this->path,
            functionDecl.line,
            functionDecl.column,
            "function '" +
                functionSignature +
                "' is already defined"
        );
    }

    // set symbol of functionDecl
    functionDecl.functionSymbol = functionSymbol;
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processStm(Scope* scope, const ast::Stm& stm) {
    if (auto* varDecl = dynamic_cast<const ast::StmVarDecl*>(&stm)) {
        return this->processStmVarDecl(scope, *varDecl, false);
    }
    if (auto* assignment = dynamic_cast<const ast::StmAssignment*>(&stm)) {
        return this->processAssignment(scope, *assignment);
    }
    if (auto* block = dynamic_cast<const ast::Block*>(&stm)) {
        return this->processBlock(*block, ScopeKind::BLOCK);
    }
    if (auto* ifStm = dynamic_cast<const ast::IfStm*>(&stm)) {
        return this->processIfStatement(scope, *ifStm);
    }
    if (auto* whileStm = dynamic_cast<const ast::WhileStm*>(&stm)) {
        return this->processWhileStatement(scope, *whileStm);
    }
    if (auto* forStm = dynamic_cast<const ast::ForStm*>(&stm)) {
        return this->processForStatement(scope, *forStm);
    }
    if (auto* continueStm = dynamic_cast<const ast::ContinueStm*>(&stm)) {
        return this->processContinueStatement(scope, *continueStm);
    }
    if (auto* breakStm = dynamic_cast<const ast::BreakStm*>(&stm)) {
        return this->processBreakStatement(scope, *breakStm);
    }
    if (auto* returnStm = dynamic_cast<const ast::ReturnStm*>(&stm)) {
        return this->processReturnStatement(scope, *returnStm);
    }
    if (auto* expressionStatement = dynamic_cast<const ast::ExpressionStatement*>(&stm)) {
        return this->processExpressionStatement(scope, *expressionStatement);
    }
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processBlock(const ast::Block& block, const ScopeKind scopeKind) {
    Scope* newScope = this->symbolTable->enterScope(scopeKind);
    return this->processBlockWithScope(newScope, block);
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processBlockWithScope(Scope* scope, const ast::Block& block) {
    block.scope = scope; // set scope of block

    bool alwaysReturns = false;

    // process each statement inside the block
    for (auto& stm : block.statements) {
        const auto semanticAnalysisResult = this->processStm(block.scope, *stm);
        if (alwaysReturns == false && semanticAnalysisResult.alwaysReturns) {
            alwaysReturns = true;
        }
    }
    this->symbolTable->leaveScope(); // leave scope after block is processed

    return SemanticAnalysisResult{alwaysReturns};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processFunctionBody(const ast::FunctionDecl& functionDecl, FunctionSymbol* functionSymbol) {
    Scope* newScope = this->symbolTable->enterFunctionScope(functionDecl.identifier->name, functionSymbol);
    functionDecl.body->scope = newScope; // set scope of block

    // add parameters to the list of symbols in newScope
    for (int parameterIndex = 0; parameterIndex < functionDecl.parameters.size(); parameterIndex++) {
        newScope->declareSymbol(functionDecl.parameters[parameterIndex]->identifier->name, functionDecl.parameters[parameterIndex]->typeInfo->type, parameterIndex + 1, true);
    }

    bool alwaysReturns = false;

    // process each statement inside the block
    for (auto& stm : functionDecl.body->statements) {
        const auto semanticAnalysisResult = this->processStm(functionDecl.body->scope, *stm);
        if (alwaysReturns == false && semanticAnalysisResult.alwaysReturns) {
            alwaysReturns = true;
        }
    }
    this->symbolTable->leaveScope(); // leave scope after block is processed

    return SemanticAnalysisResult{alwaysReturns};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processStmVarDecl(Scope* scope, const ast::StmVarDecl& varDecl, const bool markAsInitialised) {
    // check symbol with same name has not been initialised already
    if (scope->symbols.find(varDecl.identifier->name) != scope->symbols.end()) {
        throw SemanticError(
            *this->path,
            varDecl.line,
            varDecl.column,
            "variable '" +
                varDecl.identifier->name +
                "' is already defined"
        );
    }

    // declare symbol (as uninitialised)
    scope->declareSymbol(varDecl.identifier->name, varDecl.typeInfo->type, 0, false);

    if (markAsInitialised) {
        scope->lookup(varDecl.identifier->name).value()->isInitialised = true;
    }

    // if var decl does not have an initialiser
    if (varDecl.optionalInitialiser == nullptr) {
        return SemanticAnalysisResult{false};
    }

    // check type of expr
    const auto initializerType = this->processExpr(scope, *varDecl.optionalInitialiser).type;
    // set symbol as initialised after checking optional initialiser (prevents self initialisation)
    scope->lookup(varDecl.identifier->name).value()->isInitialised = true;

    if (!canImplicitlyConvert(initializerType, varDecl.typeInfo->type)) {
        throw TypeError(
            *this->path,
            varDecl.optionalInitialiser->line,
            varDecl.optionalInitialiser->column,
            "cannot assign '" +
                typeToString(initializerType) +
                "' to type '" +
                typeToString(varDecl.typeInfo->type) +
                "'"
        );
    }
    return SemanticAnalysisResult{false};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processAssignment(Scope* scope, const ast::StmAssignment& assignment) {
    const auto identifierSymbol = this->checkSymbolIsDefined(
        scope,
        assignment.identifier->name,
        assignment.identifier->line,
        assignment.identifier->column
    );

    if (assignment.indices.size() > 0) {
        // check if array is initialised
        if (!identifierSymbol->isInitialised) {
            throw SemanticError(
                *this->path,
                assignment.identifier->line,
                assignment.identifier->column,
                "variable '" +
                    assignment.identifier->name +
                    "' may not have been initialised"
            );
        }
    }

    const auto exprType = this->processExpr(scope, *assignment.expression).type;

    // check type of indices
    for (const auto& index : assignment.indices) {
        this->processIndex(scope, *index);
    }

    // check var access index depth is smaller than the variable's dimensions
    const unsigned int indexDepth = this->resolveAccessArrayDepth(identifierSymbol->type, assignment.indices);

    if (!canImplicitlyConvert(exprType, Type{identifierSymbol->type.typeId, indexDepth})) {
        throw TypeError(
            *this->path,
            assignment.line,
            assignment.column,
            "cannot assign " +
                typeToString(exprType) +
                "' to type '" +
                typeToString(identifierSymbol->type) +
                "'"
        );
    }

    // update isInitialised of identifier in symbol table
    if (!identifierSymbol->isInitialised) identifierSymbol->isInitialised = true;
    return SemanticAnalysisResult{false};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processIfStatement(Scope *scope, const ast::IfStm &ifStm) {
    // ensure condition type is bool
    const auto conditionType = this->processExpr(scope, *ifStm.condition).type;
    this->checkConditionType(conditionType, ifStm.condition->line, ifStm.condition->column);

    const bool ifBlockAlwaysReturns = this->processBlock(*ifStm.ifBlock, ScopeKind::BLOCK).alwaysReturns;

    if (ifStm.elseStm != nullptr) {
        const bool elseBlockAlwaysReturns = this->processStm(scope, *ifStm.elseStm).alwaysReturns;
        return SemanticAnalysisResult{ifBlockAlwaysReturns && elseBlockAlwaysReturns};
    }
    return SemanticAnalysisResult{false};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processWhileStatement(Scope* scope, const ast::WhileStm& whileStm) {
    // ensure condition type is bool
    const auto conditionType = this->processExpr(scope, *whileStm.condition).type;
    this->checkConditionType(conditionType, whileStm.condition->line, whileStm.condition->column);

    this->processBlock(*whileStm.body, ScopeKind::LOOP);
    return SemanticAnalysisResult{false};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processForStatement(Scope *scope, const ast::ForStm &forStm) {
    // create scope for both forStm and body
    // this means bodyScope's parent an never be the global scope
    // which prevents 2 scope functions being created (1 from the for loop and 1 from the body)
    this->symbolTable->enterScope(ScopeKind::LOOP);
    auto bodyScope = this->symbolTable->enterScope(ScopeKind::BLOCK);

    // process range iterable
    // (this is done before variable so if the same assignable expression is used as both variable and start range, an error occurs)
    bool isIterableRange = false;
    bool rangeHasStep = false;
    ExpressionInfo rangeStartExprInfo;
    ExpressionInfo rangeEndExprInfo;
    ExpressionInfo rangeStepExprInfo;
    if (std::holds_alternative<std::unique_ptr<ast::ForRange>>(forStm.iterable)) {
        const auto& forRange = std::get<std::unique_ptr<ast::ForRange>>(forStm.iterable);
        rangeStartExprInfo = this->processExpr(scope, *forRange->start);
        rangeEndExprInfo = this->processExpr(scope, *forRange->end);
        if (forRange->step) {
            rangeStepExprInfo = this->processExpr(scope, *forRange->step);
            rangeHasStep = true;
        }
        isIterableRange = true;
    }

    // process variable
    Type variableType;
    uint32_t forVariableLine;
    uint16_t forVariableColumn;
    if (std::holds_alternative<std::unique_ptr<ast::StmVarDecl>>(forStm.variable)) {
        const auto& varDecl = *std::get<std::unique_ptr<ast::StmVarDecl>>(forStm.variable);
        this->processStmVarDecl(bodyScope, varDecl, true); // declare variable within nested scope, and mark variable as initialised
        // enforce forVariable is numeric if used as a range
        if (isIterableRange && !varDecl.typeInfo->type.isNumeric()) {
            this->throwTypeErrorFromForVariable(varDecl.typeInfo->type, varDecl.line, varDecl.column);
        }
        variableType = varDecl.typeInfo->type;
        forVariableLine = varDecl.line;
        forVariableColumn = varDecl.column;
    } else {
        const auto& forVariable = *std::get<std::unique_ptr<ast::ExprIdentifier>>(forStm.variable);
        ExpressionInfo forVariableExpressionInfo;
        // process exprIdentifier and mark identifierExpr as initialised
        forVariableExpressionInfo = this->processIdentifierExpr(scope, forVariable, true);

        if (!forVariableExpressionInfo.type.isNumeric()) {
            this->throwTypeErrorFromForVariable(forVariableExpressionInfo.type, forVariable.line, forVariable.column);
        }
        variableType = forVariableExpressionInfo.type;
        forVariableLine = forVariable.line;
        forVariableColumn = forVariable.column;
    }

    if (isIterableRange) {
        // enforce range types can implicitly convert to variableType
        if (!canImplicitlyConvert(rangeStartExprInfo.type, variableType)) {
            this->throwTypeErrorFromForRange(*std::get<std::unique_ptr<ast::ForRange>>(forStm.iterable)->start, rangeStartExprInfo.type, variableType);
        }
        if (!canImplicitlyConvert(rangeEndExprInfo.type, variableType)) {
            this->throwTypeErrorFromForRange(*std::get<std::unique_ptr<ast::ForRange>>(forStm.iterable)->end, rangeEndExprInfo.type, variableType);
        }
        if (rangeHasStep && !canImplicitlyConvert(rangeStepExprInfo.type, variableType)) {
            this->throwTypeErrorFromForRange(*std::get<std::unique_ptr<ast::ForRange>>(forStm.iterable)->step, rangeStepExprInfo.type, variableType);
        }
    } else {
        // enforce iterable is an array
        const auto& iterable = std::get<std::unique_ptr<ast::Expr>>(forStm.iterable);
        this->processExpr(scope, *iterable);
        if (!iterable->resultingType.isArray()) {
            throw TypeError(
                *this->path,
                iterable->line,
                iterable->column,
                "cannot iterate over value of type '" +
                    typeToString(iterable->resultingType) +
                    "'"
            );
        }
        // enforce element of array can implicitly convert to variableType
        Type elementType = iterable->resultingType;
        elementType.dimension--;

        if (!canImplicitlyConvert(elementType, variableType)) {
            throw TypeError(
                *this->path,
                forVariableLine,
                forVariableColumn,
                "cannot use type '" +
                    typeToString(variableType) +
                    "' as a loop variable for elements of type '" +
                    typeToString(elementType) +
                    "'"
            );
        }
    }

    this->processBlockWithScope(bodyScope, *forStm.body);

    this->symbolTable->leaveScope();

    return SemanticAnalysisResult{false};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processContinueStatement(Scope* scope, const ast::ContinueStm &continueStm) {
    const auto loopScope = scope->lookupLoopScope();

    if (loopScope == nullptr) {
        throw SemanticError(
            *this->path,
            continueStm.line,
            continueStm.column,
            "'continue' outside a loop"
        );
    }

    return SemanticAnalysisResult{false};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processBreakStatement(Scope* scope, const ast::BreakStm &breakStm) {
    const auto loopScope = scope->lookupLoopScope();

    if (loopScope == nullptr) {
        throw SemanticError(
            *this->path,
            breakStm.line,
            breakStm.column,
            "'break' outside a loop"
        );
    }

    return SemanticAnalysisResult{false};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processReturnStatement(Scope *scope, const ast::ReturnStm &returnStm) {
    auto currentFunctionSymbol = this->symbolTable->getCurrentFunctionSymbol();
    if (currentFunctionSymbol == nullptr) {
        throw SemanticError(
            *this->path,
            returnStm.line,
            returnStm.column,
            "'return' outside a function"
        );
    }

    returnStm.functionSymbol = currentFunctionSymbol;

    if (returnStm.returnExpression == nullptr) {
        if (currentFunctionSymbol->returnType.typeId != VOID_TYPE_ID) {
            // function has non-void return type and return has no expression
            throw SemanticError(
                *this->path,
                returnStm.line,
                returnStm.column,
                "function '" +
                currentFunctionSymbol->label +
                "' must return a value"
            );
        }
    } else {
        // return has expression

        const auto exprType = this->processExpr(scope, *returnStm.returnExpression).type;

        if (currentFunctionSymbol->returnType.typeId == VOID_TYPE_ID) { // function return type is void
            if (returnStm.returnExpression != nullptr) { // return has an expression
                throw SemanticError(
                    *this->path,
                    returnStm.line,
                    returnStm.column,
                    "cannot return a value from void function '" +
                        currentFunctionSymbol->label +
                        "'"
                );
            }
        }

        if (!canImplicitlyConvert(exprType, currentFunctionSymbol->returnType)) {
            throw TypeError(
                *this->path,
                returnStm.line,
                returnStm.column,
                "cannot return '" +
                    typeToString(exprType) +
                    "' from a function returning '" +
                    typeToString(currentFunctionSymbol->returnType) +
                    "'"
            );
        }
    }

    return SemanticAnalysisResult{true};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processExpressionStatement(Scope *scope, const ast::ExpressionStatement &expressionStm) {
    if (auto* functionCallExpr = dynamic_cast<const ast::FunctionCall*>(expressionStm.expression.get())) {
        const auto exprInfo = this->processFunctionCall(scope, *functionCallExpr);

        // ensure functionCallExpr has a return type of void
        if (exprInfo.type.typeId != VOID_TYPE_ID) {
            throw SemanticError(
                *this->path,
                functionCallExpr->line,
                functionCallExpr->column,
                "return value of function '" +
                functionSignatureToString(functionCallExpr->identifier->name, functionCallExpr->getArgumentTypes()) +
                "' must be used"
            );
        }

    } else if (auto* unaryExpr = dynamic_cast<const ast::ExprUnaryOperator*>(expressionStm.expression.get())) {
        // ensure unaryExpr has an inc / dec operator
        if (unaryExpr->unaryOperatorInfo->unaryOperator != UnaryOperator::DECREMENT &&
            unaryExpr->unaryOperatorInfo->unaryOperator != UnaryOperator::INCREMENT

        ) this->throwInvalidExpressionTypeAsStatement(*unaryExpr);


    } else if (auto* postfixExpr = dynamic_cast<const ast::ExprPostfix*>(expressionStm.expression.get())) {
        // ensure postfixExpr has an inc / dec operator
        if (postfixExpr->unaryOperatorInfo == nullptr) this->throwInvalidExpressionTypeAsStatement(*postfixExpr);

    // invalid expression type
    } else this->throwInvalidExpressionTypeAsStatement(*expressionStm.expression);

    this->processExpr(scope, *expressionStm.expression);

    return SemanticAnalysisResult{false};
}

void compiler::SemanticAnalyser::processIndex(Scope* scope, const ast::Index& index) {
    const auto indexType = this->processExpr(scope, *index.index).type;

    if (indexType.typeId != INT_TYPE_ID || indexType.isArray()) {
        throw TypeError(
            *this->path,
            index.index->line,
            index.index->column,
            "cannot use type '" +
                typeToString(indexType) +
                "' as an array index"
        );
    }
}

unsigned int compiler::SemanticAnalyser::resolveAccessArrayDepth(const Type& arrayType, const std::vector<std::unique_ptr<ast::Index>>& indices) const {
    if (indices.size() > arrayType.dimension) {
        throw TypeError(
            *this->path,
            indices[indices.size() - arrayType.dimension - 1]->line,
            indices[indices.size() - arrayType.dimension - 1]->column,
            "cannot index a value of type '" +
                typeToString(Type{arrayType.typeId, 0}) +
                "'"
            );
    }
    return arrayType.dimension - indices.size();
}

compiler::ExpressionInfo compiler::SemanticAnalyser::processExpr(Scope* scope, const ast::Expr& expr) {
    if (auto* binaryOperatorExpr = dynamic_cast<const ast::ExprBinaryOperator*>(&expr)) {
        return this->processBinaryExpr(scope, *binaryOperatorExpr);
    }
    if (auto* unaryOperatorExpr = dynamic_cast<const ast::ExprUnaryOperator*>(&expr)) {
        return this->processUnaryExpr(scope, *unaryOperatorExpr);
    }
    if (auto* postfixExpr = dynamic_cast<const ast::ExprPostfix*>(&expr)) {
        return this->processPostfixExpr(scope, *postfixExpr);
    }
    if (auto* castExpr = dynamic_cast<const ast::ExprCast*>(&expr)) {
        return this->processCastExpr(scope, *castExpr);
    }
    if (auto* functionCall = dynamic_cast<const ast::FunctionCall*>(&expr)) {
        return this->processFunctionCall(scope, *functionCall);
    }
    if (auto* newExpr = dynamic_cast<const ast::ExprNew*>(&expr)) {
        return this->processNewExpr(scope, *newExpr);
    }
    if (auto* identifierExpr = dynamic_cast<const ast::ExprIdentifier*>(&expr)) {
        return this->processIdentifierExpr(scope, *identifierExpr, false);
    }
    if (auto* intLit = dynamic_cast<const ast::ExprIntegerLiteral*>(&expr)) {
        return this->processIntegerLiteral(*intLit);
    }
    if (auto* floatLit = dynamic_cast<const ast::ExprFloatLiteral*>(&expr)) {
        return this->processFloatLiteral(*floatLit);
    }
    if (auto* boolLit = dynamic_cast<const ast::ExprBoolLiteral*>(&expr)) {
        return this->processBoolLiteral(*boolLit);
    }
    if (auto charLit = dynamic_cast<const ast::ExprCharLiteral*>(&expr)) {
        return this->processCharLiteral(*charLit);
    }
}

compiler::ExpressionInfo compiler::SemanticAnalyser::processBinaryExpr(Scope* scope, const ast::ExprBinaryOperator& binaryOperatorExpr) {
    const Type leftType = this->processExpr(scope, *binaryOperatorExpr.left).type;
    const Type rightType = this->processExpr(scope, *binaryOperatorExpr.right).type;

    if (leftType.isArray() || rightType.isArray()) this->throwTypeErrorFromBinaryOperator(binaryOperatorExpr, leftType, rightType);

    switch (binaryOperatorExpr.binaryOperatorInfo->binaryOperator) {
        case BinaryOperator::ADD: return ExpressionInfo{this->processAddition(binaryOperatorExpr, leftType, rightType), Assignability::NON_ASSIGNABLE};
        case BinaryOperator::SUBTRACT: return ExpressionInfo{this->processSubtraction(binaryOperatorExpr, leftType, rightType), Assignability::NON_ASSIGNABLE};
        case BinaryOperator::MULTIPLY: return ExpressionInfo{this->processMultiplication(binaryOperatorExpr, leftType, rightType), Assignability::NON_ASSIGNABLE};
        case BinaryOperator::DIVIDE: return ExpressionInfo{this->processDivision(binaryOperatorExpr, leftType, rightType), Assignability::NON_ASSIGNABLE};
        case BinaryOperator::INTEGER_DIVIDE: return ExpressionInfo{this->processIntegerDivision(binaryOperatorExpr, leftType, rightType), Assignability::NON_ASSIGNABLE};
        case BinaryOperator::MODULO: return ExpressionInfo{this->processModulo(binaryOperatorExpr, leftType, rightType), Assignability::NON_ASSIGNABLE};

        case BinaryOperator::BITWISE_OR:
        case BinaryOperator::BITWISE_XOR:
        case BinaryOperator::BITWISE_AND:
        case BinaryOperator::LEFT_SHIFT:
        case BinaryOperator::ARITHMETIC_RIGHT_SHIFT:
        case BinaryOperator::LOGICAL_RIGHT_SHIFT:
            return ExpressionInfo{this->processBitwiseOp(binaryOperatorExpr, leftType, rightType), Assignability::NON_ASSIGNABLE};

        case BinaryOperator::LOGICAL_OR:
        case BinaryOperator::LOGICAL_AND:
            return ExpressionInfo{this->processLogical(binaryOperatorExpr, leftType, rightType), Assignability::NON_ASSIGNABLE};

        case BinaryOperator::EQUAL_EQUAL:
        case BinaryOperator::NOT_EQUAL:
            return ExpressionInfo{this->processEquality(binaryOperatorExpr, leftType, rightType), Assignability::NON_ASSIGNABLE};

        case BinaryOperator::LESS_THAN:
        case BinaryOperator::LESS_THAN_OR_EQUAL:
        case BinaryOperator::GREATER_THAN:
        case BinaryOperator::GREATER_THAN_OR_EQUAL:
            return ExpressionInfo{this->processComparison(binaryOperatorExpr, leftType, rightType), Assignability::NON_ASSIGNABLE};
    }
}

compiler::ExpressionInfo compiler::SemanticAnalyser::processUnaryExpr(Scope* scope, const ast::ExprUnaryOperator& unaryOperatorExpr) {
    const auto exprTypeResult = this->processExpr(scope, *unaryOperatorExpr.expr);

    // if operator is increment or decrement and expression is not assignable
    if (unaryOperatorExpr.unaryOperatorInfo->unaryOperator == UnaryOperator::INCREMENT ||
        unaryOperatorExpr.unaryOperatorInfo->unaryOperator == UnaryOperator::DECREMENT
    ) {
        if (exprTypeResult.assignability != Assignability::ASSIGNABLE) {
            throw SemanticError(
                *this->path,
                unaryOperatorExpr.unaryOperatorInfo->line,
                unaryOperatorExpr.unaryOperatorInfo->column,
                "cannot apply operator '" +
                    unaryOperatorToString(unaryOperatorExpr.unaryOperatorInfo->unaryOperator) +
                    "' to a non-assignable expression"
            );
        }
    }

    if (exprTypeResult.type.isArray()) {
        this->throwTypeErrorFromUnaryOperator(unaryOperatorExpr, exprTypeResult.type);
    }

    switch (unaryOperatorExpr.unaryOperatorInfo->unaryOperator) {
        case UnaryOperator::PLUS:
        case UnaryOperator::MINUS: {
            if (
                exprTypeResult.type.typeId == INT_TYPE_ID ||
                exprTypeResult.type.typeId == FLOAT_TYPE_ID
            ) {
                unaryOperatorExpr.resultingType = exprTypeResult.type;
                return exprTypeResult;
            }
            if (exprTypeResult.type.typeId == CHAR_TYPE_ID) {
                const auto resultingType = Type{INT_TYPE_ID, 0};
                unaryOperatorExpr.resultingType = resultingType;
                return ExpressionInfo{resultingType, Assignability::NON_ASSIGNABLE};
            }
            this->throwTypeErrorFromUnaryOperator(unaryOperatorExpr, exprTypeResult.type);
        }
        case UnaryOperator::LOGICAL_NOT: {
            if (exprTypeResult.type.typeId == BOOL_TYPE_ID) {
                unaryOperatorExpr.resultingType = exprTypeResult.type;
                return exprTypeResult;
            }
            this->throwTypeErrorFromUnaryOperator(unaryOperatorExpr, exprTypeResult.type);
        }
        case UnaryOperator::BITWISE_NOT: {
            if (
                exprTypeResult.type.typeId == INT_TYPE_ID ||
                exprTypeResult.type.typeId == CHAR_TYPE_ID
            ) {
                const auto resultingType = Type{INT_TYPE_ID, 0};
                unaryOperatorExpr.resultingType = resultingType;
                return ExpressionInfo{resultingType, Assignability::NON_ASSIGNABLE};
            }
            this->throwTypeErrorFromUnaryOperator(unaryOperatorExpr, exprTypeResult.type);
        }
        case UnaryOperator::INCREMENT:
        case UnaryOperator::DECREMENT: {
            if (
                exprTypeResult.type.isNumeric()
            ) {
                unaryOperatorExpr.resultingType = exprTypeResult.type;
                return exprTypeResult;
            }
            this->throwTypeErrorFromUnaryOperator(unaryOperatorExpr, exprTypeResult.type);
        }
    }
}

compiler::ExpressionInfo compiler::SemanticAnalyser::processPostfixExpr(Scope* scope, const ast::ExprPostfix& postfixExpr) {
    auto exprTypeInfo = this->processExpr(scope, *postfixExpr.expression);

    if (!postfixExpr.postfixOperators.empty()) {
        for (const auto& postfixOperator : postfixExpr.postfixOperators) {
            if (std::holds_alternative<std::unique_ptr<ast::Index>>(postfixOperator)) {

                // ensure expr is an array
                if (!exprTypeInfo.type.isArray()) {
                    throw TypeError(
                        *this->path,
                        postfixExpr.line,
                        postfixExpr.column,
                        "cannot index a value of type '" +
                            typeToString(exprTypeInfo.type) +
                            "'"
                    );
                }

                // process index
                this->processIndex(scope, *std::get<std::unique_ptr<ast::Index>>(postfixOperator));

                // decrement resulting dimension
                exprTypeInfo.type.dimension--;
                exprTypeInfo.assignability = Assignability::ASSIGNABLE;

            } else {

                auto fieldAccess = std::get<std::unique_ptr<ast::FieldAccess>>(postfixOperator).get();

                // get field info
                auto fieldInfo = this->typeRegistry->getFieldInfo(exprTypeInfo.type, fieldAccess->identifier->name);

                if (!fieldInfo.has_value()) {
                    throw SemanticError(
                        *this->path,
                        fieldAccess->line,
                        fieldAccess->column,
                        "type '" +
                            typeToString(exprTypeInfo.type) +
                            "' has no field '" +
                            fieldAccess->identifier->name +
                            "'"
                    );
                }
                exprTypeInfo.type = fieldInfo.value()->type;
                exprTypeInfo.assignability = fieldInfo.value()->assignability;
                fieldAccess->fieldInfo = fieldInfo.value();
            }
        }
    }

    if (postfixExpr.unaryOperatorInfo != nullptr) {
        // throw error if expr is not assignable
        if (exprTypeInfo.assignability != Assignability::ASSIGNABLE) {
            throw SemanticError(
                *this->path,
                postfixExpr.line,
                postfixExpr.column,
                "cannot apply operator '" +
                    unaryOperatorToString(postfixExpr.unaryOperatorInfo->unaryOperator) +
                    "' to a non-assignable expression"
            );
        }

        // throw error if expr type is not numeric
        if (!exprTypeInfo.type.isNumeric()) {
            throw TypeError(
                *this->path,
                postfixExpr.line,
                postfixExpr.column,
                "cannot apply operator '" +
                    unaryOperatorToString(postfixExpr.unaryOperatorInfo->unaryOperator) +
                        "' to type '" +
                        typeToString(postfixExpr.expression->resultingType) +
                        "'"
            );
        }
    }

    postfixExpr.resultingType = exprTypeInfo.type;

    // postfix is only assignable if expression is assignable
    // AND
    // does not have a ++ or -- operator
    // AND
    // resulting expression is not an array
    return ExpressionInfo{
        exprTypeInfo.type,
        exprTypeInfo.assignability == Assignability::ASSIGNABLE &&
            postfixExpr.unaryOperatorInfo == nullptr &&
            !exprTypeInfo.type.isArray()
        ? Assignability::ASSIGNABLE : Assignability::NON_ASSIGNABLE
    };
}

compiler::ExpressionInfo compiler::SemanticAnalyser::processCastExpr(Scope* scope, const ast::ExprCast& castExpr) {
    const auto exprTypeResult = this->processExpr(scope, *castExpr.expr);

    if (canCastToType(exprTypeResult.type, castExpr.typeInfo->type)) {
        const auto resultingType = castExpr.typeInfo->type;
        castExpr.resultingType = resultingType;
        return ExpressionInfo{resultingType, Assignability::NON_ASSIGNABLE};
    }

    throw TypeError(
            *this->path,
            castExpr.line,
            castExpr.column,
            "cannot cast from type '" +
                typeToString(exprTypeResult.type) +
                "' to type '" +
                typeToString(castExpr.typeInfo->type) +
                "'"
        );
}

compiler::ExpressionInfo compiler::SemanticAnalyser::processFunctionCall(Scope* scope, const ast::FunctionCall& functionCall) {
    // process function call's arguments
    std::vector<Type> argumentTypes;
    for (const auto& argument : functionCall.arguments) {
        argumentTypes.push_back(this->processExpr(scope, *argument).type);
    }

    FunctionSymbol* functionSymbol = this->resolveFunctionCall(this->symbolTable->getFunctionSymbols(functionCall.identifier->name), functionCall, argumentTypes);

    if (functionSymbol == nullptr) {
        // if function is not defined
        throw SemanticError(
            *this->path,
            functionCall.line,
            functionCall.column,
            "function '" +
                functionSignatureToString(functionCall.identifier->name, argumentTypes) +
                "' is undefined"
        );
    }

    // set functionSymbol in FunctionCall ast for code gen
    functionCall.functionSymbol = functionSymbol;

    functionCall.resultingType = functionSymbol->returnType;

    return ExpressionInfo{
        functionSymbol->returnType,
        functionSymbol->returnType.isArray() ? Assignability::ASSIGNABLE : Assignability::NON_ASSIGNABLE
    };
}

compiler::ExpressionInfo compiler::SemanticAnalyser::processNewExpr(Scope *scope, const ast::ExprNew& newExpr) {
        for (const auto& index : newExpr.arrayDimensions) {
            this->processIndex(scope, *index);
        }

        if (newExpr.optionalInitialiser != nullptr) {
            std::queue<const std::unique_ptr<ast::ArrayInitialiser>*> initialisersToProcess;
            std::queue<unsigned int> depths;

            initialisersToProcess.push(&newExpr.optionalInitialiser);
            depths.push(1);

            while (!initialisersToProcess.empty()) {
                // get first element in queue
                const auto arrayInitialiser = initialisersToProcess.front();
                initialisersToProcess.pop();
                const auto depth = depths.front();
                depths.pop();

                // if initialiser requires a nested initialiser
                if (depth < newExpr.arrayDimensions.size()) {
                    // expecting each element to be an array initialiser
                    // add each nested initialiser to queue
                    for (const ast::ArrayInitialiserElement& element : arrayInitialiser->get()->elements) {
                        // initialiser element is expr instead
                        if (std::holds_alternative<std::unique_ptr<ast::Expr>>(element)) {
                            const auto expr = &std::get<std::unique_ptr<ast::Expr>>(element);
                            throw TypeError(
                                *this->path,
                                expr->get()->line,
                                expr->get()->column,
                                "array initialiser has too few dimensions for array of type '" +
                                    typeToString(newExpr.typeInfo->type) +
                                    "'"
                            );
                        }
                        initialisersToProcess.push(&std::get<std::unique_ptr<ast::ArrayInitialiser>>(element));
                        depths.push(depth + 1);
                    }
                } else {
                    // expecting each element to be expr which can be converted to array base type
                    for (const ast::ArrayInitialiserElement& element : arrayInitialiser->get()->elements) {
                        if (std::holds_alternative<std::unique_ptr<ast::ArrayInitialiser>>(element)) {
                            // initialiser element is a nested initialiser instead
                            const auto initialiser = &std::get<std::unique_ptr<ast::ArrayInitialiser>>(element);
                            throw TypeError(
                                *this->path,
                                initialiser->get()->line,
                                initialiser->get()->column,
                                "array initialiser has too many dimensions for array of type '" +
                                    typeToString(newExpr.typeInfo->type) +
                                    "'"
                            );
                        }
                        // check expr is valid for array type
                        const auto expr = &std::get<std::unique_ptr<ast::Expr>>(element);
                        if (!canImplicitlyConvert(this->processExpr(scope, *expr->get()).type, Type{newExpr.typeInfo->type.typeId, 0})) {
                            newExpr.typeInfo->type.dimension = 0; // for error message
                            throw TypeError(
                                *this->path,
                                expr->get()->line,
                                expr->get()->column,
                                "cannot initialise an array element of type '" +
                                    typeToString(newExpr.typeInfo->type) +
                                    "' with type '" +
                                    typeToString(expr->get()->resultingType) +
                                    "'"
                            );
                        }
                    }
                }
            }
        }

        const auto resultingType = newExpr.typeInfo->type;
        newExpr.resultingType = resultingType;
        return ExpressionInfo{resultingType, Assignability::NON_ASSIGNABLE};
}

compiler::ExpressionInfo compiler::SemanticAnalyser::processIdentifierExpr(Scope *scope, const ast::ExprIdentifier& identifierExpr, bool markAsInitialised) {
    // check if symbol has been initialised
    const auto symbol = this->checkSymbolIsDefined(scope, identifierExpr.identifier->name, identifierExpr.line, identifierExpr.column);

    if (markAsInitialised) {
        symbol->isInitialised = true;
    } else if (!symbol->isInitialised) {
        throw SemanticError(
            *this->path,
            identifierExpr.line,
            identifierExpr.column,
            "variable '" +
                identifierExpr.identifier->name +
                "' may not have been initialised"
        );
    }

    // get return type of identifier
    const auto resultingType = symbol->type;

    identifierExpr.resultingType = resultingType;
    return ExpressionInfo{resultingType, Assignability::ASSIGNABLE};
}

compiler::ExpressionInfo compiler::SemanticAnalyser::processIntegerLiteral(const ast::ExprIntegerLiteral& integerLiteral) {
    integerLiteral.resultingType = Type{INT_TYPE_ID, 0};
    return ExpressionInfo{
        Type{INT_TYPE_ID, 0},
        Assignability::NON_ASSIGNABLE
    };
}

compiler::ExpressionInfo compiler::SemanticAnalyser::processFloatLiteral(const ast::ExprFloatLiteral& floatLiteral) {
    floatLiteral.resultingType = Type{FLOAT_TYPE_ID, 0};
    return ExpressionInfo{
        Type{FLOAT_TYPE_ID, 0},
        Assignability::NON_ASSIGNABLE
    };
}

compiler::ExpressionInfo compiler::SemanticAnalyser::processBoolLiteral(const ast::ExprBoolLiteral& boolLiteral) {
    boolLiteral.resultingType = Type{BOOL_TYPE_ID, 0};
    return ExpressionInfo{
        Type{BOOL_TYPE_ID, 0},
        Assignability::NON_ASSIGNABLE
    };
}

compiler::ExpressionInfo compiler::SemanticAnalyser::processCharLiteral(const ast::ExprCharLiteral& charLiteral) {
    charLiteral.resultingType = Type{CHAR_TYPE_ID, 0};
    return ExpressionInfo{
        Type{CHAR_TYPE_ID, 0},
        Assignability::NON_ASSIGNABLE
    };
}

compiler::FunctionSymbol *compiler::SemanticAnalyser::resolveFunctionCall(std::vector<FunctionSymbol> *functionSymbols, const ast::FunctionCall& functionCall, const std::vector<Type>& argumentTypes) const {
    FunctionSymbol* functionSymbol = nullptr;

    if (functionSymbols != nullptr) {
        std::vector<FunctionSymbol*> validFunctionSymbols;

        // loop through each overloaded function
        for (auto& symbol : *functionSymbols) {

            if (symbol.parameterTypes.size() != argumentTypes.size()) {
                continue;
            }

            bool validSignature = true;
            bool isSignatureIdentical = true;;
            // loop through each parameter
            for (size_t i = 0; i < symbol.parameterTypes.size(); i++) {
                if (symbol.parameterTypes[i].typeId == argumentTypes[i].typeId &&
                    symbol.parameterTypes[i].dimension == argumentTypes[i].dimension
                ) {
                    continue;
                }
                isSignatureIdentical = false;
                validSignature = canImplicitlyConvert(argumentTypes[i], symbol.parameterTypes[i]);
                if (!validSignature) break;
            }

            if (isSignatureIdentical) { // signature matches 1 : 1
                functionSymbol = &symbol;
                break;
            }

            if (validSignature) { // signature is valid but requires implicit conversion
                validFunctionSymbols.push_back(&symbol);
            }
        }

        // if argument types are identical to parameter types
        if (functionSymbol != nullptr) {
            return functionSymbol;
        }

        if (validFunctionSymbols.size() == 1) { // function is unambiguous (only 1 function symbol to choose from)
            functionSymbol = validFunctionSymbols[0];

        } else if (validFunctionSymbols.size() > 1) { // function is ambiguous (multiple function symbols to choose from)
            throw SemanticError(
                *this->path,
                functionCall.line,
                functionCall.column,
                "ambiguous call to function '" +
                    functionCall.identifier->name +
                    "'"
            );
        }
    }
    // no valid function signature results in functionSymbol == nullptr
    return functionSymbol;
}

compiler::Type compiler::SemanticAnalyser::processAddition(const ast::ExprBinaryOperator& binaryOperator, const Type &leftType, const Type &rightType) {
    /*
       | Left \ Right | `int`   | `float` | `char`  |
       |--------------|---------|---------|---------|
       | `int`        | `int`   | `float` | `int`   |
       | `float`      | `float` | `float` | `float` |
       | `char`       | `int`   | `float` | `int`   |
     */

    // numeric + numeric
    if (leftType.isNumeric() && rightType.isNumeric()) {
        Type resultingType;
        if (leftType.typeId == FLOAT_TYPE_ID || rightType.typeId == FLOAT_TYPE_ID) {
            resultingType = Type{FLOAT_TYPE_ID, 0};
        } else {
            resultingType = Type{INT_TYPE_ID, 0};
        }
        binaryOperator.resultingType = resultingType;
        return resultingType;
    }

    this->throwTypeErrorFromBinaryOperator(binaryOperator, leftType, rightType);
}

compiler::Type compiler::SemanticAnalyser::processSubtraction(const ast::ExprBinaryOperator& binaryOperator, const Type &leftType, const Type &rightType) {
    /*
       | Left \ Right | `int`   | `float` | `char`  |
       |--------------|---------|---------|---------|
       | `int`        | `int`   | `float` | `int`   |
       | `float`      | `float` | `float` | `float` |
       | `char`       | `int`   | `float` | `int`   |
     */

    // numeric - numeric
    if (leftType.isNumeric() && rightType.isNumeric()) {
        Type resultingType;
        if (leftType.typeId == FLOAT_TYPE_ID || rightType.typeId == FLOAT_TYPE_ID) {
            resultingType = Type{FLOAT_TYPE_ID, 0};
        } else {
            resultingType = Type{INT_TYPE_ID, 0};
        }
        binaryOperator.resultingType = resultingType;
        return resultingType;
    }

    this->throwTypeErrorFromBinaryOperator(binaryOperator, leftType, rightType);
}

compiler::Type compiler::SemanticAnalyser::processMultiplication(const ast::ExprBinaryOperator& binaryOperator, const Type &leftType, const Type &rightType) {
    /*
       | Left \ Right | `int`   | `float` | `char`  |
       |--------------|---------|---------|---------|
       | `int`        | `int`   | `float` | `int`   |
       | `float`      | `float` | `float` | `float` |
       | `char`       | `int`   | `float` | `int`   |
     */

    // numeric * numeric
    if (leftType.isNumeric() && rightType.isNumeric()) {
        Type resultingType;
        if (leftType.typeId == FLOAT_TYPE_ID || rightType.typeId == FLOAT_TYPE_ID) {
            resultingType = Type{FLOAT_TYPE_ID, 0};
        } else {
            resultingType = Type{INT_TYPE_ID, 0};
        }
        binaryOperator.resultingType = resultingType;
        return resultingType;
    }

    this->throwTypeErrorFromBinaryOperator(binaryOperator, leftType, rightType);
}

compiler::Type compiler::SemanticAnalyser::processDivision(const ast::ExprBinaryOperator& binaryOperator, const Type &leftType, const Type &rightType) {
    /*
       | Left \ Right | `int`   | `float` | `char`  |
       |--------------|---------|---------|---------|
       | `int`        | `float` | `float` | `float` |
       | `float`      | `float` | `float` | `float` |
       | `char`       | `float` | `float` | `float` |
     */

    //    numeric / numeric
    if (leftType.isNumeric() && rightType.isNumeric()) {
        const auto resultingType = Type{FLOAT_TYPE_ID, 0};
        binaryOperator.resultingType = resultingType;
        return resultingType;
    }

    this->throwTypeErrorFromBinaryOperator(binaryOperator, leftType, rightType);
}

compiler::Type compiler::SemanticAnalyser::processIntegerDivision(const ast::ExprBinaryOperator& binaryOperator, const Type &leftType, const Type &rightType) {
    /*
       | Left \ Right | `int`   | `float` | `char`  |
       |--------------|---------|---------|---------|
       | `int`        | `int`   | `int`   | `int`   |
       | `float`      | `int`   | `int`   | `int`   |
       | `char`       | `int`   | `int`   | `int`   |
     */

    //    numeric // numeric
    if (leftType.isNumeric() && rightType.isNumeric()) {
        const auto resultingType = Type{INT_TYPE_ID, 0};
        binaryOperator.resultingType = resultingType;
        return resultingType;
    }

    this->throwTypeErrorFromBinaryOperator(binaryOperator, leftType, rightType);
}

compiler::Type compiler::SemanticAnalyser::processModulo(const ast::ExprBinaryOperator& binaryOperator, const Type &leftType, const Type &rightType) {
    /*
       | Left \ Right | `int`   | `float` | `char`  |
       |--------------|---------|---------|---------|
       | `int`        | `int`   | `float` | `int`   |
       | `float`      | `float` | `float` | `float` |
       | `char`       | `int`   | `float` | `int`   |
     */

    // numeric % numeric
    if (leftType.isNumeric() && rightType.isNumeric()) {
        Type resultingType;
        if (leftType.typeId == FLOAT_TYPE_ID || rightType.typeId == FLOAT_TYPE_ID) {
            resultingType = Type{FLOAT_TYPE_ID, 0};
        } else {
            resultingType = Type{INT_TYPE_ID, 0};
        }
        binaryOperator.resultingType = resultingType;
        return resultingType;
    }

    this->throwTypeErrorFromBinaryOperator(binaryOperator, leftType, rightType);
}

compiler::Type compiler::SemanticAnalyser::processBitwiseOp(const ast::ExprBinaryOperator &binaryOperator, const Type &leftType, const Type &rightType) {
    /*
       | Left \ Right | `int` | `char` |
       |--------------|-------|--------|
       | `int`        | `int` | `int`  |
       | `char`       | `int` | `int`  |
     */

    if (
        // int & int  // int & char
        (leftType.typeId == INT_TYPE_ID && (rightType.typeId == INT_TYPE_ID || rightType.typeId == CHAR_TYPE_ID)) ||
        // char & int  // char & char
        (leftType.typeId == CHAR_TYPE_ID && (rightType.typeId == INT_TYPE_ID || rightType.typeId == CHAR_TYPE_ID))
    ) {
        const auto resultingType = Type{INT_TYPE_ID, 0};
        binaryOperator.resultingType = resultingType;
        return resultingType;
    }
    this->throwTypeErrorFromBinaryOperator(binaryOperator, leftType, rightType);
}


compiler::Type compiler::SemanticAnalyser::processLogical(const ast::ExprBinaryOperator& binaryOperator, const Type &leftType, const Type &rightType) {
    /*
       | Left \ Right | `bool`  |
       |--------------|---------|
       | `bool`       | `bool`  |
     */

    // bool && bool
    if (leftType.typeId == BOOL_TYPE_ID && rightType.typeId == BOOL_TYPE_ID) {
        const auto resultingType = Type{BOOL_TYPE_ID, 0};
        binaryOperator.resultingType = resultingType;
        return resultingType;
    }

    this->throwTypeErrorFromBinaryOperator(binaryOperator, leftType, rightType);
}

compiler::Type compiler::SemanticAnalyser::processEquality(const ast::ExprBinaryOperator& binaryOperator, const Type &leftType, const Type &rightType) {
    /*
       | Left \ Right | `int`   | `float` | `bool`  | `char`  |
       |--------------|---------|---------|---------|---------|
       | `int`        | `bool`  | `bool`  | invalid | `bool`  |
       | `float`      | `bool`  | `bool`  | invalid | `bool`  |
       | `bool`       | invalid | invalid | `bool`  | invalid |
       | `char`       | `bool`  | `bool`  | invalid | `bool`  |
     */

    if (
        // numeric == numeric
        leftType.isNumeric() && rightType.isNumeric() ||
        // bool == bool
        leftType.typeId == rightType.typeId
    ) {
        const auto resultingType = Type{BOOL_TYPE_ID, 0};
        binaryOperator.resultingType = resultingType;
        return resultingType;
    }

    this->throwTypeErrorFromBinaryOperator(binaryOperator, leftType, rightType);
}

compiler::Type compiler::SemanticAnalyser::processComparison(const ast::ExprBinaryOperator& binaryOperator, const Type &leftType, const Type &rightType) {
    /*
       | Left \ Right | `int`  | `float` | `char`  |
       |--------------|--------|---------|---------|
       | `int`        | `bool` | `bool`  | `bool`  |
       | `float`      | `bool` | `bool`  | `bool`  |
       | `char`       | `bool` | `bool`  | `bool`  |
     */

    // numeric < numeric
    if (leftType.isNumeric() && rightType.isNumeric()) {
        const auto resultingType = Type{BOOL_TYPE_ID, 0};
        binaryOperator.resultingType = resultingType;
        return resultingType;
    }

    this->throwTypeErrorFromBinaryOperator(binaryOperator, leftType, rightType);
}

bool compiler::SemanticAnalyser::canCastToType(const Type& from, const Type& to) {
    /*
       | Source \ Target | `int`   | `float` | `bool`  | `char`  |
       |-----------------|---------|---------|---------|---------|
       | `int`           | `int`   | `float` | invalid | `char`  |
       | `float`         | `int`   | `float` | invalid | `char`  |
       | `bool`          | invalid | invalid | `bool`  | invalid |
       | `char`          | `int`   | 'float' | invalid | `char`  |
     */

    if (
        // numeric -> numeric
        from.isNumeric() && to.isNumeric() ||
        // bool -> bool
        from.typeId == BOOL_TYPE_ID && to.typeId == BOOL_TYPE_ID
    ) {
        return true;
    }
    return false;
}

bool compiler::SemanticAnalyser::canImplicitlyConvert(const Type& from, const Type& to) {
    if (from.typeId == to.typeId && from.dimension == to.dimension) return true; // types are identical
    if (from.dimension != 0 || to.dimension != 0) return false; // return false as array types cannot be implicity converted

    switch (from.typeId) {
        case INT_TYPE_ID:
            return to.typeId == FLOAT_TYPE_ID;

        case CHAR_TYPE_ID:
            return to.typeId == INT_TYPE_ID || to.typeId == FLOAT_TYPE_ID;

        default:
            return false;
    }
}

compiler::Symbol* compiler::SemanticAnalyser::checkSymbolIsDefined(Scope* scope, const std::string& identifier, const size_t line, const size_t column) const {
    auto symbol = scope->lookup(identifier);
    if (!symbol.has_value()) {
        throw SemanticError(
            *this->path,
            line,
            column,
            "variable '" +
                identifier +
                "' is undefined"
        );
    }
    return symbol.value();
}

std::string compiler::SemanticAnalyser::typeToString(const Type& type) {
    std::string result;
    switch (type.typeId) {
        case VOID_TYPE_ID: result += "void"; break;
        case INT_TYPE_ID: result += "int"; break;
        case FLOAT_TYPE_ID: result += "float"; break;
        case BOOL_TYPE_ID: result += "bool"; break;
        case CHAR_TYPE_ID: result += "char"; break;
    }
    for (int i = 0; i < type.dimension; i++) {
        result += "[]";
    }
    return result;
}

std::string compiler::SemanticAnalyser::binaryOperatorToString(const BinaryOperator &binaryOperator) {
    switch (binaryOperator) {
        case BinaryOperator::ADD: return "+";
        case BinaryOperator::SUBTRACT: return "-";
        case BinaryOperator::MULTIPLY: return "*";
        case BinaryOperator::DIVIDE: return "/";
        case BinaryOperator::INTEGER_DIVIDE: return "//";
        case BinaryOperator::MODULO: return "%";

        case BinaryOperator::BITWISE_OR: return "|";
        case BinaryOperator::BITWISE_XOR: return "^";
        case BinaryOperator::BITWISE_AND: return "&";

        case BinaryOperator::LEFT_SHIFT: return "<<";
        case BinaryOperator::ARITHMETIC_RIGHT_SHIFT: return ">>";
        case BinaryOperator::LOGICAL_RIGHT_SHIFT: return ">>>";

        case BinaryOperator::LOGICAL_OR: return "||";
        case BinaryOperator::LOGICAL_AND: return "&&";

        case BinaryOperator::EQUAL_EQUAL: return "==";
        case BinaryOperator::NOT_EQUAL: return "!=";

        case BinaryOperator::LESS_THAN: return "<";
        case BinaryOperator::LESS_THAN_OR_EQUAL: return "<=";
        case BinaryOperator::GREATER_THAN: return ">";
        case BinaryOperator::GREATER_THAN_OR_EQUAL: return ">=";
    }
}

std::string compiler::SemanticAnalyser::unaryOperatorToString(const UnaryOperator& unaryOperator) {
    switch (unaryOperator) {
        case UnaryOperator::PLUS: return "+";
        case UnaryOperator::MINUS: return "-";
        case UnaryOperator::BITWISE_NOT: return "~";
        case UnaryOperator::LOGICAL_NOT: return "!";
        case UnaryOperator::INCREMENT: return "++";
        case UnaryOperator::DECREMENT: return "--";
    }
}

void compiler::SemanticAnalyser::throwTypeErrorFromUnaryOperator(const ast::ExprUnaryOperator& unaryOperator, const Type& type) const {
    throw TypeError(
            *this->path,
            unaryOperator.line,
            unaryOperator.column,
            "cannot apply operator '" +
                unaryOperatorToString(unaryOperator.unaryOperatorInfo->unaryOperator) +
                "' to type '" +
                typeToString(type) +
                "'"
        );
}


void compiler::SemanticAnalyser::throwTypeErrorFromBinaryOperator(const ast::ExprBinaryOperator& binaryOperator, const Type& leftType, const Type& rightType) const {
    throw TypeError(
        *this->path,
        binaryOperator.line,
        binaryOperator.column,
        "cannot apply operator '" +
            binaryOperatorToString(binaryOperator.binaryOperatorInfo->binaryOperator) +
            "' to types '" +
            typeToString(leftType) +
            "' and '" +
            typeToString(rightType) +
            "'"
    );
}

void compiler::SemanticAnalyser::throwInvalidExpressionTypeAsStatement(const ast::Expr& expr) const {
    throw SemanticError(
        *this->path,
        expr.line,
        expr.column,
        "expression cannot be used as a statement"
    );
}

void compiler::SemanticAnalyser::throwTypeErrorFromForVariable(const Type &variableType, const uint32_t forVariableLine, const uint16_t forVariableColumn) const {
    throw TypeError(
        *this->path,
        forVariableLine,
        forVariableColumn,
        "cannot use type '" +
            typeToString(variableType) +
            "' as a range"
    );
}

void compiler::SemanticAnalyser::throwTypeErrorFromForRange(const ast::Expr& rangeExpr, const Type &rangeType, const Type &variableType) const {
    throw TypeError(
        *this->path,
        rangeExpr.line,
        rangeExpr.column,
        "cannot use type '" +
            typeToString(rangeType) +
            "' as a range for type '" +
            typeToString(variableType) +
            "'"
    );
}

void compiler::SemanticAnalyser::checkConditionType(const Type& conditionType, const size_t line, const size_t column) const {
    if (conditionType.isArray() ||
        conditionType.typeId != BOOL_TYPE_ID
    ) {
        throw TypeError(
            *this->path,
            line,
            column,
            "cannot use type '" +
                typeToString(conditionType) +
                "' as a condition"
        );
    }
}

std::string compiler::SemanticAnalyser::functionSignatureToString(const std::string& functionIdentifier, const std::vector<Type>& parameterTypes) {
    std::string result = functionIdentifier + "(";

    if (parameterTypes.size() > 0) {
        result+= typeToString(parameterTypes.at(0));
    }

    for (int i = 1; i < parameterTypes.size(); i++) {
        result += ",";
        result += typeToString(parameterTypes.at(i));
    }

    result+= ")";
    return result;
}
