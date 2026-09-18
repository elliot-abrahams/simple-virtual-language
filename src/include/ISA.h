#ifndef SVM_ISA_H
#define SVM_ISA_H

namespace ISA {

    enum class Opcode : uint8_t {
        NOP = 0x00,
        HALT = 0x01,
        PUSH = 0x02,
        POP = 0x03,
        DUP = 0x04,
        SWAP = 0x05,
        ROTD = 0x06,
        ROTU = 0x07,
        LOAD = 0x08,
        LOADB = 0x09,
        LOADG = 0x0a,
        LOADL = 0x0b,
        STORE = 0x0c,
        STOREB = 0x0d,
        STOREG = 0x0e,
        STOREL = 0x0f,
        ADDRL = 0x10,
        ALLOC = 0x11,
        FREE = 0x12,
        NATIVE = 0x13,
        CALL = 0x14,
        RET = 0x15,
        JMP = 0x16,
        JEZ = 0x17,
        JNZ = 0x18,
        ADD = 0x19,
        SUB = 0x1a,
        MUL = 0x1b,
        DIV = 0x1c,
        MOD = 0x1d,
        NOT = 0x1e,
        AND = 0x1f,
        ORR = 0x20,
        XOR = 0x21,
        SHL = 0x22,
        SHR = 0x23,
        SAR = 0x24,
        CEQ = 0x25,
        CNE = 0x26,
        CLT = 0x27,
        CLE = 0x28,
        CGT = 0x29,
        CGE = 0x2a,
        CONV = 0x2b,
        THROW = 0x2c
    };

    enum class Type : uint8_t {
        I32 = 0x00,
        UI32 = 0x01,
        I64 = 0x02,
        UI64 = 0x03,
        F32 = 0x04,
        F64 = 0x05,
        PTR = 0x06,
        STR = 0x07,
    };
}

#endif //SVM_ISA_H