#include "CodeGenerator.h"

#include <stdexcept>

compiler::CodeGenerator::CodeGenerator(SymbolTable* symbolTable) :
    symbolTable(symbolTable), labelCounter(0), scopeFunctionCounter(0) {}

std::vector<std::string> compiler::CodeGenerator::generateCode(const ast::Program& program) {
    this->compileProgram(program);
    this->compileBuiltinFunctions();
    this->compileGlobalVariables();
    return this->generatedCode;
}

void compiler::CodeGenerator::compileProgram(const ast::Program& program) {
    for (auto& stm : program.statements) {
        this->compileStm(this->symbolTable->globalScope, *stm);
    }
    this->emitWithDoubleIndent("halt");
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

void compiler::CodeGenerator::compileFunctionDeclaration(const std::string& functionIdentifier, const ast::Block& body, const uint8_t numberOfArguments, const uint32_t numberOfLocals, const bool includeDefaultReturn) {
    // compile function header
    this->emit("def " + functionIdentifier + ":");
    this->emitWithDoubleIndent("args " + std::to_string(numberOfArguments));
    this->emitWithDoubleIndent("locals " + std::to_string(numberOfLocals));
    this->emit("");

    // compile body
    for (const auto& stm : body.statements) {
        this->compileStm(body.scope, *stm);
    }

    if (includeDefaultReturn) {
        this->emitWithDoubleIndent("ret");
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
        this->compileWhileStatement(scope, *whileStm);
    }
    else if (auto* continueStm = dynamic_cast<const ast::ContinueStm*>(&stm)) {
        this->compileContinueStatement(scope, *continueStm);
    }
    else if (auto* breakStm = dynamic_cast<const ast::BreakStm*>(&stm)) {
        this->compileBreakStatement(scope, *breakStm);
    }
    else if (auto* functionCallStm = dynamic_cast<const ast::FunctionCallStm*>(&stm)) {
        this->compileFunctionCallStatement(scope, *functionCallStm);
    }
    else if (auto* returnStm = dynamic_cast<const ast::ReturnStm*>(&stm)) {
        this->compileReturnStatement(scope, *returnStm);
    }
}

void compiler::CodeGenerator::compileBlock(const ast::Block& block) {
    // if scope declared in global scope and not a scope within a function
    if (block.scope->parent->isGlobalScope()) {
        this->pendingScopeFunctions.push_back(&block);
        emitWithDoubleIndent("call " + generateScopeFunctionIdentifier(this->scopeFunctionCounter++));
    } else {
        for (auto& stm : block.statements) {
            this->compileStm(block.scope, *stm);
        }
    }
}

void compiler::CodeGenerator::compileStmVarDecl(Scope* scope, const ast::StmVarDecl& varDecl) {
    if (varDecl.optionalInitialiser != nullptr) {
        this->compileExpr(scope, *varDecl.optionalInitialiser);

        const auto symbol = scope->lookup(varDecl.identifier->name).value();

        if (symbol->isGlobal()) {
            this->emitWithDoubleIndent("storeG $" + varDecl.identifier->name);
        } else {
            this->emitWithDoubleIndent("storeL #" + std::to_string(symbol->localSlot));
        }
    }
}

void compiler::CodeGenerator::compileStmAssignment(Scope* scope, const ast::StmAssignment& assignment) {
    this->compileExpr(scope, *assignment.expression);

    const auto symbol = scope->lookup(assignment.varAccess->identifier->name).value();

    if (symbol->isGlobal()) {
        this->emitWithDoubleIndent("storeG $" + assignment.varAccess->identifier->name);
    } else {
        this->emitWithDoubleIndent("storeL #" + std::to_string(symbol->localSlot));
    }
}

void compiler::CodeGenerator::compileIfStatement(Scope* scope, const ast::IfStm& ifStm) {
    const std::string endIfLabel = generateLabel("end_if");

    this->compileExpr(scope, *ifStm.condition);

    if (ifStm.elseStm == nullptr) { // if statement without else
        emitWithDoubleIndent("jez " + endIfLabel); // skip if block when condition is false
        this->compileBlock(*ifStm.ifBlock);
        emitWithSingleIndent(generateLabelDefFromLabel(endIfLabel));

    } else { // if statement with else
        const std::string elseLabel = generateLabel("else");
        emitWithDoubleIndent("jez " + elseLabel); // skip if block when condition is false
        this->compileBlock(*ifStm.ifBlock);
        emitWithDoubleIndent("jmp " + endIfLabel);
        emitWithSingleIndent(generateLabelDefFromLabel(elseLabel));
        this->compileStm(scope, *ifStm.elseStm);
        emitWithSingleIndent(generateLabelDefFromLabel(endIfLabel));
    }
}

void compiler::CodeGenerator::compileWhileStatement(Scope* scope, const ast::WhileStm &whileStm) {
    const std::string startWhileLabel = generateLabel("start_while");
    const std::string endWhileLabel = generateLabel("end_while");

    whileStm.block->scope->loopContext = new LoopContext{startWhileLabel, endWhileLabel};

    emitWithSingleIndent(generateLabelDefFromLabel(startWhileLabel));

    this->compileExpr(scope, *whileStm.condition);

    // skip block if condition is false
    emitWithDoubleIndent("jez " + endWhileLabel);

    this->compileBlock(*whileStm.block);
    emitWithDoubleIndent("jmp " + startWhileLabel);

    emitWithSingleIndent(generateLabelDefFromLabel(endWhileLabel));
}

void compiler::CodeGenerator::compileContinueStatement(Scope* scope, const ast::ContinueStm &continueStm) {
    const auto loopContext = scope->lookupWhileScope()->loopContext;
    emitWithDoubleIndent("jmp " + loopContext->continueLabel);
}

void compiler::CodeGenerator::compileBreakStatement(Scope* scope, const ast::BreakStm &breakStm) {
    const auto loopContext = scope->lookupWhileScope()->loopContext;
    emitWithDoubleIndent("jmp " + loopContext->breakLabel);
}

void compiler::CodeGenerator::compileFunctionCallStatement(Scope* scope, const ast::FunctionCallStm &functionCallStm) {
    this->compileFunctionCall(scope, *functionCallStm.functionCall);
}

void compiler::CodeGenerator::compileReturnStatement(Scope* scope, const ast::ReturnStm &returnStm) {
    if (returnStm.returnExpression != nullptr) {
        this->compileExpr(scope, *returnStm.returnExpression);
    }
    this->emitWithDoubleIndent("ret");
}

void compiler::CodeGenerator::compileExpr(Scope* scope, const ast::Expr& expr) {
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
        this->compileExprIdentifier(scope, *identifier);
    }
    if (auto* binaryExpr = dynamic_cast<const ast::ExprBinaryOperator*>(&expr)) {
        this->compileBinaryExpr(scope, *binaryExpr);
    }
    if (auto* unaryExpr = dynamic_cast<const ast::ExprUnaryOperator*>(&expr)) {
        this->compileUnaryExpr(scope, *unaryExpr);
    }
    if (auto* functionCall = dynamic_cast<const ast::FunctionCall*>(&expr)) {
        this->compileFunctionCall(scope, *functionCall);
    }
}

