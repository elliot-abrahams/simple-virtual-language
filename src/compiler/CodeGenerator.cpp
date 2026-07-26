#include "CodeGenerator.h"

#include <stdexcept>

compiler::CodeGenerator::CodeGenerator(SymbolTable* symbolTable) :
    symbolTable(symbolTable), labelCounter(0), scopeFunctionCounter(0) {}

std::vector<std::string> compiler::CodeGenerator::generateCode(const ast::Program& program) {
    this->compileProgram(program);
    this->compileGlobalVariables();
    return this->generatedCode;
}

void compiler::CodeGenerator::compileProgram(const ast::Program& program) {
    for (auto& stm : program.statements) {
        this->compileStm(this->symbolTable->globalScope, *stm);
    }
    this->emitWithIndent("halt");
    this->emit("");

    this->compilePendingScopeFunctions();
    for (auto& functionDecl : program.functionDecls) {
        std::vector<Type> parameterTypes;
        for (auto& parameter : functionDecl->parameters) {
            parameterTypes.push_back(parameter->typeInfo->type);
        }

        const auto functionSymbol = this->symbolTable->getFunctionSymbol(functionDecl->identifier->name, parameterTypes);

        this->compileFunctionDeclaration(
            functionSymbol->label,
            *functionDecl->body,
            functionDecl->parameters.size(),
            functionDecl->body->scope->calculateNumberOfLocalSlots(),
            functionDecl->returnTypeInfo->type == Type::VOID
        );
    }
}

void compiler::CodeGenerator::compileFunctionDeclaration(const std::string& functionIdentifier, const ast::Block& body, const uint8_t numberOfArguments, const uint32_t numberOfLocals, const bool includeDefualtReturn) {
    // compile function header
    this->emit("def " + functionIdentifier + ":");
    this->emitWithIndent("args " + std::to_string(numberOfArguments));
    this->emitWithIndent("locals " + std::to_string(numberOfLocals));
    this->emit("");

    // compile body
    for (const auto& stm : body.statements) {
        this->compileStm(body.scope, *stm);
    }

    if (includeDefualtReturn) {
        this->emitWithIndent("ret");
    }
    this->emit("");
}

void compiler::CodeGenerator::compilePendingScopeFunctions() {
    for (int scopeCounter = 0; scopeCounter < this->pendingScopeFunctions.size(); scopeCounter++) {
        const auto block = this->pendingScopeFunctions.at(scopeCounter);
        // compile a function declaration for this scope
        this->compileFunctionDeclaration(
            generateScopeFunctionIdentifier(scopeCounter),
            *block,
            0,
            block->scope->calculateNumberOfLocalSlots(),
            true
        );
    }
}

void compiler::CodeGenerator::compileStm(Scope* scope, const ast::Stm& stm) {
    if (auto* varDecl = dynamic_cast<const ast::StmVarDecl*>(&stm)) {
        this->compileStmVarDecl(scope, *varDecl);
    }
    else if (auto* assignment = dynamic_cast<const ast::StmAssignment*>(&stm)) {
        this->compileStmAssignment(scope, *assignment);
    }
    else if (auto* block = dynamic_cast<const ast::Block*>(&stm)) {
        this->compileBlock(*block);
    }
    else if (auto* ifStm = dynamic_cast<const ast::IfStm*>(&stm)) {
        this->compileIfStatement(scope, *ifStm);
    }
    else if (auto* whileStm = dynamic_cast<const ast::WhileStm*>(&stm)) {
        this->compileWhileStatement(*whileStm);
    }
    else if (auto* functionCallStm = dynamic_cast<const ast::FunctionCallStm*>(&stm)) {
        this->compileFunctionCallStatement(*functionCallStm);
    }
    else if (auto* returnStm = dynamic_cast<const ast::ReturnStm*>(&stm)) {
        this->compileReturnStatement(*returnStm);
    }
}

void compiler::CodeGenerator::compileBlock(const ast::Block& block) {
    // if scope declared in global scope and not a scope within a function
    if (block.scope->parent->isGlobalScope()) {
        this->pendingScopeFunctions.push_back(&block);
        emitWithIndent("call " + generateScopeFunctionIdentifier(this->scopeFunctionCounter++));
    } else {
        for (auto& stm : block.statements) {
            this->compileStm(block.scope, *stm);
        }
    }
}

void compiler::CodeGenerator::compileStmVarDecl(Scope* scope, const ast::StmVarDecl& varDecl) {
    if (varDecl.optionalInitialiser != nullptr) {
        this->compileExpr(*varDecl.optionalInitialiser);

        const auto symbol = scope->lookup(varDecl.identifier->name).value();

        if (symbol->isGlobal()) {
            this->emitWithIndent("storeG $" + varDecl.identifier->name);
        } else {
            this->emitWithIndent("storeL #" + std::to_string(symbol->localSlot));
        }
    }
}

