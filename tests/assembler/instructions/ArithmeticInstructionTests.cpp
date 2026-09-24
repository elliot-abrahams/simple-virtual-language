#include <gtest/gtest.h>
#include "../AssemblerTestsUtils.h"

using namespace assemblerTests;

TEST(ARITHMETIC, ENCODING) {
    ASSERT_BYTECODE_EQ(
R"(
            add
            sub
            mul
            div
            mod
            not
            and
            orr
            xor
            shl
            shr
            sar
            ceq
            cne
            clt
            cle
            cgt
            cge
        )",
        std::vector<uint8_t>{
            0x25, // header -> data
            0x00,
            0x00,
            0x00,
            0x25, // header -> sources
            0x00,
            0x00,
            0x00,
            0x25, // header -> functions
            0x00,
            0x00,
            0x00,
            0x25, // header -> line_table
            0x00,
            0x00,
            0x00,
            0x25, // header -> end
            0x00,
            0x00,
            0x00,
            0x19, // add
            0x1a, // sub
            0x1b, // mul
            0x1c, // div
            0x1d, // mod
            0x1e, // not
            0x1f, // and
            0x20, // orr
            0x21, // xor
            0x22, // shl
            0x23, // shr
            0x24, // sar
            0x25, // ceq
            0x26, // cne
            0x27, // clt
            0x28, // cle
            0x29, // cgt
            0x2a  // cge
        }
    );
}

TEST(ARITHMETIC, INVALID_ADD_ONE_OPERAND) {
    ASSERT_THROWS_ASSEMBLER_ERROR("add #5");
}

TEST(ARITHMETIC, INVALID_SUB_ONE_OPERAND) {
    ASSERT_THROWS_ASSEMBLER_ERROR("sub #5");
}

TEST(ARITHMETIC, INVALID_MUL_ONE_OPERAND) {
    ASSERT_THROWS_ASSEMBLER_ERROR("mul #5");
}

TEST(ARITHMETIC, INVALID_DIV_ONE_OPERAND) {
    ASSERT_THROWS_ASSEMBLER_ERROR("div #5");
}

TEST(ARITHMETIC, INVALID_MOD_ONE_OPERAND) {
    ASSERT_THROWS_ASSEMBLER_ERROR("mod #5");
}

TEST(ARITHMETIC, INVALID_NOT_ONE_OPERAND) {
    ASSERT_THROWS_ASSEMBLER_ERROR("not #5");
}

TEST(ARITHMETIC, INVALID_AND_ONE_OPERAND) {
    ASSERT_THROWS_ASSEMBLER_ERROR("and #5");
}

TEST(ARITHMETIC, INVALID_ORR_ONE_OPERAND) {
    ASSERT_THROWS_ASSEMBLER_ERROR("orr #5");
}

TEST(ARITHMETIC, INVALID_XOR_ONE_OPERAND) {
    ASSERT_THROWS_ASSEMBLER_ERROR("xor #5");
}

TEST(ARITHMETIC, INVALID_SHL_ONE_OPERAND) {
    ASSERT_THROWS_ASSEMBLER_ERROR("shl #5");
}

TEST(ARITHMETIC, INVALID_SHR_ONE_OPERAND) {
    ASSERT_THROWS_ASSEMBLER_ERROR("shr #5");
}

TEST(ARITHMETIC, INVALID_SAR_ONE_OPERAND) {
    ASSERT_THROWS_ASSEMBLER_ERROR("sar #5");
}

TEST(ARITHMETIC, INVALID_CEQ_ONE_OPERAND) {
    ASSERT_THROWS_ASSEMBLER_ERROR("ceq #5");
}

TEST(ARITHMETIC, INVALID_CNE_ONE_OPERAND) {
    ASSERT_THROWS_ASSEMBLER_ERROR("cne #5");
}

TEST(ARITHMETIC, INVALID_CLT_ONE_OPERAND) {
    ASSERT_THROWS_ASSEMBLER_ERROR("clt #5");
}

TEST(ARITHMETIC, INVALID_CLE_ONE_OPERAND) {
    ASSERT_THROWS_ASSEMBLER_ERROR("cle #5");
}

TEST(ARITHMETIC, INVALID_CGT_ONE_OPERAND) {
    ASSERT_THROWS_ASSEMBLER_ERROR("cgt #5");
}

TEST(ARITHMETIC, INVALID_CGE_ONE_OPERAND) {
    ASSERT_THROWS_ASSEMBLER_ERROR("cge #5");
}


