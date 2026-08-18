#include "SemanticAnalyser.h"

#include <algorithm>

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
    const std::vector<Type> parameterTypes = this->processParameterList(functionDecl.parameters);
    const std::string functionSignature = functionSignatureToString(functionDecl.identifier->name, parameterTypes);
    FunctionSymbol* functionSymbol = this->symbolTable->declareFunction(functionDecl.identifier->name, "$" + functionSignature, functionDecl.returnTypeInfo->type, parameterTypes);

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

std::vector<compiler::Type> compiler::SemanticAnalyser::processParameterList(const std::vector<std::unique_ptr<ast::Parameter>> &parameterList) {
    std::vector<Type> parameterTypes;

    for (auto& parameter : parameterList) {
        parameterTypes.push_back(parameter->typeInfo->type);
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
            this->path->string(),
            varDecl.line,
            varDecl.column,
            "variable '" + varDecl.identifier->name + "' is already defined"
        );
    }

    // declare symbol (as uninitialised)
    scope->declareSymbol(varDecl.identifier->name, varDecl.typeInfo->type, 0, false);

    // if var decl does not have an initialiser
    if (varDecl.optionalInitialiser == nullptr) {
        return SemanticAnalysisResult{false};
    }

    // check type of expr
    const auto initializerType = this->checkExprType(scope, *varDecl.optionalInitialiser);
    // set symbol as initialised after checking optional initialiser (prevents self initialisation)
    scope->lookup(varDecl.identifier->name).value()->isInitialised = true;

    if (!canImplicitlyConvert(initializerType, varDecl.typeInfo->type)) {
        throw TypeError(
            this->path->string(),
            varDecl.optionalInitialiser->line,
            varDecl.optionalInitialiser->column,
            "cannot assign " + typeToString(initializerType) + " to " + typeToString(varDecl.typeInfo->type)
        );
    }
    return SemanticAnalysisResult{false};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processAssignment(Scope* scope, const ast::StmAssignment& assignment) {
    const auto identifierSymbol = this->checkSymbolIsDefined(
        scope,
        assignment.varAccess->identifier->name,
        assignment.varAccess->identifier->line,
        assignment.varAccess->identifier->column
    );

    const auto exprType = this->checkExprType(scope, *assignment.expression);

    if (!canImplicitlyConvert(exprType, identifierSymbol->type)) {
        throw TypeError(
            this->path->string(),
            assignment.line,
            assignment.column,
            "cannot assign " + typeToString(exprType) + " to " + typeToString(identifierSymbol->type)
        );
    }

    // update isInitialised of identifier in symbol table
    identifierSymbol->isInitialised = true;
    return SemanticAnalysisResult{false};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processIfStatement(Scope *scope, const ast::IfStm &ifStm) {
    // ensure condition type is bool
    const auto conditionType = this->checkExprType(scope, *ifStm.condition);
    this->checkType({Type::BOOL}, conditionType, ifStm.condition->line, ifStm.condition->column);

    const bool ifBlockAlwaysReturns = this->processBlock(*ifStm.ifBlock, ScopeKind::BLOCK).alwaysReturns;

    if (ifStm.elseStm != nullptr) {
        const bool elseBlockAlwaysReturns = this->processStm(scope, *ifStm.elseStm).alwaysReturns;
        return SemanticAnalysisResult{ifBlockAlwaysReturns && elseBlockAlwaysReturns};
    }
    return SemanticAnalysisResult{false};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processWhileStatement(Scope* scope, const ast::WhileStm& whileStm) {
    // ensure condition type is bool
    const auto conditionType = this->checkExprType(scope, *whileStm.condition);
    this->checkType({Type::BOOL}, conditionType, whileStm.condition->line, whileStm.condition->column);

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
    std::vector<Type> argumentTypes;
    for (const auto& argument : functionCallStm.functionCall->arguments) {
        argumentTypes.push_back(this->checkExprType(scope, *argument));
    }

    std::vector<FunctionSymbol>* functionSymbols = this->symbolTable->getFunctionSymbols(functionCallStm.functionCall->identifier->name, argumentTypes);

    FunctionSymbol* functionSymbol = this->resolveFunctionCall(functionSymbols, *functionCallStm.functionCall, argumentTypes);

    // process function arguments
    this->processFunctionCall(functionSymbol, *functionCallStm.functionCall, argumentTypes);

    functionCallStm.functionCall->resultingType = functionSymbol->returnType;

    if (functionSymbol->returnType != Type::VOID_RETURN_TYPE) {
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
        if (currentFunctionSymbol->returnType != Type::VOID_RETURN_TYPE) {
            throw SemanticError(
                this->path->string(),
                returnStm.line,
                returnStm.column,
                "non-void function must return a value"
            );
        }
    } else {
        // return has expression

        const Type exprType = this->checkExprType(scope, *returnStm.returnExpression);

        if (currentFunctionSymbol->returnType == Type::VOID_RETURN_TYPE) { // function return type is void
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

compiler::Type compiler::SemanticAnalyser::checkExprType(Scope* scope, const ast::Expr& expr) {
    if (auto* intLit = dynamic_cast<const ast::ExprIntegerLiteral*>(&expr)) {
        intLit->resultingType = Type::INT;
        return Type::INT;
    }
    if (auto* floatLit = dynamic_cast<const ast::ExprFloatLiteral*>(&expr)) {
        floatLit->resultingType = Type::FLOAT;
        return Type::FLOAT;
    }
    if (auto* boolLit = dynamic_cast<const ast::ExprBoolLiteral*>(&expr)) {
        boolLit->resultingType = Type::BOOL;
        return Type::BOOL;
    }
    if (auto* exprIdent = dynamic_cast<const ast::ExprIdentifier*>(&expr)) {
        // check if symbol has been initialised
        const auto symbol = this->checkSymbolIsDefined(scope, exprIdent->name, exprIdent->line, exprIdent->column);
        if (!symbol->isInitialised) {
            throw SemanticError(
                this->path->string(),
                exprIdent->line,
                exprIdent->column,
                "variable '" + exprIdent->name + "' may not have been initialised"
            );
        }
        // return type of expr identifier
        expr.resultingType = symbol->type;
        return symbol->type;
    }

    if (auto* binaryOperator = dynamic_cast<const ast::ExprBinaryOperator*>(&expr)) {
        const Type leftType = this->checkExprType(scope, *binaryOperator->left);
        const Type rightType = this->checkExprType(scope, *binaryOperator->right);

        switch (binaryOperator->binaryOperatorInfo->binaryOperator) {
            case BinaryOperator::PLUS:
            case BinaryOperator::MINUS:
            case BinaryOperator::MULTIPLY:
            case BinaryOperator::DIVIDE:
            case BinaryOperator::MODULO: {
                // ensure either operand is not bool
                if (leftType == Type::BOOL || rightType == Type::BOOL) throwTypeErrorFromBinaryOperator(*binaryOperator, leftType, rightType);

                // result is float if either operand is a float or binary operator is divide
                if ((leftType == Type::FLOAT || rightType == Type::FLOAT) ||
                    binaryOperator->binaryOperatorInfo->binaryOperator == BinaryOperator::DIVIDE
                ) {
                    binaryOperator->resultingType = Type::FLOAT;
                    return Type::FLOAT;
                }
                binaryOperator->resultingType = Type::INT;
                return Type::INT;
            }

            case BinaryOperator::INTEGER_DIVIDE: {
                // ensure either operand is not bool
                if (leftType == Type::BOOL || rightType == Type::BOOL) throwTypeErrorFromBinaryOperator(*binaryOperator, leftType, rightType);
                binaryOperator->resultingType = Type::INT;
                return Type::INT;
            }

            case BinaryOperator::LOGICAL_OR:
            case BinaryOperator::LOGICAL_AND: {
                if (leftType != Type::BOOL || rightType != Type::BOOL) throwTypeErrorFromBinaryOperator(*binaryOperator, leftType, rightType);
                binaryOperator->resultingType = Type::BOOL;
                return Type::BOOL;
            }

            case BinaryOperator::LESS_THAN:
            case BinaryOperator::LESS_THAN_OR_EQUAL:
            case BinaryOperator::GREATER_THAN:
            case BinaryOperator::GREATER_THAN_OR_EQUAL: {
                if (leftType == Type::BOOL || rightType == Type::BOOL) throwTypeErrorFromBinaryOperator(*binaryOperator, leftType, rightType);
                binaryOperator->resultingType = Type::BOOL;
                return Type::BOOL;
            }

            default:
                // EQUAL_EQUAL / NOT_EQUAL

                // if either left or right type is bool and the other operand is not bool
                if ((leftType == Type::BOOL || rightType == Type::BOOL) &&
                    leftType != rightType
                ) {
                    throwTypeErrorFromBinaryOperator(*binaryOperator, leftType, rightType);
                }

                binaryOperator->resultingType = Type::BOOL;
                return Type::BOOL;
        }
    }
    if (auto* unaryOperator = dynamic_cast<const ast::ExprUnaryOperator*>(&expr)) {
        const Type type = this->checkExprType(scope, *unaryOperator->expr);

        if (type == Type::BOOL) {
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
                    "cannot apply '" + unaryOperatorToString(unaryOperator->unaryOperatorInfo->unaryOperator) + "' to type '" + typeToString(type) + "'"
                );
            }
        }

        unaryOperator->resultingType = type;
        return type;
    }

    if (auto* castExpression = dynamic_cast<const ast::ExprCast*>(&expr)) {
        const Type exprType = this->checkExprType(scope, *castExpression->expr);

        // if either source or target type is bool
        if (exprType == Type::BOOL || castExpression->typeInfo->type == Type::BOOL) {
            throw TypeError(
                this->path->string(),
                castExpression->line,
                castExpression->column,
                "cannot cast from '" + typeToString(exprType) + "' to '" + typeToString(castExpression->typeInfo->type) + "'"
            );
        }

        castExpression->resultingType = castExpression->typeInfo->type;
        return castExpression->typeInfo->type;
    }

    if (auto* functionCall = dynamic_cast<const ast::FunctionCall*>(&expr)) {
        std::vector<Type> argumentTypes;
        for (const auto& argument : functionCall->arguments) {
            argumentTypes.push_back(this->checkExprType(scope, *argument));
        }

        std::vector<FunctionSymbol>* functionSymbols = this->symbolTable->getFunctionSymbols(functionCall->identifier->name, argumentTypes);

        FunctionSymbol* functionSymbol = this->resolveFunctionCall(functionSymbols, *functionCall, argumentTypes);

        // process function
        this->processFunctionCall(functionSymbol, *functionCall, argumentTypes);

        functionCall->resultingType = functionSymbol->returnType;
        return functionSymbol->returnType;
    }
    throw std::runtime_error("Unknown expression type");
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processFunctionCall(FunctionSymbol* functionSymbol, const ast::FunctionCall &functionCall, const std::vector<Type>& argumentTypes) const {
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
                if (symbol.parameterTypes[i] == argumentTypes[i]) {
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

bool compiler::SemanticAnalyser::canImplicitlyConvert(const Type from, const Type to) {
    if (from == to) return true;

    switch (from) {
        case Type::INT:
            return to == Type::FLOAT;

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

std::string compiler::SemanticAnalyser::typeToString(const Type& type) {
    switch (type) {
        case Type::VOID_RETURN_TYPE: return "void";
        case Type::INT: return "int";
        case Type::FLOAT: return "float";
        case Type::BOOL: return "bool";
    }
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

void compiler::SemanticAnalyser::throwTypeErrorFromBinaryOperator(const ast::ExprBinaryOperator& binaryOperator, const Type leftType, const Type rightType) const {
    throw TypeError(
        this->path->string(),
        binaryOperator.line,
        binaryOperator.column,
        "cannot apply operator '" + binaryOperatorToString(binaryOperator.binaryOperatorInfo->binaryOperator) + "' to types '" + typeToString(leftType) + "' and '" + typeToString(rightType) + "'"
    );
}

void compiler::SemanticAnalyser::checkType(const std::vector<Type>& expectedTypes, const Type &actualType, const size_t line, const size_t column) const {
    if (std::count(expectedTypes.begin(), expectedTypes.end(), actualType) == 0) {

        std::string string;

        string += "Error: type mismatch";
        string += "\nExpected: ";
        for (Type expectedType : expectedTypes) {
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
}

std::string compiler::SemanticAnalyser::functionSignatureToString(const std::string& functionIdentifier, const std::vector<Type>& parameterTypes) {
    std::string result = "";

    result+= functionIdentifier + "(";

    if (parameterTypes.size() > 0) {
        result+= typeToString(parameterTypes.at(0));
    }

    for (int i = 1; i < parameterTypes.size(); i++) {
        result+= ",";
        result+= typeToString(parameterTypes.at(i));
    }

    result+= ")";
    return result;
}
