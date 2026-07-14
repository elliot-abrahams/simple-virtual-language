#include "CodeGenerator.h"

compiler::CodeGenerator::CodeGenerator(SymbolTable* symbolTable) :
    symbolTable(symbolTable) {}

std::vector<std::string> compiler::CodeGenerator::generateCode(const ast::Program& program) {
    this->compileProgram(program);
    this->compileGlobalVariables();
    return this->generatedCode;
}

void compiler::CodeGenerator::compileProgram(const ast::Program& program) {
    for (auto& stm : program.statements) {
        this->compileStm(*stm);
    }
    emit("halt");
}

void compiler::CodeGenerator::compileStm(const ast::Stm& stm) {
    if (auto* varDecl = dynamic_cast<const ast::StmVarDecl*>(&stm)) {
        this->compileStmVarDecl(*varDecl);
    }
    if (auto* assignment = dynamic_cast<const ast::StmAssignment*>(&stm)) {
        this->compileStmAssignment(*assignment);
    }
}

void compiler::CodeGenerator::compileStmVarDecl(const ast::StmVarDecl& varDecl) {
    if (varDecl.optionalInitialiser != nullptr) {
        this->compileExpr(*varDecl.optionalInitialiser);
        emit("storeG $" + varDecl.identifier.name);
    }
}

void compiler::CodeGenerator::compileStmAssignment(const ast::StmAssignment& assignment) {
    //this->compileVarAccess(*assignment.varAccess);
    this->compileExpr(*assignment.expression);

    switch (assignment.assignmentOperatorInfo->assignmentOperator) {
        case ast::AssignmentOperator::EQUAL : {
            emit("storeG $" + assignment.varAccess->identifier.name);
            break;
        }
    }
}

void compiler::CodeGenerator::compileExpr(const ast::Expr& expr) {
    if (auto* integerLiteral = dynamic_cast<const ast::ExprIntegerLiteral*>(&expr)) {
        this->compileExprIntegerLiteral(*integerLiteral);
    }
    if (auto* identifier = dynamic_cast<const ast::ExprIdentifier*>(&expr)) {
        this->compileExprIdentifier(*identifier);
    }
    if (auto* binaryExpr = dynamic_cast<const ast::ExprBinaryOperator*>(&expr)) {
        this->compileBinaryExpr(*binaryExpr);
    }
    if (auto* unaryExpr = dynamic_cast<const ast::ExprUnaryOperator*>(&expr)) {
        this->compileUnaryExpr(*unaryExpr);
    }
}

void compiler::CodeGenerator::compileBinaryExpr(const ast::ExprBinaryOperator &expr) {
    this->compileExpr(*expr.left);
    this->compileExpr(*expr.right);
    this->compileArithmeticOperator(expr.arithmeticOperatorInfo->arithmeticOperator);
}

void compiler::CodeGenerator::compileUnaryExpr(const ast::ExprUnaryOperator& expr) {
    this->compileExpr(*expr.expr);
    this->compileArithmeticOperator(expr.arithmeticOperatorInfo->arithmeticOperator);
}

void compiler::CodeGenerator::compileExprIdentifier(const ast::ExprIdentifier& identifier) {
    emit("loadG $" + identifier.name);
}

void compiler::CodeGenerator::compileVarAccess(const ast::VarAccess &varAccess) {
    emit("push ptr $" + varAccess.identifier.name);
}

void compiler::CodeGenerator::compileExprIntegerLiteral(const ast::ExprIntegerLiteral& integerLiteral) {
    emit("push i32 #" + std::to_string(integerLiteral.value));
}

void compiler::CodeGenerator::compileArithmeticOperator(const ast::ArithmeticOperator &arithmeticOperator) {
    switch (arithmeticOperator) {
        case ast::ArithmeticOperator::PLUS: {
            emit("add");
            break;
        }
        case ast::ArithmeticOperator::MINUS: {
            emit("sub");
            break;
        }
        case ast::ArithmeticOperator::MULTIPLY: {
            emit("mul");
            break;
        }
        case ast::ArithmeticOperator::DIVIDE: {
            emit("div");
            break;
        }
        case ast::ArithmeticOperator::MODULO: {
            emit("mod");
            break;
        }
    }
}


void compiler::CodeGenerator::compileGlobalVariables() {
    emit(".data");

    auto globalVariables = this->symbolTable->getGlobalVariables();
    for (auto it = globalVariables.begin(); it != globalVariables.end(); ++it) {
        this->emit("$" + it->first + ": " + typeToString(it->second.type) + " 0");
    }
}

std::string compiler::CodeGenerator::typeToString(const ast::Type& type) {
    switch (type) {
        case ast::Type::INT : return "i32";
    }
}

void compiler::CodeGenerator::emit(const std::string& code) {
    this->generatedCode.push_back(code);
}

