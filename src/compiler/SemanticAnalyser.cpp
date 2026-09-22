#include "SemanticAnalyser.h"

#include <iostream>
#include <queue>

#include "../include/Error.h"

compiler::SemanticAnalyser::SemanticAnalyser(SymbolTable* symbolTable, const std::filesystem::path* path) :
    symbolTable(symbolTable), path(path) {}

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
        if (functionDecl->returnTypeInfo->type.type != Type::VOID_RETURN_TYPE &&
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
        return this->processStmVarDecl(scope, *varDecl);
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
    block.scope = newScope; // set scope of block

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

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processStmVarDecl(Scope* scope, const ast::StmVarDecl& varDecl) {
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

    // if var decl does not have an initialiser
    if (varDecl.optionalInitialiser == nullptr) {
        return SemanticAnalysisResult{false};
    }

    // check type of expr
    const auto initializerType = this->checkExprType(scope, *varDecl.optionalInitialiser).type;
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

    const auto exprType = this->checkExprType(scope, *assignment.expression).type;

    // check type of indices
    for (const auto& index : assignment.indices) {
        this->processIndex(scope, *index);
    }

    // check var access index depth is smaller than the variable's dimensions
    const unsigned int indexDepth = this->resolveAccessArrayDepth(SemanticType{identifierSymbol->type, identifierSymbol->dimension}, assignment.indices);

    if (!canImplicitlyConvert(exprType, SemanticType{identifierSymbol->type, indexDepth})) {
        throw TypeError(
            *this->path,
            assignment.line,
            assignment.column,
            "cannot assign " +
                typeToString(exprType) +
                "' to type '" +
                typeToString(SemanticType{identifierSymbol->type, identifierSymbol->dimension}) +
                "'"
        );
    }

    // update isInitialised of identifier in symbol table
    if (!identifierSymbol->isInitialised) identifierSymbol->isInitialised = true;
    return SemanticAnalysisResult{false};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processIfStatement(Scope *scope, const ast::IfStm &ifStm) {
    // ensure condition type is bool
    const auto conditionType = this->checkExprType(scope, *ifStm.condition).type;
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
    const auto conditionType = this->checkExprType(scope, *whileStm.condition).type;
    this->checkConditionType(conditionType, whileStm.condition->line, whileStm.condition->column);

    this->processBlock(*whileStm.block, ScopeKind::WHILE);
    return SemanticAnalysisResult{false};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processContinueStatement(Scope* scope, const ast::ContinueStm &continueStm) {
    const auto loopScope = scope->lookupWhileScope();

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
    const auto loopScope = scope->lookupWhileScope();

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
        if (currentFunctionSymbol->returnType.type != Type::VOID_RETURN_TYPE) {
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

        const auto exprType = this->checkExprType(scope, *returnStm.returnExpression).type;

        if (currentFunctionSymbol->returnType.type == Type::VOID_RETURN_TYPE) { // function return type is void
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
        const auto exprResult = this->processFunctionCall(scope, *functionCallExpr);

        // ensure functionCallExpr has a return type of void
        if (exprResult.type.type != Type::VOID_RETURN_TYPE) {
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

    this->checkExprType(scope, *expressionStm.expression);

    return SemanticAnalysisResult{false};
}

void compiler::SemanticAnalyser::processIndex(Scope* scope, const ast::Index& index) {
    const auto indexType = this->checkExprType(scope, *index.index).type;

    if (indexType.type != Type::INT || indexType.isArray()) {
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

unsigned int compiler::SemanticAnalyser::resolveAccessArrayDepth(const SemanticType& arrayType, const std::vector<std::unique_ptr<ast::Index>>& indices) const {
    if (indices.size() > arrayType.dimension) {
        throw TypeError(
            *this->path,
            indices[indices.size() - arrayType.dimension - 1]->line,
            indices[indices.size() - arrayType.dimension - 1]->column,
            "cannot index a value of type '" +
                typeToString(SemanticType{arrayType.type, 0}) +
                "'"
            );
    }
    return arrayType.dimension - indices.size();
}

compiler::SemanticExprResult compiler::SemanticAnalyser::checkExprType(Scope* scope, const ast::Expr& expr) {
    if (auto* intLit = dynamic_cast<const ast::ExprIntegerLiteral*>(&expr)) {
        intLit->resultingType = SemanticType{Type::INT, 0};
        return SemanticExprResult{
            SemanticType{Type::INT, 0},
            ExprCategory::VALUE
        };
    }
    if (auto* floatLit = dynamic_cast<const ast::ExprFloatLiteral*>(&expr)) {
        floatLit->resultingType = SemanticType{Type::FLOAT, 0};
        return SemanticExprResult{
            SemanticType{Type::FLOAT, 0},
            ExprCategory::VALUE
        };
    }
    if (auto* boolLit = dynamic_cast<const ast::ExprBoolLiteral*>(&expr)) {
        boolLit->resultingType = SemanticType{Type::BOOL, 0};
        return SemanticExprResult{
            SemanticType{Type::BOOL, 0},
            ExprCategory::VALUE
        };
    }
    if (auto* binaryOperator = dynamic_cast<const ast::ExprBinaryOperator*>(&expr)) {
        const SemanticType leftType = this->checkExprType(scope, *binaryOperator->left).type;
        const SemanticType rightType = this->checkExprType(scope, *binaryOperator->right).type;

        if (leftType.isArray() || rightType.isArray()) {
            this->throwTypeErrorFromBinaryOperator(*binaryOperator, leftType, rightType);
        }

        switch (binaryOperator->binaryOperatorInfo->binaryOperator) {
            case BinaryOperator::PLUS:
            case BinaryOperator::MINUS:
            case BinaryOperator::MULTIPLY:
            case BinaryOperator::DIVIDE:
            case BinaryOperator::MODULO: {
                // ensure either operand is not bool
                if (leftType.type == Type::BOOL || rightType.type == Type::BOOL) throwTypeErrorFromBinaryOperator(*binaryOperator, leftType, rightType);

                // result is float if either operand is a float or binary operator is divide
                if ((leftType.type == Type::FLOAT || rightType.type == Type::FLOAT) ||
                    binaryOperator->binaryOperatorInfo->binaryOperator == BinaryOperator::DIVIDE
                ) {
                    constexpr auto resultingType = SemanticType{Type::FLOAT, 0};
                    binaryOperator->resultingType = resultingType;
                    return SemanticExprResult{resultingType, ExprCategory::VALUE};
                }
                constexpr auto resultingType = SemanticType{Type::INT, 0};
                binaryOperator->resultingType = resultingType;
                return SemanticExprResult{resultingType, ExprCategory::VALUE};
            }

            case BinaryOperator::INTEGER_DIVIDE: {
                // ensure either operand is not bool
                if (leftType.type == Type::BOOL || rightType.type == Type::BOOL) throwTypeErrorFromBinaryOperator(*binaryOperator, leftType, rightType);
                constexpr auto resultingType = SemanticType{Type::INT, 0};
                binaryOperator->resultingType = resultingType;
                return SemanticExprResult{resultingType, ExprCategory::VALUE};
            }

            case BinaryOperator::LOGICAL_OR:
            case BinaryOperator::LOGICAL_AND: {
                if (leftType.type != Type::BOOL || rightType.type != Type::BOOL) throwTypeErrorFromBinaryOperator(*binaryOperator, leftType, rightType);
                constexpr auto resultingType = SemanticType{Type::BOOL, 0};
                binaryOperator->resultingType = resultingType;
                return SemanticExprResult{resultingType, ExprCategory::VALUE};
            }

            case BinaryOperator::LESS_THAN:
            case BinaryOperator::LESS_THAN_OR_EQUAL:
            case BinaryOperator::GREATER_THAN:
            case BinaryOperator::GREATER_THAN_OR_EQUAL: {
                if (leftType.type == Type::BOOL || rightType.type == Type::BOOL) throwTypeErrorFromBinaryOperator(*binaryOperator, leftType, rightType);
                constexpr auto resultingType = SemanticType{Type::BOOL, 0};
                binaryOperator->resultingType = resultingType;
                return SemanticExprResult{resultingType, ExprCategory::VALUE};
            }

            default:
                // EQUAL_EQUAL / NOT_EQUAL

                // if either left or right type is bool and the other operand is not bool
                if ((leftType.type == Type::BOOL || rightType.type == Type::BOOL) &&
                    leftType.type != rightType.type
                ) {
                    throwTypeErrorFromBinaryOperator(*binaryOperator, leftType, rightType);
                }
                constexpr auto resultingType = SemanticType{Type::BOOL, 0};
                binaryOperator->resultingType = resultingType;
                return SemanticExprResult{resultingType, ExprCategory::VALUE};
        }
    }
    if (auto* unaryOperator = dynamic_cast<const ast::ExprUnaryOperator*>(&expr)) {
        const auto exprTypeResult = this->checkExprType(scope, *unaryOperator->expr);

        // if operator is logical not and (type is not bool or type is an array)
        // OR
        // if operator is not logical not and (types is not numeric)
        if ((unaryOperator->unaryOperatorInfo->unaryOperator == UnaryOperator::LOGICAL_NOT &&
                        (exprTypeResult.type.type != Type::BOOL || exprTypeResult.type.isArray())) ||
             (unaryOperator->unaryOperatorInfo->unaryOperator != UnaryOperator::LOGICAL_NOT && !exprTypeResult.type.isNumeric())
        ) {
            throw TypeError(
                *this->path,
                unaryOperator->line,
                unaryOperator->column,
                "cannot apply operator '" +
                    unaryOperatorToString(unaryOperator->unaryOperatorInfo->unaryOperator) +
                    "' to type '" +
                    typeToString(exprTypeResult.type) +
                    "'"
            );
        }

        // if operator is increment or decrement and expression is not assignable
        if (unaryOperator->unaryOperatorInfo->unaryOperator == UnaryOperator::INCREMENT ||
            unaryOperator->unaryOperatorInfo->unaryOperator == UnaryOperator::DECREMENT
        ) {
            if (exprTypeResult.category != ExprCategory::ASSIGNABLE) {
                throw TypeError(
                    *this->path,
                    unaryOperator->unaryOperatorInfo->line,
                    unaryOperator->unaryOperatorInfo->column,
                    "cannot apply operator '" +
                        unaryOperatorToString(unaryOperator->unaryOperatorInfo->unaryOperator) +
                        "' to a non-assignable expression"
                );
            }
        }

        unaryOperator->resultingType = exprTypeResult.type;
        return exprTypeResult;
    }

    if (auto* exprPostfix = dynamic_cast<const ast::ExprPostfix*>(&expr)) {
        auto exprTypeResult = this->checkExprType(scope, *exprPostfix->expression);

        if (exprPostfix->indices.size() > 0) {

            if (!exprTypeResult.type.isArray()) {
                throw TypeError(
                    *this->path,
                    exprPostfix->line,
                    exprPostfix->column,
                    "cannot index a value of type '" +
                        typeToString(exprTypeResult.type) +
                        "'"
                );
            }

            if (exprPostfix->indices.size() > exprTypeResult.type.dimension) {
                exprTypeResult.type.dimension = 0; // for error message
                throw TypeError(
                    *this->path,
                    exprPostfix->line,
                    exprPostfix->column,
                    "cannot index a value of type '" +
                        typeToString(exprTypeResult.type) +
                        "'"
                );
            }

            exprTypeResult.type.dimension -= exprPostfix->indices.size();

            // process indices
            for (const auto& index : exprPostfix->indices) {
                this->processIndex(scope, *index);
            }
        }
        if (exprPostfix->unaryOperatorInfo != nullptr) {
            // throw error if
            // expr type is not numeric
            // OR
            // expression is not assignable

            if (exprTypeResult.category != ExprCategory::ASSIGNABLE || !exprTypeResult.type.isNumeric()) {
                std::string errorMessage = "cannot apply operator '" +
                        unaryOperatorToString(exprPostfix->unaryOperatorInfo->unaryOperator) +
                        "' ";

                errorMessage += exprTypeResult.category != ExprCategory::ASSIGNABLE ||
                                exprTypeResult.type.isArray()
                    ? "to a non-assignable expression"
                    : "to type '" + typeToString(exprPostfix->expression->resultingType) + "'";

                throw TypeError(
                    *this->path,
                    exprPostfix->line,
                    exprPostfix->column,
                    errorMessage
                );
            }
        }

        exprPostfix->resultingType = exprTypeResult.type;

        // postfix is only assignable if expression is assignable
        // AND
        // does not have a ++ or -- operator
        // AND
        // resulting expression is not an array
        return SemanticExprResult{
            exprTypeResult.type,
            exprTypeResult.category == ExprCategory::ASSIGNABLE &&
                exprPostfix->unaryOperatorInfo == nullptr &&
                !exprTypeResult.type.isArray()
            ? ExprCategory::ASSIGNABLE : ExprCategory::VALUE
        };
    }

    if (auto* exprIdentifier = dynamic_cast<const ast::ExprIdentifier*>(&expr)) {
        // check if symbol has been initialised
        const auto symbol = this->checkSymbolIsDefined(scope, exprIdentifier->identifier->name, exprIdentifier->line, exprIdentifier->column);
        if (!symbol->isInitialised) {
            throw SemanticError(
                *this->path,
                exprIdentifier->line,
                exprIdentifier->column,
                "variable '" +
                    exprIdentifier->identifier->name +
                    "' may not have been initialised"
            );
        }

        // get return type of identifier

        const auto resultingType = SemanticType{symbol->type, symbol->dimension};

        expr.resultingType = resultingType;
        return SemanticExprResult{resultingType, ExprCategory::ASSIGNABLE};
    }

    if (auto* newExpression = dynamic_cast<const ast::ExprNew*>(&expr)) {
        for (const auto& index : newExpression->arrayDimensions) {
            this->processIndex(scope, *index);
        }

        if (newExpression->optionalInitialiser != nullptr) {
            std::queue<const std::unique_ptr<ast::ArrayInitialiser>*> initialisersToProcess;
            std::queue<unsigned int> depths;

            initialisersToProcess.push(&newExpression->optionalInitialiser);
            depths.push(1);

            while (!initialisersToProcess.empty()) {
                // get first element in queue
                const auto arrayInitialiser = initialisersToProcess.front();
                initialisersToProcess.pop();
                const auto depth = depths.front();
                depths.pop();

                // if initialiser requires a nested initialiser
                if (depth < newExpression->arrayDimensions.size()) {
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
                                    typeToString(newExpression->typeInfo->type) +
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
                                    typeToString(newExpression->typeInfo->type) +
                                    "'"
                            );
                        }
                        // check expr is valid for array type
                        const auto expr = &std::get<std::unique_ptr<ast::Expr>>(element);
                        if (!canImplicitlyConvert(this->checkExprType(scope, *expr->get()).type, SemanticType{newExpression->typeInfo->type.type, 0})) {
                            newExpression->typeInfo->type.dimension = 0; // for error message
                            throw TypeError(
                                *this->path,
                                expr->get()->line,
                                expr->get()->column,
                                "cannot initialise an array element of type '" +
                                    typeToString(newExpression->typeInfo->type) +
                                    "' with type '" +
                                    typeToString(expr->get()->resultingType) +
                                    "'"
                            );
                        }
                    }
                }
            }
        }

        const auto resultingType = newExpression->typeInfo->type;
        newExpression->resultingType = resultingType;
        return SemanticExprResult{resultingType, ExprCategory::VALUE};
    }

    if (auto* castExpression = dynamic_cast<const ast::ExprCast*>(&expr)) {
        const auto exprTypeResult = this->checkExprType(scope, *castExpression->expr);

        // if target's type is an array
        // OR
        // if expr's type is an array
        // OR
        // if target's type is a bool
        // OR
        // if expr's type is a bool
        if (castExpression->typeInfo->type.isArray() ||
            exprTypeResult.type.isArray() ||
            exprTypeResult.type.type == Type::BOOL ||
            castExpression->typeInfo->type.type == Type::BOOL
        ) {
            throw TypeError(
                *this->path,
                castExpression->line,
                castExpression->column,
                "cannot cast from type '" +
                    typeToString(exprTypeResult.type) +
                    "' to type '" +
                    typeToString(castExpression->typeInfo->type) +
                    "'"
            );
        }

        const auto resultingType = castExpression->typeInfo->type;
        castExpression->resultingType = resultingType;
        return SemanticExprResult{resultingType, ExprCategory::VALUE};
    }

    if (auto* functionCall = dynamic_cast<const ast::FunctionCall*>(&expr)) {
        // process function call
        return this->processFunctionCall(scope, *functionCall);
    }
}

compiler::SemanticExprResult compiler::SemanticAnalyser::processFunctionCall(Scope* scope, const ast::FunctionCall& functionCall) {
    // process function call's arguments
    std::vector<SemanticType> argumentTypes;
    for (const auto& argument : functionCall.arguments) {
        argumentTypes.push_back(this->checkExprType(scope, *argument).type);
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

    return SemanticExprResult{
        functionSymbol->returnType,
        functionSymbol->returnType.isArray() ? ExprCategory::ASSIGNABLE : ExprCategory::VALUE
    };
}

compiler::FunctionSymbol *compiler::SemanticAnalyser::resolveFunctionCall(std::vector<FunctionSymbol> *functionSymbols, const ast::FunctionCall& functionCall, const std::vector<SemanticType>& argumentTypes) const {
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
                if (symbol.parameterTypes[i].type == argumentTypes[i].type &&
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

bool compiler::SemanticAnalyser::canImplicitlyConvert(const SemanticType& from, const SemanticType& to) {
    if (from.type == to.type && from.dimension == to.dimension) return true; // types are identical
    if (from.dimension != 0 || to.dimension != 0) return false; // return false as array types cannot be implicity converted

    switch (from.type) {
        case Type::INT:
            return to.type == Type::FLOAT;

        case Type::FLOAT:
            return false;

        case Type::BOOL:
            return false;

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

std::string compiler::SemanticAnalyser::typeToString(const SemanticType& type) {
    std::string result;
    switch (type.type) {
        case Type::VOID_RETURN_TYPE: result += "void"; break;
        case Type::INT: result += "int"; break;
        case Type::FLOAT: result += "float"; break;
        case Type::BOOL: result += "bool"; break;
    }
    for (int i = 0; i < type.dimension; i++) {
        result += "[]";
    }
    return result;
}

std::string compiler::SemanticAnalyser::binaryOperatorToString(const BinaryOperator &binaryOperator) {
    switch (binaryOperator) {
        case BinaryOperator::PLUS: return "+";
        case BinaryOperator::MINUS: return "-";
        case BinaryOperator::MULTIPLY: return "*";
        case BinaryOperator::DIVIDE: return "/";
        case BinaryOperator::INTEGER_DIVIDE: return "//";
        case BinaryOperator::MODULO: return "%";

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
        case UnaryOperator::LOGICAL_NOT: return "!";
        case UnaryOperator::INCREMENT: return "++";
        case UnaryOperator::DECREMENT: return "--";
    }
}

void compiler::SemanticAnalyser::throwTypeErrorFromBinaryOperator(const ast::ExprBinaryOperator& binaryOperator, const SemanticType& leftType, const SemanticType& rightType) const {
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

void compiler::SemanticAnalyser::checkConditionType(const SemanticType& conditionType, const size_t line, const size_t column) const {
    if (conditionType.isArray() ||
        conditionType.type != Type::BOOL
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

std::string compiler::SemanticAnalyser::functionSignatureToString(const std::string& functionIdentifier, const std::vector<SemanticType>& parameterTypes) {
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
