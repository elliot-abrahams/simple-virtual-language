#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(THROW, ARRAY_INDEX_OP2_I32) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push ui32 #10
            push i32 #20
            throw array_index
            halt
        )",
        RuntimeErrorType::EXPLICIT_ARRAY_INDEX_OUT_OF_RANGE
    );
}

TEST(THROW, ARRAY_INDEX_OP2_UI32) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push ui32 #10
            push ui32 #20
            throw array_index
            halt
        )",
        RuntimeErrorType::EXPLICIT_ARRAY_INDEX_OUT_OF_RANGE
    );
}

TEST(THROW, ARRAY_INDEX_OP2_I64) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push ui32 #10
            push i64 #20
            throw array_index
            halt
        )",
        RuntimeErrorType::EXPLICIT_ARRAY_INDEX_OUT_OF_RANGE
    );
}

TEST(THROW, ARRAY_INDEX_OP2_UI64) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push ui32 #10
            push ui64 #20
            throw array_index
            halt
        )",
        RuntimeErrorType::EXPLICIT_ARRAY_INDEX_OUT_OF_RANGE
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_OP1_I32) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push i32 #10
            push i32 #20
            throw array_index
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_OP1_I64) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push i64 #10
            push i32 #20
            throw array_index
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_OP1_UI64) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push ui64 #10
            push i32 #20
            throw array_index
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_OP1_F32) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push f32 #10.0
            push i32 #20
            throw array_index
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_OP1_F64) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push f64 #10.0
            push i32 #20
            throw array_index
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_OP1_PTR) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push ptr $x
            push i32 #20
            throw array_index
            halt

            .data
            $x: i32 5
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_OP2_F32) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push ui32 #10
            push f32 #20.0
            throw array_index
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_OP2_F64) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push ui32 #10
            push f64 #20.0
            throw array_index
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_OP2_PTR) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push ui32 #10
            push ptr $x
            throw array_index
            halt

            .data
            $x: i32 5
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_UNDERFLOW_BY_ONE) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push ui32 #10
            throw array_index
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(THROW, INVALID_ARRAY_INDEX_UNDERFLOW_BY_TWO) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            throw array_index
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}