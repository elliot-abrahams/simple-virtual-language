#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(ROT, I32) {
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        R"(
            push i32 #1
            push i32 #2
            push i32 #3
            rot
            halt
        )",
        ISA::Type::I32,
        std::vector{int32_t(1), int32_t(3), int32_t(2)}
    );
}

TEST(ROT, UI32) {
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        R"(
            push ui32 #1
            push ui32 #2
            push ui32 #3
            rot
            halt
        )",
        ISA::Type::UI32,
        std::vector{uint32_t(1), uint32_t(3), uint32_t(2)}
    );
}

TEST(ROT, I64) {
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        R"(
            push i64 #1
            push i64 #2
            push i64 #3
            rot
            halt
        )",
        ISA::Type::I64,
        std::vector{int64_t(1), int64_t(3), int64_t(2)}
    );
}

TEST(ROT, UI64) {
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        R"(
            push ui64 #1
            push ui64 #2
            push ui64 #3
            rot
            halt
        )",
        ISA::Type::UI64,
        std::vector{uint64_t(1), uint64_t(3), uint64_t(2)}
    );
}

TEST(ROT, F32) {
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        R"(
            push f32 #1.0
            push f32 #2.0
            push f32 #3.0
            rot
            halt
        )",
        ISA::Type::F32,
        std::vector{1.0f, 3.0f, 2.0f}
    );
}

TEST(ROT, F64) {
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        R"(
            push f64 #1.0
            push f64 #2.0
            push f64 #3.0
            rot
            halt
        )",
        ISA::Type::F64,
        std::vector{1.0f, 3.0f, 2.0f}
    );
}

TEST(ROT, PTR) {
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        R"(
            push ptr $x
            push ptr $y
            push ptr $z
            rot
            halt

            .data
            $x: i32 5
            $y: i32 5
            $z: i32 5
        )",
        ISA::Type::PTR,
        std::vector{uint32_t(21), uint32_t(31), uint32_t(26)}
    );
}

TEST(ROT, INVALID_UNDERFLOW_BY_ONE) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push i32 #5
            push i32 #2
            rot
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(ROT, INVALID_UNDERFLOW_BY_TWO) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push i32 #5
            rot
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(ROT, INVALID_UNDERFLOW_BY_THREE) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            rot
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}
