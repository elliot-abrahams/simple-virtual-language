#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(THROW, ARRAY_INDEX_OUT_OF_RANGE_OP2_I32) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push ui32 #10
            push i32 #20
            throw array_index_out_of_range
            halt
        )",
        RuntimeErrorType::EXPLICIT_ARRAY_INDEX_OUT_OF_RANGE
    );
}

TEST(THROW, ARRAY_INDEX_OUT_OF_RANGE_OP2_UI32) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push ui32 #10
            push ui32 #20
            throw array_index_out_of_range
            halt
        )",
        RuntimeErrorType::EXPLICIT_ARRAY_INDEX_OUT_OF_RANGE
    );
}

TEST(THROW, ARRAY_INDEX_OUT_OF_RANGE_OP2_I64) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push ui32 #10
            push i64 #20
            throw array_index_out_of_range
            halt
        )",
        RuntimeErrorType::EXPLICIT_ARRAY_INDEX_OUT_OF_RANGE
    );
}

TEST(THROW, ARRAY_INDEX_OUT_OF_RANGE_OP2_UI64) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push ui32 #10
            push ui64 #20
            throw array_index_out_of_range
            halt
        )",
        RuntimeErrorType::EXPLICIT_ARRAY_INDEX_OUT_OF_RANGE
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_OUT_OF_RANGE_OP1_I32) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push i32 #10
            push i32 #20
            throw array_index_out_of_range
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_OUT_OF_RANGE_OP1_I64) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push i64 #10
            push i32 #20
            throw array_index_out_of_range
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_OUT_OF_RANGE_OP1_UI64) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push ui64 #10
            push i32 #20
            throw array_index_out_of_range
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_OUT_OF_RANGE_OP1_F32) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push f32 #10.0
            push i32 #20
            throw array_index_out_of_range
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_OUT_OF_RANGE_OP1_F64) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push f64 #10.0
            push i32 #20
            throw array_index_out_of_range
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_OUT_OF_RANGE_OP1_PTR) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push ptr $x
            push i32 #20
            throw array_index_out_of_range
            halt

            .data
            $x: i32 5
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_OUT_OF_RANGE_OP2_F32) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push ui32 #10
            push f32 #20.0
            throw array_index_out_of_range
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_OUT_OF_RANGE_OP2_F64) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push ui32 #10
            push f64 #20.0
            throw array_index_out_of_range
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_OUT_OF_RANGE_OP2_PTR) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push ui32 #10
            push ptr $x
            throw array_index_out_of_range
            halt

            .data
            $x: i32 5
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_OUT_OF_RANGE_UNDERFLOW_BY_ONE) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push ui32 #10
            throw array_index_out_of_range
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_OUT_OF_RANGE_UNDERFLOW_BY_TWO) {
    EXPECT_RUNTIME_ERROR(
        R"(
            throw array_index_out_of_range
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, NEGATIVE_ARRAY_LENGTH_I32) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push i32 #-1
            throw negative_array_length
            halt
        )",
        RuntimeErrorType::EXPLICIT_NEGATIVE_ARRAY_LENGTH
    );
}

TEST(THROW, NEGATIVE_ARRAY_LENGTH_I64) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push i64 #-1
            throw negative_array_length
            halt
        )",
        RuntimeErrorType::EXPLICIT_NEGATIVE_ARRAY_LENGTH
    );
}

TEST(THROW, INVALID_NEGATIVE_ARRAY_LENGTH_F32) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push f32 #-1.0
            throw negative_array_length
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_NEGATIVE_ARRAY_LENGTH_F64) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push f64 #-1.0
            throw negative_array_length
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, NEGATIVE_ARRAY_LENGTH_UNDERFLOW) {
    EXPECT_RUNTIME_ERROR(
        R"(
            throw negative_array_length
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, ARRAY_INITIALISER_LENGTH_UI32) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push ui32 #5
            throw array_initialiser_length
            halt
        )",
        RuntimeErrorType::EXPLICIT_ARRAY_INITIALISER_LENGTH
    );
}

TEST(THROW, INVALID_ARRAY_INITIALISER_LENGTH_I32) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push i32 #5
            throw array_initialiser_length
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INITIALISER_LENGTH_I64) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push i64 #5
            throw array_initialiser_length
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INITIALISER_LENGTH_UI64) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push ui64 #5
            throw array_initialiser_length
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INITIALISER_LENGTH_F32) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push f32 #5.0
            throw array_initialiser_length
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INITIALISER_LENGTH_F64) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push f64 #5.0
            throw array_initialiser_length
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INITIALISER_LENGTH_PTR) {
    EXPECT_RUNTIME_ERROR(
        R"(
            push ptr $x
            throw array_initialiser_length
            halt

            .data
            $x: i32 5
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INITIALISER_UNDERFLOW) {
    EXPECT_RUNTIME_ERROR(
        R"(
            throw array_initialiser_length
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}