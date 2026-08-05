#ifndef SVM_INTEGRATIONTESTUTILS_H
#define SVM_INTEGRATIONTESTUTILS_H
#include <iostream>
#include <sstream>
#include <string>
#include "../../src/compiler/Tokeniser.h"
#include "../../src/compiler/Parser.h"
#include "../../src/compiler/SemanticAnalyser.h"
#include "../../src/compiler/CodeGenerator.h"
#include "../../src/include/Error.h"
#include "../../src/assembler/Assembler.h"
#include "../../src/vm/VM.h"
#include "gtest/gtest.h"


namespace integrationTests {

    inline void compileAndRun(
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
            compiler::BuiltinFunctions::registerBuiltinFunctions(*symbolTable);

            // build symbol table and type check the program
            auto semanticAnalyser = compiler::SemanticAnalyser(symbolTable, path);
            semanticAnalyser.processProgram(*program);

            // assign slots to local symbols
            symbolTable->assignSlotsToLocalSymbols();

            // generate assembly code
            const auto codeGenerator = new compiler::CodeGenerator(symbolTable);
            auto code = codeGenerator->generateCode(*program);

            std::string assemblyCode;
            for (std::string& line : code) {
                assemblyCode += line + "\n";
            }

            assembler::Assembler assembler;
            VM vm;

            const auto bytecode = assembler.assembleString(assemblyCode);

            vm.run(&bytecode.value());

        } catch (const CompilerError& e) {
            std::cerr << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    inline void ASSERT_OUTPUT_EQ(
        const std::string& sourceCode,
        const std::string& expectedOutput
    ) {
        std::stringstream buffer;
        std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

        try {
            compileAndRun(sourceCode);
        } catch (const CompilerError& e) {
            std::cerr << e.what() << std::endl;
            FAIL();
        }
        std::cout.rdbuf(old);
        EXPECT_EQ(buffer.str(), expectedOutput);
    }

}


#endif //SVM_INTEGRATIONTESTUTILS_H