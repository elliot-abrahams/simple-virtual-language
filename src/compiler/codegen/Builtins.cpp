#include "Builtins.h"

#include "../SymbolTable.h"

void compiler::Builtins::registerBuiltinFunctions(SymbolTable& symbolTable) {

    // void exit(int)
    symbolTable.declareBuiltinFunction(
        BuiltinFunctionId::EXIT_INT,
        "exit",
        SemanticType{Type::VOID_RETURN_TYPE, 0},
        {SemanticType{Type::INT, 0}}
    );

    // void print(int)
    symbolTable.declareBuiltinFunction(
        BuiltinFunctionId::PRINT_INT,
        "print",
        SemanticType{Type::VOID_RETURN_TYPE, 0},
        {SemanticType{Type::INT, 0}}
    );

    // void print(float)
    symbolTable.declareBuiltinFunction(
        BuiltinFunctionId::PRINT_FLOAT,
        "print",
        SemanticType{Type::VOID_RETURN_TYPE, 0},
        {SemanticType{Type::FLOAT, 0}}
    );

    // void print(bool)
    symbolTable.declareBuiltinFunction(
        BuiltinFunctionId::PRINT_BOOL,
        "print",
        SemanticType{Type::VOID_RETURN_TYPE, 0},
        {SemanticType{Type::BOOL, 0}}
    );
}
