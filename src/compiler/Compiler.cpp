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

void compiler::Compiler::compile(const std::filesystem::path& path) {

    const std::string source = readFile(path);

    auto* tokeniser = new Tokeniser(source, &path);

    const auto parser = new Parser(tokeniser, &path);

    try {
        // generate AST
        const std::unique_ptr<ast::Program> program = parser->parseProgram();

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
        auto code = codeGenerator->generateCode(*program);

        // generate svma file
        generateFile(path, code);

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

std::string compiler::Compiler::readFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);

    if (!file) {
        throw std::runtime_error("Failed to open file.");
    }

    return {
        std::istreambuf_iterator<char>(file),
    std::istreambuf_iterator<char>()
    };
}

void compiler::Compiler::generateFile(const std::filesystem::path &path, const std::vector<std::string>& assembly) {
    const std::string outputFilePathName = path.parent_path().string() + '/' + path.stem().string() + std::string(ASSEMBLY_FILE_EXTENSION);
    std::ofstream outputFile(outputFilePathName);

    if (!outputFile.is_open()) {
        std::cerr << "Error: could not create and open " << outputFilePathName << std::endl;
        return;
    }

    for (const auto& line : assembly) {
        outputFile << line << '\n';
    }
}
