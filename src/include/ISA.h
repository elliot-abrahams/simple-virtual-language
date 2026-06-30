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
        LOAD = 0x06,
        LOADB = 0x07,
        LOADG = 0x08,
        LOADL = 0x09,
        STORE = 0x0a,
        STOREB = 0x0b,
        STOREG = 0x0c,
        STOREL = 0x0d,
        ALLOC = 0x0e,
        FREE = 0x0f,
        CALL = 0x10,
        RET = 0x11,
        JMP = 0x12,
        JEZ = 0x13,
        JNZ = 0x14,
        ADD = 0x15,
        SUB = 0x16,
        MUL = 0x17,
        DIV = 0x18,
        MOD = 0x19,
        NOT = 0x1a,
        AND = 0x1b,
        ORR = 0x1c,
        XOR = 0x1d,
        SHL = 0x1e,
        SHR = 0x1f,
        SAR = 0x20,
        CEQ = 0x21,
        CNE = 0x22,
        CLT = 0x23,
        CLE = 0x24,
        CGT = 0x25,
        CGE = 0x26,
        OUT = 0x27,
        INN = 0x28,
        CONV = 0x29,
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