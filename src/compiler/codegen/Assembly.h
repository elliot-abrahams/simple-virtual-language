#ifndef SV_ASSEMBLY_H
#define SV_ASSEMBLY_H

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>


namespace compiler {

    enum class Opcode {
        NOP,
        HALT,

        PUSH,
        POP,
        DUP,
        SWAP,

        LOAD,
        LOADB,
        LOADG,
        LOADL,
        STORE,
        STOREB,
        STOREG,
        STOREL,
        ALLOC,
        FREE,

        NATIVE,
        CALL,
        RET,
        JMP,
        JEZ,
        JNZ,

        ADD,
        SUB,
        MUL,
        DIV,
        MOD,
        NOT,
        AND,
        ORR,
        XOR,
        SHL,
        SHR,
        SAR,
        CEQ,
        CNE,
        CLT,
        CLE,
        CGT,
        CGE,

        CONV
    };

    enum class AssemblyType {
        I32,
        UI32,
        I64,
        UI64,
        F32,
        F64,
        PTR,

        STR
    };

    enum class Directive {
        DATA
    };

    enum class IRMarker {
        METHOD_DEF_END
    };

    enum class NativeRef {
        EXIT,
        PRINT,
        PRINT_STR
    };

    enum class MethodDefType {
        USER,
        SCOPE,
        BUILTIN
    };

    struct Number {
        const std::variant<
            int32_t,
            uint32_t,
            int64_t,
            uint64_t,
            float,
            double
        > value;
    };

    struct LabelRef {
        const std::string name;
    };

    struct LabelDef {
        const std::string name;
    };

    struct Immediate {
        const Number value;
    };

    struct SourceLocation {
        const uint16_t sourceId;
        const uint32_t line;
        const uint16_t column;
    };

    using Operand = std::variant<
        AssemblyType,
        Immediate,
        LabelRef,
        NativeRef
    >;

    struct Instruction {
        const Opcode opcode;
        const std::vector<Operand> operands;

        const std::optional<SourceLocation> source;
    };

    struct MethodDef {
        const LabelDef name;
        const uint8_t numberOfArguments;
        const uint32_t numberOfLocals;
        const MethodDefType type;

        const std::optional<SourceLocation> source;
    };

    using DataValue = std::variant<std::string, Number>;

    struct DataDef {
        const LabelDef name;
        const AssemblyType type;
        const DataValue value;
    };

    using AssemblyItem = std::variant<
        Directive,
        Instruction,
        LabelDef,
        MethodDef,
        DataDef,
        IRMarker
    >;

}


#endif //SV_ASSEMBLY_H