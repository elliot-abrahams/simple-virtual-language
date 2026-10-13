#include "AssemblyGenerator.h"

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
    if (auto* block = dynamic_cast<const ast::StmBlock*>(&body)) {
        for (const auto& stm : block->statements) {
            this->compileStm(block->scope, *stm);
        }
    } else {
        auto* forStm = dynamic_cast<const ast::StmFor*>(&body);
        this->compileStmFor(forStm->body->scope, *forStm);
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
        if (std::holds_alternative<const ast::StmBlock*>(this->pendingScopeFunctions.at(scopeCounter))) {
            const auto* block = std::get<const ast::StmBlock*>(this->pendingScopeFunctions.at(scopeCounter));

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
            const auto* forStm = std::get<const ast::StmFor*>(this->pendingScopeFunctions.at(scopeCounter));

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
    if (auto* stmVarDecl = dynamic_cast<const ast::StmVarDecl*>(&stm)) {
        this->compileStmVarDecl(scope, *stmVarDecl);
    }
    else if (auto* stmAssignment = dynamic_cast<const ast::StmAssignment*>(&stm)) {
        this->compileStmAssignment(scope, *stmAssignment);
    }
    else if (auto* stmBlock = dynamic_cast<const ast::StmBlock*>(&stm)) {
        this->compileBlock(*stmBlock);
    }
    else if (auto* stmIf = dynamic_cast<const ast::StmIf*>(&stm)) {
        this->compileStmIf(scope, *stmIf);
    }
    else if (auto* stmWhile = dynamic_cast<const ast::StmWhile*>(&stm)) {
        this->compileStmWhile(scope, *stmWhile);
    }
    else if (auto* stmFor = dynamic_cast<const ast::StmFor*>(&stm)) {
        this->compileStmFor(scope, *stmFor);
    }
    else if (auto* stmContinue = dynamic_cast<const ast::StmContinue*>(&stm)) {
        this->compileStmContinue(scope, *stmContinue);
    }
    else if (auto* stmBreak = dynamic_cast<const ast::StmBreak*>(&stm)) {
        this->compileStmBreak(scope, *stmBreak);
    }
    else if (auto* stmReturn = dynamic_cast<const ast::StmReturn*>(&stm)) {
        this->compileStmReturn(scope, *stmReturn);
    }
    else if (auto* stmExpr = dynamic_cast<const ast::StmExpr*>(&stm)) {
        this->compileStmExpr(scope, *stmExpr);
    }
}

void compiler::AssemblyGenerator::compileBlock(const ast::StmBlock& stmBlock) {
    // if scope declared in global scope and not a scope within a function
    if (stmBlock.scope->parent->isGlobalScope()) {
        this->registerScopeFunction(&stmBlock, stmBlock.blockEndLine, stmBlock.blockEndColumn);
    } else {
        for (auto& stm : stmBlock.statements) {
            this->compileStm(stmBlock.scope, *stm);
        }
    }
}

void compiler::AssemblyGenerator::compileStmVarDecl(Scope* scope, const ast::StmVarDecl& stmVarDecl) {
    if (stmVarDecl.optionalInitialiser != nullptr) {
        this->compileExpr(scope, *stmVarDecl.optionalInitialiser, ExprResult::VALUE);

        const auto symbol = scope->lookup(stmVarDecl.identifier->name).value();

        // convert expr to variable type
        this->compileTypeConversionIfRequired(stmVarDecl.optionalInitialiser->resultingType, symbol->type, SourceLocation{0, stmVarDecl.optionalInitialiser->line, stmVarDecl.optionalInitialiser->column});

        if (symbol->isGlobal()) {
            // store optional initialiser in global variable
            this->emit(Instruction{Opcode::STOREG,
                {LabelRef{stmVarDecl.identifier->name}},
                SourceLocation{0, stmVarDecl.line, stmVarDecl.column}
            });
        } else {
            // store optional initialiser in local variable
            this->emit(Instruction{Opcode::STOREL,
                {Immediate{Number{symbol->localSlot}}},
                SourceLocation{0, stmVarDecl.line, stmVarDecl.column}
            });
        }
    }
}

void compiler::AssemblyGenerator::compileStmAssignment(Scope* scope, const ast::StmAssignment& stmAssignment) {

    const auto symbol = scope->lookup(stmAssignment.identifier->name).value();

    std::unique_ptr<ast::ExprIdentifier> identifierExpr = nullptr;

    Type typeAtDepth;

    if (stmAssignment.indices.empty()) {
        if (stmAssignment.assignmentOperatorInfo->assignmentOperator == AssignmentOperator::EQUAL) {
            this->compileExpr(scope, *stmAssignment.expr, ExprResult::VALUE);
            // convert expr to variable type
            this->compileTypeConversionIfRequired(stmAssignment.expr->resultingType, symbol->type, SourceLocation{0, stmAssignment.expr->line, stmAssignment.expr->column});

            if (symbol->isGlobal()) {
                // store result of expression in global variable
                this->emit(Instruction{Opcode::STOREG,
                    {LabelRef{stmAssignment.identifier->name}},
                    SourceLocation{0, stmAssignment.line, stmAssignment.column}
                });
            } else {
                // store result of expression in local variable
                this->emit(Instruction{Opcode::STOREL,
                    {Immediate{Number{symbol->localSlot}}},
                    SourceLocation{0, stmAssignment.line, stmAssignment.column}
                });
            }
            return;
        }
        // assignment operator is not 'EQUAL'

        // generate exprIdentifier
        identifierExpr = std::make_unique<ast::ExprIdentifier>(
            stmAssignment.identifier->line,
            stmAssignment.identifier->column,
            std::make_unique<ast::Identifier>(
                stmAssignment.identifier->line,
                stmAssignment.identifier->column,
                stmAssignment.identifier->name
            )
        );
        this->compileExprIdentifier(scope, *identifierExpr, ExprResult::PTR);
    } else {
        if (symbol->isGlobal()) {
            // store result of expression in global variable
            this->emit(Instruction{Opcode::LOADG,
                {LabelRef{stmAssignment.identifier->name}},
                SourceLocation{0, stmAssignment.line, stmAssignment.column}
            });
        } else {
            // store result of expression in local variable
            this->emit(Instruction{Opcode::LOADL,
                {
                     AssemblyType::PTR,
                    Immediate{Number{symbol->localSlot}}
                },
                SourceLocation{0, stmAssignment.line, stmAssignment.column}
            });
        }

        typeAtDepth = symbol->type;

        for (int index = 0; index < stmAssignment.indices.size(); ++index) {

            typeAtDepth.dimension--;

            this->compileArrayIndex(scope, *stmAssignment.indices[index], typeAtDepth);

            if (index < stmAssignment.indices.size() - 1) {

                // =============================================================
                // load array element if not the deepest index
                // =============================================================

                this->emit(Instruction{Opcode::LOAD,
                    {AssemblyType::PTR},
                    SourceLocation{0, stmAssignment.indices[index]->line, stmAssignment.indices[index]->column}
                });
            }
        }
    }

    // =============================================================
    // store expression
    // =============================================================

    const Type targetType = !stmAssignment.indices.empty()? typeAtDepth : symbol->type;

    if (stmAssignment.assignmentOperatorInfo->assignmentOperator == AssignmentOperator::EQUAL) {
        this->compileExpr(scope, *stmAssignment.expr, ExprResult::VALUE);

    } else {
        // assignment operator is not 'EQUAL'

        // [ptr]
        this->emit(Instruction{Opcode::DUP,
            {Immediate{Number{static_cast<uint32_t>(0)}}},
            SourceLocation{0, stmAssignment.identifier->line, stmAssignment.identifier->column}
        });
        // [ptr, ptr]

        this->emit(Instruction{Opcode::LOAD,
            {toAssemblyType(symbol->type)},
            SourceLocation{0, stmAssignment.identifier->line, stmAssignment.identifier->column}
        });
        // [ptr, val]

        // =============================================================
        // compile binary expression
        // =============================================================

        // generate binaryOp
        const auto* binaryOperatorInfo = new ast::BinaryOperatorInfo(
            stmAssignment.line,
            stmAssignment.column,
            assignmentOperatorToBinaryOperator(stmAssignment.assignmentOperatorInfo->assignmentOperator)
        );

        this->compileExprBinaryWithLeftExprOnStack(
            scope,
            *binaryOperatorInfo,
            stmAssignment.assignmentResultingType,
            stmAssignment.line,
            stmAssignment.column,
            targetType,
            stmAssignment.line,
            stmAssignment.column,
            *stmAssignment.expr
        );
    }
    // convert expr to variable type
    this->compileTypeConversionIfRequired(stmAssignment.assignmentResultingType, targetType, SourceLocation{0, stmAssignment.expr->line, stmAssignment.expr->column});

    // [ptr, resulting_val]

    this->emit(Instruction{Opcode::STORE,
        {},
        SourceLocation{0, stmAssignment.line, stmAssignment.column}
    });
}

void compiler::AssemblyGenerator::compileStmIf(Scope* scope, const ast::StmIf& stmIf) {
    this->compileExpr(scope, *stmIf.condition, ExprResult::VALUE);

    const std::string endIfLabel = generateLabel("if_end");

    if (stmIf.elseStm == nullptr) { // if statement without else
        // skip if block when condition is false
        this->emit(Instruction{Opcode::JEZ,
            {LabelRef{endIfLabel}},
            SourceLocation{0, stmIf.line, stmIf.column}
        });

        this->compileBlock(*stmIf.ifBlock); // compile if block

        this->emit(LabelDef{endIfLabel});

    } else { // if statement with else
        const std::string elseLabel = generateLabel("if_else");

        // jump to else block when condition is false
        this->emit(Instruction{Opcode::JEZ,
            {LabelRef{elseLabel}},
            SourceLocation{0, stmIf.line, stmIf.column}
        });

        this->compileBlock(*stmIf.ifBlock); // compile if block

        // jump to end of else block
        this->emit(Instruction{Opcode::JMP,
            {LabelRef{endIfLabel}},
            SourceLocation{0, stmIf.line, stmIf.column}
        });

        this->emit(LabelDef{elseLabel});

        this->compileStm(scope, *stmIf.elseStm); // compile else block

        this->emit(LabelDef{endIfLabel});
    }
}

void compiler::AssemblyGenerator::compileStmWhile(Scope* scope, const ast::StmWhile& stmWhile) {
    const std::string whileStartLabel = this->generateLabel("while_start");
    const std::string whileEndLabel = this->generateLabel("while_end");

    stmWhile.body->scope->loopContext = new LoopContext{whileStartLabel, whileEndLabel};

    this->emit(LabelDef{whileStartLabel});

    this->compileExpr(scope, *stmWhile.condition, ExprResult::VALUE);

    // skip block if condition is false
    this->emit(Instruction{Opcode::JEZ,
        {LabelRef{whileEndLabel}},
        SourceLocation{0, stmWhile.line, stmWhile.column}
    });

    this->compileBlock(*stmWhile.body); // compile while block

    // jump to start of while (evaluate condition again)
    this->emit(Instruction{Opcode::JMP,
        {LabelRef{whileStartLabel}},
        SourceLocation{0, stmWhile.line, stmWhile.column}
    });

    this->emit(LabelDef{whileEndLabel});
}

void compiler::AssemblyGenerator::compileStmFor(Scope* scope, const ast::StmFor& stmFor) {
    if (!isCompilingScopeFunctions) {
        // to prevent a forVariable that gets declared within the nested scope from being accessed from a global scope context
        // -> add forStm to pendingScopeFunctions
        this->registerScopeFunction(&stmFor, stmFor.body->blockEndLine, stmFor.body->blockEndColumn);
        return;
    }

    const auto forSourceLocation = SourceLocation{0, stmFor.line, stmFor.column};

    const bool forHasVarDecl = std::holds_alternative<std::unique_ptr<ast::StmVarDecl>>(stmFor.variable);
    Type* forVariableType;
    const bool forHasRange = std::holds_alternative<std::unique_ptr<ast::ForRange>>(stmFor.iterable);
    bool rangeHasStep = false;

    const std::unique_ptr<ast::StmVarDecl>* forVarDecl;
    const std::unique_ptr<ast::ExprIdentifier>* forVariable;
    const std::unique_ptr<ast::ForRange>* forRange;
    const std::unique_ptr<ast::Expr>* forIterable;

    if (forHasVarDecl) {
        forVarDecl = &std::get<std::unique_ptr<ast::StmVarDecl>>(stmFor.variable);
        forVariableType = &forVarDecl->get()->typeInfo->type;
    } else {
        forVariable = &std::get<std::unique_ptr<ast::ExprIdentifier>>(stmFor.variable);
        forVariableType = &forVariable->get()->resultingType;
    }

    if (forHasRange) {
        forRange = &std::get<std::unique_ptr<ast::ForRange>>(stmFor.iterable);
    } else {
        forIterable = &std::get<std::unique_ptr<ast::Expr>>(stmFor.iterable);
    }

    if (forHasRange) {
        rangeHasStep = forRange->get()->step != nullptr;
    }

    std::string forStepValidLabel;
    if (rangeHasStep) {
        forStepValidLabel = this->generateLabel("for_step_valid");
    }

    const auto forStartLabel = this->generateLabel("for_start");

    std::string forNegativeStepCondition;
    std::string forConditionJoinLabel;
    if (rangeHasStep) {
        forNegativeStepCondition = this->generateLabel("for_negative_step_condition");
        forConditionJoinLabel = this->generateLabel("for_condition_join");
    }
    const auto forContinueLabel = this->generateLabel("for_continue");
    const auto forEndLabel = this->generateLabel("for_end");

    stmFor.body->scope->parent->loopContext = new LoopContext{forContinueLabel, forEndLabel};

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
                {LabelRef{forStepValidLabel}},
                SourceLocation{0, forRange->get()->step->line, forRange->get()->step->column}
            });
            // [range_step]

            this->emit(Instruction{Opcode::THROW,
                {ErrorRef::ZERO_RANGE_STEP},
                SourceLocation{0, forRange->get()->step->line, forRange->get()->step->column}
            });

            this->emit(LabelDef{forStepValidLabel});
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
        this->compileExprIdentifier(stmFor.body->scope, *identifierExpr, ExprResult::PTR);

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

    this->emit(LabelDef{forStartLabel});

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
            {LabelRef{forNegativeStepCondition}},
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
        {LabelRef{forEndLabel}},
        forSourceLocation
    });
    // [is_range_step_positive           , iterable_element_ptr    , range_step, range_end, for_variable_ptr, range_current]
    // [if for loop has user defined step, if for loop has iterable,           ,          ,                 ,              ]

    if (rangeHasStep) {
        // step is positive -> jump to end of condition
        this->emit(Instruction{Opcode::JMP,
            {LabelRef{forConditionJoinLabel}},
            forSourceLocation
        });

        // ====================================================
        // compile condition with negative step
        // ====================================================

        this->emit(LabelDef{forNegativeStepCondition});

        // [is_range_step_positive, range_step, range_end, for_variable_ptr, range_current, range_end, range_current]

        this->emit(Instruction{Opcode::CGE,
            {},
            forSourceLocation
        });
        // [is_range_step_positive, range_step, range_end, for_variable_ptr, range_current, range_end >= range_current]

        this->emit(Instruction{Opcode::JNZ,
            {LabelRef{forEndLabel}},
            forSourceLocation
        });
        // [is_range_step_positive, range_step, range_end, for_variable_ptr, range_current]

        this->emit(LabelDef{forConditionJoinLabel});
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

    this->compileBlock(*stmFor.body);

    this->emit(LabelDef{forContinueLabel});

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
        {LabelRef{forStartLabel}},
        forSourceLocation
    });

    // ====================================================
    // remove items on stack relating to forStm
    // ====================================================

    this->emit(LabelDef{forEndLabel});
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

