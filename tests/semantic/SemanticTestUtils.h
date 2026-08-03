#ifndef SVM_SEMANTICTESTUTILS_H
#define SVM_SEMANTICTESTUTILS_H
#include <filesystem>
#include <string>
#include "../../src/compiler/Tokeniser.h"
#include "../../src/compiler/Parser.h"
#include "../../src/compiler/SemanticAnalyser.h"
#include "../../src/include/Error.h"
#include "gtest/gtest.h"

namespace semanticTest {

    inline void testSemanticAnalyser (
        const std::string& sourceCode
    ) {
        const auto path = new std::filesystem::path("Testing");
        auto* tokeniser = new compiler::Tokeniser(sourceCode, path);
        const auto parser = new compiler::Parser(tokeniser, path);

        // generate AST
        const auto program = parser->parseProgram();

        const auto symbolTable = new compiler::SymbolTable();

        // add builtin functions to symbol table
        compiler::BuiltinFunctions::registerBuiltinFunctions(*symbolTable);

        // build symbol table and type check the program
        auto semanticAnalyser = compiler::SemanticAnalyser(symbolTable, path);
        semanticAnalyser.processProgram(*program);
    };

    inline void ASSERT_SEMANTICALLY_VALID (
        const std::string& sourceCode
    ) {
        ASSERT_NO_THROW(testSemanticAnalyser(sourceCode));
    }

    inline void ASSERT_THROWS_SEMANTIC_ERROR (
        const std::string& sourceCode
    ) {
        ASSERT_THROW(testSemanticAnalyser(sourceCode), SemanticError);
    }

    inline void ASSERT_THROWS_TYPE_ERROR(
        const std::string& sourceCode
    ) {
        ASSERT_THROW(testSemanticAnalyser(sourceCode), TypeError);
    }

}


#endif //SVM_SEMANTICTESTUTILS_H