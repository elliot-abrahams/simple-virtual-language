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
        METADATA,
        METADATA_SOURCE,
        METADATA_FUNCTION,
        METADATA_LINE_TABLE
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
        ERROR_REF,
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
        {"rot", 0x06},
        // memory
        {"load", 0x07},
        {"loadB", 0x08},
        {"loadG", 0x09},
        {"loadL", 0x0a},
        {"store", 0x0b},
        {"storeB", 0x0c},
        {"storeG", 0x0d},
        {"storeL", 0x0e},
        {"alloc", 0x0f},
        {"free", 0x10},
        // control
        {"native", 0x11},
        {"call", 0x12},
        {"ret", 0x13},
        {"jmp", 0x14},
        {"jez", 0x15},
        {"jnz", 0x16},
        // arithmetic
        {"add", 0x17},
        {"sub", 0x18},
        {"mul", 0x19},
        {"div", 0x1a},
        {"mod", 0x1b},
        {"not", 0x1c},
        {"and", 0x1d},
        {"orr", 0x1e},
        {"xor", 0x1f},
        {"shl", 0x20},
        {"shr", 0x21},
        {"sar", 0x22},
        {"ceq", 0x23},
        {"cne", 0x24},
        {"clt", 0x25},
        {"cle", 0x26},
        {"cgt", 0x27},
        {"cge", 0x28},
        // other
        {"conv", 0x29},
        {"throw", 0x2a}
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

    inline const std::map<std::string, uint8_t> errorRef {
        {"array_index", 0x00}
    };

    enum class OperandType {
        IMMEDIATE,
        STRING,
        TYPE,
        DATA_TYPE,
        LABEL_REF,
        NATIVE_REF,
        ERROR_REF
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

    struct SourceMetadata {
        uint16_t sourceId;
        std::string path;
    };

    struct FunctionMetadata {
        uint32_t startAddress;
        uint32_t endAddress;
        uint16_t sourceId;
        std::string name;
    };

    struct LineTableMetadata {
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
        SourceMetadata,
        FunctionMetadata,
        LineTableMetadata
    >;
}


#endif //SIMPLE_VM_TYPES_H