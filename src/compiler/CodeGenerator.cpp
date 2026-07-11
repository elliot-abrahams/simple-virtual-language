#include "CodeGenerator.h"

compiler::CodeGenerator::CodeGenerator(SymbolTable* symbolTable) :
    symbolTable(symbolTable) {}

std::vector<std::string> compiler::CodeGenerator::generateCode(const ast::Program& program) {
    this->processProgram(program);
    this->processGlobalVariables();
    return this->generatedCode;
}

void compiler::CodeGenerator::processProgram(const ast::Program& program) {
    for (auto& stm : program.statements) {
        this->processStm(*stm);
    }
}

void compiler::CodeGenerator::processStm(const ast::Stm& stm) {
    if (auto* varDecl = dynamic_cast<const ast::StmVarDecl*>(&stm)) {
        this->processStmVarDecl(*varDecl);
    }
}

void compiler::CodeGenerator::processStmVarDecl(const ast::StmVarDecl& varDecl) {
    if (varDecl.optionalInitializer != nullptr) {
        this->processExpr(*varDecl.optionalInitializer);
        emit("storeG $" + varDecl.identifier.name);
    }
}

void compiler::CodeGenerator::processExpr(const ast::Expr& expr) {
    if (auto* integerLiteral = dynamic_cast<const ast::ExprIntegerLiteral*>(&expr)) {
        this->processExprIntegerLiteral(*integerLiteral);
    }
    if (auto* identifier = dynamic_cast<const ast::ExprIdentifier*>(&expr)) {
        this->processExprIdentifier(*identifier);
    }
}

void compiler::CodeGenerator::processExprIntegerLiteral(const ast::ExprIntegerLiteral& integerLiteral) {
    emit("push i32 #" + std::to_string(integerLiteral.value));
}

void compiler::CodeGenerator::processExprIdentifier(const ast::ExprIdentifier& identifier) {
    emit("loadG $" + identifier.name);
}

void compiler::CodeGenerator::processGlobalVariables() {
    emit(".data");

    auto globalVariables = this->symbolTable->getGlobalVariables();
    for (auto it = globalVariables.begin(); it != globalVariables.end(); ++it) {
        this->emit("$" + it->first + ": i32 #0");
    }
}

std::string compiler::CodeGenerator::typeToString(ast::Type type) {
    switch (type) {
        case ast::Type::INT : return "i32";
    }
}


void compiler::CodeGenerator::emit(const std::string& code) {
    this->generatedCode.push_back(code);
}

