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
        ALLOC = 0x10,
        FREE = 0x11,
        NATIVE = 0x12,
        CALL = 0x13,
        RET = 0x14,
        JMP = 0x15,
        JEZ = 0x16,
        JNZ = 0x17,
        ADD = 0x18,
        SUB = 0x19,
        MUL = 0x1a,
        DIV = 0x1b,
        MOD = 0x1c,
        NOT = 0x1d,
        AND = 0x1e,
        ORR = 0x1f,
        XOR = 0x20,
        SHL = 0x21,
        SHR = 0x22,
        SAR = 0x23,
        CEQ = 0x24,
        CNE = 0x25,
        CLT = 0x26,
        CLE = 0x27,
        CGT = 0x28,
        CGE = 0x29,
        CONV = 0x2a,
        THROW = 0x2b
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