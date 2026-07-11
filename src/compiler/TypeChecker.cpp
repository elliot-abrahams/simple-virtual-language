#include "TypeChecker.h"

#include "../include/Error.h"

compiler::TypeChecker::TypeChecker(SymbolTable* symbolTable, const std::filesystem::path* path) :
    symbolTable(symbolTable), path(path) {}

void compiler::TypeChecker::checkProgram(const ast::Program& program) const {
    for (auto& stm : program.statements) {
        this->checkStm(*stm);
    }
}

void compiler::TypeChecker::checkStm(const ast::Stm& stm) const {
    if (auto* varDecl = dynamic_cast<const ast::StmVarDecl*>(&stm)) {
        this->checkStmVarDecl(*varDecl);
    }
}

void compiler::TypeChecker::checkStmVarDecl(const ast::StmVarDecl& varDecl) const {
    if (varDecl.optionalInitializer == nullptr) {
        this->symbolTable->declareGlobalVariable(varDecl.identifier.name, varDecl.typeInfo.type, false);
        return;
    }

    this->symbolTable->declareGlobalVariable(varDecl.identifier.name, varDecl.typeInfo.type, true);
    const auto initializerType = this->getTypeOfExpr(*varDecl.optionalInitializer);

    if (initializerType != varDecl.typeInfo.type) {
        throw TypeError(
            this->path->string(),
            varDecl.optionalInitializer->line,
            varDecl.optionalInitializer->column,
            "cannot initialise '" + varDecl.identifier.name + "' with expression of different type."
        );
    }
}

ast::Type compiler::TypeChecker::getTypeOfExpr(const ast::Expr& expr) const {
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
            "variable " + exprIdent->name + " may not have been initialised"
        );
        }
        // return type of expr identifier
        return this->symbolTable->getGlobalVariables().at(exprIdent->name).type;
    }
    throw std::runtime_error("Unknown expression type");
}
