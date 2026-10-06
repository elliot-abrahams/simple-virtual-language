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

    // void println(int)
    symbolTable.declareBuiltinFunction(
        BuiltinFunctionId::PRINTLN_INT,
        "println",
        Type{VOID_TYPE_ID, 0},
        {Type{INT_TYPE_ID, 0}}
    );

    // void println(float)
    symbolTable.declareBuiltinFunction(
        BuiltinFunctionId::PRINTLN_FLOAT,
        "println",
        Type{VOID_TYPE_ID, 0},
        {Type{FLOAT_TYPE_ID, 0}}
    );

    // void println(bool)
    symbolTable.declareBuiltinFunction(
        BuiltinFunctionId::PRINTLN_BOOL,
        "println",
        Type{VOID_TYPE_ID, 0},
        {Type{BOOL_TYPE_ID, 0}}
    );

    // void println(char)
    symbolTable.declareBuiltinFunction(
        BuiltinFunctionId::PRINTLN_CHAR,
        "println",
        Type{VOID_TYPE_ID, 0},
        {Type{CHAR_TYPE_ID, 0}}
    );
}