void compiler::CodeGenerator::compileBinaryExpr(Scope* scope, const ast::ExprBinaryOperator &expr) {
    this->compileExpr(scope, *expr.left);
    switch (expr.binaryOperatorInfo->binaryOperator) {
        case BinaryOperator::PLUS: {
            this->compileExpr(scope, *expr.right);
            this->emitWithDoubleIndent("add");
            break;
        }
        case BinaryOperator::MINUS: {
            this->compileExpr(scope, *expr.right);
            this->emitWithDoubleIndent("sub");
            break;
        }
        case BinaryOperator::MULTIPLY: {
            this->compileExpr(scope, *expr.right);
            this->emitWithDoubleIndent("mul");
            break;
        }
        case BinaryOperator::DIVIDE: {
            this->compileExpr(scope, *expr.right);
            this->emitWithDoubleIndent("div");
            break;
        }
        case BinaryOperator::MODULO: {
            this->compileExpr(scope, *expr.right);
            this->emitWithDoubleIndent("mod");
            break;
        }
        case BinaryOperator::LOGICAL_OR: {
            const std::string evaluateToTrueLabel = this->generateLabel("evaluate_to_true");
            const std::string endOrLabel = this->generateLabel("end_or");

            // skip right expression if left expression results to true
            this->emitWithDoubleIndent("jnz " + evaluateToTrueLabel); // evaluate to true if left expression is true

            this->compileExpr(scope, *expr.right);
            this->emitWithDoubleIndent("jnz " + evaluateToTrueLabel); // evaluate to true if right expression is true

            this->emitWithDoubleIndent("push ui32 #0"); // evaluate to false
            this->emitWithDoubleIndent ("jmp " + endOrLabel);

            this->emitWithSingleIndent(generateLabelDefFromLabel(evaluateToTrueLabel));
            this->emitWithDoubleIndent("push ui32 #1"); // evaluate to true

            this->emitWithSingleIndent(generateLabelDefFromLabel(endOrLabel));
            break;
        }
        case BinaryOperator::LOGICAL_AND: {
            const std::string evaluateToFalseLabel = this->generateLabel("evaluate_to_false");
            const std::string endAndLabel = this->generateLabel("end_and");

            // skip right expression if left expression results to false
            this->emitWithDoubleIndent("jez " + evaluateToFalseLabel); // evaluate to false if left expression is false

            this->compileExpr(scope, *expr.right);
            this->emitWithDoubleIndent("jez " + evaluateToFalseLabel); // evaluate to false if right expression is false

            this->emitWithDoubleIndent("push ui32 #1"); // evaluate to true
            this->emitWithDoubleIndent("jmp " + endAndLabel);

            this->emitWithSingleIndent(generateLabelDefFromLabel(evaluateToFalseLabel));
            this->emitWithDoubleIndent("push ui32 #0"); // evaluate to false

            this->emitWithSingleIndent(generateLabelDefFromLabel(endAndLabel));
            break;
        }
        case BinaryOperator::EQUAL_EQUAL: {
            this->compileExpr(scope, *expr.right);
            this->emitWithDoubleIndent("ceq");
            break;
        }
        case BinaryOperator::NOT_EQUAL: {
            this->compileExpr(scope, *expr.right);
            this->emitWithDoubleIndent("cne");
            break;
        }
        case BinaryOperator::LESS_THAN: {
            this->compileExpr(scope, *expr.right);
            this->emitWithDoubleIndent("clt");
            break;
        }
        case BinaryOperator::LESS_THAN_OR_EQUAL: {
            this->compileExpr(scope, *expr.right);
            this->emitWithDoubleIndent("cle");
            break;
        }
        case BinaryOperator::GREATER_THAN: {
            this->compileExpr(scope, *expr.right);
            this->emitWithDoubleIndent("cgt");
            break;
        }
        case BinaryOperator::GREATER_THAN_OR_EQUAL: {
            this->compileExpr(scope, *expr.right);
            this->emitWithDoubleIndent("cge");
            break;
        }
    }
}