void compiler::CodeGenerator::compileStmAssignment(Scope* scope, const ast::StmAssignment& assignment) {
    this->compileExpr(*assignment.expression);

    const auto symbol = scope->lookup(assignment.varAccess->identifier->name).value();

    if (symbol->isGlobal()) {
        this->emitWithIndent("storeG $" + assignment.varAccess->identifier->name);
    } else {
        this->emitWithIndent("storeL #" + std::to_string(symbol->localSlot));
    }
}

void compiler::CodeGenerator::compileIfStatement(Scope* scope, const ast::IfStm& ifStm) {
    const std::string endIfLabel = generateLabel("end_if");

    this->compileExpr(*ifStm.condition);

    if (ifStm.elseStm == nullptr) { // if statement without else
        emitWithIndent("jez " + endIfLabel); // skip if block when condition is false
        this->compileBlock(*ifStm.ifBlock);
        emit(generateLabelDefFromLabel(endIfLabel));

    } else { // if statement with else
        const std::string elseLabel = generateLabel("else");
        emitWithIndent("jez " + elseLabel); // skip if block when condition is false
        this->compileBlock(*ifStm.ifBlock);
        emitWithIndent("jmp " + endIfLabel);
        emit(generateLabelDefFromLabel(elseLabel));
        this->compileStm(scope, *ifStm.elseStm);
        emit(generateLabelDefFromLabel(endIfLabel));
    }
}

void compiler::CodeGenerator::compileWhileStatement(const ast::WhileStm &whileStm) {
    const std::string startWhileLabel = generateLabel("start_while");
    const std::string endWhileLabel = generateLabel("end_while");

    emit(generateLabelDefFromLabel(startWhileLabel));

    this->compileExpr(*whileStm.condition);

    // skip block if condition is false
    emitWithIndent("jez " + endWhileLabel);

    this->compileBlock(*whileStm.block);
    emitWithIndent("jmp " + startWhileLabel);

    emit(generateLabelDefFromLabel(endWhileLabel));
}

void compiler::CodeGenerator::compileFunctionCallStatement(const ast::FunctionCallStm &functionCallStm) {
    this->compileFunctionCall(*functionCallStm.functionCall);
}

void compiler::CodeGenerator::compileReturnStatement(const ast::ReturnStm &returnStm) {
    this->compileExpr(*returnStm.returnExpression);
    this->emitWithIndent("ret");
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
    if (auto* functionCall = dynamic_cast<const ast::FunctionCall*>(&expr)) {
        this->compileFunctionCall(*functionCall);
    }
}

void compiler::CodeGenerator::compileBinaryExpr(const ast::ExprBinaryOperator &expr) {
    this->compileExpr(*expr.left);
    switch (expr.binaryOperatorInfo->binaryOperator) {
        case BinaryOperator::PLUS: {
            this->compileExpr(*expr.right);
            this->emitWithIndent("add");
            break;
        }
        case BinaryOperator::MINUS: {
            this->compileExpr(*expr.right);
            this->emitWithIndent("sub");
            break;
        }
        case BinaryOperator::MULTIPLY: {
            this->compileExpr(*expr.right);
            this->emitWithIndent("mul");
            break;
        }
        case BinaryOperator::DIVIDE: {
            this->compileExpr(*expr.right);
            this->emitWithIndent("div");
            break;
        }
        case BinaryOperator::MODULO: {
            this->compileExpr(*expr.right);
            this->emitWithIndent("mod");
            break;
        }
        case BinaryOperator::LOGICAL_OR: {
            const std::string evaluateToTrueLabel = this->generateLabel("evaluate_to_true");
            const std::string endOrLabel = this->generateLabel("end_or");

            // skip right expression if left expression results to true
            this->emitWithIndent("jnz " + evaluateToTrueLabel); // evaluate to true if left expression is true

            this->compileExpr(*expr.right);
            this->emitWithIndent("jnz " + evaluateToTrueLabel); // evaluate to true if right expression is true

            this->emitWithIndent("push ui32 #0"); // evaluate to false
            this->emitWithIndent ("jmp " + endOrLabel);

            this->emit(generateLabelDefFromLabel(evaluateToTrueLabel));
            this->emitWithIndent("push ui32 #1"); // evaluate to true

            this->emit(generateLabelDefFromLabel(endOrLabel));
            break;
        }
        case BinaryOperator::LOGICAL_AND: {
            const std::string evaluateToFalseLabel = this->generateLabel("evaluate_to_false");
            const std::string endAndLabel = this->generateLabel("end_and");

            // skip right expression if left expression results to false
            this->emitWithIndent("jez " + evaluateToFalseLabel); // evaluate to false if left expression is false

            this->compileExpr(*expr.right);
            this->emitWithIndent("jez " + evaluateToFalseLabel); // evaluate to false if right expression is false

            this->emitWithIndent("push ui32 #1"); // evaluate to true
            this->emitWithIndent("jmp " + endAndLabel);

            this->emit(generateLabelDefFromLabel(evaluateToFalseLabel));
            this->emitWithIndent("push ui32 #0"); // evaluate to false

            this->emit(generateLabelDefFromLabel(endAndLabel));
            break;
        }
        case BinaryOperator::EQUAL_EQUAL: {
            this->compileExpr(*expr.right);
            this->emitWithIndent("ceq");
            break;
        }
        case BinaryOperator::NOT_EQUAL: {
            this->compileExpr(*expr.right);
            this->emitWithIndent("cne");
            break;
        }
        case BinaryOperator::LESS_THAN: {
            this->compileExpr(*expr.right);
            this->emitWithIndent("clt");
            break;
        }
        case BinaryOperator::LESS_THAN_OR_EQUAL: {
            this->compileExpr(*expr.right);
            this->emitWithIndent("cle");
            break;
        }
        case BinaryOperator::GREATER_THAN: {
            this->compileExpr(*expr.right);
            this->emitWithIndent("cgt");
            break;
        }
        case BinaryOperator::GREATER_THAN_OR_EQUAL: {
            this->compileExpr(*expr.right);
            this->emitWithIndent("cge");
            break;
        }
    }
}

