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
        NATIVE = 0x10,
        CALL = 0x11,
        RET = 0x12,
        JMP = 0x13,
        JEZ = 0x14,
        JNZ = 0x15,
        ADD = 0x16,
        SUB = 0x17,
        MUL = 0x18,
        DIV = 0x19,
        MOD = 0x1a,
        NOT = 0x1b,
        AND = 0x1c,
        ORR = 0x1d,
        XOR = 0x1e,
        SHL = 0x1f,
        SHR = 0x20,
        SAR = 0x21,
        CEQ = 0x22,
        CNE = 0x23,
        CLT = 0x24,
        CLE = 0x25,
        CGT = 0x26,
        CGE = 0x27,
        CONV = 0x28,
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