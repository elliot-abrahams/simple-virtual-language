#include "CodeGenerator.h"

#include <stdexcept>

compiler::CodeGenerator::CodeGenerator(SymbolTable* symbolTable) :
    symbolTable(symbolTable), labelCounter(0), scopeFunctionCounter(0) {}

std::vector<std::string> compiler::CodeGenerator::generateCode(const ast::Program& program) {
    this->compileProgram(program);
    this->compileBuiltinFunctions();
    this->compileGlobalVariables();
    this->compileBuiltinData();
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

        this->compileFunctionDeclaration(
            functionDecl->functionSymbol->label,
            *functionDecl->body,
            functionDecl->parameters.size(),
            functionDecl->body->scope->calculateNumberOfLocalSlots(),
            functionDecl->returnTypeInfo->type == Type::VOID_RETURN_TYPE
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

        // convert expr to variable type
        this->emitWithDoubleIndentConversion(varDecl.optionalInitialiser->resultingType, symbol->type);

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

    // convert expr to variable type
    this->emitWithDoubleIndentConversion(assignment.expression->resultingType, symbol->type);

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
        case BinaryOperator::PLUS:
        case BinaryOperator::MINUS:
        case BinaryOperator::MULTIPLY:
        case BinaryOperator::DIVIDE:
        case BinaryOperator::MODULO: {
            // convert left expr to expr resulting type
            this->emitWithDoubleIndentConversion(expr.left->resultingType, expr.resultingType);
            // compile right expr
            this->compileExpr(scope, *expr.right);
            // convert right expr to expr resulting type
            this->emitWithDoubleIndentConversion(expr.right->resultingType, expr.resultingType);
            // compile binary operator
            this->compileBinaryOperator(expr.binaryOperatorInfo->binaryOperator);
            return;
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

        case BinaryOperator::INTEGER_DIVIDE:
        case BinaryOperator::EQUAL_EQUAL:
        case BinaryOperator::NOT_EQUAL:
        case BinaryOperator::LESS_THAN:
        case BinaryOperator::LESS_THAN_OR_EQUAL:
        case BinaryOperator::GREATER_THAN:
        case BinaryOperator::GREATER_THAN_OR_EQUAL: {

            if (expr.left->resultingType == Type::FLOAT ||
                expr.right->resultingType == Type::FLOAT
            ) {
                // if either operand is float -> convert left expr to float
                this->emitWithDoubleIndentConversion(expr.left->resultingType, Type::FLOAT);
            }

            // compile right expression
            this->compileExpr(scope, *expr.right);

            if (expr.left->resultingType == Type::FLOAT ||
                expr.right->resultingType == Type::FLOAT
            ) {
                // if either operand is float -> convert right expr to float
                this->emitWithDoubleIndentConversion(expr.right->resultingType, Type::FLOAT);
            }

            this->compileBinaryOperator(expr.binaryOperatorInfo->binaryOperator);
            break;
        }

        default: {}
    }

    if (expr.binaryOperatorInfo->binaryOperator == BinaryOperator::INTEGER_DIVIDE) {
        // if either operand is float -> convert result to int
        if (expr.left->resultingType == Type::FLOAT ||
            expr.right->resultingType == Type::FLOAT
        ) {
            this->emitWithDoubleIndent("conv i32");
        }
    }

}

void compiler::CodeGenerator::compileBinaryOperator(const BinaryOperator binaryOperator) {
    switch (binaryOperator) {
        case BinaryOperator::PLUS: emitWithDoubleIndent("add"); break;
        case BinaryOperator::MINUS: emitWithDoubleIndent("sub"); break;
        case BinaryOperator::MULTIPLY: emitWithDoubleIndent("mul"); break;

        case BinaryOperator::DIVIDE:
        case BinaryOperator::INTEGER_DIVIDE:
            emitWithDoubleIndent("div");
            break;

        case BinaryOperator::MODULO: emitWithDoubleIndent("mod"); break;
        case BinaryOperator::EQUAL_EQUAL: emitWithDoubleIndent("ceq"); break;
        case BinaryOperator::NOT_EQUAL: emitWithDoubleIndent("cne"); break;
        case BinaryOperator::LESS_THAN: emitWithDoubleIndent("clt"); break;
        case BinaryOperator::LESS_THAN_OR_EQUAL: emitWithDoubleIndent("cle"); break;
        case BinaryOperator::GREATER_THAN: emitWithDoubleIndent("cgt"); break;
        case BinaryOperator::GREATER_THAN_OR_EQUAL: emitWithDoubleIndent("cge"); break;

        default: {}
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

            this->compileExpr(scope, *expr.expr);
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

    for (size_t argIndex = 0; argIndex < functionCall.arguments.size(); argIndex++) {
        this->compileExpr(scope, *functionCall.arguments[argIndex]);

        // convert any arguments that require implicit conversion
        this->emitWithDoubleIndentConversion(functionCall.arguments[argIndex]->resultingType, functionCall.functionSymbol->parameterTypes[argIndex]);
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

        // store needed builtinData
        for (const auto& builtinData : builtinFunction->requiredBuiltinData) {
            this->calledBuiltinData.insert(builtinData);
        }

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

void compiler::CodeGenerator::compileBuiltinData() {
    for (const auto builtinDataId : this->calledBuiltinData) {
        this->emit(*BuiltinFunctions::getBuiltinData(builtinDataId));
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

void compiler::CodeGenerator::emitWithDoubleIndentConversion(const Type currentType, const Type newType) {
    if (currentType != newType) {
        emitWithDoubleIndent("conv " + typeToString(newType));
    }
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
