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
        {"rotD", 0x06},
        {"rotU", 0x07},
        // memory
        {"load", 0x08},
        {"loadB", 0x09},
        {"loadG", 0x0a},
        {"loadL", 0x0b},
        {"store", 0x0c},
        {"storeB", 0x0d},
        {"storeG", 0x0e},
        {"storeL", 0x0f},
        {"addrL", 0x10},
        {"alloc", 0x11},
        {"free", 0x12},
        // control
        {"native", 0x13},
        {"call", 0x14},
        {"ret", 0x15},
        {"jmp", 0x16},
        {"jez", 0x17},
        {"jnz", 0x18},
        // arithmetic
        {"add", 0x19},
        {"sub", 0x1a},
        {"mul", 0x1b},
        {"div", 0x1c},
        {"mod", 0x1d},
        {"not", 0x1e},
        {"and", 0x1f},
        {"orr", 0x20},
        {"xor", 0x21},
        {"shl", 0x22},
        {"shr", 0x23},
        {"sar", 0x24},
        {"ceq", 0x25},
        {"cne", 0x26},
        {"clt", 0x27},
        {"cle", 0x28},
        {"cgt", 0x29},
        {"cge", 0x2a},
        // other
        {"conv", 0x2b},
        {"throw", 0x2c}
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
        {"array_index_out_of_range", 0x00},
        {"negative_array_length", 0x01},
        {"array_initialiser_length", 0x02}
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