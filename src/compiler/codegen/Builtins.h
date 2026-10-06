#ifndef SVM_BUILTINFUNCTIONS_H
#define SVM_BUILTINFUNCTIONS_H
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>


namespace compiler {
    class SymbolTable;
}

namespace compiler {

    enum class BuiltinFunctionId {
        NONE,

        EXIT_INT,

        PRINT_INT,
        PRINT_FLOAT,
        PRINT_BOOL,
        PRINT_CHAR,

        PRINTLN_INT,
        PRINTLN_FLOAT,
        PRINTLN_BOOL,
        PRINTLN_CHAR,
    };

    enum class BuiltinDataId {
        TRUE_STRING,
        FALSE_STRING,
    };

    struct BuiltinFunction {
        const std::string functionLabel;
        const uint8_t numberOfArguments;
        const uint32_t numberOfLocals;
        const std::vector<std::string> functionBodyAssembly;
        const std::vector<BuiltinFunctionId> requiredBuiltinFunctions;
        const std::vector<BuiltinDataId> requiredBuiltinData;
    };

    class Builtins {
    public:
        static void registerBuiltinFunctions(SymbolTable& symbolTable);

        static BuiltinFunction* getBuiltinFunction(const BuiltinFunctionId id) {
            return &builtinFunctionAssembly.at(id);
        }

        static std::string* getBuiltinData(const BuiltinDataId id) {
            return &builtinData.at(id);
        }

        static std::string getBuiltinFunctionLabel(const BuiltinFunctionId id) {
            return builtinFunctionAssembly.at(id).functionLabel;
        }

    private:
        inline static std::unordered_map<BuiltinFunctionId, BuiltinFunction> builtinFunctionAssembly = {

            {BuiltinFunctionId::EXIT_INT, BuiltinFunction{
                "__Builtin__exit(int)",
                1,
                0,
                {
                "    loadL i32 #1",
                "    native exit",
                "    ret"
                },
                {},
                {}
            }},

            {BuiltinFunctionId::PRINT_INT, BuiltinFunction{
                "__Builtin__print(int)",
                1,
                0,
                {
                "    loadL i32 #1",
                "    native print",
                "    ret"
            },
                {},
                {}
            }},

            {BuiltinFunctionId::PRINT_FLOAT, BuiltinFunction{
                "__Builtin__print(float)",
                1,
                0,
                {
                "    loadL f32 #1",
                "    native print",
                "    ret"
            },
                {},
                {}
            }},

            {BuiltinFunctionId::PRINT_BOOL, BuiltinFunction{
                "__Builtin__print(bool)",
                1,
                0,
                {
                "    loadL ui32 #1",
                "    jez $__print(bool)__false",
                "    push ptr $__true__string",
                "    jmp $__print(bool)__print",
                "$__print(bool)__false:",
                "    push ptr $__false__string",
                "$__print(bool)__print:",
                "    native print_str",
                "    ret",
            },
                {},
                {BuiltinDataId::TRUE_STRING, BuiltinDataId::FALSE_STRING}
            }},

        {BuiltinFunctionId::PRINT_CHAR, BuiltinFunction{
            "__Builtin__print(char)",
            1,
            0,
            {
                "    loadL ui32 #1",
                "    native print_char",
                "    ret"
            },
                {},
                {}
            }},

        {BuiltinFunctionId::PRINTLN_INT, BuiltinFunction{
            "__Builtin__println(int)",
            1,
            0,
            {
                "    loadL i32 #1",
                "    call $__Builtin__print(int)",
                "    push ui32 #10",
                "    native print_char",
                "    ret"
            },
                {BuiltinFunctionId::PRINT_INT},
                {}
        }},

        {BuiltinFunctionId::PRINTLN_FLOAT, BuiltinFunction{
            "__Builtin__println(float)",
            1,
            0,
            {
                "    loadL f32 #1",
                "    call $__Builtin__print(float)",
                "    push ui32 #10",
                "    native print_char",
                "    ret"
            },
                {BuiltinFunctionId::PRINT_FLOAT},
                {}
        }},

        {BuiltinFunctionId::PRINTLN_BOOL, BuiltinFunction{
            "__Builtin__println(bool)",
            1,
            0,
            {
                "    loadL i32 #1",
                "    call $__Builtin__print(bool)",
                "    push ui32 #10",
                "    native print_char",
                "    ret"
            },
                {BuiltinFunctionId::PRINT_BOOL},
                {}
        }},

        {BuiltinFunctionId::PRINTLN_CHAR, BuiltinFunction{
            "__Builtin__println(char)",
            1,
            0,
            {
                "    loadL i32 #1",
                "    call $__Builtin__print(char)",
                "    push ui32 #10",
                "    native print_char",
                "    ret"
            },
                {BuiltinFunctionId::PRINT_CHAR},
                {}
        }}

        };

        inline static std::unordered_map<BuiltinDataId, std::string> builtinData = {
            {BuiltinDataId::TRUE_STRING, "$__true__string: str \"true\""},
            {BuiltinDataId::FALSE_STRING, "$__false__string: str \"false\""},
        };
    };
}


#endif //SVM_BUILTINFUNCTIONS_H