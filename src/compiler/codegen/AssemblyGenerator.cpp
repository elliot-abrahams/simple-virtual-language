#include "AssemblyGenerator.h"

#include <stdexcept>
#include <bits/fs_fwd.h>

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
    this->compileFunctionDeclaration(
        functionDecl.functionSymbol->builtinId == BuiltinFunctionId::NONE ? MethodDefType::USER : MethodDefType::BUILTIN,
        functionDecl.functionSymbol->label,
        *functionDecl.body,
        functionDecl.parameters.size(),
        functionDecl.body->scope->calculateNumberOfLocalSlots(),
        functionDecl.returnTypeInfo->type.typeId == VOID_TYPE_ID,
        functionDecl.line,
        functionDecl.column,
        functionDecl.body->blockEndLine,
        functionDecl.body->blockEndColumn
    );
}

void compiler::AssemblyGenerator::compileFunctionDeclaration(
    const MethodDefType methodType,
    const std::string& functionIdentifier,
    const ast::Stm& body,
    const uint8_t numberOfArguments,
    const uint32_t numberOfLocals,
    const bool includeDefaultReturn,
    const uint32_t line,
    const uint16_t column,
    const uint32_t functionBodyEndLine,
    const uint16_t functionBodyEndColumn
) {
    // compile function header

    this->emit(MethodDef{
        functionIdentifier,
        numberOfArguments,
        numberOfLocals,
        methodType,
        SourceLocation{0, line, column}
    });

    // compile body
    if (auto* block = dynamic_cast<const ast::Block*>(&body)) {
        for (const auto& stm : block->statements) {
            this->compileStm(block->scope, *stm);
        }
    } else {
        auto* forStm = dynamic_cast<const ast::ForStm*>(&body);
        this->compileForStatement(forStm->body->scope, *forStm);
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
    this->isCompilingScopeFunctions = true;
    for (int scopeCounter = 0; scopeCounter < this->pendingScopeFunctions.size(); scopeCounter++) {
        if (std::holds_alternative<const ast::Block*>(this->pendingScopeFunctions.at(scopeCounter))) {
            const auto* block = std::get<const ast::Block*>(this->pendingScopeFunctions.at(scopeCounter));

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
        } else {
            // forStm
            const auto* forStm = std::get<const ast::ForStm*>(this->pendingScopeFunctions.at(scopeCounter));

            // compile a function declaration for this scope
            this->compileFunctionDeclaration(
                MethodDefType::SCOPE,
                generateScopeFunctionIdentifier(scopeCounter),
                *forStm,
                0,
                forStm->body->scope->calculateNumberOfLocalSlots(),
                true,
                forStm->line,
                forStm->column,
                forStm->body->blockEndLine,
                forStm->body->blockEndColumn
            );
        }
    }
    this->isCompilingScopeFunctions = false;
}

void compiler::AssemblyGenerator::compileArrayIndex(Scope *scope, const ast::Index &index, Type& typeAtDepth) {
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

    this->compileExpr(scope, *index.index, ExprResult::VALUE);
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

    this->compileTypeConversionIfRequired(toAssemblyType(index.index->resultingType), AssemblyType::UI32, *indexSourceLocation);

    // =============================================================
    // throw index out of range error if index >= array_length
    // =============================================================

    this->emit(Instruction{Opcode::DUP,
        {Immediate{Number{static_cast<uint32_t>(1)}}},
        *indexSourceLocation
    });
    // [root_array_ptr, root_array_length, array_index, root_array_length]

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
    else if (auto* forStm = dynamic_cast<const ast::ForStm*>(&stm)) {
        this->compileForStatement(scope, *forStm);
    }
    else if (auto* continueStm = dynamic_cast<const ast::ContinueStm*>(&stm)) {
        this->compileContinueStatement(scope, *continueStm);
    }
    else if (auto* breakStm = dynamic_cast<const ast::BreakStm*>(&stm)) {
        this->compileBreakStatement(scope, *breakStm);
    }
    else if (auto* returnStm = dynamic_cast<const ast::ReturnStm*>(&stm)) {
        this->compileReturnStatement(scope, *returnStm);
    }
    else if (auto* expressionStatement = dynamic_cast<const ast::ExpressionStatement*>(&stm)) {
        this->compileExpressionStatement(scope, *expressionStatement);
    }
}

void compiler::AssemblyGenerator::compileBlock(const ast::Block& block) {
    // if scope declared in global scope and not a scope within a function
    if (block.scope->parent->isGlobalScope()) {
        this->registerScopeFunction(&block, block.blockEndLine, block.blockEndColumn);
    } else {
        for (auto& stm : block.statements) {
            this->compileStm(block.scope, *stm);
        }
    }
}

void compiler::AssemblyGenerator::compileStmVarDecl(Scope* scope, const ast::StmVarDecl& varDecl) {
    if (varDecl.optionalInitialiser != nullptr) {
        this->compileExpr(scope, *varDecl.optionalInitialiser, ExprResult::VALUE);

        const auto symbol = scope->lookup(varDecl.identifier->name).value();

        // convert expr to variable type
        this->compileTypeConversionIfRequired(varDecl.optionalInitialiser->resultingType, symbol->type, SourceLocation{0, varDecl.optionalInitialiser->line, varDecl.optionalInitialiser->column});

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

    const auto symbol = scope->lookup(assignment.identifier->name).value();


    if (assignment.indices.size() == 0) {
        this->compileExpr(scope, *assignment.expression, ExprResult::VALUE);
        // convert expr to variable type
        this->compileTypeConversionIfRequired(assignment.expression->resultingType, symbol->type, SourceLocation{0, assignment.expression->line, assignment.expression->column});

        if (symbol->isGlobal()) {
            // store result of expression in global variable
            this->emit(Instruction{Opcode::STOREG,
                {LabelRef{assignment.identifier->name}},
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
                {LabelRef{assignment.identifier->name}},
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

        Type typeAtDepth = symbol->type;

        for (int index = 0; index < assignment.indices.size(); ++index) {

            typeAtDepth.dimension--;

            this->compileArrayIndex(scope, *assignment.indices[index], typeAtDepth);

            if (index < assignment.indices.size() - 1) {

                // =============================================================
                // load array element if not the deepest index
                // =============================================================

                this->emit(Instruction{Opcode::LOAD,
                    {AssemblyType::PTR},
                    SourceLocation{0, assignment.indices[index]->line, assignment.indices[index]->column}
                });
            }
        }

        // =============================================================
        // store expression
        // =============================================================

        this->compileExpr(scope, *assignment.expression, ExprResult::VALUE);
        // convert expr to variable type
        this->compileTypeConversionIfRequired(assignment.expression->resultingType, symbol->type, SourceLocation{0, assignment.expression->line, assignment.expression->column});


        this->emit(Instruction{Opcode::STORE,
            {},
            SourceLocation{0, assignment.line, assignment.column}
        });
    }
}

void compiler::AssemblyGenerator::compileIfStatement(Scope* scope, const ast::IfStm& ifStm) {
    this->compileExpr(scope, *ifStm.condition, ExprResult::VALUE);

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
    const std::string startWhileLabel = this->generateLabel("start_while");
    const std::string endWhileLabel = this->generateLabel("end_while");

    whileStm.body->scope->loopContext = new LoopContext{startWhileLabel, endWhileLabel};

    this->emit(LabelDef{startWhileLabel});

    this->compileExpr(scope, *whileStm.condition, ExprResult::VALUE);

    // skip block if condition is false
    this->emit(Instruction{Opcode::JEZ,
        {LabelRef{endWhileLabel}},
        SourceLocation{0, whileStm.line, whileStm.column}
    });

    this->compileBlock(*whileStm.body); // compile while block

    // jump to start of while (evaluate condition again)
    this->emit(Instruction{Opcode::JMP,
        {LabelRef{startWhileLabel}},
        SourceLocation{0, whileStm.line, whileStm.column}
    });

    this->emit(LabelDef{endWhileLabel});
}

void compiler::AssemblyGenerator::compileForStatement(Scope* scope, const ast::ForStm& forStm) {
    if (!isCompilingScopeFunctions) {
        // to prevent a forVariable that gets declared within the nested scope from being accessed from a global scope context
        // -> add forStm to pendingScopeFunctions
        this->registerScopeFunction(&forStm, forStm.body->blockEndLine, forStm.body->blockEndColumn);
        return;
    }

    const auto forSourceLocation = SourceLocation{0, forStm.line, forStm.column};

    const bool forHasVarDecl = std::holds_alternative<std::unique_ptr<ast::StmVarDecl>>(forStm.variable);
    Type* forVariableType;
    const bool forHasRange = std::holds_alternative<std::unique_ptr<ast::ForRange>>(forStm.iterable);
    bool rangeHasStep = false;

    const std::unique_ptr<ast::StmVarDecl>* forVarDecl;
    const std::unique_ptr<ast::ExprIdentifier>* forVariable;
    const std::unique_ptr<ast::ForRange>* forRange;
    const std::unique_ptr<ast::Expr>* forIterable;

    if (forHasVarDecl) {
        forVarDecl = &std::get<std::unique_ptr<ast::StmVarDecl>>(forStm.variable);
        forVariableType = &forVarDecl->get()->typeInfo->type;
    } else {
        forVariable = &std::get<std::unique_ptr<ast::ExprIdentifier>>(forStm.variable);
        forVariableType = &forVariable->get()->resultingType;
    }

    if (forHasRange) {
        forRange = &std::get<std::unique_ptr<ast::ForRange>>(forStm.iterable);
    } else {
        forIterable = &std::get<std::unique_ptr<ast::Expr>>(forStm.iterable);
    }

    if (forHasRange) {
        rangeHasStep = forRange->get()->step != nullptr;
    }

    std::string forLoopStepValidLabel;
    if (rangeHasStep) {
        forLoopStepValidLabel = this->generateLabel("for_loop_step_valid");
    }

    const auto forLoopStartLabel = this->generateLabel("for_loop_start");

    std::string forLoopNegativeStepCondition;
    std::string forLoopConditionJoinLabel;
    if (rangeHasStep) {
        forLoopNegativeStepCondition = this->generateLabel("for_loop_negative_step_condition");
        forLoopConditionJoinLabel = this->generateLabel("for_loop_condition_join");
    }
    const auto forLoopContinueLabel = this->generateLabel("for_loop_continue");
    const auto forLoopEndLabel = this->generateLabel("for_loop_end");

    forStm.body->scope->parent->loopContext = new LoopContext{forLoopContinueLabel, forLoopEndLabel};

    // ====================================================
    // compile step
    // ====================================================
    if (forHasRange) {
        if (rangeHasStep) {
            // compile step expr
            this->compileExpr(scope, *forRange->get()->step, ExprResult::VALUE);
            this->compileTypeConversionIfRequired(forRange->get()->step->resultingType, *forVariableType, SourceLocation{0, forRange->get()->step->line, forRange->get()->step->column});
            // [range_step]

            // ====================================================
            // throw runtime error if step is zero
            // ====================================================

            this->emit(Instruction{Opcode::DUP,
                {Immediate{static_cast<uint32_t>(0)}},
                SourceLocation{0, forRange->get()->step->line, forRange->get()->step->column}
            });
            // [range_step, range_step]

            this->emit(Instruction{Opcode::JNZ,
                {LabelRef{forLoopStepValidLabel}},
                SourceLocation{0, forRange->get()->step->line, forRange->get()->step->column}
            });
            // [range_step]

            this->emit(Instruction{Opcode::THROW,
                {ErrorRef::ZERO_RANGE_STEP},
                SourceLocation{0, forRange->get()->step->line, forRange->get()->step->column}
            });

            this->emit(LabelDef{forLoopStepValidLabel});
            // [range_step]

            // ====================================================
            // check if range_step is negative or positive
            // positive = 1
            // negative = 0
            // ====================================================

            this->emit(Instruction{Opcode::DUP,
                {Immediate{Number{static_cast<uint32_t>(0)}}},
                SourceLocation{0, forRange->get()->step->line, forRange->get()->step->column}
            });
            // [range_step, range_step]

            this->emit(Instruction{Opcode::PUSH,
                {
                    toAssemblyType(*forVariableType),
                    Immediate{getNumber(toAssemblyType(*forVariableType), 0)}
                },
                 SourceLocation{0, forRange->get()->step->line, forRange->get()->step.get()->column}
            });
            // [range_step, range_step, 0]

            this->emit(Instruction{Opcode::CGE,
                {},
                SourceLocation{0, forRange->get()->step->line, forRange->get()->step->column}
            });
            // [range_step, is_range_step_positive]

            this->emit(Instruction{Opcode::SWAP,
                {},
                forSourceLocation
            });
            // [is_range_step_positive, range_step]

        } else {
            // for with a range but with no step -> range_step is defaulted to a value of 1
            this->emit(Instruction{Opcode::PUSH,
                {
                    toAssemblyType(*forVariableType),
                    Immediate{getNumber(*forVariableType, 1)}
                },
                forSourceLocation
            });
        }

    } else {
        // for with an iterable -> range_step is defaulted to a value of 1 of type ui32

        // ====================================================
        // get first element of iterable
        // ====================================================

        this->compileExpr(scope, **forIterable, ExprResult::PTR);
        // [iterable_ptr]

        this->emit(Instruction{Opcode::DUP,
             {Immediate{Number{static_cast<uint32_t>(0)}}},
            forSourceLocation
        });
        // [iterable_ptr, iterable_ptr]

        this->emit(Instruction{Opcode::PUSH,
            {
                AssemblyType::UI32,
                Immediate{static_cast<uint32_t>(4)}
            },
            forSourceLocation
        });
        // [iterable_ptr, iterable_ptr, 4]

        this->emit(Instruction{Opcode::ADD,
            {},
            forSourceLocation
        });
        // [iterable_ptr, first_element_of_iterable_ptr]

        this->emit(Instruction{Opcode::PUSH,
            {
                AssemblyType::UI32,
                Immediate{getNumber(AssemblyType::UI32, 1)}
            },
            forSourceLocation
        });
        // [iterable_ptr, first_element_of_iterable_ptr, 1]
    }
    // [is_range_step_positive           , iterable_ptr            , first_element_of_iterable_ptr, range_step]
    // [if for loop has user defined step, if for loop has iterable, if for loop has iterable     ,           ]

    // ====================================================
    // compile range end
    // ====================================================

    if (forHasRange) {
        // range_end is defined by the user
        this->compileExpr(scope, *forRange->get()->end, ExprResult::VALUE);
        this->compileTypeConversionIfRequired(forRange->get()->end->resultingType, *forVariableType, SourceLocation{0, forRange->get()->line, forRange->get()->column});
    } else {
        // range end is the length of the iterator (array's length)

        // [iterable_ptr, first_element_of_iterable_ptr, range_step]

        this->emit(Instruction{Opcode::ROTD,
            {Immediate{Number{static_cast<uint32_t>(3)}}},
            forSourceLocation
        });
        // [first_element_of_iterable_ptr, range_step, iterable_ptr]

        this->emit(Instruction{Opcode::LOAD,
            {
                AssemblyType::UI32
            },
            SourceLocation{0, forIterable->get()->line, forIterable->get()->column}
        });
        // [first_element_of_iterable_ptr, range_step, iterable_length]
    }
    // [is_range_step_positive           , first_element_of_iterable_ptr, range_step, range_end]
    // [if for loop has user defined step, if for loop has iterable     ,           ,          ]

    // ====================================================
    // get pointer of for_variable
    // ====================================================

    if (forHasVarDecl) {
        // create identifierExpr from VarDecl
        auto identifierExpr = std::make_unique<ast::ExprIdentifier>(
            forVarDecl->get()->line,
            forVarDecl->get()->column,
            std::make_unique<ast::Identifier>(
                forVarDecl->get()->line,
                forVarDecl->get()->column,
                forVarDecl->get()->identifier->name
            )
        );
        this->compileExprIdentifier(forStm.body->scope, *identifierExpr, ExprResult::PTR);

    } else {
        // compile identifierExpr
        this->compileExprIdentifier(scope, **forVariable, ExprResult::PTR);
    }
    // [is_range_step_positive           , first_element_of_iterable_ptr, range_step, range_end, for_variable_ptr]
    // [if for loop has user defined step, if for loop has iterable     ,           ,          ,                 ]

    // ====================================================
    // compile range start
    // ====================================================

    if (forHasRange) {
        this->compileExpr(scope, *forRange->get()->start.get(), ExprResult::VALUE);
        this->compileTypeConversionIfRequired(forRange->get()->start->resultingType, *forVariableType, SourceLocation{0, forRange->get()->start->line, forRange->get()->column});
    } else {
        // for with an iterable -> range_start is defaulted to 0
        this->emit(Instruction{Opcode::PUSH,
            {
                AssemblyType::UI32,
                Immediate{getNumber(AssemblyType::UI32, 0)}
            },
            SourceLocation{0, forIterable->get()->line, forIterable->get()->column}
        });
    }
    // [is_range_step_positive           , first_element_of_iterable_ptr, range_step, range_end, for_variable_ptr, range_start]
    // [if for loop has user defined step, if for loop has iterable     ,           ,          ,                 ,            ]

    // ====================================================
    // start for loop
    // ====================================================

    this->emit(LabelDef{forLoopStartLabel});

    // [is_range_step_positive           , iterable_element_ptr    , range_step, range_end, for_variable_ptr, range_current]
    // [if for loop has user defined step, if for loop has iterable,           ,          ,                 ,              ]

    this->emit(Instruction{Opcode::DUP,
        {Immediate{Number{static_cast<uint32_t>(2)}}},
        forSourceLocation
    });
    // [is_range_step_positive           , iterable_element_ptr    , range_step, range_end, for_variable_ptr, range_current, range_end]
    // [if for loop has user defined step, if for loop has iterable,           ,          ,                 ,              ,          ]

    this->emit(Instruction{Opcode::DUP,
        {Immediate{Number{static_cast<uint32_t>(1)}}},
        forSourceLocation
    });
    // [is_range_step_positive           , iterable_element_ptr    , range_step, range_end, for_variable_ptr, range_current, range_end, range_current]
    // [if for loop has user defined step, if for loop has iterable,           ,          ,                 ,              ,          ,              ]

    if (rangeHasStep) {

        // ====================================================
        // jump to appropriate condition check
        // if range_step is positive -> use instruction cle
        // if range_step is negative -> use instruction cge
        // ====================================================

        // [is_range_step_positive, range_step, range_end, for_variable_ptr, range_current, range_end, range_current]

        this->emit(Instruction{Opcode::DUP,
            {Immediate{Number{static_cast<uint32_t>(6)}}},
            forSourceLocation
        });
        // [is_range_step_positive, range_step, range_end, for_variable_ptr, range_current, range_end, range_current, is_range_step_positive]

        this->emit(Instruction{Opcode::JEZ,
            {LabelRef{forLoopNegativeStepCondition}},
            forSourceLocation
        });
        // [is_range_step_positive, range_step, range_end, for_variable_ptr, range_current, range_end, range_current]
    }

    // ====================================================
    // compile condition with positive step
    // ====================================================

    // [is_range_step_positive           , iterable_element_ptr    , range_step, range_end, for_variable_ptr, range_current, range_end, range_current]
    // [if for loop has user defined step, if for loop has iterable,           ,          ,                 ,              ,          ,              ]

    this->emit(Instruction{Opcode::CLE,
        {},
        forSourceLocation
    });
    // [is_range_step_positive           , iterable_element_ptr    , range_step, range_end, for_variable_ptr, range_current, range_end <= range_current]
    // [if for loop has user defined step, if for loop has iterable,           ,          ,                 ,              ,                           ]

    this->emit(Instruction{Opcode::JNZ,
        {LabelRef{forLoopEndLabel}},
        forSourceLocation
    });
    // [is_range_step_positive           , iterable_element_ptr    , range_step, range_end, for_variable_ptr, range_current]
    // [if for loop has user defined step, if for loop has iterable,           ,          ,                 ,              ]

    if (rangeHasStep) {
        // step is positive -> jump to end of condition
        this->emit(Instruction{Opcode::JMP,
            {LabelRef{forLoopConditionJoinLabel}},
            forSourceLocation
        });

        // ====================================================
        // compile condition with negative step
        // ====================================================

        this->emit(LabelDef{forLoopNegativeStepCondition});

        // [is_range_step_positive, range_step, range_end, for_variable_ptr, range_current, range_end, range_current]

        this->emit(Instruction{Opcode::CGE,
            {},
            forSourceLocation
        });
        // [is_range_step_positive, range_step, range_end, for_variable_ptr, range_current, range_end >= range_current]

        this->emit(Instruction{Opcode::JNZ,
            {LabelRef{forLoopEndLabel}},
            forSourceLocation
        });
        // [is_range_step_positive, range_step, range_end, for_variable_ptr, range_current]

        this->emit(LabelDef{forLoopConditionJoinLabel});
    }
    // [is_range_step_positive           , iterable_element_ptr    , range_step, range_end, for_variable_ptr, range_current]
    // [if for loop has user defined step, if for loop has iterable,           ,          ,                 ,              ]

    // ====================================================
    // if forHasRange -> store range_current to forVariable
    // else (for has iterable) -> store element stored at index range_current to forVariable
    //                         -> increase iterable_element_ptr to next element
    // ====================================================

    if (forHasRange) {
        // [is_range_step_positive           , range_step, range_end, for_variable_ptr, range_current]
        // [if for loop has user defined step,           ,          ,                 ,              ]

        this->emit(Instruction{Opcode::DUP,
            {Immediate{Number{static_cast<uint32_t>(1)}}},
            forSourceLocation
        });
        // [is_range_step_positive           , range_step, range_end, for_variable_ptr, range_current, for_variable_ptr]
        // [if for loop has user defined step,           ,          ,                 ,              ,                 ]

        this->emit(Instruction{Opcode::DUP,
            {Immediate{Number{static_cast<uint32_t>(1)}}},
            forSourceLocation
        });
        // [is_range_step_positive           , range_step, range_end, for_variable_ptr, range_current, for_variable_ptr, range_current]
        // [if for loop has user defined step,           ,          ,                 ,              ,                 ,              ]

        this->emit(Instruction{Opcode::STORE,
            {},
            forSourceLocation
        });
        // [is_range_step_positive           , range_step, range_end, for_variable_ptr, range_current]
        // [if for loop has user defined step,           ,          ,                 ,              ]

    } else {
        auto iterableElementType = forIterable->get()->resultingType;
        iterableElementType.dimension--;

        // [iterable_element_ptr, range_step, range_end, for_variable_ptr, range_current]

        this->emit(Instruction{Opcode::DUP,
            {Immediate{Number{static_cast<uint32_t>(1)}}},
            forSourceLocation
        });
        // [iterable_element_ptr, range_step, range_end, for_variable_ptr, range_current, for_variable_ptr]

        this->emit(Instruction{Opcode::DUP,
            {Immediate{Number{static_cast<uint32_t>(5)}}},
            forSourceLocation
        });
        // [iterable_element_ptr, range_step, range_end, for_variable_ptr, range_current, for_variable_ptr, iterable_element_ptr]

        this->emit(Instruction{Opcode::LOAD,
            {toAssemblyType(iterableElementType)},
            forSourceLocation
        });
        // [iterable_element_ptr, range_step, range_end, for_variable_ptr, range_current, for_variable_ptr, iterable_element]

        this->emit(Instruction{Opcode::STORE,
            {},
            forSourceLocation
        });
        // [iterable_element_ptr, range_step, range_end, for_variable_ptr, range_current]

        // ================================================
        // move iterable_element_ptr to the next element
        // ================================================

        this->emit(Instruction{Opcode::ROTD,
            {Immediate{Number{static_cast<uint32_t>(5)}}},
            forSourceLocation
        });
        // [range_step, range_end, for_variable_ptr, range_current, iterable_element_ptr]

        this->emit(Instruction{Opcode::PUSH,
            {
                AssemblyType::UI32,
                Immediate{Number{iterableElementType.getSize()}}
            }
        });
        // [range_step, range_end, for_variable_ptr, range_current, iterable_element_ptr, size_of_iterable_element]

        this->emit(Instruction{Opcode::ADD,
            {},
            forSourceLocation
        });
        // [range_step, range_end, for_variable_ptr, range_current, next_iterable_element_ptr]

        this->emit(Instruction{Opcode::ROTU,
            {Immediate{Number{static_cast<uint32_t>(5)}}},
            forSourceLocation
        });
        // [next_iterable_element_ptr, range_step, range_end, for_variable_ptr, range_current]
    }

    // ====================================================
    // compile body
    // ====================================================

    this->compileBlock(*forStm.body);

    this->emit(LabelDef{forLoopContinueLabel});

    // ====================================================
    // add range_step to range_current
    // ====================================================

    this->emit(Instruction{Opcode::DUP,
        {Immediate{Number{static_cast<uint32_t>(3)}}},
        forSourceLocation
    });
    // [is_range_step_positive           , iterable_element_ptr    , range_step, range_end, for_variable_ptr, range_current, range_step]
    // [if for loop has user defined step, if for loop has iterable,           ,          ,                 ,              ,           ]

    this->emit(Instruction{Opcode::ADD,
        {},
        forSourceLocation
    });
    // [is_range_step_positive           , iterable_element_ptr    , range_step, range_end, for_variable_ptr, range_current + range_step]
    // [if for loop has user defined step, if for loop has iterable,           ,          ,                 ,                           ]

    // ====================================================
    // jump to start of for loop
    // ====================================================

    this->emit(Instruction{Opcode::JMP,
        {LabelRef{forLoopStartLabel}},
        forSourceLocation
    });

    // ====================================================
    // remove items on stack relating to forStm
    // ====================================================

    this->emit(LabelDef{forLoopEndLabel});
    // [is_range_step_positive           , iterable_element_ptr    , range_step, range_end, for_variable_ptr, range_current]
    // [if for loop has user defined step, if for loop has iterable,           ,          ,                 ,              ]

    const int itemsOnStack = (rangeHasStep || !forHasRange)? 5 : 4;

    for (int i = 0; i < itemsOnStack; i++) {
        this->emit(Instruction{Opcode::POP,
            {},
            forSourceLocation
        });
    }
    // []
}

void compiler::AssemblyGenerator::compileContinueStatement(Scope* scope, const ast::ContinueStm &continueStm) {
    const auto loopContext = scope->lookupLoopScope()->loopContext;
    // jump to start of while block (evaluate condition again)
    this->emit(Instruction{Opcode::JMP,
        {LabelRef{loopContext->continueLabel}},
        SourceLocation{0, continueStm.line, continueStm.column}
    });
}

void compiler::AssemblyGenerator::compileBreakStatement(Scope* scope, const ast::BreakStm &breakStm) {
    const auto loopContext = scope->lookupLoopScope()->loopContext;
    // jump to end of while block
    this->emit(Instruction{Opcode::JMP,
        {LabelRef{loopContext->breakLabel}},
        SourceLocation{0, breakStm.line, breakStm.column}
    });
}

void compiler::AssemblyGenerator::compileReturnStatement(Scope* scope, const ast::ReturnStm &returnStm) {
    if (returnStm.returnExpression != nullptr) {
        this->compileExpr(scope, *returnStm.returnExpression, ExprResult::VALUE);
        this->compileTypeConversionIfRequired(returnStm.returnExpression->resultingType, returnStm.functionSymbol->returnType, SourceLocation{0, returnStm.returnExpression->line, returnStm.returnExpression->column});
    }
    // return execution to caller
    this->emit(Instruction{Opcode::RET,
        {},
        SourceLocation{0, returnStm.line, returnStm.column}
    });
}

void compiler::AssemblyGenerator::compileExpressionStatement(Scope *scope, const ast::ExpressionStatement &expressionStm) {
    if (auto* functionCallExpr = dynamic_cast<const ast::FunctionCall*>(expressionStm.expression.get())) {
        this->compileFunctionCall(scope, *functionCallExpr);

    } else if (auto* unaryExpr = dynamic_cast<const ast::ExprUnaryOperator*>(expressionStm.expression.get())) {
        this->compileUnaryExpr(scope, *unaryExpr, ExprResult::DISCARD);

    } else { // POSTFIX EXPR
        auto* postfixExpr = dynamic_cast<const ast::ExprPostfix*>(expressionStm.expression.get());
        this->compilePostfixExpr(scope, *postfixExpr, ExprResult::DISCARD);
    }
}

void compiler::AssemblyGenerator::compileExpr(Scope* scope, const ast::Expr& expr, const ExprResult exprResult) {
    if (auto* integerLiteral = dynamic_cast<const ast::ExprIntegerLiteral*>(&expr)) {
        this->compileExprIntegerLiteral(*integerLiteral);
    }
    else if (auto* floatLiteral = dynamic_cast<const ast::ExprFloatLiteral*>(&expr)) {
        this->compileExprFloatLiteral(*floatLiteral);
    }
    else if (auto* boolLiteral = dynamic_cast<const ast::ExprBoolLiteral*>(&expr)) {
        this->compileExprBoolLiteral(*boolLiteral);
    }
    else if (auto* charLiteral = dynamic_cast<const ast::ExprCharLiteral*>(&expr)) {
        this->compilerExprCharLiteral(*charLiteral);
    }
    else if (auto* identifier = dynamic_cast<const ast::ExprIdentifier*>(&expr)) {
        this->compileExprIdentifier(scope, *identifier, exprResult);
    }
    else if (auto* binaryExpr = dynamic_cast<const ast::ExprBinaryOperator*>(&expr)) {
        this->compileBinaryExpr(scope, *binaryExpr);
    }
    else if (auto* unaryExpr = dynamic_cast<const ast::ExprUnaryOperator*>(&expr)) {
        this->compileUnaryExpr(scope, *unaryExpr, ExprResult::VALUE);
    }
    else if (auto* postfixExpr = dynamic_cast<const ast::ExprPostfix*>(&expr)) {
        this->compilePostfixExpr(scope, *postfixExpr, exprResult);
    }
    else if (auto* castExpr = dynamic_cast<const ast::ExprCast*>(&expr)) {
        this->compileCastExpr(scope, *castExpr);
    }
    else if (auto* functionCall = dynamic_cast<const ast::FunctionCall*>(&expr)) {
        this->compileFunctionCall(scope, *functionCall);
    }
    else if (auto* newExpr = dynamic_cast<const ast::ExprNew*>(&expr)) {
        this->compileNewExpr(scope, *newExpr);
    }
}

void compiler::AssemblyGenerator::compileBinaryExpr(Scope* scope, const ast::ExprBinaryOperator &expr) {
    this->compileExpr(scope, *expr.left, ExprResult::VALUE);

    switch (expr.binaryOperatorInfo->binaryOperator) {
        case BinaryOperator::ADD:
        case BinaryOperator::SUBTRACT:
        case BinaryOperator::MULTIPLY:
        case BinaryOperator::DIVIDE:
        case BinaryOperator::MODULO: {
            // convert left expr to expr resulting type
            this->compileTypeConversionIfRequired(expr.left->resultingType, expr.resultingType, SourceLocation{0, expr.left->line, expr.left->column});
            // compile right expr
            this->compileExpr(scope, *expr.right, ExprResult::VALUE);
            // convert right expr to expr resulting type
            this->compileTypeConversionIfRequired(expr.right->resultingType, expr.resultingType, SourceLocation{0, expr.right->line, expr.right->column});
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

            this->compileExpr(scope, *expr.right, ExprResult::VALUE);

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

            this->compileExpr(scope, *expr.right, ExprResult::VALUE);

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

            if (expr.left->resultingType.typeId == FLOAT_TYPE_ID ||
                expr.right->resultingType.typeId == FLOAT_TYPE_ID
            ) {
                // if either operand is float -> convert left expr to float
                this->compileTypeConversionIfRequired(expr.left->resultingType, Type{FLOAT_TYPE_ID, 0}, SourceLocation{0, expr.left->line, expr.left->column});
            } else {
                // convert left expr to int
                this->compileTypeConversionIfRequired(expr.left->resultingType, Type{INT_TYPE_ID, 0}, SourceLocation{0, expr.left->line, expr.left->column});
            }

            // compile right expression
            this->compileExpr(scope, *expr.right, ExprResult::VALUE);

            if (expr.left->resultingType.typeId == FLOAT_TYPE_ID ||
                expr.right->resultingType.typeId == FLOAT_TYPE_ID
            ) {
                // if either operand is float -> convert right expr to float
                this->compileTypeConversionIfRequired(expr.right->resultingType, Type{FLOAT_TYPE_ID, 0}, SourceLocation{0, expr.right->line, expr.right->column});
            } else {
                // convert right expr to int
                this->compileTypeConversionIfRequired(expr.right->resultingType, Type{INT_TYPE_ID, 0}, SourceLocation{0, expr.right->line, expr.right->column});
            }

            this->compileBinaryOperator(*expr.binaryOperatorInfo);
            break;
        }

        default: {}
    }

    if (expr.binaryOperatorInfo->binaryOperator == BinaryOperator::INTEGER_DIVIDE) {
        // if either operand is float -> convert result to int
        if (expr.left->resultingType.typeId == FLOAT_TYPE_ID ||
            expr.right->resultingType.typeId == FLOAT_TYPE_ID
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
        case BinaryOperator::ADD: this->emit(Instruction{Opcode::ADD, {}, sourceLocation}); break;
        case BinaryOperator::SUBTRACT: this->emit(Instruction{Opcode::SUB, {}, sourceLocation}); break;
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


void compiler::AssemblyGenerator::compileUnaryExpr(Scope* scope, const ast::ExprUnaryOperator& expr, const ExprResult exprResult) {
    switch (expr.unaryOperatorInfo->unaryOperator) {
        case UnaryOperator::PLUS: {
            this->compileExpr(scope, *expr.expr, ExprResult::VALUE);
            break;
        }
        case UnaryOperator::MINUS: {
            // push 0 onto stack
             this->emit(Instruction{Opcode::PUSH,
                {
                    toAssemblyType(expr.expr->resultingType),
                    Immediate{getNumber(expr.expr->resultingType, 0)}
                },
                 SourceLocation{0, expr.unaryOperatorInfo->line, expr.unaryOperatorInfo->column}
             });

            this->compileExpr(scope, *expr.expr, ExprResult::VALUE);

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

            this->compileExpr(scope, *expr.expr, ExprResult::VALUE);

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
        case UnaryOperator::INCREMENT:
        case UnaryOperator::DECREMENT: {
            this->compileExpr(scope, *expr.expr, ExprResult::PTR);

            // [ptr]

            this->emit(Instruction{Opcode::DUP,
                {Immediate{Number{static_cast<uint32_t>(0)}}},
                SourceLocation{0, expr.unaryOperatorInfo->line, expr.unaryOperatorInfo->column}
            });
            // [ptr, ptr]

            this->emit(Instruction{Opcode::LOAD,
                {toAssemblyType(expr.expr->resultingType)},
                SourceLocation{0, expr.unaryOperatorInfo->line, expr.unaryOperatorInfo->column}
            });
            // [ptr, val]

            this->emit(Instruction{Opcode::PUSH,
                {
                    toAssemblyType(expr.expr->resultingType),
                    Immediate{getNumber(expr.expr->resultingType, 1)}
                },
                SourceLocation{0, expr.unaryOperatorInfo->line, expr.unaryOperatorInfo->column}
            });
            // [ptr, val, 1]

            const auto arithmeticOpcode = expr.unaryOperatorInfo->unaryOperator == UnaryOperator::INCREMENT ? Opcode::ADD : Opcode::SUB;

            this->emit(Instruction{arithmeticOpcode,
                {},
                SourceLocation{0, expr.unaryOperatorInfo->line, expr.unaryOperatorInfo->column}
            });
            // [ptr, val +/- 1]

            if (exprResult != ExprResult::DISCARD) {
                this->emit(Instruction{Opcode::DUP,
                    {Immediate{Number{static_cast<uint32_t>(0)}}},
                    SourceLocation{0, expr.unaryOperatorInfo->line, expr.unaryOperatorInfo->column}
                });
                // [ptr, val +/- 1, val +/- 1]

                this->emit(Instruction{Opcode::ROTU,
                    {Immediate{Number{static_cast<uint16_t>(3)}}}
                });
                // [val +/- 1, ptr, val +/- 1]
            }
            // if exprResult != DISCARD [val +/- 1, ptr, val +/- 1]
            // if exprResult == DISCARD [ptr, val +/- 1]

            this->emit(Instruction{Opcode::STORE,
                {},
                SourceLocation{0, expr.unaryOperatorInfo->line, expr.unaryOperatorInfo->column}
            });
            // if exprResult != DISCARD [val +/- 1]
            // if exprResult == DISCARD []
        }
    }
}

void compiler::AssemblyGenerator::compilePostfixExpr(Scope* scope, const ast::ExprPostfix& expr, const ExprResult exprResult) {
    this->compileExpr(scope, *expr.expression, expr.unaryOperatorInfo != nullptr ? ExprResult::PTR : exprResult);

    for (int idx = 0; idx < expr.postfixOperators.size(); idx++) {

        if (std::holds_alternative<std::unique_ptr<ast::Index>>(expr.postfixOperators[idx])) {

            auto& index = std::get<std::unique_ptr<ast::Index>>(expr.postfixOperators[idx]);

            this->compileArrayIndex(scope, *index, expr.resultingType);

            // load ptr if not last postfix operator
            if (idx < expr.postfixOperators.size() - 1) {
                this->emit(Instruction{Opcode::LOAD,
                    {AssemblyType::PTR},
                    SourceLocation{0, expr.expression->line, expr.expression->column}
                });
            }
        } else {
            // compile field access
            auto& fieldAccess = std::get<std::unique_ptr<ast::FieldAccess>>(expr.postfixOperators[idx]);

            if (fieldAccess->fieldInfo->addressOffset != 0) {
                this->emit(Instruction{Opcode::PUSH,
                {
                        AssemblyType::UI32,
                        Immediate{Number{fieldAccess->fieldInfo->addressOffset}}
                    },
                    SourceLocation{0, fieldAccess->line, fieldAccess->column}
                });

                this->emit(Instruction{Opcode::ADD,
                    {},
                    SourceLocation{0, fieldAccess->line, fieldAccess->column}
                });
            }
        }
    }

    if (expr.unaryOperatorInfo == nullptr && exprResult == ExprResult::VALUE) {
        this->emit(Instruction{Opcode::LOAD,
            {toAssemblyType(expr.resultingType)},
            SourceLocation{0, expr.expression->line, expr.expression->column}
        });
    } else if (expr.unaryOperatorInfo != nullptr) {
        // [ptr]

        this->emit(Instruction{Opcode::DUP,
            {Immediate{Number{static_cast<uint32_t>(0)}}},
            SourceLocation{0, expr.unaryOperatorInfo->line, expr.unaryOperatorInfo->column}
        });
        // [ptr, ptr]

        this->emit(Instruction{Opcode::LOAD,
            {toAssemblyType(expr.resultingType)},
            SourceLocation{0, expr.unaryOperatorInfo->line, expr.unaryOperatorInfo->column}
        });
        // [ptr, val]

        if (exprResult != ExprResult::DISCARD) {
            this->emit(Instruction{Opcode::SWAP,
                {},
                SourceLocation{0, expr.unaryOperatorInfo->line, expr.unaryOperatorInfo->column}
            });
            // [val, ptr]

            this->emit(Instruction{Opcode::DUP,
                {Immediate{Number{static_cast<uint32_t>(1)}}},
                SourceLocation{0, expr.unaryOperatorInfo->line, expr.unaryOperatorInfo->column}
            });
            // [val, ptr, val]
        }
        // if exprResult != DISCARD [val, ptr, val]
        // if exprResult == DISCARD [ptr, val]

        this->emit(Instruction{Opcode::PUSH,
            {
                toAssemblyType(expr.resultingType),
                Immediate{getNumber(expr.resultingType, 1)}
            },
            SourceLocation{0, expr.unaryOperatorInfo->line, expr.unaryOperatorInfo->column}
        });
        // if exprResult != DISCARD [val, ptr, val, 1]
        // if exprResult == DISCARD [ptr, val, 1]

        const auto arithmeticOpcode = expr.unaryOperatorInfo->unaryOperator == UnaryOperator::INCREMENT ? Opcode::ADD : Opcode::SUB;

        this->emit(Instruction{arithmeticOpcode,
            {},
            SourceLocation{0, expr.unaryOperatorInfo->line, expr.unaryOperatorInfo->column}
        });
        // if exprResult != DISCARD [val, ptr, val +/- 1]
        // if exprResult == DISCARD [ptr, val +/- 1]

        this->emit(Instruction{Opcode::STORE,
            {},
            SourceLocation{0, expr.unaryOperatorInfo->line, expr.unaryOperatorInfo->column}
        });
        // if exprResult != DISCARD [val]
        // if exprResult == DISCARD []
    }
}

void compiler::AssemblyGenerator::compileCastExpr(Scope* scope, const ast::ExprCast& castExpr) {
    this->compileExpr(scope, *castExpr.expr, ExprResult::VALUE);

    // convert to result of expression to the cast type
    this->compileTypeConversionIfRequired(castExpr.expr->resultingType, castExpr.resultingType, SourceLocation{0, castExpr.line, castExpr.column});
}

void compiler::AssemblyGenerator::compileNewExpr(Scope* scope, const ast::ExprNew& newExpr) {
    // allocate space in heap
    const unsigned int dimension = newExpr.typeInfo->type.dimension - 1;

    // compile index expressions
    for (int index = newExpr.arrayDimensions.size() - 1; index >= 0; index--) {
        const auto arraySizeNotNegativeLabel = this->generateLabel("array_size_not_negative");

        this->compileExpr(scope, *newExpr.arrayDimensions[index]->index, ExprResult::VALUE);

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

    auto* nestedType = new Type{newExpr.typeInfo->type.typeId, dimension};
    this->compileArrayAlloc(scope, newExpr, 0, *nestedType);
    delete nestedType;

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

        this->compileArrayInitialiser(scope, *newExpr.optionalInitialiser, SourceLocation{0, newExpr.optionalInitialiser->line, newExpr.optionalInitialiser->column}, Type{newExpr.typeInfo->type.typeId, 0});
        // [root_array_ptr]
    }
}

void compiler::AssemblyGenerator::compileArrayAlloc(Scope *scope, const ast::ExprNew& newExpr, unsigned int depth, Type& typeAtDepth) {
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

    if (!typeAtDepth.isArray()) {
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
    // [root_array_ptr, nested_arrays_left_to_alloc, root_array_length - nested_arrays_left_to_alloc]
    // [ptr           , ui32                       , ui32                                           ]

    this->emit(Instruction{Opcode::PUSH,
        {
            AssemblyType::UI32,
            Immediate{Number{typeAtDepth.getSize()}}
        },
        *newExprSourceLocation
    });
    // [root_array_ptr, nested_arrays_left_to_alloc, root_array_length - nested_arrays_left_to_alloc, element_size_of_root_array]
    // [ptr           , ui32                       , ui32                                            , ui32                     ]

    this->emit(Instruction{Opcode::MUL,
        {},
        *newExprSourceLocation
    });
    // [root_array_ptr, nested_arrays_left_to_alloc, (root_array_length - nested_arrays_left_to_alloc) * element_size_of_root_array]
    // [ptr           , ui32                       , ui32                                                                          ]

    this->emit(Instruction{Opcode::PUSH,
        {
            AssemblyType::UI32,
            Immediate{Number{static_cast<uint32_t>(4)}}
        },
        *newExprSourceLocation
    });
    // [root_array_ptr, nested_arrays_left_to_alloc, (root_array_length - (nested_arrays_left_to_alloc) * element_size_of_root_array), 4   ]
    // [ptr           , ui32                       , ui32                                                                            , ui32]

    this->emit(Instruction{Opcode::ADD,
        {},
        *newExprSourceLocation
    });
    // [root_array_ptr, nested_arrays_left_to_alloc, (root_array_length - (nested_arrays_left_to_alloc) * element_size_of_root_array) + 4]
    // [ptr           , ui32                       , ui32                                                                                ]

    this->emit(Instruction{Opcode::DUP,
        {Immediate{Number{static_cast<uint32_t>(2)}}},
        *newExprSourceLocation
    });
    // [root_array_ptr, nested_arrays_left_to_alloc, (root_array_length - (nested_arrays_left_to_alloc) * element_size_of_root_array) + 4, root_array_ptr]
    // [ptr           , ui32                       , ui32                                                                                , ptr           ]

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

void compiler::AssemblyGenerator::compileArrayInitialiser(Scope *scope, const ast::ArrayInitialiser& arrayInitialiser, const SourceLocation& optionalInitialiserSource, const Type& typeOfDeepestElement) {

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

            if (std::get<std::unique_ptr<ast::ArrayInitialiser>>(arrayInitialiser.elements[index])->elements.empty()) {
                this->emit(Instruction{Opcode::POP,
                    {},
                    optionalInitialiserSource
                });
                // [ptr_to_element     ]
                // [ptr                ]
                // [if not last element]
            } else {
                this->emit(Instruction{Opcode::LOAD,
                    {AssemblyType::PTR},
                    optionalInitialiserSource
                });
                // [ptr_to_element     , ptr_to_nested_array]
                // [ptr                , ptr                ]
                // [if not last element,                    ]

                this->compileArrayInitialiser(scope, *std::get<std::unique_ptr<ast::ArrayInitialiser>>(arrayInitialiser.elements[index]), optionalInitialiserSource, typeOfDeepestElement);
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

            this->compileExpr(scope, *std::get<std::unique_ptr<ast::Expr>>(arrayInitialiser.elements[index]), ExprResult::VALUE);
            // [ptr_to_element     , ptr_to_element, value_to_store        ]
            // [ptr                , ptr           , type_of_value_to_store]
            // [if not last element,                                       ]

            this->compileTypeConversionIfRequired(std::get<std::unique_ptr<ast::Expr>>(arrayInitialiser.elements[index])->resultingType, typeOfDeepestElement, optionalInitialiserSource);
            // [ptr_to_element     , ptr_to_element, value_to_store        ]
            // [ptr                , ptr           , type_of_element       ]
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
                        Immediate{Number{static_cast<uint32_t>(typeOfDeepestElement.getSize())}}
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
        this->compileExpr(scope, *functionCall.arguments[argIndex], ExprResult::VALUE);

        // convert any arguments that require implicit conversion
        this->compileTypeConversionIfRequired(functionCall.arguments[argIndex]->resultingType, functionCall.functionSymbol->parameterTypes[argIndex], SourceLocation{0, functionCall.arguments[argIndex]->line, functionCall.arguments[argIndex]->column});
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
        for (const BuiltinFunctionId requiredFunctionId : Builtins::getBuiltinFunction(functionCall.functionSymbol->builtinId)->requiredBuiltinFunctions) {
            this->requiredBuiltinFunctions.insert(requiredFunctionId);
        }
    }
}

void compiler::AssemblyGenerator::compileExprIdentifier(Scope* scope, const ast::ExprIdentifier& exprIdentifier, const ExprResult exprResult) {
    const auto symbol = scope->lookup(exprIdentifier.identifier->name).value();

    if (exprResult == ExprResult::VALUE || exprIdentifier.resultingType.isArray()) {
        if (symbol->isGlobal()) {
            // push global variable onto stack
            this->emit(Instruction{Opcode::LOADG,
                {LabelRef{exprIdentifier.identifier->name}},
                SourceLocation{0, exprIdentifier.identifier->line, exprIdentifier.column}
            });
        } else {
            // push local variable onto stack
            this->emit(Instruction{Opcode::LOADL,
                {
                    toAssemblyType(symbol->type),
                    Immediate{Number{symbol->localSlot}}
                },
                SourceLocation{0, exprIdentifier.line, exprIdentifier.column}
            });
        }
    } else {
        if (symbol->isGlobal()) {
            // push pointer to global variable onto stack
            this->emit(Instruction{Opcode::PUSH,
                {
                    AssemblyType::PTR,
                    LabelRef{exprIdentifier.identifier->name}
                },
                SourceLocation{0, exprIdentifier.identifier->line, exprIdentifier.column}
            });
        } else {
            // push pointer to local variable onto stack
            this->emit(Instruction{Opcode::ADDRL,
                {Immediate{Number{symbol->localSlot}}},
                SourceLocation{0, exprIdentifier.line, exprIdentifier.column}
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

void compiler::AssemblyGenerator::compilerExprCharLiteral(const ast::ExprCharLiteral &charLiteral) {
    // push char value onto stack
    this->emit(Instruction{Opcode::PUSH,
        {
            AssemblyType::UI32,
            Immediate{getNumber(charLiteral.resultingType, charLiteral.value)}
        }
    });
}

void compiler::AssemblyGenerator::compileTypeConversionIfRequired(const Type& currentType, const Type& newType, const SourceLocation& sourceLocation) {
    // convert to new type if current type is different from the new type
    this->compileTypeConversionIfRequired(toAssemblyType(currentType), toAssemblyType(newType), sourceLocation);
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
            toAssemblyType(it->second.type),
            getNumber(it->second.type, 0)
        });
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

void compiler::AssemblyGenerator::registerScopeFunction(const ScopeFunctionBody& scopeFunctionBody, const uint32_t blockEndLine, const uint16_t blockEndColumn) {
    this->pendingScopeFunctions.push_back(scopeFunctionBody);
    // call scope function

    this->emit(Instruction{Opcode::CALL,
        {LabelRef{generateScopeFunctionIdentifier(this->scopeFunctionCounter++)}},
        SourceLocation{0, blockEndLine, blockEndColumn}
    });
}

void compiler::AssemblyGenerator::emit(const AssemblyItem& assemblyItem) {
    this->assembly.push_back(assemblyItem);
}

compiler::AssemblyType compiler::AssemblyGenerator::toAssemblyType(const Type& type) {
    if (type.isArray()) return AssemblyType::PTR;
    switch (type.typeId) {
        case INT_TYPE_ID: return AssemblyType::I32;
        case FLOAT_TYPE_ID: return AssemblyType::F32;

        case BOOL_TYPE_ID:
        case CHAR_TYPE_ID:
            return AssemblyType::UI32;
    }
}

compiler::Number compiler::AssemblyGenerator::getNumber(const Type& type, const uint64_t value) {
    if (type.isArray()) return Number{static_cast<uint32_t>(value)};
    switch (type.typeId) {
        case INT_TYPE_ID: return Number{static_cast<int32_t>(value)};
        case FLOAT_TYPE_ID: return Number{static_cast<float>(value)};

        case BOOL_TYPE_ID:
        case CHAR_TYPE_ID:
            return Number{static_cast<uint32_t>(value)};
    }
}

compiler::Number compiler::AssemblyGenerator::getNumber(const AssemblyType type, const uint64_t value) {
    switch (type) {
        case AssemblyType::I32: return Number{static_cast<int32_t>(value)};
        case AssemblyType::I64: return Number{static_cast<int64_t>(value)};
        case AssemblyType::UI64: return Number{static_cast<uint64_t>(value)};
        case AssemblyType::F32: return Number{static_cast<float>(value)};
        case AssemblyType::F64: return Number{static_cast<double>(value)};

        case AssemblyType::UI32:
        case AssemblyType::PTR:
            return Number{static_cast<uint32_t>(value)};
    }
}
