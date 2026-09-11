#include "AssemblyGenerator.h"

#include <stdexcept>

compiler::AssemblyGenerator::AssemblyGenerator(SymbolTable* symbolTable) :
    symbolTable(symbolTable), labelCounter(0), scopeFunctionCounter(0) {}

std::vector<compiler::AssemblyItem> compiler::AssemblyGenerator::compileProgram(const ast::Program& program) {
    // compile global statements
    for (auto& stm : program.statements) {
        this->compileStm(this->symbolTable->globalScope, *stm);
    }
    // emit halt to terminate the program
    this->emit(Instruction{Opcode::HALT});

    this->compilePendingScopeFunctions();

    for (auto& functionDecl : program.functionDecls) {
        this->compileUserDefinedFunction(*functionDecl);
    }

    this->compileBuiltinFunctions();
    this->compileGlobalVariables();
    return this->assembly;
}

std::unordered_set<compiler::BuiltinFunctionId> compiler::AssemblyGenerator::getRequiredBuiltinFunctions() {
    return this->requiredBuiltinFunctions;
}

std::unordered_set<compiler::BuiltinDataId> compiler::AssemblyGenerator::getRequiredBuiltinData() {
    return this->requiredBuiltinData;
}

void compiler::AssemblyGenerator::compileUserDefinedFunction(const ast::FunctionDecl &functionDecl) {
    std::vector<Type> parameterTypes;
    for (auto& parameter : functionDecl.parameters) {
        parameterTypes.push_back(parameter->typeInfo->type);
    }
    this->compileFunctionDeclaration(
        functionDecl.functionSymbol->builtinId == BuiltinFunctionId::NONE ? MethodDefType::USER : MethodDefType::BUILTIN,
        functionDecl.functionSymbol->label,
        *functionDecl.body,
        functionDecl.parameters.size(),
        functionDecl.body->scope->calculateNumberOfLocalSlots(),
        functionDecl.returnTypeInfo->type == Type::VOID_RETURN_TYPE,
        functionDecl.line,
        functionDecl.column,
        functionDecl.body->blockEndLine,
        functionDecl.body->blockEndColumn
    );
}

void compiler::AssemblyGenerator::compileFunctionDeclaration(const MethodDefType methodType, const std::string& functionIdentifier, const ast::Block& body, const uint8_t numberOfArguments, const uint32_t numberOfLocals, const bool includeDefaultReturn, const uint32_t line, const uint16_t column, const uint32_t functionBodyEndLine, const uint16_t functionBodyEndColumn) {
    // compile function header

    this->emit(MethodDef{
        functionIdentifier,
        numberOfArguments,
        numberOfLocals,
        methodType,
        SourceLocation{0, line, column}
    });

    // compile body
    for (const auto& stm : body.statements) {
        this->compileStm(body.scope, *stm);
    }

    if (includeDefaultReturn) {
        // default ret for scope and void functions to return execution to caller
        this->emit(
            Instruction{Opcode::RET,
                {},
                SourceLocation{0, functionBodyEndLine, functionBodyEndColumn}
            }
        );
    }

    this->emit(IRMarker::METHOD_DEF_END);
}

void compiler::AssemblyGenerator::compilePendingScopeFunctions() {
    for (int scopeCounter = 0; scopeCounter < this->pendingScopeFunctions.size(); scopeCounter++) {
        const auto block = this->pendingScopeFunctions.at(scopeCounter);
        // compile a function declaration for this scope
        this->compileFunctionDeclaration(
            MethodDefType::SCOPE,
            generateScopeFunctionIdentifier(scopeCounter),
            *block,
            0,
            block->scope->calculateNumberOfLocalSlots(),
            true,
            block->line,
            block->column,
            block->blockEndLine,
            block->blockEndColumn
        );
    }
}

void compiler::AssemblyGenerator::compileArrayIndex(Scope *scope, const ast::Index &index, SemanticType& typeAtDepth) {
    const auto arrayIndexOutOfRangeRangeLabel = this->generateLabel("array_index_out_of_range");
    const auto arrayIndexInRangeRangeLabel = this->generateLabel("array_index_in_range");

    const auto indexSourceLocation = new SourceLocation{0, index.line, index.column};

    // [root_array_ptr]

    this->emit(Instruction{Opcode::DUP,
        {Immediate{Number{static_cast<uint32_t>(0)}}},
        *indexSourceLocation
    });
    // [root_array_ptr, root_array_ptr]

    this->emit(Instruction{Opcode::LOAD,
        {AssemblyType::UI32},
        *indexSourceLocation
    });
    // [root_array_ptr, root_array_length]

    this->compileExpr(scope, *index.index);
    // [root_array_ptr, root_array_length, array_index]

    // =============================================================
    // throw index out of range error if index < 0
    // =============================================================

    this->emit(Instruction{Opcode::DUP,
        {Immediate{Number{static_cast<uint32_t>(0)}}},
        *indexSourceLocation
    });
    // [root_array_ptr, root_array_length, array_index, array_index]

    this->emit(Instruction{Opcode::PUSH,
        {
            toAssemblyType(index.index->resultingType),
            Immediate{Number{static_cast<uint32_t>(0)}}
        },
        *indexSourceLocation
    });
    // [root_array_ptr, root_array_length, array_index, array_index, 0]

    this->emit(Instruction{Opcode::CLT,
        {},
        *indexSourceLocation
    });
    // [root_array_ptr, root_array_length, array_index, array_index < 0]

    this->emit(Instruction{Opcode::JNZ,
        {LabelRef{arrayIndexOutOfRangeRangeLabel}},
        *indexSourceLocation
    });
    // [root_array_ptr, root_array_length, array_index]

    // =============================================================
    // throw index out of range error if index >= array_length
    // =============================================================

    this->emit(Instruction{Opcode::DUP,
        {Immediate{Number{static_cast<uint32_t>(1)}}},
        *indexSourceLocation
    });
    // [root_array_ptr, root_array_length, array_index, root_array_length]

    this->compileTypeConversionIfRequired(AssemblyType::UI32, toAssemblyType(index.index->resultingType), *indexSourceLocation);

    this->emit(Instruction{Opcode::DUP,
        {Immediate{Number{static_cast<uint32_t>(1)}}},
        *indexSourceLocation
    });
    // [root_array_ptr, root_array_length, array_index, root_array_length, array_index]

    this->emit(Instruction{Opcode::CLE,
        {},
        SourceLocation{0, index.line, index.column}
    });
    // [root_array_ptr, root_array_length, array_index, root_array_length <= array_index]

    this->emit(Instruction{Opcode::JNZ,
        {LabelRef{arrayIndexOutOfRangeRangeLabel}},
        *indexSourceLocation
    });
    // [root_array_ptr, root_array_length, array_index]

    this->emit(Instruction{Opcode::JMP,
        {LabelRef{arrayIndexInRangeRangeLabel}},
        *indexSourceLocation
    });
    // [root_array_ptr, root_array_length, array_index]

    this->emit(LabelDef{arrayIndexOutOfRangeRangeLabel});

    this->emit(Instruction{Opcode::THROW,
        {ErrorRef::ARRAY_INDEX_OUT_OF_RANGE},
        *indexSourceLocation
    });
    // [root_array_ptr]

    this->emit(LabelDef{arrayIndexInRangeRangeLabel});

    // [root_array_ptr, root_array_length, array_index]

    this->emit(Instruction{Opcode::SWAP,
        {},
        *indexSourceLocation
    });
    // [root_array_ptr, array_index, root_array_length]

    this->emit(Instruction{Opcode::POP,
        {},
        *indexSourceLocation
    });
    // [root_array_ptr, array_index]

    this->compileTypeConversionIfRequired(toAssemblyType(index.index->resultingType), AssemblyType::UI32, *indexSourceLocation);

    // =============================================================
    // calculate ptr to array element
    // =============================================================

    this->emit(Instruction{Opcode::PUSH,
        {
            AssemblyType::UI32,
            Immediate{Number{static_cast<uint32_t>(typeAtDepth.getSize())}}
        },
        *indexSourceLocation
    });
    // [root_array_ptr, array_index, number_of_bytes_per_element]

    this->emit(Instruction{Opcode::MUL,
        {},
        *indexSourceLocation
    });
    // [root_array_ptr, array_index * number_of_bytes_per_element]

    this->emit(Instruction{Opcode::PUSH,
        {
            AssemblyType::UI32,
            Immediate{Number{static_cast<uint32_t>(4)}}
        },
        *indexSourceLocation
    });
    // [root_array_ptr, array_index * number_of_bytes_per_element, 4]

    this->emit(Instruction{Opcode::ADD,
        {},
        *indexSourceLocation
    });
    // [root_array_ptr, address_offset]

    this->emit(Instruction{Opcode::ADD,
        {},
        *indexSourceLocation
    });
    // [nested_array_ptr]

}

