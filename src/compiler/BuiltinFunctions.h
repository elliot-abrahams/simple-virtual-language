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
        PRINT_INT,
        PRINT_FLOAT,
        PRINT_BOOL,
    };

    struct BuiltinFunction {
        const std::string functionLabel;
        const uint8_t numberOfArguments;
        const uint32_t numberOfLocals;
        const std::vector<std::string> functionBodyAssembly;
    };

    class BuiltinFunctions {
    public:
        static void registerBuiltinFunctions(SymbolTable& symbolTable);

        static BuiltinFunction* getBuiltinFunction(const BuiltinId id) {
            return &builtinFunctionAssembly.at(id);
        }

        static std::string getBuiltinFunctionLabel(const BuiltinId id) {
            return builtinFunctionAssembly.at(id).functionLabel;
        }

    private:
        inline static std::unordered_map<BuiltinId, BuiltinFunction> builtinFunctionAssembly = {
            {BuiltinId::PRINT_INT, BuiltinFunction{
                "$__Builtin__print(int)",
                1,
                0,
                {
                "loadL i32 #1",
                "out i32",
                "ret"
            }}},

            {BuiltinId::PRINT_FLOAT, BuiltinFunction{
                "$__Builtin__print(float)",
                1,
                0,
                {
                "loadL f32 #1",
                "out f32",
                "ret"
            }}},

            {BuiltinId::PRINT_BOOL, BuiltinFunction{
                "$__Builtin__print(bool)",
                1,
                0,
                {
                "loadL ui32 #1",
                "out ui32", // TODO: change to string "true" / "false"
                "ret"
            }}}
        };
    };
}


#endif //SVM_BUILTINFUNCTIONS_H