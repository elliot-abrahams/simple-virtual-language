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
    this->emit("halt");
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
        this->emit("storeG $" + varDecl.identifier.name);
    }
}

void compiler::CodeGenerator::compileStmAssignment(const ast::StmAssignment& assignment) {
    //this->compileVarAccess(*assignment.varAccess);
    this->compileExpr(*assignment.expression);

    switch (assignment.assignmentOperatorInfo->assignmentOperator) {
        case ast::AssignmentOperator::EQUAL : {
            this->emit("storeG $" + assignment.varAccess->identifier.name);
            break;
        }
    }
}

void compiler::CodeGenerator::compileExpr(const ast::Expr& expr) {
    if (auto* integerLiteral = dynamic_cast<const ast::ExprIntegerLiteral*>(&expr)) {
        this->compileExprIntegerLiteral(*integerLiteral);
    }
    if (auto* floatLiteral = dynamic_cast<const ast::ExprFloatLiteral*>(&expr)) {
        this->compileExprFloatLiteral(*floatLiteral);
    }
    if (auto* boolLiteral = dynamic_cast<const ast::ExprBoolLiteral*>(&expr)) {
        this->compileExprBoolLiteral(*boolLiteral);
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
    switch (expr.binaryOperatorInfo->binaryOperator) {
        case ast::BinaryOperator::PLUS: {
            this->compileExpr(*expr.right);
            this->emit("add");
            break;
        }
        case ast::BinaryOperator::MINUS: {
            this->compileExpr(*expr.right);
            this->emit("sub");
            break;
        }
        case ast::BinaryOperator::MULTIPLY: {
            this->compileExpr(*expr.right);
            this->emit("mul");
            break;
        }
        case ast::BinaryOperator::DIVIDE: {
            this->compileExpr(*expr.right);
            this->emit("div");
            break;
        }
        case ast::BinaryOperator::MODULO: {
            this->compileExpr(*expr.right);
            this->emit("mod");
            break;
        }
        case ast::BinaryOperator::LOGICAL_OR: {
            const std::string evaluateToTrueLabel = this->generateLabel("evaluate_to_true");
            const std::string endOrLabel = this->generateLabel("end_or");

            // skip right expression if left expression results to true
            this->emit("jnz $" + evaluateToTrueLabel); // evaluate to true if left expression is true

            this->compileExpr(*expr.right);
            this->emit("jnz $" + evaluateToTrueLabel); // evaluate to true if right expression is true

            this->emit("push ui32 #0"); // evaluate to false
            this->emit ("jmp $" + endOrLabel);

            this->emitLabelDef(evaluateToTrueLabel);
            this->emit("push ui32 #1"); // evaluate to true

            this->emitLabelDef(endOrLabel);
            break;
        }
        case ast::BinaryOperator::LOGICAL_AND: {
            const std::string evaluateToFalseLabel = this->generateLabel("evaluate_to_false");
            const std::string endAndLabel = this->generateLabel("end_and");

            // skip right expression if left expression results to false
            this->emit("jez $" + evaluateToFalseLabel); // evaluate to false if left expression is false

            this->compileExpr(*expr.right);
            this->emit("jez $" + evaluateToFalseLabel); // evaluate to false if right expression is false

            this->emit("push ui32 #1"); // evaluate to true
            this->emit("jmp $" + endAndLabel);

            this->emitLabelDef(evaluateToFalseLabel);
            this->emit("push ui32 #0"); // evaluate to false

            this->emitLabelDef(endAndLabel);
            break;
        }
        case ast::BinaryOperator::EQUAL_EQUAL: {
            this->compileExpr(*expr.right);
            this->emit("ceq");
            break;
        }
        case ast::BinaryOperator::NOT_EQUAL: {
            this->compileExpr(*expr.right);
            this->emit("cne");
            break;
        }
        case ast::BinaryOperator::LESS_THAN: {
            this->compileExpr(*expr.right);
            this->emit("clt");
            break;
        }
        case ast::BinaryOperator::LESS_THAN_OR_EQUAL: {
            this->compileExpr(*expr.right);
            this->emit("cle");
            break;
        }
        case ast::BinaryOperator::GREATER_THAN: {
            this->compileExpr(*expr.right);
            this->emit("cgt");
            break;
        }
        case ast::BinaryOperator::GREATER_THAN_OR_EQUAL: {
            this->compileExpr(*expr.right);
            this->emit("cge");
            break;
        }
    }
}

void compiler::CodeGenerator::compileUnaryExpr(const ast::ExprUnaryOperator& expr) {
    this->compileExpr(*expr.expr);
    switch (expr.unaryOperatorInfo->unaryOperator) {
        case ast::UnaryOperator::PLUS: {
            this->emit("add");
            break;
        }
        case ast::UnaryOperator::MINUS: {
            this->emit("sub");
            break;
        }
        case ast::UnaryOperator::LOGICAL_NOT: {
            this->emit("not");
            break;
        }
    }
}

void compiler::CodeGenerator::compileExprIdentifier(const ast::ExprIdentifier& identifier) {
    this->emit("loadG $" + identifier.name);
}

void compiler::CodeGenerator::compileVarAccess(const ast::VarAccess &varAccess) {
    this->emit("push ptr $" + varAccess.identifier.name);
}

void compiler::CodeGenerator::compileExprIntegerLiteral(const ast::ExprIntegerLiteral& integerLiteral) {
    this->emit("push i32 #" + std::to_string(integerLiteral.value));
}

void compiler::CodeGenerator::compileExprFloatLiteral(const ast::ExprFloatLiteral& floatLiteral) {
    this->emit("push f32 #" + std::to_string(floatLiteral.value));
}

void compiler::CodeGenerator::compileExprBoolLiteral(const ast::ExprBoolLiteral& boolLiteral) {
    std::string emittedCode = "push ui32 #";
    emittedCode.append(boolLiteral.value ? "1" : "0");
    this->emit(emittedCode);
}

void compiler::CodeGenerator::compileGlobalVariables() {
    this->emitStartOfDataRegion();

    auto globalVariables = this->symbolTable->getGlobalVariables();
    for (auto it = globalVariables.begin(); it != globalVariables.end(); ++it) {
        this->emit("$" + it->first + ": " + typeToString(it->second.type) + " 0");
    }
}

std::string compiler::CodeGenerator::typeToString(const ast::Type& type) {
    switch (type) {
        case ast::Type::INT : return "i32";
        case ast::Type::FLOAT: return "f32";
        case ast::Type::BOOL: return "ui32";
    }
}

std::string compiler::CodeGenerator::generateLabel(const std::string& label) {
    std::string newLabel = label + "_" + std::to_string(this->labelCounter);
    this->labelCounter++;
    return newLabel;
}

void compiler::CodeGenerator::emit(const std::string& code) {
    this->generatedCode.push_back("        " + code);
}

void compiler::CodeGenerator::emitStartOfDataRegion() {
    this->generatedCode.push_back(".data");
}

void compiler::CodeGenerator::emitLabelDef(const std::string& label) {
    this->generatedCode.push_back("$" + label + ":");
}

