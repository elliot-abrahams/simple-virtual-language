#include "SemanticAnalyser.h"

#include <algorithm>
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
        // get FunctionSymbol
        std::vector<Type> parameterTypes = std::vector<Type>{};
        for (const auto& parameterType : functionDecl->parameters) {
            parameterTypes.push_back(parameterType->typeInfo->type);
        }

        const auto semanticAnalysisResult = this->processFunctionBody(*functionDecl, functionDecl->functionSymbol);

        // check function body always reaches returnStm if return type is non-void
        if (functionDecl->returnTypeInfo->type != Type::VOID_RETURN_TYPE &&
            !semanticAnalysisResult.alwaysReturns
        ) {
            throw SemanticError(
                this->path->string(),
                functionDecl->line,
                functionDecl->column,
                "function '" + functionDecl->identifier->name + "'" + "may not return a value on all paths"
            );
        }
    }
}

void compiler::SemanticAnalyser::processFunctionDecl(const ast::FunctionDecl& functionDecl) {
    // declare function in symbol table
    const std::vector<SemanticType> parameterTypes = this->processParameterList(functionDecl.parameters);
    const std::string functionSignature = functionSignatureToString(functionDecl.identifier->name, parameterTypes);
    FunctionSymbol* functionSymbol = this->symbolTable->declareFunction(functionDecl.identifier->name, functionSignature, SemanticType{functionDecl.returnTypeInfo->type, functionDecl.returnTypeInfo->dimension}, parameterTypes);

    if (functionSymbol == nullptr) {
        std::string errorMsg = "function '";
        errorMsg += functionSignature;
        errorMsg += "' is already defined";

        throw SemanticError(
            this->path->string(),
            functionDecl.line,
            functionDecl.column,
            errorMsg
        );
    }

    // set symbol of functionDecl
    functionDecl.functionSymbol = functionSymbol;
}

