#include "SemanticAnalyser.h"

#include <algorithm>

#include "../include/Error.h"

compiler::SemanticAnalyser::SemanticAnalyser(SymbolTable* symbolTable, const std::filesystem::path* path) :
    symbolTable(symbolTable), path(path) {}

void compiler::SemanticAnalyser::processProgram(const ast::Program& program) {
    Scope* globalScope = this->symbolTable->enterScope(); // generate global scope
    // process global varDecls
    for (auto& stm : program.statements) {
        if (auto* varDecl = dynamic_cast<const ast::StmVarDecl*>(stm.get())) {
            this->declareGlobalStmVarDecl(globalScope, *varDecl);
        }
    }

    // process function declarations
    for (auto& functionDecl : program.functionDecls) {
        this->processFunctionDecl(*functionDecl);
    }

    // process function bodies
    for (auto& functionDecl : program.functionDecls) {
        // get FunctionSymbol
        std::vector<Type> parameterTypes = std::vector<Type>{};
        for (const auto& parameterType : functionDecl->parameters) {
            parameterTypes.push_back(parameterType->typeInfo->type);
        }
        auto functionSymbol = this->symbolTable->getFunctionSymbol(functionDecl->identifier->name, parameterTypes);

        const auto semanticAnalysisResult = this->processFunctionBody(*functionDecl, functionSymbol);

        // check function body always reaches returnStm if return type is non-void
        if (functionDecl->returnTypeInfo->type != Type::VOID &&
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

    // process statements
    for (auto& stm : program.statements) {
        this->processStm(globalScope, *stm);
    }
}

void compiler::SemanticAnalyser::declareGlobalStmVarDecl(Scope* scope, const ast::StmVarDecl& varDecl) {
    scope->declareSymbol(varDecl.identifier->name, varDecl.typeInfo->type, 0, false);
}

void compiler::SemanticAnalyser::processFunctionDecl(const ast::FunctionDecl& functionDecl) {
    // declare function in symbol table
    const std::vector<Type> parameterTypes = this->processParameterList(functionDecl.parameters);
    const bool validFunctionSignature = this->symbolTable->declareFunction(functionDecl.identifier->name, functionDecl.returnTypeInfo->type, parameterTypes);

    if (!validFunctionSignature) {
        std::string errorMsg = "function '";
        errorMsg += functionDecl.identifier->name + "(";

        if (functionDecl.parameters.size() > 0) {
            errorMsg += typeToString(functionDecl.parameters[0]->typeInfo->type);
        }

        for (int i = 1; i < functionDecl.parameters.size(); i++) {
            errorMsg += ", ";
            errorMsg += typeToString(functionDecl.parameters[i]->typeInfo->type);
        }

        errorMsg += ")' is already defined";

        throw SemanticError(
            this->path->string(),
            functionDecl.line,
            functionDecl.column,
            errorMsg
        );
    }

    // generate function label
    std::string functionLabel = "$" + functionDecl.identifier->name;
    functionLabel += "(";
    if (parameterTypes.size() > 0) {
        functionLabel += typeToString(parameterTypes[0]);
    }
    for (int i = 1; i < parameterTypes.size(); i++) {
        functionLabel+= ",";
        functionLabel += typeToString(parameterTypes[i]);
    }
    functionLabel += ")";
    // set label of functionSymbol
    FunctionSymbol* functionSymbol = this->symbolTable->getFunctionSymbol(functionDecl.identifier->name, parameterTypes);
    functionSymbol->label = functionLabel;
}

std::vector<compiler::Type> compiler::SemanticAnalyser::processParameterList(const std::vector<std::unique_ptr<ast::Parameter> > &parameterList) {
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
        return this->processBlock(*block);
    }
    if (auto* ifStm = dynamic_cast<const ast::IfStm*>(&stm)) {
        return this->processIfStatement(scope, *ifStm);
    }
    if (auto* whileStm = dynamic_cast<const ast::WhileStm*>(&stm)) {
        return this->processWhileStatement(scope, *whileStm);
    }
    if (auto* functionCallStm = dynamic_cast<const ast::FunctionCallStm*>(&stm)) {
        return this->processFunctionCallStatement(scope, *functionCallStm);
    }
    if (auto* returnStm = dynamic_cast<const ast::ReturnStm*>(&stm)) {
        return this->processReturnStatement(scope, *returnStm);
    }
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processBlock(const ast::Block& block) {
    Scope* newScope = this->symbolTable->enterScope();
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
    // if scope is global -> symbol has already been declared on first pass of semantic analysis
    if (!scope->isGlobalScope()) {
        // declare symbol (as uninitialised)
        scope->declareSymbol(varDecl.identifier->name, varDecl.typeInfo->type, 0, false);
    }

    // if var decl does not have an initialiser
    if (varDecl.optionalInitialiser == nullptr) {
        return SemanticAnalysisResult{false};
    }

    // check type of expr
    const auto initializerType = this->checkExprType(scope, *varDecl.optionalInitialiser);
    // set symbol as initialised after checking optional initialiser (prevents self initialisation)
    scope->lookup(varDecl.identifier->name).value()->isInitialised = true;

    if (initializerType != varDecl.typeInfo->type) {
        throw TypeError(
            this->path->string(),
            varDecl.optionalInitialiser->line,
            varDecl.optionalInitialiser->column,
            "cannot initialise '" + varDecl.identifier->name + "' with expression of different type."
        );
    }
    return SemanticAnalysisResult{false};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processAssignment(Scope* scope, const ast::StmAssignment& assignment) {
    auto identifierSymbol = this->checkSymbolIsDefined(
        scope,
        assignment.varAccess->identifier->name,
        assignment.varAccess->identifier->line,
        assignment.varAccess->identifier->column
    );

    const auto exprType = this->checkExprType(scope, *assignment.expression);

    if (identifierSymbol->type != exprType) {
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

    const bool ifBlockAlwaysReturns = this->processBlock(*ifStm.ifBlock).alwaysReturns;

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

    this->processBlock(*whileStm.block);
    return SemanticAnalysisResult{false};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processFunctionCallStatement(Scope *scope, const ast::FunctionCallStm &functionCallStm) {
    this->checkExprType(scope, *functionCallStm.functionCall);
    return SemanticAnalysisResult{false};
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processReturnStatement(Scope *scope, const ast::ReturnStm &returnStm) {
    const auto currentFunctionSymbol = this->symbolTable->getCurrentFunctionSymbol();
    if (currentFunctionSymbol == nullptr) {
        throw SemanticError(
            this->path->string(),
            returnStm.line,
            returnStm.column,
            "return statement exists outside of a function"
        );
    }

    if (returnStm.returnExpression == nullptr) { // return has no expression
        if (currentFunctionSymbol->returnType != Type::VOID) {
            throw SemanticError(
                this->path->string(),
                returnStm.line,
                returnStm.column,
                "non-void function must return a value"
            );
        }
    } else {
        // return has expression

        const Type type = this->checkExprType(scope, *returnStm.returnExpression);

        if (currentFunctionSymbol->returnType == Type::VOID) { // function return type is void
            if (returnStm.returnExpression != nullptr) { // return has an expression
                throw SemanticError(
                    this->path->string(),
                    returnStm.line,
                    returnStm.column,
                    "cannot return an expression from a void function"
                );
            }
        }

        if (type != currentFunctionSymbol->returnType) {
            throw TypeError(
                this->path->string(),
                returnStm.line,
                returnStm.column,
                "return type mismatch: expected '" + typeToString(currentFunctionSymbol->returnType) + "', got '" + typeToString(type) + "'"
            );
        }
    }

    return SemanticAnalysisResult{true};
}

compiler::Type compiler::SemanticAnalyser::checkExprType(Scope* scope, const ast::Expr& expr) {
    if (auto* intLit = dynamic_cast<const ast::ExprIntegerLiteral*>(&expr)) {
        return Type::INT;
    }
    if (auto* floatLit = dynamic_cast<const ast::ExprFloatLiteral*>(&expr)) {
        return Type::FLOAT;
    }
    if (auto* boolLit = dynamic_cast<const ast::ExprBoolLiteral*>(&expr)) {
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
        return symbol->type;
    }

    if (auto* binaryOperator = dynamic_cast<const ast::ExprBinaryOperator*>(&expr)) {
        const Type leftType = this->checkExprType(scope, *binaryOperator->left);
        const Type rightType = this->checkExprType(scope, *binaryOperator->right);

        if (leftType != rightType) {
            throw TypeError(
                this->path->string(),
                binaryOperator->line,
                binaryOperator->column,
                "cannot apply operator '" + binaryOperatorToString(binaryOperator->binaryOperatorInfo->binaryOperator) +
                "' to types " + typeToString(leftType) + " and " + typeToString(rightType)
            );
        }

        switch (binaryOperator->binaryOperatorInfo->binaryOperator) {
            case BinaryOperator::PLUS:
            case BinaryOperator::MINUS:
            case BinaryOperator::MULTIPLY:
            case BinaryOperator::DIVIDE:
            case BinaryOperator::MODULO:
                return leftType;

            default:
                return Type::BOOL;
        }
    }
    if (auto* unaryOperator = dynamic_cast<const ast::ExprUnaryOperator*>(&expr)) {
        return this->checkExprType(scope, *unaryOperator->expr);
    }
    if (auto* functionCall = dynamic_cast<const ast::FunctionCall*>(&expr)) {
        std::vector<Type> argumentTypes;
        for (const auto& argument : functionCall->arguments) {
            argumentTypes.push_back(this->checkExprType(scope, *argument));
        }

        FunctionSymbol* functionSymbol = this->symbolTable->getFunctionSymbol(functionCall->identifier->name, argumentTypes);

        // process function arguments
        this->processFunctionCall(functionSymbol, *functionCall, argumentTypes);

        return functionSymbol->returnType;
    }
    throw std::runtime_error("Unknown expression type");
}

compiler::SemanticAnalysisResult compiler::SemanticAnalyser::processFunctionCall(FunctionSymbol* functionSymbol, const ast::FunctionCall &functionCall, const std::vector<Type>& argumentTypes) const {
    if (functionSymbol == nullptr) { // if function is not defined
        std::string errorMsg = "function '";
        errorMsg += functionCall.identifier->name + "(";

        if (argumentTypes.size() > 0) {
            errorMsg += typeToString(argumentTypes.at(0));
        }

        for (int argumentIndex = 1; argumentIndex < argumentTypes.size(); argumentIndex++) {
            errorMsg += ", ";
            errorMsg += typeToString(argumentTypes.at(argumentIndex));
        }
        errorMsg += ")' is undefined";

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

compiler::Symbol* compiler::SemanticAnalyser::checkSymbolIsDefined(Scope* scope, const std::string& identifier, const size_t line, const size_t column) const {
    auto symbol = scope->lookup(identifier);
    if (!symbol.has_value()) {
        throw TypeError(
            this->path->string(),
            line,
            column,
            "undefined label '" + identifier + "'"
        );
    }
    return symbol.value();
}

std::string compiler::SemanticAnalyser::typeToString(const Type& type) {
    switch (type) {
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