void compiler::CodeGenerator::compileUnaryExpr(Scope* scope, const ast::ExprUnaryOperator& expr) {

    switch (expr.unaryOperatorInfo->unaryOperator) {
        case UnaryOperator::PLUS: {
            this->compileExpr(scope, *expr.expr);
            break;
        }
        case UnaryOperator::MINUS: {
            std::string pushZeroAssembly = "push " + typeToString(expr.resultingType) + " #";

            if (expr.resultingType == Type::FLOAT) {
                pushZeroAssembly += "0.0";
            } else {
                pushZeroAssembly += "0";
            }

            this->emitWithDoubleIndent(pushZeroAssembly);
            this->compileExpr(scope, *expr.expr);
            this->emitWithDoubleIndent("sub");
            break;
        }
        case UnaryOperator::LOGICAL_NOT: {
            const std::string evaluateToTrueLabel = this->generateLabel("evaluate_to_true");
            const std::string endNotLabel = this->generateLabel("end_not");

            emitWithDoubleIndent("jez " + evaluateToTrueLabel);
            this->emitWithDoubleIndent("push ui32 #0");
            this->emitWithDoubleIndent("jmp " + endNotLabel);

            this->emitWithSingleIndent(generateLabelDefFromLabel(evaluateToTrueLabel));
            this->emitWithDoubleIndent("push ui32 #1");

            this->emitWithSingleIndent(generateLabelDefFromLabel(endNotLabel));

            break;
        }
    }
}

