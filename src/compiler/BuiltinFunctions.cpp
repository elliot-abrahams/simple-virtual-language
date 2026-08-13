#include "BuiltinFunctions.h"

#include "SymbolTable.h"

void compiler::BuiltinFunctions::registerBuiltinFunctions(SymbolTable& symbolTable) {

    // void print(int)
    symbolTable.declareBuiltinFunction(
        BuiltinId::PRINT_INT,
        "print",
        Type::VOID_RETURN_TYPE,
        {Type::INT}
    );

    // void print(float)
    symbolTable.declareBuiltinFunction(
        BuiltinId::PRINT_FLOAT,
        "print",
        Type::VOID_RETURN_TYPE,
        {Type::FLOAT}
    );

    // void print(bool)
    symbolTable.declareBuiltinFunction(
        BuiltinId::PRINT_BOOL,
        "print",
        Type::VOID_RETURN_TYPE,
        {Type::BOOL}
    );
}
