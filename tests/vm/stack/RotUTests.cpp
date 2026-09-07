#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(ROT_U, I32_3) {
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        R"(
            push i32 #1
            push i32 #2
            push i32 #3
            rotU #3
            halt
        )",
        ISA::Type::I32,
        std::vector{int32_t(2), int32_t(1), int32_t(3)}
    );
}
/*
top
3
2
1

res
2
1
3
*/

TEST(ROT_U, UI32_3) {
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        R"(
            push ui32 #1
            push ui32 #2
            push ui32 #3
            rotU #3
            halt
        )",
        ISA::Type::UI32,
        std::vector{uint32_t(2), uint32_t(1), uint32_t(3)}
    );
}

TEST(ROT_U, I64_3) {
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        R"(
            push i64 #1
            push i64 #2
            push i64 #3
            rotU #3
            halt
        )",
        ISA::Type::I64,
        std::vector{int64_t(2), int64_t(1), int64_t(3)}
    );
}

TEST(ROT_U, UI64_3) {
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        R"(
            push ui64 #1
            push ui64 #2
            push ui64 #3
            rotU #3
            halt
        )",
        ISA::Type::UI64,
        std::vector{uint64_t(2), uint64_t(1), uint64_t(3)}
    );
}



TEST(ROT_U, F32_3) {
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        R"(
            push f32 #1.0
            push f32 #2.0
            push f32 #3.0
            rotU #3
            halt
        )",
        ISA::Type::F32,
        std::vector{2.0f, 1.0f, 3.0f}
    );
}

TEST(ROT_U, F64_3) {
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        R"(
            push f64 #1.0
            push f64 #2.0
            push f64 #3.0
            rotU #3
            halt
        )",
        ISA::Type::F64,
        std::vector{2.0f, 1.0f, 3.0f}
    );
}

TEST(ROT_U, PTR_3) {
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        R"(
            push ptr $x
            push ptr $y
            push ptr $z
            rotU #3
            halt

            .data
            $x: i32 5
            $y: i32 5
            $z: i32 5
        )",
        ISA::Type::PTR,
        std::vector{uint32_t(28), uint32_t(23), uint32_t(33)}
    );
}


TEST(ROT_U, I32_4) {
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        R"(
            push i32 #1
            push i32 #2
            push i32 #3
            push i32 #4
            rotU #4
            halt
        )",
        ISA::Type::I32,
        std::vector{int32_t(3), int32_t(2), int32_t(1), int32_t(4)}
    );
}

TEST(ROT_U, INVALID_UNDERFLOW_BY_ONE) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push i32 #5
            push i32 #2
            rotU #3
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(ROT_U, INVALID_UNDERFLOW_BY_TWO) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            push i32 #5
            rotU #3
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(ROT_U, INVALID_UNDERFLOW_BY_THREE) {
    EXPECT_INTERNAL_RUNTIME_ERROR(
        R"(
            rotU #3
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}