void compiler::CodeGenerator::compileFunctionCall(Scope* scope, const ast::FunctionCall& functionCall) {
    // compile arguments
    for (auto& argument : functionCall.arguments) {
        this->compileExpr(scope, *argument);
    }
    if (functionCall.functionSymbol->builtinId == BuiltinId::NONE) {
        // function is user-defined
        this->emitWithDoubleIndent("call " + functionCall.functionSymbol->label);
    } else {
        // function is builtin
        this->emitWithDoubleIndent("call " + BuiltinFunctions::getBuiltinFunctionLabel(functionCall.functionSymbol->builtinId));
        this->calledBuiltins.insert(functionCall.functionSymbol->builtinId);
    }
}

void compiler::CodeGenerator::compileExprIdentifier(Scope* scope, const ast::ExprIdentifier& identifier) {
    const auto symbol = scope->lookup(identifier.name).value();

    if (symbol->isGlobal()) {
        this->emitWithDoubleIndent("loadG $" + identifier.name);
    } else {
        this->emitWithDoubleIndent("loadL " + typeToString(symbol->type) + " #" + std::to_string(symbol->localSlot));
    }

}

void compiler::CodeGenerator::compileExprIntegerLiteral(const ast::ExprIntegerLiteral& integerLiteral) {
    this->emitWithDoubleIndent("push i32 #" + std::to_string(integerLiteral.value));
}

void compiler::CodeGenerator::compileExprFloatLiteral(const ast::ExprFloatLiteral& floatLiteral) {
    this->emitWithDoubleIndent("push f32 #" + std::to_string(floatLiteral.value));
}

void compiler::CodeGenerator::compileExprBoolLiteral(const ast::ExprBoolLiteral& boolLiteral) {
    std::string emittedCode = "push ui32 #";
    emittedCode.append(boolLiteral.value ? "1" : "0");
    this->emitWithDoubleIndent(emittedCode);
}

void compiler::CodeGenerator::compileBuiltinFunctions() {
    for (const auto& builtinFunctionId : this->calledBuiltins) {
        BuiltinFunction* builtinFunction = BuiltinFunctions::getBuiltinFunction(builtinFunctionId);

        // compile function def
        this->emit("def " + builtinFunction->functionLabel + ":");

        // compile method metadata
        this->emitWithDoubleIndent("args " + std::to_string(builtinFunction->numberOfArguments));
        this->emitWithDoubleIndent("locals " + std::to_string(builtinFunction->numberOfLocals));

        this->emit("");
        // compile method body
        for (const auto& assembly : builtinFunction->functionBodyAssembly) {
            this->emit(assembly);
        }
        this->emit("");
    }
}

void compiler::CodeGenerator::compileGlobalVariables() {
    this->emit(".data");

    auto globalVariables = this->symbolTable->getGlobalVariables();
    for (auto it = globalVariables.begin(); it != globalVariables.end(); ++it) {
        std::string staticDataDef = "$" + it->first + ": " + typeToString(it->second.type) + " ";

        if (it->second.type == Type::FLOAT) {
            staticDataDef += "0.0";
        } else {
            staticDataDef += "0";
        }

        this->emitWithSingleIndent(staticDataDef);
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

void compiler::CodeGenerator::emitWithDoubleIndent(const std::string& assembly) {
    this->generatedCode.push_back("        " + assembly);
}

void compiler::CodeGenerator::emitWithSingleIndent(const std::string& assembly) {
    this->generatedCode.push_back("    " + assembly);
}

void compiler::CodeGenerator::emit(const std::string& code) {
    this->generatedCode.push_back(code);
}
