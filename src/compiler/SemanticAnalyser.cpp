#include "SemanticAnalyser.h"

#include "../include/Error.h"

compiler::SemanticAnalyser::SemanticAnalyser(SymbolTable* symbolTable, const std::filesystem::path* path) :
    symbolTable(symbolTable), path(path) {}

void compiler::SemanticAnalyser::processProgram(const ast::Program& program) {
    Scope* globalScope = this->symbolTable->enterScope(); // generate global scope
    for (auto& stm : program.statements) {
        this->processStm(globalScope, *stm);
    }
}

void compiler::SemanticAnalyser::processStm(Scope* scope, const ast::Stm& stm) {
    if (auto* varDecl = dynamic_cast<const ast::StmVarDecl*>(&stm)) {
        this->processStmVarDecl(scope, *varDecl);
    }
    else if (auto* assignment = dynamic_cast<const ast::StmAssignment*>(&stm)) {
        this->processAssignment(scope, *assignment);
    }
    else if (auto* block = dynamic_cast<const ast::Block*>(&stm)) {
        this->processBlock(*block);
    }
    else if (auto* ifStm = dynamic_cast<const ast::IfStm*>(&stm)) {
        this->processIfStatement(scope, *ifStm);
    }
}

void compiler::SemanticAnalyser::processBlock(const ast::Block& block) {
    const auto newScope = this->symbolTable->enterScope();
    block.scope = newScope;

    for (auto& stm : block.statements) {
        this->processStm(block.scope, *stm);
    }
    this->symbolTable->leaveScope();
}

void compiler::SemanticAnalyser::processStmVarDecl(Scope* scope, const ast::StmVarDecl& varDecl) {
    if (varDecl.optionalInitialiser == nullptr) {
       scope->declareSymbol(varDecl.identifier.name, varDecl.typeInfo.type, 0, false);
        return;
    }
    // declare global (as uninitialised)
    scope->declareSymbol(varDecl.identifier.name, varDecl.typeInfo.type, 0, false);
    // check type of expr
    const auto initializerType = this->checkExprType(scope, *varDecl.optionalInitialiser);
    // set symbol as initialised after checking optional initialiser (prevents self initialisation)
    scope->lookup(varDecl.identifier.name).value()->isInitialised = true;

    if (initializerType != varDecl.typeInfo.type) {
        throw TypeError(
            this->path->string(),
            varDecl.optionalInitialiser->line,
            varDecl.optionalInitialiser->column,
            "cannot initialise '" + varDecl.identifier.name + "' with expression of different type."
        );
    }
}

void compiler::SemanticAnalyser::processAssignment(Scope* scope, const ast::StmAssignment& assignment) {
    auto identifierSymbol = this->checkSymbolIsDefined(
        scope,
        assignment.varAccess->identifier.name,
        assignment.varAccess->identifier.line,
        assignment.varAccess->identifier.column
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
}

void compiler::SemanticAnalyser::processIfStatement(Scope *scope, const ast::IfStm &ifStm) {
    const auto conditionType = this->checkExprType(scope, *ifStm.condition);

    // ensure condition type is bool
    if (conditionType != Type::BOOL) {
        throw TypeError(
            this->path->string(),
            ifStm.condition->line,
            ifStm.condition->column,
            "Expecting type " + typeToString(Type::BOOL) + " but found " + typeToString(conditionType)
        );
    }

    this->processBlock(*ifStm.ifBlock);
    if (ifStm.condition != nullptr) {
        this->processStm(scope, *ifStm.elseStm);
    }
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
        return leftType;
    }
    if (auto* unaryOperator = dynamic_cast<const ast::ExprUnaryOperator*>(&expr)) {
        return this->checkExprType(scope, *unaryOperator->expr);
    }
    throw std::runtime_error("Unknown expression type");
}

compiler::Symbol* compiler::SemanticAnalyser::checkSymbolIsDefined(Scope* scope, const std::string& identifier, const size_t line, const size_t column) {
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
        case Type::INT: return "integer";
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

