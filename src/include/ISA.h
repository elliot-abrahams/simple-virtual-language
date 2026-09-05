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
        ROT = 0x06,
        LOAD = 0x07,
        LOADB = 0x08,
        LOADG = 0x09,
        LOADL = 0x0a,
        STORE = 0x0b,
        STOREB = 0x0c,
        STOREG = 0x0d,
        STOREL = 0x0e,
        ALLOC = 0x0f,
        FREE = 0x10,
        NATIVE = 0x11,
        CALL = 0x12,
        RET = 0x13,
        JMP = 0x14,
        JEZ = 0x15,
        JNZ = 0x16,
        ADD = 0x17,
        SUB = 0x18,
        MUL = 0x19,
        DIV = 0x1a,
        MOD = 0x1b,
        NOT = 0x1c,
        AND = 0x1d,
        ORR = 0x1e,
        XOR = 0x1f,
        SHL = 0x20,
        SHR = 0x21,
        SAR = 0x22,
        CEQ = 0x23,
        CNE = 0x24,
        CLT = 0x25,
        CLE = 0x26,
        CGT = 0x27,
        CGE = 0x28,
        CONV = 0x29,
        THROW = 0x2a
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