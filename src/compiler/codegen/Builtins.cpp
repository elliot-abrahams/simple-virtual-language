#include "Builtins.h"

#include "../semantic/SymbolTable.h"

void compiler::Builtins::registerBuiltinFunctions(SymbolTable& symbolTable) {

    // void exit(int)
    symbolTable.declareBuiltinFunction(
        BuiltinFunctionId::EXIT_INT,
        "exit",
        Type{VOID_TYPE_ID, 0},
        {Type{INT_TYPE_ID, 0}}
    );

    // void print(int)
    symbolTable.declareBuiltinFunction(
        BuiltinFunctionId::PRINT_INT,
        "print",
        Type{VOID_TYPE_ID, 0},
        {Type{INT_TYPE_ID, 0}}
    );

    // void print(float)
    symbolTable.declareBuiltinFunction(
        BuiltinFunctionId::PRINT_FLOAT,
        "print",
        Type{VOID_TYPE_ID, 0},
        {Type{FLOAT_TYPE_ID, 0}}
    );

    // void print(bool)
    symbolTable.declareBuiltinFunction(
        BuiltinFunctionId::PRINT_BOOL,
        "print",
        Type{VOID_TYPE_ID, 0},
        {Type{BOOL_TYPE_ID, 0}}
    );

    // void print(char)
    symbolTable.declareBuiltinFunction(
        BuiltinFunctionId::PRINT_CHAR,
        "print",
        Type{VOID_TYPE_ID, 0},
        {Type{CHAR_TYPE_ID, 0}}
    );
}
