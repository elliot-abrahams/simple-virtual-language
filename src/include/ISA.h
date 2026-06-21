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
        LOADG = 0x07,
        LOADL = 0x08,
        STORE = 0x09,
        STOREG = 0x0a,
        STOREL = 0x0b,
        ALLOC = 0x0c,
        FREE = 0x0d,
        CALL = 0x0e,
        RET = 0x0f,
        JMP = 0x10,
        JEZ = 0x11,
        JNZ = 0x12,
        ADD = 0x13,
        SUB = 0x14,
        MUL = 0x15,
        DIV = 0x16,
        MOD = 0x17,
        NOT = 0x18,
        AND = 0x19,
        ORR = 0x1a,
        XOR = 0x1b,
        SHL = 0x1c,
        SHR = 0x1d,
        SAR = 0x1e,
        CEQ = 0x1f,
        CNE = 0x20,
        CLT = 0x21,
        CLE = 0x22,
        CGT = 0x23,
        CGE = 0x24,
        OUT = 0x25,
        INN = 0x26,
        CONV = 0x27,
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