std::vector<compiler::SemanticType> compiler::SemanticAnalyser::processParameterList(const std::vector<std::unique_ptr<ast::Parameter>> &parameterList) {
    std::vector<SemanticType> parameterTypes;

    for (auto& parameter : parameterList) {
        parameterTypes.push_back(SemanticType{parameter->typeInfo->type, parameter->typeInfo->dimension});
    }
    return parameterTypes;
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
    if (auto* functionCallStm = dynamic_cast<const ast::FunctionCallStm*>(&stm)) {
        return this->processFunctionCallStatement(scope, *functionCallStm);
    }
    if (auto* returnStm = dynamic_cast<const ast::ReturnStm*>(&stm)) {
        return this->processReturnStatement(scope, *returnStm);
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
        newScope->declareSymbol(functionDecl.parameters[parameterIndex]->identifier->name, SemanticType{functionDecl.parameters[parameterIndex]->typeInfo->type, functionDecl.parameters[parameterIndex]->typeInfo->dimension}, parameterIndex + 1, true);
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
            this->path->string(),
            varDecl.line,
            varDecl.column,
            "variable '" + varDecl.identifier->name + "' is already defined"
        );
    }

    // declare symbol (as uninitialised)
    scope->declareSymbol(varDecl.identifier->name, SemanticType{varDecl.typeInfo->type, varDecl.typeInfo->dimension}, 0, false);

    // if var decl does not have an initialiser
    if (varDecl.optionalInitialiser == nullptr) {
        return SemanticAnalysisResult{false};
    }

    // check type of expr
    const auto initializerType = this->checkExprType(scope, *varDecl.optionalInitialiser).type;
    // set symbol as initialised after checking optional initialiser (prevents self initialisation)
    scope->lookup(varDecl.identifier->name).value()->isInitialised = true;

    if (!canImplicitlyConvert(initializerType, SemanticType{varDecl.typeInfo->type, varDecl.typeInfo->dimension})) {
        throw TypeError(
            this->path->string(),
            varDecl.optionalInitialiser->line,
            varDecl.optionalInitialiser->column,
            "cannot assign " + typeToString(initializerType) + " to " + typeToString(SemanticType{varDecl.typeInfo->type, varDecl.typeInfo->dimension})
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
                this->path->string(),
                assignment.identifier->line,
                assignment.identifier->column,
                "variable '" + assignment.identifier->name + "' may not have been initialised"
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
            this->path->string(),
            assignment.line,
            assignment.column,
            "cannot assign " + typeToString(exprType) + " to " + typeToString(SemanticType{identifierSymbol->type, identifierSymbol->dimension})
        );
    }

    // update isInitialised of identifier in symbol table
    if (!identifierSymbol->isInitialised) identifierSymbol->isInitialised = true;
    return SemanticAnalysisResult{false};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processIfStatement(Scope *scope, const ast::IfStm &ifStm) {
    // ensure condition type is bool
    const auto conditionType = this->checkExprType(scope, *ifStm.condition).type;
    this->checkType({SemanticType{Type::BOOL, 0}}, conditionType, ifStm.condition->line, ifStm.condition->column);

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
    this->checkType({SemanticType{Type::BOOL, 0}}, conditionType, whileStm.condition->line, whileStm.condition->column);

    this->processBlock(*whileStm.block, ScopeKind::WHILE);
    return SemanticAnalysisResult{false};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processContinueStatement(Scope* scope, const ast::ContinueStm &continueStm) {
    const auto loopScope = scope->lookupWhileScope();

    if (loopScope == nullptr) {
        throw SemanticError(
            this->path->string(),
            continueStm.line,
            continueStm.column,
            "'continue' statement can only be used within a loop"
        );
    }

    return SemanticAnalysisResult{false};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processBreakStatement(Scope* scope, const ast::BreakStm &breakStm) {
    const auto loopScope = scope->lookupWhileScope();

    if (loopScope == nullptr) {
        throw SemanticError(
            this->path->string(),
            breakStm.line,
            breakStm.column,
            "'break' statement can only be used within a loop"
        );
    }

    return SemanticAnalysisResult{false};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processFunctionCallStatement(Scope *scope, const ast::FunctionCallStm &functionCallStm) {
    std::vector<SemanticType> argumentTypes;
    for (const auto& argument : functionCallStm.functionCall->arguments) {
        argumentTypes.push_back(this->checkExprType(scope, *argument).type);
    }

    std::vector<FunctionSymbol>* functionSymbols = this->symbolTable->getFunctionSymbols(functionCallStm.functionCall->identifier->name);

    FunctionSymbol* functionSymbol = this->resolveFunctionCall(functionSymbols, *functionCallStm.functionCall, argumentTypes);

    // process function arguments
    this->processFunctionCall(functionSymbol, *functionCallStm.functionCall, argumentTypes);

    functionCallStm.functionCall->resultingType = functionSymbol->returnType;

    if (functionSymbol->returnType.type != Type::VOID_RETURN_TYPE) {
        throw SemanticError(
            this->path->string(),
            functionCallStm.line,
            functionCallStm.column,
            "return value of function '" + functionSignatureToString(functionCallStm.functionCall->identifier->name, argumentTypes) + "' must be used"
        );
    }

    return SemanticAnalysisResult{false};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processReturnStatement(Scope *scope, const ast::ReturnStm &returnStm) {
    auto currentFunctionSymbol = this->symbolTable->getCurrentFunctionSymbol();
    if (currentFunctionSymbol == nullptr) {
        throw SemanticError(
            this->path->string(),
            returnStm.line,
            returnStm.column,
            "return statement exists outside of a function"
        );
    }

    returnStm.functionSymbol = currentFunctionSymbol;

    if (returnStm.returnExpression == nullptr) { // return has no expression
        if (currentFunctionSymbol->returnType.type != Type::VOID_RETURN_TYPE) {
            throw SemanticError(
                this->path->string(),
                returnStm.line,
                returnStm.column,
                "non-void function must return a value"
            );
        }
    } else {
        // return has expression

        const auto exprType = this->checkExprType(scope, *returnStm.returnExpression).type;

        if (currentFunctionSymbol->returnType.type == Type::VOID_RETURN_TYPE) { // function return type is void
            if (returnStm.returnExpression != nullptr) { // return has an expression
                throw SemanticError(
                    this->path->string(),
                    returnStm.line,
                    returnStm.column,
                    "cannot return an expression from a void function"
                );
            }
        }

        if (!canImplicitlyConvert(exprType, currentFunctionSymbol->returnType)) {
            throw TypeError(
                this->path->string(),
                returnStm.line,
                returnStm.column,
                "return type mismatch: expected '" + typeToString(currentFunctionSymbol->returnType) + "', got '" + typeToString(exprType) + "'"
            );
        }
    }

    return SemanticAnalysisResult{true};
}

void compiler::SemanticAnalyser::processIndex(Scope* scope, const ast::Index& index) {
    const auto indexType = this->checkExprType(scope, *index.index).type;

    if (indexType.type != Type::INT) {
        throw TypeError(
            this->path->string(),
            index.index->line,
            index.index->column,
            "array index must have type int, but found " + typeToString(indexType)
        );
    }
}

unsigned int compiler::SemanticAnalyser::resolveAccessArrayDepth(const SemanticType& arrayType, const std::vector<std::unique_ptr<ast::Index>>& indices) const {
    if (indices.size() > arrayType.dimension) {
        throw TypeError(
            this->path->string(),
            indices[indices.size() - arrayType.dimension - 1]->line,
            indices[indices.size() - arrayType.dimension - 1]->column,
            "array required, but " + typeToString(SemanticType{arrayType.type, 0}) + " found"
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

        if (exprTypeResult.type.type == Type::BOOL) {
            if (unaryOperator->unaryOperatorInfo->unaryOperator == UnaryOperator::MINUS ||
                unaryOperator->unaryOperatorInfo->unaryOperator == UnaryOperator::PLUS) {

                throw TypeError(
                    this->path->string(),
                    unaryOperator->line,
                    unaryOperator->column,
                    "cannot apply '" + unaryOperatorToString(unaryOperator->unaryOperatorInfo->unaryOperator) + "' to type 'bool'"
                );
            }
        } else { // expr is not of type bool
            if (unaryOperator->unaryOperatorInfo->unaryOperator == UnaryOperator::LOGICAL_NOT) {
                throw TypeError(
                    this->path->string(),
                    unaryOperator->line,
                    unaryOperator->column,
                    "cannot apply '" + unaryOperatorToString(unaryOperator->unaryOperatorInfo->unaryOperator) + "' to type '" + typeToString(exprTypeResult.type) + "'"
                );
            }
        }

        unaryOperator->resultingType = exprTypeResult.type;
        return exprTypeResult;
    }

    if (auto* exprPostfix = dynamic_cast<const ast::ExprPostfix*>(&expr)) {
        auto exprTypeResult = this->checkExprType(scope, *exprPostfix->expression);

        if (exprPostfix->indices.size() > 0) {

            if (exprTypeResult.type.dimension == 0) {
                throw TypeError(
                    this->path->string(),
                    exprPostfix->line,
                    exprPostfix->column,
                    "array required, but " + typeToString(exprTypeResult.type) + " found"
                );
            }

            if (exprPostfix->indices.size() > exprTypeResult.type.dimension) {
                exprTypeResult.type.dimension = 0; // for error message
                throw TypeError(
                    this->path->string(),
                    exprPostfix->line,
                    exprPostfix->column,
                    "array required, but " + typeToString(exprTypeResult.type) + " found"
                );
            }

            exprTypeResult.type.dimension -= exprPostfix->indices.size();

            // process indices
            for (const auto& index : exprPostfix->indices) {
                this->processIndex(scope, *index);
            }
        }
        if (exprPostfix->incDecOperator.has_value()) {
            // throw error if
            // expr type is not numeric
            // OR
            // expression is not assignable

            if (exprTypeResult.category != ExprCategory::ASSIGNABLE || !exprTypeResult.type.isNumeric()) {
                std::string errorMessage = "cannot apply operator '" + incrementDecrementOperatorToString(exprPostfix->incDecOperator.value()->incDecOperator) + "' ";
                errorMessage += exprTypeResult.category != ExprCategory::ASSIGNABLE || exprTypeResult.type.dimension > 0 ? "to a non-assignable expression" : "to type '" + typeToString(exprPostfix->expression->resultingType) + "'";

                throw TypeError(
                    this->path->string(),
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
                !exprPostfix->incDecOperator.has_value() &&
                exprTypeResult.type.dimension == 0
            ? ExprCategory::ASSIGNABLE : ExprCategory::VALUE
        };
    }

    if (auto* exprIdentifier = dynamic_cast<const ast::ExprIdentifier*>(&expr)) {
        // check if symbol has been initialised
        const auto symbol = this->checkSymbolIsDefined(scope, exprIdentifier->identifier->name, exprIdentifier->line, exprIdentifier->column);
        if (!symbol->isInitialised) {
            throw SemanticError(
                this->path->string(),
                exprIdentifier->line,
                exprIdentifier->column,
                "variable '" + exprIdentifier->identifier->name + "' may not have been initialised"
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
                        if (std::holds_alternative<std::unique_ptr<ast::Expr>>(element)) {
                            const auto expr = &std::get<std::unique_ptr<ast::Expr>>(element);
                            unsigned int initialiserDepth = newExpression->arrayDimensions.size() - depth;
                            throw TypeError(
                                this->path->string(),
                                expr->get()->line,
                                expr->get()->column,
                                "expected '" + typeToString(SemanticType{newExpression->typeInfo->type, initialiserDepth}) + "' but found '" + typeToString(SemanticType{newExpression->typeInfo->type, 0}) + "'"
                            );
                        }
                        initialisersToProcess.push(&std::get<std::unique_ptr<ast::ArrayInitialiser>>(element));
                        depths.push(depth + 1);
                    }
                } else {
                    // expecting each element to be expr which can be converted to array base type
                    for (const ast::ArrayInitialiserElement& element : arrayInitialiser->get()->elements) {
                        if (std::holds_alternative<std::unique_ptr<ast::ArrayInitialiser>>(element)) {
                            const auto initialiser = &std::get<std::unique_ptr<ast::ArrayInitialiser>>(element);
                            throw TypeError(
                                this->path->string(),
                                initialiser->get()->line,
                                initialiser->get()->column,
                                "array initialiser exceeds array dimensions"
                            );
                        }
                        // check expr is valid for array type
                        const auto expr = &std::get<std::unique_ptr<ast::Expr>>(element);
                        if (!canImplicitlyConvert(this->checkExprType(scope, *expr->get()).type, SemanticType{newExpression->typeInfo->type, 0})) {
                            throw TypeError(
                                this->path->string(),
                                expr->get()->line,
                                expr->get()->column,
                                "expected '" + typeToString(SemanticType{newExpression->typeInfo->type, 0}) + "' but found '" + typeToString(expr->get()->resultingType) + "'"
                            );
                        }
                    }
                }
            }
        }

        const auto resultingType = SemanticType{newExpression->typeInfo->type, newExpression->typeInfo->dimension};
        newExpression->resultingType = resultingType;
        return SemanticExprResult{resultingType, ExprCategory::VALUE};
    }

    if (auto* castExpression = dynamic_cast<const ast::ExprCast*>(&expr)) {
        const auto exprTypeResult = this->checkExprType(scope, *castExpression->expr);

        // if target's dimension is larger than zero
        if (castExpression->typeInfo->dimension > 0) {
            throw TypeError(
                this->path->string(),
                castExpression->typeInfo->line,
                castExpression->typeInfo->column,
                "cannot cast to array type '" + typeToString(SemanticType{castExpression->typeInfo->type, castExpression->typeInfo->dimension}) + "'"
            );
        }

        // if expr's dimension is larger than zero
        if (exprTypeResult.type.dimension > 0) {
            throw TypeError(
                this->path->string(),
                castExpression->typeInfo->line,
                castExpression->typeInfo->column,
                "cannot cast from array type '" + typeToString(SemanticType{castExpression->typeInfo->type, castExpression->typeInfo->dimension}) + "'"
            );
        }

        // if either source or target type is bool
        if (exprTypeResult.type.type == Type::BOOL || castExpression->typeInfo->type == Type::BOOL) {
            throw TypeError(
                this->path->string(),
                castExpression->line,
                castExpression->column,
                "cannot cast from '" + typeToString(exprTypeResult.type) + "' to '" + typeToString(SemanticType{castExpression->typeInfo->type, castExpression->typeInfo->dimension}) + "'"
            );
        }

        const auto resultingType = SemanticType{castExpression->typeInfo->type, 0};
        castExpression->resultingType = resultingType;
        return SemanticExprResult{resultingType, ExprCategory::VALUE};
    }

    if (auto* functionCall = dynamic_cast<const ast::FunctionCall*>(&expr)) {
        std::vector<SemanticType> argumentTypes;
        for (const auto& argument : functionCall->arguments) {
            argumentTypes.push_back(this->checkExprType(scope, *argument).type);
        }

        FunctionSymbol* functionSymbol = this->resolveFunctionCall(this->symbolTable->getFunctionSymbols(functionCall->identifier->name), *functionCall, argumentTypes);

        // process function
        this->processFunctionCall(functionSymbol, *functionCall, argumentTypes);

        functionCall->resultingType = functionSymbol->returnType;
        return SemanticExprResult{
            functionSymbol->returnType,
            functionSymbol->returnType.dimension > 0 ? ExprCategory::ASSIGNABLE : ExprCategory::VALUE
        };
    }
    throw std::runtime_error("Unknown expression type");
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processFunctionCall(FunctionSymbol* functionSymbol, const ast::FunctionCall &functionCall, const std::vector<SemanticType>& argumentTypes) const {
    if (functionSymbol == nullptr) { // if function is not defined
        std::string errorMsg = "function '";
        errorMsg += functionSignatureToString(functionCall.identifier->name, argumentTypes);
        errorMsg += "' is undefined";

        throw SemanticError(
            this->path->string(),
            functionCall.line,
            functionCall.column,
            errorMsg
        );
    }

    // set functionSymbol in FunctionCall ast for code gen
    functionCall.functionSymbol = functionSymbol;

    return SemanticAnalysisResult{false};
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
                this->path->string(),
                functionCall.line,
                functionCall.column,
                "ambiguous function call to '" + functionCall.identifier->name + "'"
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
            this->path->string(),
            line,
            column,
            "undefined identifier '" + identifier + "'"
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
    }
}

std::string compiler::SemanticAnalyser::incrementDecrementOperatorToString(const IncrementDecrementOperator &incDecOperator) {
    switch (incDecOperator) {
        case IncrementDecrementOperator::INCREMENT: return "++";
        case IncrementDecrementOperator::DECREMENT: return "--";
    }
}

void compiler::SemanticAnalyser::throwTypeErrorFromBinaryOperator(const ast::ExprBinaryOperator& binaryOperator, const SemanticType& leftType, const SemanticType& rightType) const {
    throw TypeError(
        this->path->string(),
        binaryOperator.line,
        binaryOperator.column,
        "cannot apply operator '" + binaryOperatorToString(binaryOperator.binaryOperatorInfo->binaryOperator) + "' to types '" + typeToString(leftType) + "' and '" + typeToString(rightType) + "'"
    );
}

void compiler::SemanticAnalyser::checkType(const std::vector<SemanticType>& expectedTypes, const SemanticType &actualType, const size_t line, const size_t column) const {
    for (const auto type : expectedTypes) {
        if (type.type == actualType.type && type.dimension == actualType.dimension) {
            return;
        }
    }

    std::string string;

    string += "Error: type mismatch";
    string += "\nExpected: ";
    for (SemanticType expectedType : expectedTypes) {
        string += typeToString(expectedType)  + " ";
    }
    string += "\nActual: " + typeToString(actualType);

    throw TypeError(
        this->path->string(),
        line,
        column,
        string
    );
}

std::string compiler::SemanticAnalyser::functionSignatureToString(const std::string& functionIdentifier, const std::vector<SemanticType>& parameterTypes) {
    std::string result = "";

    result+= functionIdentifier + "(";

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