void compiler::AssemblyGenerator::compileStmContinue(const Scope* scope, const ast::StmContinue& stmContinue) {
    const auto loopContext = scope->lookupLoopScope()->loopContext;
    // jump to start of while block (evaluate condition again)
    this->emit(Instruction{Opcode::JMP,
        {LabelRef{loopContext->continueLabel}},
        SourceLocation{0, stmContinue.line, stmContinue.column}
    });
}

void compiler::AssemblyGenerator::compileStmBreak(const Scope* scope, const ast::StmBreak& stmBreak) {
    const auto loopContext = scope->lookupLoopScope()->loopContext;
    // jump to end of while block
    this->emit(Instruction{Opcode::JMP,
        {LabelRef{loopContext->breakLabel}},
        SourceLocation{0, stmBreak.line, stmBreak.column}
    });
}

void compiler::AssemblyGenerator::compileStmReturn(Scope* scope, const ast::StmReturn& stmReturn) {
    if (stmReturn.returnExpr != nullptr) {
        this->compileExpr(scope, *stmReturn.returnExpr, ExprResult::VALUE);
        this->compileTypeConversionIfRequired(stmReturn.returnExpr->resultingType, stmReturn.functionSymbol->returnType, SourceLocation{0, stmReturn.returnExpr->line, stmReturn.returnExpr->column});
    }
    // return execution to caller
    this->emit(Instruction{Opcode::RET,
        {},
        SourceLocation{0, stmReturn.line, stmReturn.column}
    });
}

