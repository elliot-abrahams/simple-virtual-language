#include <gtest/gtest.h>
#include "../AssemblerTestsUtils.h"

using namespace assemblerTests;

TEST(STACK, ENCODING) {
    ASSERT_BYTECODE_EQ(
R"(
            push i32 #5
            push ui32 #5
            push i64 #5
            push ui64 #5
            push f32 #5.5
            push f64 #5.5
            push ptr $x

            pop
            dup #5
            swap
            rotD #5
            rotU #5

        .data
        $x: i32 5
        )",
        std::vector<uint8_t>{
            0x54, // header -> data
            0x00,
            0x00,
            0x00,
            0x59, // header -> sources
            0x00,
            0x00,
            0x00,
            0x59, // header -> functions
            0x00,
            0x00,
            0x00,
            0x59, // header -> line_table
            0x00,
            0x00,
            0x00,
            0x59, // header -> end
            0x00,
            0x00,
            0x00,
            0x02, // push
            0x00, // i32
            0x05, // #5
            0x00,
            0x00,
            0x00,
            0x02, // push
            0x01, // ui32
            0x05, // #5
            0x00,
            0x00,
            0x00,
            0x02, // push
            0x02, // i64
            0x05, // #5
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x02, // push
            0x03, // ui64
            0x05, // #5
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x02, // push
            0x04, // f32
            0x00, // #5.5
            0x00,
            0xB0,
            0x40,
            0x02, // push
            0x05, // f64
            0x00, // #5.5
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x16,
            0x40,
            0x02, // push
            0x06, // ptr
            0x42, // $x
            0x00,
            0x00,
            0x00,
            0x03, // pop
            0x04, // dup
            0x05, // #5
            0x00,
            0x05, // swap
            0x06, //rotD
            0x05, // #5
            0x00,
            0x07, // rotU
            0x05, // #5
            0x00,
            0x00, // i32
            0x05, // #5
            0x00,
            0x00,
            0x00
        }
    );
}

TEST(STACK, VALID_DUP_IMMEDIATE_MIN) {
    ASSERT_NO_ASSEMBLER_ERROR("dup #0");
}

TEST(STACK, VALID_DUP_IMMEDIATE_MAX) {
    ASSERT_NO_ASSEMBLER_ERROR("dup #1023");
}

TEST(STACK, VALID_ROTD_IMMEDIATE_MIN) {
    ASSERT_NO_ASSEMBLER_ERROR("rotD #3");
}

TEST(STACK, VALID_ROTD_IMMEDIATE_MAX) {
    ASSERT_NO_ASSEMBLER_ERROR("rotD #1024");
}

TEST(STACK, VALID_ROTU_IMMEDIATE_MIN) {
    ASSERT_NO_ASSEMBLER_ERROR("rotU #3");
}

TEST(STACK, VALID_ROTU_IMMEDIATE_MAX) {
    ASSERT_NO_ASSEMBLER_ERROR("rotU #1024");
}

TEST(STACK, INVALID_PUSH_THREE_OPERANDS) {
    ASSERT_THROWS_ASSEMBLER_ERROR("push i32 #5 #5");
}

TEST(STACK, INVALID_PUSH_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("push i32 i32");
}

TEST(STACK, INVALID_PUSH_PTR_IMMEDIATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("push ptr #5");
}

TEST(STACK, INVALID_PUSH_NON_PTR_LABEL_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("push i32 $x .data $x: i32 5");
}

TEST(STACK, INVALID_PUSH_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("push i32 exit");
}

TEST(STACK, INVALID_PUSH_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("push i32 array_index_out_of_range");
}

TEST(STACK, INVALID_POP_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("pop i32");
}

TEST(STACK, INVALID_POP_IMMEDIATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("pop #5");
}

TEST(STACK, INVALID_POP_LABEL_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label: pop $label");
}

TEST(STACK, INVALID_POP_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("pop exit");
}

TEST(STACK, INVALID_POP_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("pop array_index_out_of_range");
}

TEST(STACK, INVALID_DUP_TWO_OPERANDS) {
    ASSERT_THROWS_ASSEMBLER_ERROR("dup #5 #5");
}

TEST(STACK, INVALID_DUP_IMMEDIATE_MIN) {
    ASSERT_THROWS_ASSEMBLER_ERROR("dup #-1");
}

TEST(STACK, INVALID_DUP_IMMEDIATE_MAX) {
    ASSERT_THROWS_ASSEMBLER_ERROR("dup #1024");
}

TEST(STACK, INVALID_DUP_IMMEDIATE_DECIMAL) {
    ASSERT_THROWS_ASSEMBLER_ERROR("dup #1.5");
}

TEST(STACK, INVALID_DUP_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("dup i32");
}

TEST(STACK, INVALID_DUP_LABEL_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label: dup $label");
}

TEST(STACK, INVALID_DUP_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("dup exit");
}

TEST(STACK, INVALID_DUP_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("dup array_index_out_of_range");
}

TEST(STACK, INVALID_SWAP_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("swap i32");
}

TEST(STACK, INVALID_SWAP_IMMEDIATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("swap #5");
}

TEST(STACK, INVALID_SWAP_LABEL_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label: swap $label");
}

TEST(STACK, INVALID_SWAP_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("swap exit");
}

TEST(STACK, INVALID_SWAP_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("swap array_index_out_of_range");
}

TEST(STACK, INVALID_ROTD_TWO_OPERANDS) {
    ASSERT_THROWS_ASSEMBLER_ERROR("rotD #5 #5");
}

TEST(STACK, INVALID_ROTD_IMMEDIATE_MIN) {
    ASSERT_THROWS_ASSEMBLER_ERROR("rotD #2");
}

TEST(STACK, INVALID_ROTD_IMMEDIATE_MAX) {
    ASSERT_THROWS_ASSEMBLER_ERROR("rotD #1025");
}

TEST(STACK, INVALID_ROTD_IMMEDIATE_DECIMAL) {
    ASSERT_THROWS_ASSEMBLER_ERROR("rotD #1.5");
}

TEST(STACK, INVALID_ROTD_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("rotD i32");
}

TEST(STACK, INVALID_ROTD_LABEL_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label: rotD $label");
}

TEST(STACK, INVALID_ROTD_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("rotD exit");
}

TEST(STACK, INVALID_ROTD_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("rotD array_index_out_of_range");
}

TEST(STACK, INVALID_ROTU_TWO_OPERANDS) {
    ASSERT_THROWS_ASSEMBLER_ERROR("rotU #5 #5");
}

TEST(STACK, INVALID_ROTU_IMMEDIATE_MIN) {
    ASSERT_THROWS_ASSEMBLER_ERROR("rotU #2");
}

TEST(STACK, INVALID_ROTU_IMMEDIATE_MAX) {
    ASSERT_THROWS_ASSEMBLER_ERROR("rotU #1025");
}

TEST(STACK, INVALID_ROTU_IMMEDIATE_DECIMAL) {
    ASSERT_THROWS_ASSEMBLER_ERROR("rotU #1.5");
}

TEST(STACK, INVALID_ROTU_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("rotU i32");
}

TEST(STACK, INVALID_ROTU_LABEL_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label: rotU $label");
}

TEST(STACK, INVALID_ROTU_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("rotU exit");
}

TEST(STACK, INVALID_ROTU_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("rotU array_index_out_of_range");
}