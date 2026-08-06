#include "Compiler.h"

#include <fstream>
#include <iostream>

#include "../include/Error.h"
#include "Parser.h"
#include "SemanticAnalyser.h"
#include "CodeGenerator.h"
#include "../Driver.h"
#include "BuiltinFunctions.h"


compiler::Compiler::Compiler() {}

std::vector<std::string> compiler::Compiler::compile(const std::string& sourceCode, const std::filesystem::path& path) {
    try {
        auto* tokeniser = new Tokeniser(sourceCode, &path);
        const auto parser = new Parser(tokeniser, &path);

        // generate AST
        const std::unique_ptr<ast::Program> program = parser->parseProgram();

        delete tokeniser;
        delete parser;

        const auto symbolTable = new SymbolTable();

        // add builtin functions to symbol table
        BuiltinFunctions::registerBuiltinFunctions(*symbolTable);

        // build symbol table and type check the program
        auto semanticAnalyser = SemanticAnalyser(symbolTable, &path);
        semanticAnalyser.processProgram(*program);

        // assign slots to local symbols
        symbolTable->assignSlotsToLocalSymbols();

        // generate assembly code
        const auto codeGenerator = new CodeGenerator(symbolTable);
        return codeGenerator->generateCode(*program);

    } catch (const CompilerError& e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

std::unique_ptr<ast::Program> compiler::Compiler::testParsing(const std::string& sourceCode) {
    const auto path = new std::filesystem::path("Testing");
    auto* tokeniser = new Tokeniser(sourceCode, path);
    const auto parser = new Parser(tokeniser, path);

    // generate AST
    return parser->parseProgram();
}
