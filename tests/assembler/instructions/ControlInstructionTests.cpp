#include <gtest/gtest.h>
#include "../AssemblerTestsUtils.h"

using namespace assemblerTests;

TEST(CONTROL, ENCODING) {
    ASSERT_BYTECODE_EQ(
R"(
            native exit
            native print
            native print_str

            call $func
            ret

        $label:
            jmp $label
            jez $label
            jnz $label

            def $func:
                args 0
                locals 0
        )",
        std::vector<uint8_t>{
            0x33, // header -> data
            0x00,
            0x00,
            0x00,
            0x33, // header -> sources
            0x00,
            0x00,
            0x00,
            0x33, // header -> functions
            0x00,
            0x00,
            0x00,
            0x33, // header -> line_table
            0x00,
            0x00,
            0x00,
            0x33, // header -> end
            0x00,
            0x00,
            0x00,
            0x13, // native
            0x00, // exit
            0x13, // native
            0x01, // print
            0x13, // native
            0x03, // print_str
            0x14, // call
            0x1b, // $func
            0x00,
            0x00,
            0x00,
            0x15, // ret
            0x16, // jmp
            0x0c, // $label
            0x00,
            0x00,
            0x00,
            0x17, // jez
            0x0c, // $label
            0x00,
            0x00,
            0x00,
            0x18, // jnz
            0x0c, // $label
            0x00,
            0x00,
            0x00,
            0x00, // args 0
            0x00, // locals 0
            0x00,
            0x00,
            0x00
        }
    );
}

TEST(CONTROL, INVALID_NATIVE_TWO_OPERANDS) {
    ASSERT_THROWS_ASSEMBLER_ERROR("native exit #5");
}

TEST(CONTROL, INVALID_NATIVE_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("native i32");
}

TEST(CONTROL, INVALID_NATIVE_IMMEDIATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("native #5");
}

TEST(CONTROL, INVALID_NATIVE_LABEL_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label: native $label");
}

TEST(CONTROL, INVALID_NATIVE_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("native array_index_out_of_range");
}

TEST(CONTROL, INVALID_CALL_TWO_OPERANDS) {
    ASSERT_THROWS_ASSEMBLER_ERROR("def $foo: args 0 locals 0 call $foo #5");
}

TEST(CONTROL, INVALID_CALL_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("call i32");
}

TEST(CONTROL, INVALID_CALL_IMMEDIATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("call #5");
}

TEST(CONTROL, INVALID_CALL_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("call exit");
}

TEST(CONTROL, INVALID_CALL_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("call array_index_out_of_range");
}

TEST(CONTROL, INVALID_RET_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("ret i32");
}

TEST(CONTROL, INVALID_RET_IMMEDIATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("ret #5");
}

TEST(CONTROL, INVALID_RET_LABEL_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label: ret $label");
}

TEST(CONTROL, INVALID_RET_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("ret exit");
}

TEST(CONTROL, INVALID_RET_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("ret array_index_out_of_range");
}

TEST(CONTROL, INVALID_JMP_TWO_OPERANDS) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label: jmp $label #5");
}

TEST(CONTROL, INVALID_JMP_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("jmp i32");
}

TEST(CONTROL, INVALID_JMP_IMMEDIATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("jmp #5");
}

TEST(CONTROL, INVALID_JMP_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("jmp exit");
}

TEST(CONTROL, INVALID_JMP_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("jmp array_index_out_of_range");
}

TEST(CONTROL, INVALID_JEZ_TWO_OPERANDS) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label: jez $label #5");
}

TEST(CONTROL, INVALID_JEZ_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("jez i32");
}

TEST(CONTROL, INVALID_JEZ_IMMEDIATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("jez #5");
}

TEST(CONTROL, INVALID_JEZ_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("jez exit");
}

TEST(CONTROL, INVALID_JEZ_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("jez array_index_out_of_range");
}

TEST(CONTROL, INVALID_JNZ_TWO_OPERANDS) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label: jnz $label #5");
}

TEST(CONTROL, INVALID_JNZ_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("jnz i32");
}

TEST(CONTROL, INVALID_JNZ_IMMEDIATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("jnz #5");
}

TEST(CONTROL, INVALID_JNZ_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("jnz exit");
}
TEST(CONTROL, INVALID_JNZ_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("jnz array_index_out_of_range");
}

