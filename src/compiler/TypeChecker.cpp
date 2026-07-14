#include "TypeChecker.h"

#include "../include/Error.h"

compiler::TypeChecker::TypeChecker(SymbolTable* symbolTable, const std::filesystem::path* path) :
    symbolTable(symbolTable), path(path) {}

void compiler::TypeChecker::processProgram(const ast::Program& program) {
    for (auto& stm : program.statements) {
        this->processStm(*stm);
    }
}

void compiler::TypeChecker::processStm(const ast::Stm& stm) {
    if (auto* varDecl = dynamic_cast<const ast::StmVarDecl*>(&stm)) {
        this->processStmVarDecl(*varDecl);
    }
    if (auto* assignment = dynamic_cast<const ast::StmAssignment*>(&stm)) {
        this->processAssignment(*assignment);
    }
}

void compiler::TypeChecker::processStmVarDecl(const ast::StmVarDecl& varDecl) const {
    if (varDecl.optionalInitialiser == nullptr) {
        this->symbolTable->declareGlobalVariable(varDecl.identifier.name, varDecl.typeInfo.type, false);
        return;
    }
    // declare global (as uninitialised)
    this->symbolTable->declareGlobalVariable(varDecl.identifier.name, varDecl.typeInfo.type, false);
    // check type of expr
    const auto initializerType = this->checkExprType(*varDecl.optionalInitialiser);
    // set symbol as initialised after checking optional initialiser (prevents self initialisation)
    this->symbolTable->getGlobalVariables().at(varDecl.identifier.name).isInitialised = true;

    if (initializerType != varDecl.typeInfo.type) {
        throw TypeError(
            this->path->string(),
            varDecl.optionalInitialiser->line,
            varDecl.optionalInitialiser->column,
            "cannot initialise '" + varDecl.identifier.name + "' with expression of different type."
        );
    }
}

void compiler::TypeChecker::processAssignment(const ast::StmAssignment& assignment) {
    auto& identifierSymbol = this->symbolTable->getGlobalVariables().at(assignment.varAccess->identifier.name);
    const auto exprType = this->checkExprType(*assignment.expression);

    if (identifierSymbol.type != exprType) {
        throw TypeError(
            this->path->string(),
            assignment.line,
            assignment.column,
            "cannot assign " + typeToString(exprType) + " to " + typeToString(identifierSymbol.type)
        );
    }

    // update isInitialised of identifier in symbol table
    identifierSymbol.isInitialised = true;
}

ast::Type compiler::TypeChecker::checkExprType(const ast::Expr& expr) const {
    if (auto* intLit = dynamic_cast<const ast::ExprIntegerLiteral*>(&expr)) {
        return ast::Type::INT;
    }
    if (auto* exprIdent = dynamic_cast<const ast::ExprIdentifier*>(&expr)) {
        // check if symbol has been initialised
        if (!this->symbolTable->getGlobalVariables().at(exprIdent->name).isInitialised) {
            throw SemanticError(
                this->path->string(),
                exprIdent->line,
                exprIdent->column,
                "variable '" + exprIdent->name + "' may not have been initialised"
            );
        }
        // return type of expr identifier
        return this->symbolTable->getGlobalVariables().at(exprIdent->name).type;
    }

    if (auto* binaryOperator = dynamic_cast<const ast::ExprBinaryOperator*>(&expr)) {
        const ast::Type leftType = this->checkExprType(*binaryOperator->left);
        const ast::Type rightType = this->checkExprType(*binaryOperator->right);

        if (leftType != rightType) {
            throw TypeError(
                this->path->string(),
                binaryOperator->line,
                binaryOperator->column,
                "cannot apply operator '" + arithmeticOperatorToString(binaryOperator->arithmeticOperatorInfo->arithmeticOperator) +
                "' to types " + typeToString(leftType) + " and " + typeToString(rightType)
            );
        }
        return leftType;
    }
    if (auto* unaryOperator = dynamic_cast<const ast::ExprUnaryOperator*>(&expr)) {
        return this->checkExprType(*unaryOperator->expr);
    }
    throw std::runtime_error("Unknown expression type");
}

std::string compiler::TypeChecker::typeToString(const ast::Type& type) {
    switch (type) {
        case ast::Type::INT: return "integer";
    }
}

std::string compiler::TypeChecker::arithmeticOperatorToString(const ast::ArithmeticOperator &arithmeticOperator) {
    switch (arithmeticOperator) {
        case ast::ArithmeticOperator::PLUS: return "+";
        case ast::ArithmeticOperator::MINUS: return "-";
        case ast::ArithmeticOperator::MULTIPLY: return "*";
        case ast::ArithmeticOperator::DIVIDE: return "/";
        case ast::ArithmeticOperator::MODULO: return "%";
    }
}

