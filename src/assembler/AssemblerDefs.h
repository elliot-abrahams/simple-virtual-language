#ifndef SIMPLE_VM_TYPES_H
#define SIMPLE_VM_TYPES_H


#include <map>
#include <set>
#include <variant>
#include <string>
#include <vector>
#include <cstdint>

namespace AssemblerDefs {

    enum class Section {
        CODE,
        DATA,
        DEBUG,
        DEBUG_SOURCE,
        DEBUG_FUNCTION,
        DEBUG_LINE_TABLE
    };

    enum class SVMATokenType {
        DIRECTIVE,
        INSTRUCTION,
        TYPE,
        DATA_TYPE,
        NUMBER,
        IMMEDIATE,
        HEX,
        STRING,
        LABEL_REF,
        LABEL_DEF,
        METHOD_DEF,
        METHOD_METADATA_FIELD,
        NATIVE_REF,
        END_OF_FILE
    };

    struct SVMAToken {
        SVMATokenType type;
        std::string value;
        int lineNumber;
    };

    inline const std::map<std::string, uint8_t> opcode {
        {"nop", 0x00},
        {"halt", 0x01},
        // stack
        {"push", 0x02},
        {"pop", 0x03},
        {"dup", 0x04},
        {"swap", 0x05},
        // memory
        {"load", 0x06},
        {"loadB", 0x07},
        {"loadG", 0x08},
        {"loadL", 0x09},
        {"store", 0x0a},
        {"storeB", 0x0b},
        {"storeG", 0x0c},
        {"storeL", 0x0d},
        {"alloc", 0x0e},
        {"free", 0x0f},
        // control
        {"native", 0x10},
        {"call", 0x11},
        {"ret", 0x12},
        {"jmp", 0x13},
        {"jez", 0x14},
        {"jnz", 0x15},
        // arithmetic
        {"add", 0x16},
        {"sub", 0x17},
        {"mul", 0x18},
        {"div", 0x19},
        {"mod", 0x1a},
        {"not", 0x1b},
        {"and", 0x1c},
        {"orr", 0x1d},
        {"xor", 0x1e},
        {"shl", 0x1f},
        {"shr", 0x20},
        {"sar", 0x21},
        {"ceq", 0x22},
        {"cne", 0x23},
        {"clt", 0x24},
        {"cle", 0x25},
        {"cgt", 0x26},
        {"cge", 0x27},
        // other
        {"conv", 0x28},
    };

    inline const std::map<std::string, uint8_t> type {
        {"i32", 0x00},
        {"ui32", 0x01},
        {"i64", 0x02},
        {"ui64", 0x03},
        {"f32", 0x04},
        {"f64", 0x05},
        {"ptr", 0x06},
    };

    inline const std::map<std::string, uint8_t> dataType {
        {"str", 0x08},
    };

    inline const std::set<std::string> method_metadata_fields {
        "args",
        "locals"
    };

    inline const std::map<std::string, uint8_t> nativeRef {
        {"exit", 0x00},
        {"print", 0x01},
        {"print_str", 0x03}
    };

    enum class OperandType {
        IMMEDIATE,
        STRING,
        TYPE,
        DATA_TYPE,
        LABEL_REF,
        NATIVE_REF
    };

    struct Operand {
        OperandType type;
        std::string value;
    };

    struct Instruction {
        std::string opcode;
        std::vector<Operand> operands;
        int lineNumber;
    };

    struct Label {
        std::string name;
        int lineNumber;
    };

    struct MethodDef {
        std::string name;
        uint8_t numberOfArguments;
        uint32_t numberOfLocals;
        int lineNumber;
    };

    struct Data {
        std::string name;
        std::string type;
        std::string value;
        int lineNumber;
    };

    struct DebugSource {
        uint16_t sourceId;
        std::string path;
    };

    struct DebugFunction {
        uint32_t startAddress;
        uint32_t endAddress;
        uint16_t sourceId;
        std::string name;
    };

    struct DebugLine {
        uint32_t startAddress;
        uint32_t endAddress;
        uint16_t sourceId;
        uint32_t line;
        uint16_t column;
    };

    using Statement = std::variant<
        Instruction,
        Label,
        MethodDef,
        Data,
        Section,
        DebugSource,
        DebugFunction,
        DebugLine
    >;
}


#endif //SIMPLE_VM_TYPES_H