void compiler::AssemblyGenerator::compileStmExpr(Scope *scope, const ast::StmExpr& stmExpr) {
    if (auto* functionCallExpr = dynamic_cast<const ast::ExprFunctionCall*>(stmExpr.expr.get())) {
        this->compileExprFunctionCall(scope, *functionCallExpr);

    } else if (auto* unaryExpr = dynamic_cast<const ast::ExprUnary*>(stmExpr.expr.get())) {
        this->compileExprUnary(scope, *unaryExpr, ExprResult::DISCARD);

    } else { // POSTFIX EXPR
        auto* postfixExpr = dynamic_cast<const ast::ExprPostfix*>(stmExpr.expr.get());
        this->compileExprPostfix(scope, *postfixExpr, ExprResult::DISCARD);
    }
}

void compiler::AssemblyGenerator::compileExpr(Scope* scope, const ast::Expr& expr, const ExprResult exprResult) {
    if (auto* exprIntegerLiteral = dynamic_cast<const ast::ExprIntegerLiteral*>(&expr)) {
        this->compileExprIntegerLiteral(*exprIntegerLiteral);
    }
    else if (auto* exprFloatLiteral = dynamic_cast<const ast::ExprFloatLiteral*>(&expr)) {
        this->compileExprFloatLiteral(*exprFloatLiteral);
    }
    else if (auto* exprBoolLiteral = dynamic_cast<const ast::ExprBoolLiteral*>(&expr)) {
        this->compileExprBoolLiteral(*exprBoolLiteral);
    }
    else if (auto* exprCharLiteral = dynamic_cast<const ast::ExprCharLiteral*>(&expr)) {
        this->compilerExprCharLiteral(*exprCharLiteral);
    }
    else if (auto* exprIdentifier = dynamic_cast<const ast::ExprIdentifier*>(&expr)) {
        this->compileExprIdentifier(scope, *exprIdentifier, exprResult);
    }
    else if (auto* exprBinary = dynamic_cast<const ast::ExprBinary*>(&expr)) {
        this->compileExprBinary(scope, *exprBinary);
    }
    else if (auto* exprUnary = dynamic_cast<const ast::ExprUnary*>(&expr)) {
        this->compileExprUnary(scope, *exprUnary, ExprResult::VALUE);
    }
    else if (auto* exprPostfix = dynamic_cast<const ast::ExprPostfix*>(&expr)) {
        this->compileExprPostfix(scope, *exprPostfix, exprResult);
    }
    else if (auto* castExpr = dynamic_cast<const ast::ExprCast*>(&expr)) {
        this->compileCastExpr(scope, *castExpr);
    else if (auto* exprCast = dynamic_cast<const ast::ExprCast*>(&expr)) {
        this->compileExprCast(scope, *exprCast);
    }
    else if (auto* exprFunctionCall = dynamic_cast<const ast::ExprFunctionCall*>(&expr)) {
        this->compileExprFunctionCall(scope, *exprFunctionCall);
    }
    else if (auto* exprNew = dynamic_cast<const ast::ExprNew*>(&expr)) {
        this->compileExprNew(scope, *exprNew);
    }
}

void compiler::AssemblyGenerator::compileExprBinary(Scope* scope, const ast::ExprBinary& exprBinary) {
    this->compileExpr(scope, *exprBinary.left, ExprResult::VALUE);

    this->compileExprBinaryWithLeftExprOnStack(scope, *exprBinary.binaryOperatorInfo, exprBinary.resultingType, exprBinary.line, exprBinary.column, exprBinary.left->resultingType, exprBinary.left->line, exprBinary.left->column, *exprBinary.right);
}

void compiler::AssemblyGenerator::compileExprBinaryWithLeftExprOnStack(Scope *scope, const ast::BinaryOperatorInfo& binaryOperatorInfo, const Type& exprResultingType, const uint32_t exprLine, const uint16_t exprColumn, const Type& leftType, const uint32_t leftLine, const uint16_t leftColumn, const ast::Expr& right) {
    switch (binaryOperatorInfo.binaryOperator) {
        case BinaryOperator::ADD:
        case BinaryOperator::SUBTRACT:
        case BinaryOperator::MULTIPLY:
        case BinaryOperator::DIVIDE:
        case BinaryOperator::MODULO:
        case BinaryOperator::BITWISE_OR:
        case BinaryOperator::BITWISE_XOR:
        case BinaryOperator::BITWISE_AND:
        case BinaryOperator::LEFT_SHIFT:
        case BinaryOperator::ARITHMETIC_RIGHT_SHIFT:
        case BinaryOperator::LOGICAL_RIGHT_SHIFT: {
            // convert left expr to expr resulting type
            this->compileTypeConversionIfRequired(leftType, exprResultingType, SourceLocation{0, leftLine, leftColumn});
            // compile right expr
            this->compileExpr(scope, right, ExprResult::VALUE);
            // convert right expr to expr resulting type
            this->compileTypeConversionIfRequired(right.resultingType, exprResultingType, SourceLocation{0, right.line, right.column});
            // compile binary operator
            this->compileBinaryOperator(binaryOperatorInfo);
            return;
        }

        case BinaryOperator::LOGICAL_OR: {
            const std::string logicalOrTrueLabel = this->generateLabel("logical_or_true");
            const std::string logicalOrEndLabel = this->generateLabel("logical_or_end");

            // skip right expression if left expression results to true
            // evaluate to true if left expression is true
            this->emit(Instruction{Opcode::JNZ,
                {LabelRef{logicalOrTrueLabel}},
                SourceLocation{0, binaryOperatorInfo.line, binaryOperatorInfo.column}
            });

            this->compileExpr(scope, right, ExprResult::VALUE);

            // evaluate to true if right expression is true
            this->emit(Instruction{Opcode::JNZ,
                {LabelRef{logicalOrTrueLabel}},
                SourceLocation{0, binaryOperatorInfo.line, binaryOperatorInfo.column}
            });
            // push 'false' onto stack
            this->emit(Instruction{Opcode::PUSH,
                {
                    AssemblyType::UI32,
                    Immediate{0}
                },
                SourceLocation{0, binaryOperatorInfo.line, binaryOperatorInfo.column}
            });

            // jump to end of logical OR
            this->emit(Instruction{Opcode::JMP,
                {LabelRef{logicalOrEndLabel}},
                SourceLocation{0, binaryOperatorInfo.line, binaryOperatorInfo.column}
            });

            this->emit(LabelDef{logicalOrTrueLabel});

            // push 'true' onto stack
            this->emit(Instruction{Opcode::PUSH,
                {
                    AssemblyType::UI32,
                    Immediate{1}
                },
                SourceLocation{0, binaryOperatorInfo.line, binaryOperatorInfo.column}
            });

            this->emit(LabelDef{logicalOrEndLabel});
            break;
        }
        case BinaryOperator::LOGICAL_AND: {
            const std::string logicalAndFalseLabel = this->generateLabel("logical_and_false");
            const std::string logicalAndEndLabel = this->generateLabel("logical_and_end");

            // skip right expression if left expression results to false
            // evaluate to false if left expression is false
            this->emit(Instruction{Opcode::JEZ,
                {LabelRef{logicalAndFalseLabel}},
                SourceLocation{0, binaryOperatorInfo.line, binaryOperatorInfo.column}
            });

            this->compileExpr(scope, right, ExprResult::VALUE);

            // evaluate to false if right expression is false
            this->emit(Instruction{Opcode::JEZ,
                {LabelRef{logicalAndFalseLabel}},
                SourceLocation{0, binaryOperatorInfo.line, binaryOperatorInfo.column}
            });

            // push `true` onto stack
            this->emit(Instruction{Opcode::PUSH,
                {
                    AssemblyType::UI32,
                    Immediate{1}
                },
                SourceLocation{0, binaryOperatorInfo.line, binaryOperatorInfo.column}
            });

            // jump to end of logical AND
            this->emit(Instruction{Opcode::JMP,
                {LabelRef{logicalAndEndLabel}},
                SourceLocation{0, binaryOperatorInfo.line, binaryOperatorInfo.column}
            });

            this->emit(LabelDef{logicalAndFalseLabel});

            // push 'false' onto stack
            this->emit(Instruction{Opcode::PUSH,
                {
                    AssemblyType::UI32,
                    Immediate{0}
                },
                SourceLocation{0, binaryOperatorInfo.line, binaryOperatorInfo.column}
            });

            this->emit(LabelDef{logicalAndEndLabel});
            break;
        }

        case BinaryOperator::INTEGER_DIVIDE:
        case BinaryOperator::EQUAL_EQUAL:
        case BinaryOperator::NOT_EQUAL:
        case BinaryOperator::LESS_THAN:
        case BinaryOperator::LESS_THAN_OR_EQUAL:
        case BinaryOperator::GREATER_THAN:
        case BinaryOperator::GREATER_THAN_OR_EQUAL: {

            if (leftType.typeId == FLOAT_TYPE_ID ||
                right.resultingType.typeId == FLOAT_TYPE_ID
            ) {
                // if either operand is float -> convert left expr to float
                this->compileTypeConversionIfRequired(leftType, Type{FLOAT_TYPE_ID, 0}, SourceLocation{0, leftLine, leftColumn});
            } else {
                // convert left expr to int
                this->compileTypeConversionIfRequired(leftType, Type{INT_TYPE_ID, 0}, SourceLocation{0, leftLine, leftColumn});
            }

            // compile right expression
            this->compileExpr(scope, right, ExprResult::VALUE);

            if (leftType.typeId == FLOAT_TYPE_ID ||
                right.resultingType.typeId == FLOAT_TYPE_ID
            ) {
                // if either operand is float -> convert right expr to float
                this->compileTypeConversionIfRequired(right.resultingType, Type{FLOAT_TYPE_ID, 0}, SourceLocation{0, right.line, right.column});
            } else {
                // convert right expr to int
                this->compileTypeConversionIfRequired(right.resultingType, Type{INT_TYPE_ID, 0}, SourceLocation{0, right.line, right.column});
            }

            this->compileBinaryOperator(binaryOperatorInfo);
            break;
        }

        default: {}
    }

    if (binaryOperatorInfo.binaryOperator == BinaryOperator::INTEGER_DIVIDE) {
        // if either operand is float -> convert result to int
        if (leftType.typeId == FLOAT_TYPE_ID ||
            right.resultingType.typeId == FLOAT_TYPE_ID
        ) {
            // convert result to integer
            this->emit(Instruction{Opcode::CONV,
                {AssemblyType::I32},
                SourceLocation{0, exprLine, exprColumn}
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

        case BinaryOperator::BITWISE_OR: this->emit(Instruction{Opcode::ORR, {}, sourceLocation}); break;
        case BinaryOperator::BITWISE_XOR: this->emit(Instruction{Opcode::XOR, {}, sourceLocation}); break;
        case BinaryOperator::BITWISE_AND: this->emit(Instruction{Opcode::AND, {}, sourceLocation}); break;
        case BinaryOperator::LEFT_SHIFT: this->emit(Instruction{Opcode::SHL, {}, sourceLocation}); break;
        case BinaryOperator::ARITHMETIC_RIGHT_SHIFT: this->emit(Instruction{Opcode::SAR, {}, sourceLocation}); break;
        case BinaryOperator::LOGICAL_RIGHT_SHIFT: this->emit(Instruction{Opcode::SHR, {}, sourceLocation}); break;
        case BinaryOperator::EQUAL_EQUAL: this->emit(Instruction{Opcode::CEQ, {}, sourceLocation}); break;
        case BinaryOperator::NOT_EQUAL: this->emit(Instruction{Opcode::CNE, {}, sourceLocation}); break;
        case BinaryOperator::LESS_THAN: this->emit(Instruction{Opcode::CLT, {}, sourceLocation}); break;
        case BinaryOperator::LESS_THAN_OR_EQUAL: this->emit(Instruction{Opcode::CLE, {}, sourceLocation}); break;
        case BinaryOperator::GREATER_THAN: this->emit(Instruction{Opcode::CGT, {}, sourceLocation}); break;
        case BinaryOperator::GREATER_THAN_OR_EQUAL: this->emit(Instruction{Opcode::CGE, {}, sourceLocation}); break;

        default: {}
    }
}

void compiler::AssemblyGenerator::compileExprUnary(Scope* scope, const ast::ExprUnary& exprUnary, const ExprResult exprResult) {
    switch (exprUnary.unaryOperatorInfo->unaryOperator) {
        case UnaryOperator::PLUS: {
            this->compileExpr(scope, *exprUnary.expr, ExprResult::VALUE);
            break;
        }
        case UnaryOperator::MINUS: {
            // push 0 onto stack
             this->emit(Instruction{Opcode::PUSH,
                {
                    toAssemblyType(exprUnary.expr->resultingType),
                    Immediate{getNumber(exprUnary.expr->resultingType, 0)}
                },
                 SourceLocation{0, exprUnary.unaryOperatorInfo->line, exprUnary.unaryOperatorInfo->column}
             });

            this->compileExpr(scope, *exprUnary.expr, ExprResult::VALUE);

            // subtract value of expression from 0
            this->emit(Instruction{Opcode::SUB,
                {},
                SourceLocation{0, exprUnary.unaryOperatorInfo->line, exprUnary.unaryOperatorInfo->column}
            });
            break;
        }
        case UnaryOperator::BITWISE_NOT: {
            this->compileExpr(scope, *exprUnary.expr, ExprResult::VALUE);
            this->compileTypeConversionIfRequired(exprUnary.expr->resultingType, exprUnary.resultingType, SourceLocation{0, exprUnary.unaryOperatorInfo->line, exprUnary.unaryOperatorInfo->column});
            this->emit(Instruction{Opcode::NOT,
                {},
                SourceLocation{0, exprUnary.unaryOperatorInfo->line, exprUnary.unaryOperatorInfo->column}
            });
            break;
        }
        case UnaryOperator::LOGICAL_NOT: {
            const std::string logicalNotTrueLabel = this->generateLabel("logical_not_true");
            const std::string logicalNotEndLabel = this->generateLabel("logical_not_end");

            this->compileExpr(scope, *exprUnary.expr, ExprResult::VALUE);

            // evaluate to true if value of expression is false
            this->emit(Instruction{Opcode::JEZ,
                {LabelRef{logicalNotTrueLabel}},
                SourceLocation{0, exprUnary.unaryOperatorInfo->line, exprUnary.unaryOperatorInfo->column}
            });

            // push 'false' onto stack
            this->emit(Instruction{Opcode::PUSH,
                {
                    AssemblyType::UI32,
                    Immediate{Number{static_cast<uint32_t>(0)}},
                },
                SourceLocation{0, exprUnary.unaryOperatorInfo->line, exprUnary.unaryOperatorInfo->column}
             });

            // jump to end of logical NOT
            this->emit(Instruction{Opcode::JMP,
                {LabelRef{logicalNotEndLabel}},
                SourceLocation{0, exprUnary.unaryOperatorInfo->line, exprUnary.unaryOperatorInfo->column}
            });

            this->emit(LabelDef{logicalNotTrueLabel});

            // push `true` onto stack
            this->emit(Instruction{Opcode::PUSH,
                {
                    AssemblyType::UI32,
                    Immediate{Number{static_cast<uint32_t>(1)}}
                },
                SourceLocation{0, exprUnary.unaryOperatorInfo->line, exprUnary.unaryOperatorInfo->column}
            });

            this->emit(LabelDef{logicalNotEndLabel});
            break;
        }
        case UnaryOperator::INCREMENT:
        case UnaryOperator::DECREMENT: {
            this->compileExpr(scope, *exprUnary.expr, ExprResult::PTR);

            // [ptr]

            this->emit(Instruction{Opcode::DUP,
                {Immediate{Number{static_cast<uint32_t>(0)}}},
                SourceLocation{0, exprUnary.unaryOperatorInfo->line, exprUnary.unaryOperatorInfo->column}
            });
            // [ptr, ptr]

            this->emit(Instruction{Opcode::LOAD,
                {toAssemblyType(exprUnary.expr->resultingType)},
                SourceLocation{0, exprUnary.unaryOperatorInfo->line, exprUnary.unaryOperatorInfo->column}
            });
            // [ptr, val]

            this->emit(Instruction{Opcode::PUSH,
                {
                    toAssemblyType(exprUnary.expr->resultingType),
                    Immediate{getNumber(exprUnary.expr->resultingType, 1)}
                },
                SourceLocation{0, exprUnary.unaryOperatorInfo->line, exprUnary.unaryOperatorInfo->column}
            });
            // [ptr, val, 1]

            const auto arithmeticOpcode = exprUnary.unaryOperatorInfo->unaryOperator == UnaryOperator::INCREMENT ? Opcode::ADD : Opcode::SUB;

            this->emit(Instruction{arithmeticOpcode,
                {},
                SourceLocation{0, exprUnary.unaryOperatorInfo->line, exprUnary.unaryOperatorInfo->column}
            });
            // [ptr, val +/- 1]

            if (exprResult != ExprResult::DISCARD) {
                this->emit(Instruction{Opcode::DUP,
                    {Immediate{Number{static_cast<uint32_t>(0)}}},
                    SourceLocation{0, exprUnary.unaryOperatorInfo->line, exprUnary.unaryOperatorInfo->column}
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
                SourceLocation{0, exprUnary.unaryOperatorInfo->line, exprUnary.unaryOperatorInfo->column}
            });
            // if exprResult != DISCARD [val +/- 1]
            // if exprResult == DISCARD []
        }
    }
}

void compiler::AssemblyGenerator::compileExprPostfix(Scope* scope, const ast::ExprPostfix& exprPostfix, const ExprResult exprResult) {
    this->compileExpr(scope, *exprPostfix.expr, exprPostfix.unaryOperatorInfo != nullptr ? ExprResult::PTR : exprResult);

    for (int idx = 0; idx < exprPostfix.postfixOperators.size(); idx++) {

        if (std::holds_alternative<std::unique_ptr<ast::Index>>(exprPostfix.postfixOperators[idx])) {

            auto& index = std::get<std::unique_ptr<ast::Index>>(exprPostfix.postfixOperators[idx]);

            this->compileArrayIndex(scope, *index, exprPostfix.resultingType);

            // load ptr if not last postfix operator
            if (idx < exprPostfix.postfixOperators.size() - 1) {
                this->emit(Instruction{Opcode::LOAD,
                    {AssemblyType::PTR},
                    SourceLocation{0, exprPostfix.expr->line, exprPostfix.expr->column}
                });
            }
        } else {
            // compile field access
            auto& fieldAccess = std::get<std::unique_ptr<ast::FieldAccess>>(exprPostfix.postfixOperators[idx]);

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

    if (exprPostfix.unaryOperatorInfo == nullptr && exprResult == ExprResult::VALUE) {
        this->emit(Instruction{Opcode::LOAD,
            {toAssemblyType(exprPostfix.resultingType)},
            SourceLocation{0, exprPostfix.expr->line, exprPostfix.expr->column}
        });
    } else if (exprPostfix.unaryOperatorInfo != nullptr) {
        // [ptr]

        this->emit(Instruction{Opcode::DUP,
            {Immediate{Number{static_cast<uint32_t>(0)}}},
            SourceLocation{0, exprPostfix.unaryOperatorInfo->line, exprPostfix.unaryOperatorInfo->column}
        });
        // [ptr, ptr]

        this->emit(Instruction{Opcode::LOAD,
            {toAssemblyType(exprPostfix.resultingType)},
            SourceLocation{0, exprPostfix.unaryOperatorInfo->line, exprPostfix.unaryOperatorInfo->column}
        });
        // [ptr, val]

        if (exprResult != ExprResult::DISCARD) {
            this->emit(Instruction{Opcode::SWAP,
                {},
                SourceLocation{0, exprPostfix.unaryOperatorInfo->line, exprPostfix.unaryOperatorInfo->column}
            });
            // [val, ptr]

            this->emit(Instruction{Opcode::DUP,
                {Immediate{Number{static_cast<uint32_t>(1)}}},
                SourceLocation{0, exprPostfix.unaryOperatorInfo->line, exprPostfix.unaryOperatorInfo->column}
            });
            // [val, ptr, val]
        }
        // if exprResult != DISCARD [val, ptr, val]
        // if exprResult == DISCARD [ptr, val]

        this->emit(Instruction{Opcode::PUSH,
            {
                toAssemblyType(exprPostfix.resultingType),
                Immediate{getNumber(exprPostfix.resultingType, 1)}
            },
            SourceLocation{0, exprPostfix.unaryOperatorInfo->line, exprPostfix.unaryOperatorInfo->column}
        });
        // if exprResult != DISCARD [val, ptr, val, 1]
        // if exprResult == DISCARD [ptr, val, 1]

        const auto arithmeticOpcode = exprPostfix.unaryOperatorInfo->unaryOperator == UnaryOperator::INCREMENT ? Opcode::ADD : Opcode::SUB;

        this->emit(Instruction{arithmeticOpcode,
            {},
            SourceLocation{0, exprPostfix.unaryOperatorInfo->line, exprPostfix.unaryOperatorInfo->column}
        });
        // if exprResult != DISCARD [val, ptr, val +/- 1]
        // if exprResult == DISCARD [ptr, val +/- 1]

        this->emit(Instruction{Opcode::STORE,
            {},
            SourceLocation{0, exprPostfix.unaryOperatorInfo->line, exprPostfix.unaryOperatorInfo->column}
        });
        // if exprResult != DISCARD [val]
        // if exprResult == DISCARD []
    }
}

void compiler::AssemblyGenerator::compileCastExpr(Scope* scope, const ast::ExprCast& castExpr) {
    this->compileExpr(scope, *castExpr.expr, ExprResult::VALUE);
void compiler::AssemblyGenerator::compileExprCast(Scope* scope, const ast::ExprCast& exprCast) {
    this->compileExpr(scope, *exprCast.expr, ExprResult::VALUE);

    // convert to result of expression to the cast type
    this->compileTypeConversionIfRequired(exprCast.expr->resultingType, exprCast.resultingType, SourceLocation{0, exprCast.line, exprCast.column});
}

void compiler::AssemblyGenerator::compileExprNew(Scope* scope, const ast::ExprNew& exprNew) {
    // allocate space in heap
    const unsigned int dimension = exprNew.typeInfo->type.dimension - 1;

    // compile index expressions
    for (int index = exprNew.arrayDimensions.size() - 1; index >= 0; index--) {
        const auto arrayLengthValidLabel = this->generateLabel("array_length_valid");

        this->compileExpr(scope, *exprNew.arrayDimensions[index]->index, ExprResult::VALUE);

        // =======================================================
        // check index expr is greater than or equal to zero
        // =======================================================

        this->emit(Instruction{Opcode::DUP,
            {Immediate{Number{static_cast<uint32_t>(0)}}},
            SourceLocation{0, exprNew.line, exprNew.column}
        });

        this->emit(Instruction{Opcode::PUSH,
            {
                toAssemblyType(exprNew.arrayDimensions[index]->index->resultingType),
                Immediate{Number{static_cast<uint32_t>(0)}}
            },
            SourceLocation{0, exprNew.line, exprNew.column}
        });

        this->emit(Instruction{Opcode::CLT,
            {},
            SourceLocation{0, exprNew.line, exprNew.column}
        });

        this->emit(Instruction{Opcode::JEZ,
            {LabelRef{arrayLengthValidLabel}},
            SourceLocation{0, exprNew.line, exprNew.column}
        });

        this->emit(Instruction{Opcode::THROW,
            {ErrorRef::NEGATIVE_ARRAY_SIZE},
            SourceLocation{0, exprNew.arrayDimensions[index]->index->line, exprNew.arrayDimensions[index]->index->column}
        });

        this->emit(LabelDef{arrayLengthValidLabel});

        // =======================================================
        // convert index expr to type ui32
        // =======================================================

        this->compileTypeConversionIfRequired(
            toAssemblyType(exprNew.arrayDimensions[index]->index->resultingType),
            AssemblyType::UI32,
            SourceLocation{0, exprNew.arrayDimensions[index]->index->line, exprNew.arrayDimensions[index]->index->column}
        );
    }

    auto* nestedType = new Type{exprNew.typeInfo->type.typeId, dimension};
    this->compileArrayAlloc(scope, exprNew, 0, *nestedType);
    delete nestedType;

    // =======================================================
    // pop remaining index expressions
    // =======================================================

    if (exprNew.arrayDimensions.size() == 2) {
        // [nested_array_length, root_array_ptr]]

        this->emit(Instruction{Opcode::SWAP,
            {},
            SourceLocation{0, exprNew.line, exprNew.column}
        });
        // [root_array_ptr, nested_array_length]

    } else if (exprNew.arrayDimensions.size() > 2) {
        // [..., nested_2_array_length, nested_array_length, root_array_ptr]

        this->emit(Instruction{Opcode::ROTU,
            {Immediate{Number{exprNew.arrayDimensions.size()}}},
            SourceLocation{0, exprNew.line, exprNew.column}
        });
        // [root_array_ptr, ..., nested_2_array_length, nested_array_length]
    }

    for (int i = 0; i < exprNew.arrayDimensions.size() - 1; i++) {
        this->emit(Instruction{Opcode::POP,
            {},
            SourceLocation{0, exprNew.line, exprNew.column}
        });
    }

    // [root_array_ptr]

    if (exprNew.optionalInitialiser != nullptr && exprNew.optionalInitialiser->elements.size() > 0) {
        // initialise array
        this->emit(Instruction{Opcode::DUP,
            {Immediate{Number{static_cast<uint32_t>(0)}}},
            SourceLocation{0, exprNew.line, exprNew.column}
        });
        // [root_array_ptr, root_array_ptr]

        this->compileArrayInitialiser(scope, *exprNew.optionalInitialiser, SourceLocation{0, exprNew.optionalInitialiser->line, exprNew.optionalInitialiser->column}, Type{exprNew.typeInfo->type.typeId, 0});
        // [root_array_ptr]
    }
}

void compiler::AssemblyGenerator::compileArrayAlloc(Scope *scope, const ast::ExprNew& exprNew, unsigned int depth, Type& typeAtDepth) {
    const auto newExprSourceLocation = new SourceLocation{0, exprNew.line, exprNew.column};

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

    const auto nestedArrayAllocStartLabel = this->generateLabel("nested_array_alloc_start");
    const auto nestedArrayAllocEndLabel = this->generateLabel("nested_array_alloc_end");

    this->emit(LabelDef{nestedArrayAllocStartLabel});

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
        {LabelRef{nestedArrayAllocEndLabel}},
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
    this->compileArrayAlloc(scope, exprNew, depth, typeAtDepth);
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
        {LabelRef{nestedArrayAllocStartLabel}},
        *newExprSourceLocation
    });

    this->emit(LabelDef{nestedArrayAllocEndLabel});

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

    const auto arrayInitialiserLengthValidLabel = this->generateLabel("array_initialiser_length_valid");

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
        {LabelRef{arrayInitialiserLengthValidLabel}},
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


    this->emit(LabelDef{arrayInitialiserLengthValidLabel});
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

void compiler::AssemblyGenerator::compileExprFunctionCall(Scope* scope, const ast::ExprFunctionCall& exprFunctionCall) {
    // compile arguments
    for (size_t argIndex = 0; argIndex < exprFunctionCall.arguments.size(); argIndex++) {
        this->compileExpr(scope, *exprFunctionCall.arguments[argIndex], ExprResult::VALUE);

        // convert any arguments that require implicit conversion
        this->compileTypeConversionIfRequired(exprFunctionCall.arguments[argIndex]->resultingType, exprFunctionCall.functionSymbol->parameterTypes[argIndex], SourceLocation{0, exprFunctionCall.arguments[argIndex]->line, exprFunctionCall.arguments[argIndex]->column});
    }
    if (exprFunctionCall.functionSymbol->builtinId == BuiltinFunctionId::NONE) { // function is user-defined
        // call user-defined function
        this->emit(Instruction{Opcode::CALL,
            {LabelRef{exprFunctionCall.functionSymbol->label}},
            SourceLocation{0, exprFunctionCall.line, exprFunctionCall.column}
        });
    } else { // function is builtin
        // call builtin function
        this->emit(Instruction{Opcode::CALL,
            {LabelRef{Builtins::getBuiltinFunctionLabel(exprFunctionCall.functionSymbol->builtinId)}},
            SourceLocation{0, exprFunctionCall.line, exprFunctionCall.column}
        });
        this->requiredBuiltinFunctions.insert(exprFunctionCall.functionSymbol->builtinId);
        for (const BuiltinFunctionId requiredFunctionId : Builtins::getBuiltinFunction(exprFunctionCall.functionSymbol->builtinId)->requiredBuiltinFunctions) {
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

void compiler::AssemblyGenerator::compileExprIntegerLiteral(const ast::ExprIntegerLiteral& exprIntegerLiteral) {
    // push integer value onto stack
    this->emit(Instruction{Opcode::PUSH,
        {
            AssemblyType::I32,
            Immediate{Number{exprIntegerLiteral.value}}
        },
        SourceLocation{0, exprIntegerLiteral.line, exprIntegerLiteral.column}
    });
}

void compiler::AssemblyGenerator::compileExprFloatLiteral(const ast::ExprFloatLiteral& exprFloatLiteral) {
    // push float value onto stack
    this->emit(Instruction{Opcode::PUSH,
        {
            AssemblyType::F32,
            Immediate{Number{static_cast<float>(exprFloatLiteral.value)}}
        },
        SourceLocation{0, exprFloatLiteral.line, exprFloatLiteral.column}
    });
}

void compiler::AssemblyGenerator::compileExprBoolLiteral(const ast::ExprBoolLiteral& exprBoolLiteral) {
    // push bool value onto stack
    this->emit(Instruction{Opcode::PUSH,
        {
            AssemblyType::UI32,
            Immediate{Number{exprBoolLiteral.value ? 1 : 0}} // push 1 if 'true', push 0 if 'false'
        },
        SourceLocation{0, exprBoolLiteral.line, exprBoolLiteral.column}
    });
}

void compiler::AssemblyGenerator::compilerExprCharLiteral(const ast::ExprCharLiteral& exprCharLiteral) {
    // push char value onto stack
    this->emit(Instruction{Opcode::PUSH,
        {
            AssemblyType::UI32,
            Immediate{getNumber(exprCharLiteral.resultingType, exprCharLiteral.value)}
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
    return "__scope__" + std::to_string(scopeFunctionNumber);
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
