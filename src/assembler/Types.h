//
// Created by erael on 05/05/2026.
//

#ifndef SIMPLE_VM_TYPES_H
#define SIMPLE_VM_TYPES_H
#include <map>
#include <set>

#endif //SIMPLE_VM_TYPES_H

#pragma once
#include <variant>
#include <string>
#include <vector>
#include <cstdint>

namespace Types {

    enum class Section {
        CODE,
        DATA
    };

    enum class SVMATokenType {
        DATA_START,
        INSTRUCTION,
        TYPE,
        DATA_TYPE,
        NUMBER,
        IMMEDIATE,
        CHAR,
        STRING,
        LABEL_REF,
        LABEL_DEF,
        METHOD_DEF,
        METHOD_METADATA_FIELD,
        END_OF_FILE
    };

    struct SVMAToken {
        SVMATokenType type;
        std::string value;
        int lineNumber;
    };

    inline std::map<std::string, uint8_t> opcode {
        {"nop", 0x00},
        {"halt", 0x01},
        // stack
        {"push", 0x02},
        {"pop", 0x03},
        {"dup", 0x04},
        {"swap", 0x05},
        // memory
        {"load", 0x06},
        {"loadG", 0x07},
        {"loadL", 0x08},
        {"store", 0x09},
        {"storeG", 0x0a},
        {"storeL", 0x0b},
        {"alloc", 0x0c},
        {"free", 0x0d},
        // control
        {"call", 0x0e},
        {"ret", 0x0f},
        {"jmp", 0x10},
        {"jez", 0x11},
        {"jnz", 0x12},
        // arithmetic
        {"add", 0x13},
        {"sub", 0x14},
        {"mul", 0x15},
        {"div", 0x16},
        {"mod", 0x17},
        {"not", 0x18},
        {"notB", 0x19},
        {"and", 0x1a},
        {"orr", 0x1b},
        {"xor", 0x1c},
        {"shl", 0x1d},
        {"shr", 0x1e},
        {"ceq", 0x1f},
        {"cne", 0x20},
        {"clt", 0x21},
        {"cle", 0x22},
        {"cgt", 0x23},
        {"cge", 0x24},
        // other
        {"out", 0x25},
        {"inn", 0x26},
        {"conv", 0x27},
    };

    inline std::map<std::string, uint8_t> type {
        {"i32", 0x00},
        {"ui32", 0x01},
        {"i64", 0x02},
        {"ui64", 0x03},
        {"f32", 0x04},
        {"f64", 0x05},
        {"ptr", 0x06},
        {"char", 0x07},
    };

    inline std::map<std::string, uint8_t> dataType {
        {"str", 0x08},
    };

    inline std::set<std::string> method_metadata_fields {
        "args",
        "locals"
    };

    enum class OperandType {
        IMMEDIATE,
        CHAR,
        STRING,
        TYPE,
        DATA_TYPE,
        LABEL_REF,
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
        std::string type;
        std::string value;
        int lineNumber;
    };

    using Statement = std::variant<Instruction, Label, MethodDef, Data, Section>;
}