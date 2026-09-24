#include <gtest/gtest.h>
#include "../AssemblerTestsUtils.h"

using namespace assemblerTests;

TEST(OTHER, CONV_THROW_VALID) {
    ASSERT_BYTECODE_EQ(
    R"(
            conv i32
            conv ui32
            conv i64
            conv ui64
            conv f32
            conv f64
            conv ptr

            throw array_index_out_of_range
            throw negative_array_length
            throw array_initialiser_length
        )",
        std::vector<uint8_t>{
            0x27, // header -> data
            0x00,
            0x00,
            0x00,
            0x27, // header -> sources
            0x00,
            0x00,
            0x00,
            0x27, // header -> functions
            0x00,
            0x00,
            0x00,
            0x27, // header -> line_table
            0x00,
            0x00,
            0x00,
            0x27, // header -> end
            0x00,
            0x00,
            0x00,
            0x2b, // conv
            0x00, // i32
            0x2b, // conv
            0x01, // ui32
            0x2b, // conv
            0x02, // i64
            0x2b, // conv
            0x03, // ui64
            0x2b, // conv
            0x04, // f32
            0x2b, // conv
            0x05, // f64
            0x2b, // conv
            0x06, // ptr
            0x2c, // throw
            0x00, // array_index_out_of_range
            0x2c, // throw
            0x01, // negative_array_length
            0x2c, // throw
            0x02  // array_initialiser_length
        }
    );
}

TEST(OTHER, INVALID_CONV_TWO_OPERANDS) {
    ASSERT_THROWS_ASSEMBLER_ERROR("conv i32 #5");
}

TEST(OTHER, INVALID_CONV_IMMEDIATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("conv #5");
}

TEST(OTHER, INVALID_CONV_LABEL_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label: conv $label");
}

TEST(OTHER, INVALID_CONV_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("conv exit");
}

TEST(OTHER, INVALID_CONV_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("conv array_index_out_of_range");
}

TEST(OTHER, INVALID_THROW_TWO_OPERANDS) {
    ASSERT_THROWS_ASSEMBLER_ERROR("throw array_index_out_of_range #5");
}

TEST(OTHER, INVALID_THROW_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("throw i32");
}

TEST(OTHER, INVALID_THROW_IMMEDIATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("throw #5");
}

TEST(OTHER, INVALID_THROW_LABEL_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label: throw $label");
}

TEST(OTHER, INVALID_THROW_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("throw exit");
}
