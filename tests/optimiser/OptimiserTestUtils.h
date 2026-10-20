#ifndef SV_OPTIMISERTESTUTILS_H
#define SV_OPTIMISERTESTUTILS_H

#include "../ASTTestUtils.h"
#include "../../src/compiler/optimiser/Optimiser.h"
#include "../../src/compiler/semantic/SemanticAnalyser.h"

using namespace testUtils;

namespace optimiserTest {

    inline void ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        const ExpectedProgram& expectedProgram,
        const std::string& sourceCode
    ) {
        const auto program = compiler::Compiler::testParsing(sourceCode);

        const auto path = new std::filesystem::path("Testing");

        const auto symbolTable = new compiler::SymbolTable();

        // add builtin functions to symbol table
        compiler::Builtins::registerBuiltinFunctions(*symbolTable);

        // create type registry
        const auto typeRegistry = new compiler::TypeRegistry();

        // register builtin type information
        compiler::Compiler::registerBuiltinTypes(typeRegistry);

        // build symbol table and type check the program
        auto semanticAnalyser = compiler::SemanticAnalyser(symbolTable, typeRegistry, path);
        semanticAnalyser.processProgram(*program);

        // optimise AST
        auto optimiser = compiler::Optimiser(*path, driver::OptimisationLevel::DEFAULT);
        optimiser.optimiseProgram(*program);

        ASSERT_PROGRAM_EQ(expectedProgram, *program);
        delete path;
    }

}



#endif //SV_OPTIMISERTESTUTILS_H