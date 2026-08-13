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

    enum class BuiltinId {
        NONE,
        EXIT_INT,
        PRINT_INT,
        PRINT_FLOAT,
        PRINT_BOOL,
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
        const std::vector<BuiltinDataId> requiredBuiltinData;
    };

    class BuiltinFunctions {
    public:
        static void registerBuiltinFunctions(SymbolTable& symbolTable);

        static BuiltinFunction* getBuiltinFunction(const BuiltinId id) {
            return &builtinFunctionAssembly.at(id);
        }

        static std::string* getBuiltinData(const BuiltinDataId id) {
            return &builtinData.at(id);
        }

        static std::string getBuiltinFunctionLabel(const BuiltinId id) {
            return builtinFunctionAssembly.at(id).functionLabel;
        }

    private:
        inline static std::unordered_map<BuiltinId, BuiltinFunction> builtinFunctionAssembly = {

            {BuiltinId::EXIT_INT, BuiltinFunction{
                "$__Builtin__exit(int)",
                1,
                0,
                {
                "        loadL i32 #1",
                "        native exit",
                "        ret"
                },
                {}
            }},

            {BuiltinId::PRINT_INT, BuiltinFunction{
                "$__Builtin__print(int)",
                1,
                0,
                {
                "        loadL i32 #1",
                "        native print",
                "        ret"
            },
                {}
            }},

            {BuiltinId::PRINT_FLOAT, BuiltinFunction{
                "$__Builtin__print(float)",
                1,
                0,
                {
                "        loadL f32 #1",
                "        native print",
                "        ret"
            },
                {}
            }},

            {BuiltinId::PRINT_BOOL, BuiltinFunction{
                "$__Builtin__print(bool)",
                1,
                0,
                {
                "        loadL ui32 #1",
                "        jez $__print(bool)__false",
                "        push ptr $__true__string",
                "        jmp $__print(bool)__print",
                "    $__print(bool)__false:",
                "        push ptr $__false__string",
                "    $__print(bool)__print:",
                "        native print_str",
                "        ret",
            },
                {BuiltinDataId::TRUE_STRING, BuiltinDataId::FALSE_STRING}
            }}

        };

        inline static std::unordered_map<BuiltinDataId, std::string> builtinData = {
            {BuiltinDataId::TRUE_STRING, "$__true__string: str \"true\""},
            {BuiltinDataId::FALSE_STRING, "$__false__string: str \"false\""},
        };
    };
}


#endif //SVM_BUILTINFUNCTIONS_H