void compiler::AssemblyGenerator::compileStm(Scope* scope, const ast::Stm& stm) {
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

void compiler::AssemblyGenerator::compileBlock(const ast::Block& block) {
    // if scope declared in global scope and not a scope within a function
    if (block.scope->parent->isGlobalScope()) {
        this->pendingScopeFunctions.push_back(&block);
        // call scope function
        this->emit(Instruction{Opcode::CALL,
            {LabelRef{generateScopeFunctionIdentifier(this->scopeFunctionCounter++)}},
            SourceLocation{0, block.blockEndLine, block.blockEndColumn}
        });
    } else {
        for (auto& stm : block.statements) {
            this->compileStm(block.scope, *stm);
        }
    }
}

void compiler::AssemblyGenerator::compileStmVarDecl(Scope* scope, const ast::StmVarDecl& varDecl) {
    if (varDecl.optionalInitialiser != nullptr) {
        this->compileExpr(scope, *varDecl.optionalInitialiser);

        const auto symbol = scope->lookup(varDecl.identifier->name).value();

        // convert expr to variable type
        this->compileTypeConversionIfRequired(varDecl.optionalInitialiser->resultingType.type, symbol->type, SourceLocation{0, varDecl.optionalInitialiser->line, varDecl.optionalInitialiser->column});

        if (symbol->isGlobal()) {
            // store optional initialiser in global variable
            this->emit(Instruction{Opcode::STOREG,
                {LabelRef{varDecl.identifier->name}},
                SourceLocation{0, varDecl.line, varDecl.column}
            });
        } else {
            // store optional initialiser in local variable
            this->emit(Instruction{Opcode::STOREL,
                {Immediate{Number{symbol->localSlot}}},
                SourceLocation{0, varDecl.line, varDecl.column}
            });
        }
    }
}

void compiler::AssemblyGenerator::compileStmAssignment(Scope* scope, const ast::StmAssignment& assignment) {

    const auto symbol = scope->lookup(assignment.varAccess->identifier->name).value();


    if (assignment.varAccess->indices.size() == 0) {
        this->compileExpr(scope, *assignment.expression);
        // convert expr to variable type
        this->compileTypeConversionIfRequired(assignment.expression->resultingType.type, symbol->type, SourceLocation{0, assignment.expression->line, assignment.expression->column});

        if (symbol->isGlobal()) {
            // store result of expression in global variable
            this->emit(Instruction{Opcode::STOREG,
                {LabelRef{assignment.varAccess->identifier->name}},
                SourceLocation{0, assignment.line, assignment.column}
            });
        } else {
            // store result of expression in local variable
            this->emit(Instruction{Opcode::STOREL,
                {Immediate{Number{symbol->localSlot}}},
                SourceLocation{0, assignment.line, assignment.column}
            });
        }

    } else {
        if (symbol->isGlobal()) {
            // store result of expression in global variable
            this->emit(Instruction{Opcode::LOADG,
                {LabelRef{assignment.varAccess->identifier->name}},
                SourceLocation{0, assignment.line, assignment.column}
            });
        } else {
            // store result of expression in local variable
            this->emit(Instruction{Opcode::LOADL,
                {
                     AssemblyType::PTR,
                    Immediate{Number{symbol->localSlot}}
                },
                SourceLocation{0, assignment.line, assignment.column}
            });
        }

        SemanticType typeAtDepth = SemanticType{symbol->type, symbol->dimension};

        for (int index = 0; index < assignment.varAccess->indices.size(); ++index) {

            typeAtDepth.dimension--;

            this->compileArrayIndex(scope, *assignment.varAccess->indices[index], typeAtDepth);

            if (index < assignment.varAccess->indices.size() - 1) {

                // =============================================================
                // load array element if not the deepest index
                // =============================================================

                this->emit(Instruction{Opcode::LOAD,
                    {AssemblyType::PTR},
                    SourceLocation{0, assignment.varAccess->indices[index]->line, assignment.varAccess->indices[index]->column}
                });
            }
        }

        // =============================================================
        // store expression
        // =============================================================

        this->compileExpr(scope, *assignment.expression);
        // convert expr to variable type
        this->compileTypeConversionIfRequired(assignment.expression->resultingType.type, symbol->type, SourceLocation{0, assignment.expression->line, assignment.expression->column});


        this->emit(Instruction{Opcode::STORE,
            {},
            SourceLocation{0, assignment.line, assignment.column}
        });
    }
}

void compiler::AssemblyGenerator::compileIfStatement(Scope* scope, const ast::IfStm& ifStm) {
    this->compileExpr(scope, *ifStm.condition);

    if (ifStm.elseStm == nullptr) { // if statement without else
        const std::string endIfLabel = generateLabel("end_if");

        // skip if block when condition is false
        this->emit(Instruction{Opcode::JEZ,
            {LabelRef{endIfLabel}},
            SourceLocation{0, ifStm.line, ifStm.column}
        });

        this->compileBlock(*ifStm.ifBlock); // compile if block

        this->emit(LabelDef{endIfLabel});

    } else { // if statement with else
        const std::string elseLabel = generateLabel("else");
        const std::string endIfLabel = generateLabel("end_if");

        // jump to else block when condition is false
        this->emit(Instruction{Opcode::JEZ,
            {LabelRef{elseLabel}},
            SourceLocation{0, ifStm.line, ifStm.column}
        });

        this->compileBlock(*ifStm.ifBlock); // compile if block

        // jump to end of else block
        this->emit(Instruction{Opcode::JMP,
            {LabelRef{endIfLabel}},
            SourceLocation{0, ifStm.line, ifStm.column}
        });

        this->emit(LabelDef{elseLabel});

        this->compileStm(scope, *ifStm.elseStm); // compile else block

        this->emit(LabelDef{endIfLabel});
    }
}

void compiler::AssemblyGenerator::compileWhileStatement(Scope* scope, const ast::WhileStm &whileStm) {
    const std::string startWhileLabel = generateLabel("start_while");
    const std::string endWhileLabel = generateLabel("end_while");

    whileStm.block->scope->loopContext = new LoopContext{startWhileLabel, endWhileLabel};

    this->emit(LabelDef{startWhileLabel});

    this->compileExpr(scope, *whileStm.condition);

    // skip block if condition is false
    this->emit(Instruction{Opcode::JEZ,
        {LabelRef{endWhileLabel}},
        SourceLocation{0, whileStm.line, whileStm.column}
    });

    this->compileBlock(*whileStm.block); // compile while block

    // jump to start of while (evaluate condition again)
    this->emit(Instruction{Opcode::JMP,
        {LabelRef{startWhileLabel}},
        SourceLocation{0, whileStm.line, whileStm.column}
    });

    this->emit(LabelDef{endWhileLabel});
}

void compiler::AssemblyGenerator::compileContinueStatement(Scope* scope, const ast::ContinueStm &continueStm) {
    const auto loopContext = scope->lookupWhileScope()->loopContext;
    // jump to start of while block (evaluate condition again)
    this->emit(Instruction{Opcode::JMP,
        {LabelRef{loopContext->continueLabel}},
        SourceLocation{0, continueStm.line, continueStm.column}
    });
}

void compiler::AssemblyGenerator::compileBreakStatement(Scope* scope, const ast::BreakStm &breakStm) {
    const auto loopContext = scope->lookupWhileScope()->loopContext;
    // jump to end of while block
    this->emit(Instruction{Opcode::JMP,
        {LabelRef{loopContext->breakLabel}},
        SourceLocation{0, breakStm.line, breakStm.column}
    });
}

void compiler::AssemblyGenerator::compileFunctionCallStatement(Scope* scope, const ast::FunctionCallStm &functionCallStm) {
    this->compileFunctionCall(scope, *functionCallStm.functionCall);
}

void compiler::AssemblyGenerator::compileReturnStatement(Scope* scope, const ast::ReturnStm &returnStm) {
    if (returnStm.returnExpression != nullptr) {
        this->compileExpr(scope, *returnStm.returnExpression);
        this->compileTypeConversionIfRequired(returnStm.returnExpression->resultingType.type, returnStm.functionSymbol->returnType.type, SourceLocation{0, returnStm.returnExpression->line, returnStm.returnExpression->column});
    }
    // return execution to caller
    this->emit(Instruction{Opcode::RET,
        {},
        SourceLocation{0, returnStm.line, returnStm.column}
    });
}

void compiler::AssemblyGenerator::compileExpr(Scope* scope, const ast::Expr& expr) {
    if (auto* integerLiteral = dynamic_cast<const ast::ExprIntegerLiteral*>(&expr)) {
        this->compileExprIntegerLiteral(*integerLiteral);
    }
    if (auto* floatLiteral = dynamic_cast<const ast::ExprFloatLiteral*>(&expr)) {
        this->compileExprFloatLiteral(*floatLiteral);
    }
    if (auto* boolLiteral = dynamic_cast<const ast::ExprBoolLiteral*>(&expr)) {
        this->compileExprBoolLiteral(*boolLiteral);
    }
    if (auto* identifier = dynamic_cast<const ast::ExprVarAccess*>(&expr)) {
        this->compileExprIdentifier(scope, *identifier);
    }
    if (auto* binaryExpr = dynamic_cast<const ast::ExprBinaryOperator*>(&expr)) {
        this->compileBinaryExpr(scope, *binaryExpr);
    }
    if (auto* unaryExpr = dynamic_cast<const ast::ExprUnaryOperator*>(&expr)) {
        this->compileUnaryExpr(scope, *unaryExpr);
    }
    if (auto* castExpr = dynamic_cast<const ast::ExprCast*>(&expr)) {
        this->compileCastExpr(scope, *castExpr);
    }
    if (auto* functionCall = dynamic_cast<const ast::FunctionCall*>(&expr)) {
        this->compileFunctionCall(scope, *functionCall);
    }
    if (auto* newExpr = dynamic_cast<const ast::ExprNew*>(&expr)) {
        this->compileNewExpr(scope, *newExpr);
    }
}

void compiler::AssemblyGenerator::compileBinaryExpr(Scope* scope, const ast::ExprBinaryOperator &expr) {
    this->compileExpr(scope, *expr.left);

    switch (expr.binaryOperatorInfo->binaryOperator) {
        case BinaryOperator::PLUS:
        case BinaryOperator::MINUS:
        case BinaryOperator::MULTIPLY:
        case BinaryOperator::DIVIDE:
        case BinaryOperator::MODULO: {
            // convert left expr to expr resulting type
            this->compileTypeConversionIfRequired(expr.left->resultingType.type, expr.resultingType.type, SourceLocation{0, expr.left->line, expr.left->column});
            // compile right expr
            this->compileExpr(scope, *expr.right);
            // convert right expr to expr resulting type
            this->compileTypeConversionIfRequired(expr.right->resultingType.type, expr.resultingType.type, SourceLocation{0, expr.right->line, expr.right->column});
            // compile binary operator
            this->compileBinaryOperator(*expr.binaryOperatorInfo);
            return;
        }

        case BinaryOperator::LOGICAL_OR: {
            const std::string evaluateToTrueLabel = this->generateLabel("evaluate_to_true");
            const std::string endOrLabel = this->generateLabel("end_or");

            // skip right expression if left expression results to true
            // evaluate to true if left expression is true
            this->emit(Instruction{Opcode::JNZ,
                {LabelRef{evaluateToTrueLabel}},
                SourceLocation{0, expr.binaryOperatorInfo->line, expr.binaryOperatorInfo->column}
            });

            this->compileExpr(scope, *expr.right);

            // evaluate to true if right expression is true
            this->emit(Instruction{Opcode::JNZ,
                {LabelRef{evaluateToTrueLabel}},
                SourceLocation{0, expr.binaryOperatorInfo->line, expr.binaryOperatorInfo->column}
            });
            // push 'false' onto stack
            this->emit(Instruction{Opcode::PUSH,
                {
                    AssemblyType::UI32,
                    Immediate{0}
                },
                SourceLocation{0, expr.binaryOperatorInfo->line, expr.binaryOperatorInfo->column}
            });

            // jump to end of logical OR
            this->emit(Instruction{Opcode::JMP,
                {LabelRef{endOrLabel}},
                SourceLocation{0, expr.binaryOperatorInfo->line, expr.binaryOperatorInfo->column}
            });

            this->emit(LabelDef{evaluateToTrueLabel});

            // push 'true' onto stack
            this->emit(Instruction{Opcode::PUSH,
                {
                    AssemblyType::UI32,
                    Immediate{1}
                },
                SourceLocation{0, expr.binaryOperatorInfo->line, expr.binaryOperatorInfo->column}
            });

            this->emit(LabelDef{endOrLabel});
            break;
        }

        case BinaryOperator::LOGICAL_AND: {
            const std::string evaluateToFalseLabel = this->generateLabel("evaluate_to_false");
            const std::string endAndLabel = this->generateLabel("end_and");

            // skip right expression if left expression results to false
            // evaluate to false if left expression is false
            this->emit(Instruction{Opcode::JEZ,
                {LabelRef{evaluateToFalseLabel}},
                SourceLocation{0, expr.binaryOperatorInfo->line, expr.binaryOperatorInfo->column}
            });

            this->compileExpr(scope, *expr.right);

            // evaluate to false if right expression is false
            this->emit(Instruction{Opcode::JEZ,
                {LabelRef{evaluateToFalseLabel}},
                SourceLocation{0, expr.binaryOperatorInfo->line, expr.binaryOperatorInfo->column}
            });

            // push `true` onto stack
            this->emit(Instruction{Opcode::PUSH,
                {
                    AssemblyType::UI32,
                    Immediate{1}
                },
                SourceLocation{0, expr.binaryOperatorInfo->line, expr.binaryOperatorInfo->column}
            });

            // jump to end of logical AND
            this->emit(Instruction{Opcode::JMP,
                {LabelRef{endAndLabel}},
                SourceLocation{0, expr.binaryOperatorInfo->line, expr.binaryOperatorInfo->column}
            });

            this->emit(LabelDef{evaluateToFalseLabel});

            // push 'false' onto stack
            this->emit(Instruction{Opcode::PUSH,
                {
                    AssemblyType::UI32,
                    Immediate{0}
                },
                SourceLocation{0, expr.binaryOperatorInfo->line, expr.binaryOperatorInfo->column}
            });

            this->emit(LabelDef{endAndLabel});
            break;
        }

        case BinaryOperator::INTEGER_DIVIDE:
        case BinaryOperator::EQUAL_EQUAL:
        case BinaryOperator::NOT_EQUAL:
        case BinaryOperator::LESS_THAN:
        case BinaryOperator::LESS_THAN_OR_EQUAL:
        case BinaryOperator::GREATER_THAN:
        case BinaryOperator::GREATER_THAN_OR_EQUAL: {

            if (expr.left->resultingType.type == Type::FLOAT ||
                expr.right->resultingType.type == Type::FLOAT
            ) {
                // if either operand is float -> convert left expr to float
                this->compileTypeConversionIfRequired(expr.left->resultingType.type, Type::FLOAT, SourceLocation{0, expr.left->line, expr.left->column});
            }

            // compile right expression
            this->compileExpr(scope, *expr.right);

            if (expr.left->resultingType.type == Type::FLOAT ||
                expr.right->resultingType.type == Type::FLOAT
            ) {
                // if either operand is float -> convert right expr to float
                this->compileTypeConversionIfRequired(expr.right->resultingType.type, Type::FLOAT, SourceLocation{0, expr.right->line, expr.right->column});
            }

            this->compileBinaryOperator(*expr.binaryOperatorInfo);
            break;
        }

        default: {}
    }

    if (expr.binaryOperatorInfo->binaryOperator == BinaryOperator::INTEGER_DIVIDE) {
        // if either operand is float -> convert result to int
        if (expr.left->resultingType.type == Type::FLOAT ||
            expr.right->resultingType.type == Type::FLOAT
        ) {
            // convert result to integer
            this->emit(Instruction{Opcode::CONV,
                {AssemblyType::I32},
                SourceLocation{0, expr.line, expr.column}
            });
        }
    }
}

void compiler::AssemblyGenerator::compileBinaryOperator(const ast::BinaryOperatorInfo& binaryOperatorInfo) {

    const auto sourceLocation = SourceLocation{0, binaryOperatorInfo.line, binaryOperatorInfo.column};

    switch (binaryOperatorInfo.binaryOperator) {
        case BinaryOperator::PLUS: this->emit(Instruction{Opcode::ADD, {}, sourceLocation}); break;
        case BinaryOperator::MINUS: this->emit(Instruction{Opcode::SUB, {}, sourceLocation}); break;
        case BinaryOperator::MULTIPLY: this->emit(Instruction{Opcode::MUL, {}, sourceLocation}); break;

        case BinaryOperator::DIVIDE:
        case BinaryOperator::INTEGER_DIVIDE:
            this->emit(Instruction{Opcode::DIV, {}, sourceLocation});
            break;

        case BinaryOperator::MODULO: this->emit(Instruction{Opcode::MOD, {}, sourceLocation}); break;
        case BinaryOperator::EQUAL_EQUAL: this->emit(Instruction{Opcode::CEQ, {}, sourceLocation}); break;
        case BinaryOperator::NOT_EQUAL: this->emit(Instruction{Opcode::CNE, {}, sourceLocation}); break;
        case BinaryOperator::LESS_THAN: this->emit(Instruction{Opcode::CLT, {}, sourceLocation}); break;
        case BinaryOperator::LESS_THAN_OR_EQUAL: this->emit(Instruction{Opcode::CLE, {}, sourceLocation}); break;
        case BinaryOperator::GREATER_THAN: this->emit(Instruction{Opcode::CGT, {}, sourceLocation}); break;
        case BinaryOperator::GREATER_THAN_OR_EQUAL: this->emit(Instruction{Opcode::CGE, {}, sourceLocation}); break;

        default: {}
    }
}


void compiler::AssemblyGenerator::compileUnaryExpr(Scope* scope, const ast::ExprUnaryOperator& expr) {
    switch (expr.unaryOperatorInfo->unaryOperator) {
        case UnaryOperator::PLUS: {
            this->compileExpr(scope, *expr.expr);
            break;
        }
        case UnaryOperator::MINUS: {
            // push 0 onto stack
             this->emit(Instruction{Opcode::PUSH,
                {
                    toAssemblyType(expr.expr->resultingType),
                    Immediate{std::get<Number>(getDefaultNumber(expr.expr->resultingType))}
                },
                 SourceLocation{0, expr.unaryOperatorInfo->line, expr.unaryOperatorInfo->column}
             });

            this->compileExpr(scope, *expr.expr);

            // subtract value of expression from 0
            this->emit(Instruction{Opcode::SUB,
                {},
                SourceLocation{0, expr.unaryOperatorInfo->line, expr.unaryOperatorInfo->column}
            });
            break;
        }
        case UnaryOperator::LOGICAL_NOT: {
            const std::string evaluateToTrueLabel = this->generateLabel("evaluate_to_true");
            const std::string endNotLabel = this->generateLabel("end_not");

            this->compileExpr(scope, *expr.expr);

            // evaluate to true if value of expression is false
            this->emit(Instruction{Opcode::JEZ,
                {LabelRef{evaluateToTrueLabel}},
                SourceLocation{0, expr.unaryOperatorInfo->line, expr.unaryOperatorInfo->column}
            });

            // push 'false' onto stack
            this->emit(Instruction{Opcode::PUSH,
                {
                    AssemblyType::UI32,
                    Immediate{Number{static_cast<uint32_t>(0)}},
                },
                SourceLocation{0, expr.unaryOperatorInfo->line, expr.unaryOperatorInfo->column}
             });

            // jump to end of logical NOT
            this->emit(Instruction{Opcode::JMP,
                {LabelRef{endNotLabel}},
                SourceLocation{0, expr.unaryOperatorInfo->line, expr.unaryOperatorInfo->column}
            });

            this->emit(LabelDef{evaluateToTrueLabel});

            // push `true` onto stack
            this->emit(Instruction{Opcode::PUSH,
                {
                    AssemblyType::UI32,
                    Immediate{Number{static_cast<uint32_t>(1)}}
                },
                SourceLocation{0, expr.unaryOperatorInfo->line, expr.unaryOperatorInfo->column}
            });

            this->emit(LabelDef{endNotLabel});
            break;
        }
    }
}

void compiler::AssemblyGenerator::compileCastExpr(Scope* scope, const ast::ExprCast& castExpr) {
    this->compileExpr(scope, *castExpr.expr);

    // convert to result of expression to the cast type
    this->compileTypeConversionIfRequired(castExpr.expr->resultingType.type, castExpr.resultingType.type, SourceLocation{0, castExpr.line, castExpr.column});
}

void compiler::AssemblyGenerator::compileNewExpr(Scope* scope, const ast::ExprNew& newExpr) {
    // allocate space in heap
    const unsigned int dimension = newExpr.typeInfo->dimension - 1;

    // compile index expressions
    for (int index = newExpr.arrayDimensions.size() - 1; index >= 0; index--) {
        const auto arraySizeNotNegativeLabel = this->generateLabel("array_size_not_negative");

        this->compileExpr(scope, *newExpr.arrayDimensions[index]->index);

        // =======================================================
        // check index expr is greater than or equal to zero
        // =======================================================

        this->emit(Instruction{Opcode::DUP,
            {Immediate{Number{static_cast<uint32_t>(0)}}},
            SourceLocation{0, newExpr.line, newExpr.column}
        });

        this->emit(Instruction{Opcode::PUSH,
            {
                toAssemblyType(newExpr.arrayDimensions[index]->index->resultingType),
                Immediate{Number{static_cast<uint32_t>(0)}}
            },
            SourceLocation{0, newExpr.line, newExpr.column}
        });

        this->emit(Instruction{Opcode::CLT,
            {},
            SourceLocation{0, newExpr.line, newExpr.column}
        });

        this->emit(Instruction{Opcode::JEZ,
            {LabelRef{arraySizeNotNegativeLabel}},
            SourceLocation{0, newExpr.line, newExpr.column}
        });

        this->emit(Instruction{Opcode::THROW,
            {ErrorRef::NEGATIVE_ARRAY_SIZE},
            SourceLocation{0, newExpr.arrayDimensions[index]->index->line, newExpr.arrayDimensions[index]->index->column}
        });

        this->emit(LabelDef{arraySizeNotNegativeLabel});

        // =======================================================
        // convert index expr to type ui32
        // =======================================================

        this->compileTypeConversionIfRequired(
            toAssemblyType(newExpr.arrayDimensions[index]->index->resultingType),
            AssemblyType::UI32,
            SourceLocation{0, newExpr.arrayDimensions[index]->index->line, newExpr.arrayDimensions[index]->index->column}
        );
    }

    this->compileArrayAlloc(scope, newExpr, 0, *(new SemanticType{newExpr.typeInfo->type, dimension}));

    // =======================================================
    // pop remaining index expressions
    // =======================================================

    if (newExpr.arrayDimensions.size() == 2) {
        // [nested_array_length, root_array_ptr]]

        this->emit(Instruction{Opcode::SWAP,
            {},
            SourceLocation{0, newExpr.line, newExpr.column}
        });
        // [root_array_ptr, nested_array_length]

    } else if (newExpr.arrayDimensions.size() > 2) {
        // [..., nested_2_array_length, nested_array_length, root_array_ptr]

        this->emit(Instruction{Opcode::ROTU,
            {Immediate{Number{newExpr.arrayDimensions.size()}}},
            SourceLocation{0, newExpr.line, newExpr.column}
        });
        // [root_array_ptr, ..., nested_2_array_length, nested_array_length]
    }

    for (int i = 0; i < newExpr.arrayDimensions.size() - 1; i++) {
        this->emit(Instruction{Opcode::POP,
            {},
            SourceLocation{0, newExpr.line, newExpr.column}
        });
    }

    // [root_array_ptr]

    if (newExpr.optionalInitialiser != nullptr && newExpr.optionalInitialiser->elements.size() > 0) {
        // initialise array
        this->emit(Instruction{Opcode::DUP,
            {Immediate{Number{static_cast<uint32_t>(0)}}},
            SourceLocation{0, newExpr.line, newExpr.column}
        });
        // [root_array_ptr, root_array_ptr]

        const uint8_t numberOfBitsOfDeepestElement = SemanticType{newExpr.typeInfo->type, newExpr.typeInfo->dimension}.getSize();
        this->compileArrayInitialiser(scope, *newExpr.optionalInitialiser, SourceLocation{0, newExpr.optionalInitialiser->line, newExpr.optionalInitialiser->column}, numberOfBitsOfDeepestElement);
        // [root_array_ptr]
    }
}

void compiler::AssemblyGenerator::compileArrayAlloc(Scope *scope, const ast::ExprNew& newExpr, unsigned int depth, SemanticType& typeAtDepth) {
    const auto newExprSourceLocation = new SourceLocation{0, newExpr.line, newExpr.column};

    // [root_array_length]
    // [ui32             ]

    // =======================================================
    // calculate number of bytes to allocate
    // =======================================================

    this->emit(Instruction{Opcode::DUP,
        {Immediate{Number{static_cast<uint32_t>(0)}}},
        *newExprSourceLocation
    });
    // [root_array_length, root_array_length]
    // [ui32             , ui32             ]

    this->emit(Instruction{Opcode::PUSH,
        {
            AssemblyType::UI32,
            Immediate{Number{typeAtDepth.getSize()}}
        },
        *newExprSourceLocation
    });
    // [root_array_length, root_array_length, 4   ]
    // [ui32             , ui32             , ui32]

    this->emit(Instruction{Opcode::MUL,
        {},
        *newExprSourceLocation
    });
    // [root_array_length, root_array_length * 4]
    // [ui32             , ui32                 ]

    this->emit(Instruction{Opcode::PUSH,
        {
            AssemblyType::UI32,
            Immediate{Number{static_cast<uint32_t>(4)}}
        },
        *newExprSourceLocation
    });
    // [root_array_length, root_array_length * 4, 4   ]
    // [ui32             , ui32                 , ui32]

    this->emit(Instruction{Opcode::ADD,
        {},
        *newExprSourceLocation
    });
    // [root_array_length, bytes_to_allocate]
    // [ui32             , ui32            ]

    // =======================================================
    // allocate space in heap
    // =======================================================

    this->emit(Instruction{Opcode::ALLOC,
        {},
        *newExprSourceLocation
    });
    // [root_array_length, root_array_ptr]
    // [ui32             , ptr           ]

    // =======================================================
    // store length of array in newly allocated space
    // =======================================================

    this->emit(Instruction{Opcode::DUP,
        {Immediate{Number{static_cast<uint32_t>(0)}}},
        *newExprSourceLocation
    });
    // [root_array_length, root_array_ptr, root_array_ptr]
    // [ui32             , ptr           , ptr           ]

    this->emit(Instruction{Opcode::ROTD,
        {Immediate{Number{static_cast<uint32_t>(3)}}},
        *newExprSourceLocation
    });
    // [root_array_ptr, root_array_ptr, root_array_length]
    // [ptr           , ptr           , ui32             ]

    this->emit(Instruction{Opcode::STORE,
        {},
        *newExprSourceLocation
    });
    // [root_array_ptr]
    // [ptr           ]

    if (typeAtDepth.dimension == 0) {
        return;
    }

    depth++;

    // =======================================================
    // load root_array_length to use as the number of nested arrays to allocate
    // =======================================================

    this->emit(Instruction{Opcode::DUP,
        {Immediate{Number{static_cast<uint32_t>(0)}}},
        *newExprSourceLocation
    });
    // [root_array_ptr, root_array_ptr]
    // [ptr           , ptr           ]

    this->emit(Instruction{Opcode::LOAD,
        {AssemblyType::UI32},
        *newExprSourceLocation
    });
    // [root_array_ptr, root_array_length]
    // [ptr           , ui32             ]

    const auto nestedArrayAllocLoopLabel = this->generateLabel("nested_array_alloc_loop");
    const auto endNestedArrayAllocLoopLabel = this->generateLabel("end_nested_array_alloc_loop");

    this->emit(LabelDef{nestedArrayAllocLoopLabel});

    // =======================================================
    // loop condition (number_of_nested_arrays_left_to_alloc > 0)
    // =======================================================

    // [root_array_ptr, nested_arrays_left_to_alloc]
    // [ptr           , ui32                       ]

    this->emit(Instruction{Opcode::DUP,
        {Immediate{Number{static_cast<uint32_t>(0)}}},
        *newExprSourceLocation
    });
    // [root_array_ptr, nested_arrays_left_to_alloc, nested_arrays_left_to_alloc]
    // [ptr           , ui32                       , ui32                       ]

    this->emit(Instruction{Opcode::JEZ,
        {LabelRef{endNestedArrayAllocLoopLabel}},
        *newExprSourceLocation
    });
    // [root_array_ptr, nested_arrays_left_to_alloc]
    // [ptr           , ui32                       ]

    // =======================================================
    // loop body (allocate root_array_length number of nested_arrays)
    // =======================================================

    // =======================================================
    // calculate ptr to store nested_array_ptr (root_array_ptr + ((root_array_length + 4 - (nested_arrays_left_to_alloc)) * number_of_bytes_per_element))
    // =======================================================

    this->emit(Instruction{Opcode::DUP,
        {Immediate{Number{static_cast<uint32_t>(1)}}},
        *newExprSourceLocation
    });
    // [root_array_ptr, nested_arrays_left_to_alloc, root_array_ptr]
    // [ptr           , ui32                       , ptr           ]

    this->emit(Instruction{Opcode::LOAD,
        {AssemblyType::UI32},
        *newExprSourceLocation
    });
    // [root_array_ptr, nested_arrays_left_to_alloc, root_array_length]
    // [ptr           , ui32                       , ui32             ]

    this->emit(Instruction{Opcode::DUP,
        {Immediate{Number{static_cast<uint32_t>(1)}}},
        *newExprSourceLocation
    });
    // [root_array_ptr, nested_arrays_left_to_alloc, root_array_length, nested_arrays_left_to_alloc]
    // [ptr           , ui32                       , ui32             , ui32                        ]

    this->emit(Instruction{Opcode::SUB,
        {},
        *newExprSourceLocation
    });
    // [root_array_ptr, nested_arrays_left_to_alloc, root_array_length - (nested_arrays_left_to_alloc)]
    // [ptr           , ui32                       , ui32                                             ]

    this->emit(Instruction{Opcode::PUSH,
        {
            AssemblyType::UI32,
            Immediate{Number{typeAtDepth.getSize()}}
        },
        *newExprSourceLocation
    });
    // [root_array_ptr, nested_arrays_left_to_alloc, root_array_length - (nested_arrays_left_to_alloc), 4   ]
    // [ptr           , ui32                       , ui32                                             , ui32]

    this->emit(Instruction{Opcode::MUL,
        {},
        *newExprSourceLocation
    });
    // [root_array_ptr, nested_arrays_left_to_alloc, root_array_length - (nested_arrays_left_to_alloc) * 4]
    // [ptr           , ui32                       , ui32                                                 ]

    this->emit(Instruction{Opcode::PUSH,
        {
            AssemblyType::UI32,
            Immediate{Number{static_cast<uint32_t>(4)}}
        },
        *newExprSourceLocation
    });
    // [root_array_ptr, nested_arrays_left_to_alloc, (root_array_length - (nested_arrays_left_to_alloc) * 4), 4   ]
    // [ptr           , ui32                       , ui32                                                   , ui32]

    this->emit(Instruction{Opcode::ADD,
        {},
        *newExprSourceLocation
    });
    // [root_array_ptr, nested_arrays_left_to_alloc, (root_array_length - (nested_arrays_left_to_alloc) * 4) + 4]
    // [ptr           , ui32                       , ui32                                                       ]

    this->emit(Instruction{Opcode::DUP,
        {Immediate{Number{static_cast<uint32_t>(2)}}},
        *newExprSourceLocation
    });
    // [root_array_ptr, nested_arrays_left_to_alloc, (root_array_length - (nested_arrays_left_to_alloc) * 4) + 4, root_array_ptr]
    // [ptr           , ui32                       , ui32                                                       , ptr           ]

    this->emit(Instruction{Opcode::ADD,
        {},
        *newExprSourceLocation
    });
    // [root_array_ptr, nested_arrays_left_to_alloc, ptr_to_store_nested_array]
    // [ptr           , ui32                       , ptr                      ]

    // =======================================================
    // retrieve nested_array_length
    // =======================================================

    this->emit(Instruction{Opcode::DUP,
        {Immediate{Number{static_cast<uint32_t>((depth * 4) - 1)}}},
        *newExprSourceLocation
    });
    // [root_array_ptr, nested_arrays_left_to_alloc, ptr_to_store_nested_array, nested_array_length]
    // [ptr           , ui32                       , ptr                      , ui32               ]

    // =======================================================
    // allocate nested array
    // =======================================================

    typeAtDepth.dimension--;
    this->compileArrayAlloc(scope, newExpr, depth, typeAtDepth);
    // [root_array_ptr, nested_arrays_left_to_alloc, ptr_to_store_nested_array, nested_array_ptr]
    // [ptr           , ui32                       , ptr                      , ptr             ]

    // =======================================================
    // store nested array
    // =======================================================

    this->emit(Instruction{Opcode::STORE,
        {},
        *newExprSourceLocation
    });
    // [root_array_ptr, nested_arrays_left_to_alloc]
    // [ptr           , ui32                       ]

    // =======================================================
    // decrement counter (number_of_nested_arrays_left_to_alloc)
    // =======================================================

    this->emit(Instruction{Opcode::PUSH,
        {
            AssemblyType::UI32,
            Immediate{Number{static_cast<uint32_t>(1)}}
        },
        *newExprSourceLocation
    });
    // [root_array_ptr, nested_arrays_left_to_alloc, 1   ]
    // [ptr           , ui32                       , ui32]

    this->emit(Instruction{Opcode::SUB,
        {},
        *newExprSourceLocation
    });
    // [root_array_ptr, nested_arrays_left_to_alloc - 1]
    // [ptr           , ui32                           ]

    // =======================================================
    // jump to loop condition
    // =======================================================

    this->emit(Instruction{Opcode::JMP,
        {LabelRef{nestedArrayAllocLoopLabel}},
        *newExprSourceLocation
    });

    this->emit(LabelDef{endNestedArrayAllocLoopLabel});

    this->emit(Instruction{Opcode::POP,
        {},
        *newExprSourceLocation
    });
    // [root_array_ptr]
    // [ptr           ]
}

void compiler::AssemblyGenerator::compileArrayInitialiser(Scope *scope, const ast::ArrayInitialiser& arrayInitialiser, const SourceLocation& optionalInitialiserSource, const uint8_t numberOfBitsOfDeepestElement) {

    if (arrayInitialiser.elements.size() == 0) {
        this->emit(Instruction{Opcode::POP,
            {},
            optionalInitialiserSource
        });
        return;
    }

    const auto validArrayInitialiserSize = this->generateLabel("valid_array_initialiser_size");

    // [root_array_ptr]
    // [ptr           ]

    // =======================================================
    // check length of array initialiser is not larger than the array's length
    // =======================================================

    this->emit(Instruction{Opcode::DUP,
        {Immediate{Number{static_cast<uint32_t>(0)}}},
        optionalInitialiserSource
    });

    // [root_array_ptr, root_array_ptr]
    // [ptr           , ptr           ]

    this->emit(Instruction{Opcode::LOAD,
        {AssemblyType::UI32},
        optionalInitialiserSource
    });
    // [root_array_ptr, root_array_length]
    // [ptr           , ui32             ]

    this->emit(Instruction{Opcode::PUSH,
        {
            AssemblyType::UI32,
            Immediate{Number{static_cast<uint32_t>(arrayInitialiser.elements.size())}}
        },
        optionalInitialiserSource
    });
    // [root_array_ptr, root_array_length, root_array_initialiser_length]
    // [ptr           , ui32             , ui32                         ]

    this->emit(Instruction{Opcode::CLT,
        {},
        optionalInitialiserSource
    });
    // [root_array_ptr, root_array_length < root_array_initialiser_length]
    // [ptr           , ui32                                             ]

    this->emit(Instruction{Opcode::JEZ,
        {LabelRef{validArrayInitialiserSize}},
        optionalInitialiserSource
    });
    // [root_array_ptr]
    // [ptr           ]

    this->emit(Instruction{Opcode::LOAD,
        {AssemblyType::UI32},
        optionalInitialiserSource
    });
    // [root_array_length]
    // [ui32             ]

    this->emit(Instruction{Opcode::THROW,
        {ErrorRef::ARRAY_INITIALISER_SIZE},
        SourceLocation{0, arrayInitialiser.line, arrayInitialiser.column}
    });
    // []
    // []


    this->emit(LabelDef{validArrayInitialiserSize});
    // [root_array_ptr]
    // [ptr           ]

    // =======================================================
    // move pointer to first element in array
    // =======================================================

    this->emit(Instruction{Opcode::PUSH,
        {
            AssemblyType::UI32,
            Immediate{Number{static_cast<uint32_t>(4)}}
        },
        optionalInitialiserSource
    });
    // [root_array_ptr, 4   ]
    // [ptr           , ui32]

    this->emit(Instruction{Opcode::ADD,
        {},
        optionalInitialiserSource
    });
    // [ptr_to_first_element_in_root_array]
    // [ptr                               ]

    // =======================================================
    // compile each array initialiser element
    // =======================================================

    for (int index = 0; index < arrayInitialiser.elements.size(); index++) {

        // only dup if index does not refer to the last element
        // this is so the pointer is consumed once the last element has been compiled
        if (index < arrayInitialiser.elements.size() - 1) {
            this->emit(Instruction{Opcode::DUP,
                {Immediate{Number{static_cast<uint32_t>(0)}}},
                optionalInitialiserSource
            });
        }
        // [ptr_to_element     , ptr_to_element]
        // [ptr                , ptr           ]
        // [if not last element,               ]

        if (std::holds_alternative<std::unique_ptr<ast::ArrayInitialiser>>(arrayInitialiser.elements[index])) {
            // not at deepest level of initialiser

            this->emit(Instruction{Opcode::LOAD,
                {AssemblyType::PTR},
                optionalInitialiserSource
            });

            // [ptr_to_element     , ptr_to_nested_array]
            // [ptr                , ptr                ]
            // [if not last element,                    ]

            if (std::get<std::unique_ptr<ast::ArrayInitialiser>>(arrayInitialiser.elements[index])->elements.size() > 0) {
                this->compileArrayInitialiser(scope, *std::get<std::unique_ptr<ast::ArrayInitialiser>>(arrayInitialiser.elements[index]), optionalInitialiserSource, numberOfBitsOfDeepestElement);
            } else {
                this->emit(Instruction{Opcode::POP,
                    {},
                    optionalInitialiserSource
                });
            }

            // [ptr_to_element     ]
            // [ptr                ]
            // [if not last element]

            // =======================================================
            // move pointer to next element in array if not at the last element
            // =======================================================

            if (index < arrayInitialiser.elements.size() - 1) {
                this->emit(Instruction{Opcode::PUSH,
                    {
                        AssemblyType::UI32,
                        Immediate{Number{static_cast<uint32_t>(4)}}
                    },
                    optionalInitialiserSource
                });
                // [ptr_to_element, 4]
                // [ptr           , ui32]

                this->emit(Instruction{Opcode::ADD,
                    {},
                    optionalInitialiserSource
                });
                // [ptr_to_next_element]
                // [ptr                ]
            }

        } else {
            // at deepest level of initialiser

            // [ptr_to_element     , ptr_to_element]
            // [ptr                , ptr           ]
            // [if not last element,               ]

            this->compileExpr(scope, *std::get<std::unique_ptr<ast::Expr>>(arrayInitialiser.elements[index]));
            // [ptr_to_element     , ptr_to_element, value_to_store        ]
            // [ptr                , ptr           , type_of_value_to_store]
            // [if not last element,                                       ]

            this->emit(Instruction{Opcode::STORE,
                {},
                optionalInitialiserSource
            });
            // [ptr_to_element     ]
            // [ptr                ]
            // [if not last element]

            // =======================================================
            // move pointer to next element in array if not at the last element
            // =======================================================

            if (index < arrayInitialiser.elements.size() - 1) {
                this->emit(Instruction{Opcode::PUSH,
                    {
                        AssemblyType::UI32,
                        Immediate{Number{static_cast<uint32_t>(numberOfBitsOfDeepestElement)}}
                    },
                    optionalInitialiserSource
                });
                // [ptr_to_element, 4]
                // [ptr           , ui32]

                this->emit(Instruction{Opcode::ADD,
                    {},
                    optionalInitialiserSource
                });
                // [ptr_to_next_element]
                // [ptr                ]
            }
        }
    }
}

void compiler::AssemblyGenerator::compileFunctionCall(Scope* scope, const ast::FunctionCall& functionCall) {
    // compile arguments
    for (size_t argIndex = 0; argIndex < functionCall.arguments.size(); argIndex++) {
        this->compileExpr(scope, *functionCall.arguments[argIndex]);

        // convert any arguments that require implicit conversion
        this->compileTypeConversionIfRequired(functionCall.arguments[argIndex]->resultingType.type, functionCall.functionSymbol->parameterTypes[argIndex].type, SourceLocation{0, functionCall.arguments[argIndex]->line, functionCall.arguments[argIndex]->column});
    }
    if (functionCall.functionSymbol->builtinId == BuiltinFunctionId::NONE) { // function is user-defined
        // call user-defined function
        this->emit(Instruction{Opcode::CALL,
            {LabelRef{functionCall.functionSymbol->label}},
            SourceLocation{0, functionCall.line, functionCall.column}
        });
    } else { // function is builtin
        // call builtin function
        this->emit(Instruction{Opcode::CALL,
            {LabelRef{Builtins::getBuiltinFunctionLabel(functionCall.functionSymbol->builtinId)}},
            SourceLocation{0, functionCall.line, functionCall.column}
        });
        this->requiredBuiltinFunctions.insert(functionCall.functionSymbol->builtinId);
    }
}

void compiler::AssemblyGenerator::compileExprIdentifier(Scope* scope, const ast::ExprVarAccess& identifier) {
    const auto symbol = scope->lookup(identifier.name).value();

    if (symbol->isGlobal()) {
        // push global variable onto stack
        this->emit(Instruction{Opcode::LOADG,
            {LabelRef{identifier.name}},
            SourceLocation{0, identifier.line, identifier.column}
        });
    } else {
        // push local variable onto stack
        this->emit(Instruction{Opcode::LOADL,
            {
                toAssemblyType(SemanticType{symbol->type, symbol->dimension}),
                Immediate{Number{symbol->localSlot}}
            },
            SourceLocation{0, identifier.line, identifier.column}
        });
    }

    if (identifier.indices.size() > 0) {

        SemanticType typeAtDepth = SemanticType{symbol->type, symbol->dimension};

        for (const auto& index : identifier.indices) {

            typeAtDepth.dimension--;

            this->compileArrayIndex(scope, *index, typeAtDepth);

            // =============================================================
            // load array element
            // =============================================================

            this->emit(Instruction{Opcode::LOAD,
                {toAssemblyType(typeAtDepth)},
                SourceLocation{0, index->line, index->column}
            });
        }
    }
}

void compiler::AssemblyGenerator::compileExprIntegerLiteral(const ast::ExprIntegerLiteral& integerLiteral) {
    // push integer value onto stack
    this->emit(Instruction{Opcode::PUSH,
        {
            AssemblyType::I32,
            Immediate{Number{integerLiteral.value}}
        },
        SourceLocation{0, integerLiteral.line, integerLiteral.column}
    });
}

void compiler::AssemblyGenerator::compileExprFloatLiteral(const ast::ExprFloatLiteral& floatLiteral) {
    // push float value onto stack
    this->emit(Instruction{Opcode::PUSH,
        {
            AssemblyType::F32,
            Immediate{Number{static_cast<float>(floatLiteral.value)}}
        },
        SourceLocation{0, floatLiteral.line, floatLiteral.column}
    });
}

void compiler::AssemblyGenerator::compileExprBoolLiteral(const ast::ExprBoolLiteral& boolLiteral) {
    // push bool value onto stack
    this->emit(Instruction{Opcode::PUSH,
        {
            AssemblyType::UI32,
            Immediate{Number{boolLiteral.value ? 1 : 0}} // push 1 if 'true', push 0 if 'false'
        },
        SourceLocation{0, boolLiteral.line, boolLiteral.column}
    });
}

void compiler::AssemblyGenerator::compileTypeConversionIfRequired(const Type currentType, const Type newType, const SourceLocation& sourceLocation) {
    // convert to new type if current type is different from the new type
    this->compileTypeConversionIfRequired(toAssemblyType(SemanticType{currentType, 0}), toAssemblyType(SemanticType{newType, 0}), sourceLocation);
}

void compiler::AssemblyGenerator::compileTypeConversionIfRequired(const AssemblyType currentType, const AssemblyType newType, const SourceLocation& sourceLocation) {
    // convert to new type if current type is different from the new type
    if (currentType != newType) {
        this->emit(Instruction{Opcode::CONV,
            {newType},
            sourceLocation
        });
    }
}

void compiler::AssemblyGenerator::compileBuiltinFunctions() {
    for (const auto& builtinFunctionId : this->requiredBuiltinFunctions) {
        const BuiltinFunction* builtinFunction = Builtins::getBuiltinFunction(builtinFunctionId);

        // track the required builtin data for this builtin function
        for (const auto& builtinData : builtinFunction->requiredBuiltinData) {
            this->requiredBuiltinData.insert(builtinData);
        }
    }
}

void compiler::AssemblyGenerator::compileGlobalVariables() {
    // start of data section
    this->emit(Directive::DATA);

    auto globalVariables = this->symbolTable->getGlobalVariables();
    for (auto it = globalVariables.begin(); it != globalVariables.end(); ++it) {
        // global static data definition
        this->emit(DataDef{
            it->first,
            toAssemblyType(SemanticType{it->second.type, it->second.dimension}),
            getDefaultNumber(SemanticType{it->second.type, it->second.dimension})
        });
    }
}

std::string compiler::AssemblyGenerator::typeToString(const Type& type) {
    switch (type) {
        case Type::INT : return "i32";
        case Type::FLOAT: return "f32";
        case Type::BOOL: return "ui32";
    }
}

std::string compiler::AssemblyGenerator::generateLabel(const std::string& label) {
    std::string newLabel = label + "_" + std::to_string(this->labelCounter);
    this->labelCounter++;
    return newLabel;
}

std::string compiler::AssemblyGenerator::generateScopeFunctionIdentifier(const uint32_t scopeFunctionNumber) {
    return "__Scope__" + std::to_string(scopeFunctionNumber);
}

void compiler::AssemblyGenerator::emit(const AssemblyItem& assemblyItem) {
    this->assembly.push_back(assemblyItem);
}

compiler::AssemblyType compiler::AssemblyGenerator::toAssemblyType(const SemanticType& type) {
    if (type.dimension > 0) return AssemblyType::PTR;
    switch (type.type) {
        case Type::INT: return AssemblyType::I32;
        case Type::FLOAT: return AssemblyType::F32;
        case Type::BOOL: return AssemblyType::UI32;
    }
}

compiler::DataValue compiler::AssemblyGenerator::getDefaultNumber(const SemanticType& type) {
    if (type.dimension > 0) return Number{static_cast<uint32_t>(0)};
    switch (type.type) {
        case Type::INT: return Number{static_cast<uint32_t>(0)};
        case Type::FLOAT: return Number{0.0f};
        case Type::BOOL: return Number{static_cast<uint32_t>(0)};
    }
}
