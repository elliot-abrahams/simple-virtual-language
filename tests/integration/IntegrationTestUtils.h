#ifndef SVM_INTEGRATIONTESTUTILS_H
#define SVM_INTEGRATIONTESTUTILS_H
#include <iostream>
#include <sstream>
#include <string>
#include "../../src/compiler/Tokeniser.h"
#include "../../src/compiler/Parser.h"
#include "../../src/compiler/SemanticAnalyser.h"
#include "../../src/compiler/codegen/AssemblyGenerator.h"
#include "../../src/include/Error.h"
#include "../../src/assembler/Assembler.h"
#include "../../src/vm/VM.h"
#include "../../src/compiler/codegen/AssemblyEmitter.h"
#include "gtest/gtest.h"


namespace integrationTests {

    inline void compileAndRun(
        VM* vm,
        const std::string& sourceCode
    ) {
        const auto path = new std::filesystem::path("Testing");
        auto* tokeniser = new compiler::Tokeniser(sourceCode, path);
        const auto parser = new compiler::Parser(tokeniser, path);

        try {
            // generate AST
            const std::unique_ptr<ast::Program> program = parser->parseProgram();

            const auto symbolTable = new compiler::SymbolTable();

            // add builtin functions to symbol table
            compiler::Builtins::registerBuiltinFunctions(*symbolTable);

            // build symbol table and type check the program
            auto semanticAnalyser = compiler::SemanticAnalyser(symbolTable, path);
            semanticAnalyser.processProgram(*program);

            // assign slots to local symbols
            symbolTable->assignSlotsToLocalSymbols();

            // generate assembly code
            const auto codeGenerator = new compiler::AssemblyGenerator(symbolTable);
            auto assemblyIR = codeGenerator->compileProgram(*program);

            auto assemblyEmitter = new compiler::AssemblyEmitter();

            const auto code = assemblyEmitter->emitProgram(assemblyIR, codeGenerator->getRequiredBuiltinFunctions(), codeGenerator->getRequiredBuiltinData());

            delete codeGenerator;
            delete assemblyEmitter;

            std::string assemblyCode;
            for (std::string line : code) {
                assemblyCode += line + "\n";
            }

            assembler::Assembler assembler;

            const auto bytecode = assembler.assembleString(assemblyCode);

            vm->run(&bytecode.value());

        } catch (const CompilerError& e) {
            std::cerr << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    inline void ASSERT_OUTPUT_EQ(
        const std::string& sourceCode,
        const std::string& expectedOutput
    ) {
        const std::stringstream buffer;
        std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

        auto vm = new VM();

        try {
            compileAndRun(vm, sourceCode);
        } catch (const CompilerError& e) {
            std::cerr << e.what() << std::endl;
            FAIL();
        }
        std::cout.rdbuf(old);
        EXPECT_EQ(buffer.str(), expectedOutput);
    }

    inline void ASSERT_EXIT_CODE_EQ(
        const std::string& sourceCode,
        const int expectedExitCode
    ) {

        auto vm = new VM();

        try {
            compileAndRun(vm, sourceCode);
        } catch (const CompilerError& e) {
            std::cerr << e.what() << std::endl;
            FAIL();
        }

        ASSERT_EQ(expectedExitCode, vm->getExitStatus());

    }

}


#endif //SVM_INTEGRATIONTESTUTILS_H