void compiler::CodeGenerator::compileUnaryExpr(const ast::ExprUnaryOperator& expr) {
    this->compileExpr(*expr.expr);
    switch (expr.unaryOperatorInfo->unaryOperator) {
        case UnaryOperator::PLUS: {
            this->emitWithIndent("add");
            break;
        }
        case UnaryOperator::MINUS: {
            this->emitWithIndent("sub");
            break;
        }
        case UnaryOperator::LOGICAL_NOT: {
            const std::string evaluateToTrueLabel = this->generateLabel("evaluate_to_true");
            const std::string endNotLabel = this->generateLabel("end_not");

            emitWithIndent("jez " + evaluateToTrueLabel);
            this->emitWithIndent("push ui32 #0");
            this->emitWithIndent("jmp " + endNotLabel);

            this->emit(generateLabelDefFromLabel(evaluateToTrueLabel));
            this->emitWithIndent("push ui32 #1");

            this->emit(generateLabelDefFromLabel(endNotLabel));

            break;
        }
    }
}

void compiler::CodeGenerator::compileFunctionCall(const ast::FunctionCall& functionCall) {
    // compile arguments
    for (auto& argument : functionCall.arguments) {
        this->compileExpr(*argument);
    }

    this->emitWithIndent("call " + functionCall.functionSymbol->label);
}

void compiler::CodeGenerator::compileExprIdentifier(const ast::ExprIdentifier& identifier) {
    this->emitWithIndent("loadG $" + identifier.name);
}

void compiler::CodeGenerator::compileExprIntegerLiteral(const ast::ExprIntegerLiteral& integerLiteral) {
    this->emitWithIndent("push i32 #" + std::to_string(integerLiteral.value));
}

void compiler::CodeGenerator::compileExprFloatLiteral(const ast::ExprFloatLiteral& floatLiteral) {
    this->emitWithIndent("push f32 #" + std::to_string(floatLiteral.value));
}

void compiler::CodeGenerator::compileExprBoolLiteral(const ast::ExprBoolLiteral& boolLiteral) {
    std::string emittedCode = "push ui32 #";
    emittedCode.append(boolLiteral.value ? "1" : "0");
    this->emitWithIndent(emittedCode);
}

void compiler::CodeGenerator::compileGlobalVariables() {
    this->emit(".data");

    auto globalVariables = this->symbolTable->getGlobalVariables();
    for (auto it = globalVariables.begin(); it != globalVariables.end(); ++it) {
        this->emitWithIndent("$" + it->first + ": " + typeToString(it->second.type) + " 0");
    }
}

std::string compiler::CodeGenerator::typeToString(const Type& type) {
    switch (type) {
        case Type::INT : return "i32";
        case Type::FLOAT: return "f32";
        case Type::BOOL: return "ui32";
    }
}

std::string compiler::CodeGenerator::generateLabel(const std::string& label) {
    std::string newLabel = "$" + label + "_" + std::to_string(this->labelCounter);
    this->labelCounter++;
    return newLabel;
}

std::string compiler::CodeGenerator::generateLabelDefFromLabel(const std::string& label) {
    return label + ":";
}

std::string compiler::CodeGenerator::generateScopeFunctionIdentifier(const uint32_t scopeFunctionNumber) {
    return "$__Scope__" + std::to_string(scopeFunctionNumber);
}

void compiler::CodeGenerator::emitWithIndent(const std::string& code) {
    this->generatedCode.push_back("        " + code);
}

void compiler::CodeGenerator::emit(const std::string& code) {
    this->generatedCode.push_back(code);
}
