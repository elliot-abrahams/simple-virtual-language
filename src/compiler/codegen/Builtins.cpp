#include "Builtins.h"

#include "../SymbolTable.h"

void compiler::Builtins::registerBuiltinFunctions(SymbolTable& symbolTable) {

    // void exit(int)
    symbolTable.declareBuiltinFunction(
        BuiltinFunctionId::EXIT_INT,
        "exit",
        Type::VOID_RETURN_TYPE,
        {Type::INT}
    );

    // void print(int)
    symbolTable.declareBuiltinFunction(
        BuiltinFunctionId::PRINT_INT,
        "print",
        Type::VOID_RETURN_TYPE,
        {Type::INT}
    );

    // void print(float)
    symbolTable.declareBuiltinFunction(
        BuiltinFunctionId::PRINT_FLOAT,
        "print",
        Type::VOID_RETURN_TYPE,
        {Type::FLOAT}
    );

    // void print(bool)
    symbolTable.declareBuiltinFunction(
        BuiltinFunctionId::PRINT_BOOL,
        "print",
        Type::VOID_RETURN_TYPE,
        {Type::BOOL}
    );
}
