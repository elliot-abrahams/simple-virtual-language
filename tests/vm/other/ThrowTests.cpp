#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(THROW, ARRAY_INDEX_OUT_OF_RANGE_OP2_I32) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
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
    EXPECT_INTERNAL_RUNTIME_ERROR(
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
    EXPECT_INTERNAL_RUNTIME_ERROR(
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
    EXPECT_INTERNAL_RUNTIME_ERROR(
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
    EXPECT_INTERNAL_RUNTIME_ERROR(
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
    EXPECT_INTERNAL_RUNTIME_ERROR(
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
    EXPECT_INTERNAL_RUNTIME_ERROR(
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
    EXPECT_INTERNAL_RUNTIME_ERROR(
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
    EXPECT_INTERNAL_RUNTIME_ERROR(
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
    EXPECT_INTERNAL_RUNTIME_ERROR(
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
    EXPECT_INTERNAL_RUNTIME_ERROR(
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
    EXPECT_INTERNAL_RUNTIME_ERROR(
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
    EXPECT_INTERNAL_RUNTIME_ERROR(
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
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push ui32 #10
            throw array_index_out_of_range
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_OUT_OF_RANGE_UNDERFLOW_BY_TWO) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            throw array_index_out_of_range
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, NEGATIVE_ARRAY_SIZE_I32) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push i32 #-1
            throw negative_array_size
            halt
        )",
        RuntimeErrorType::EXPLICIT_NEGATIVE_ARRAY_SIZE
    );
}

TEST(THROW, NEGATIVE_ARRAY_SIZE_I64) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push i64 #-1
            throw negative_array_size
            halt
        )",
        RuntimeErrorType::EXPLICIT_NEGATIVE_ARRAY_SIZE
    );
}

TEST(THROW, INVALID_NEGATIVE_ARRAY_SIZE_F32) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push f32 #-1.0
            throw negative_array_size
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, NEGATIVE_ARRAY_SIZE_F64) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push f64 #-1.0
            throw negative_array_size
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, NEGATIVE_ARRAY_SIZE_UNDERFLOW) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            throw negative_array_size
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}