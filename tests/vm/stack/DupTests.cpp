#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(DUP, I32) {
    const auto assembly = R"(
        push i32 #5
        dup #0
        halt
    )";
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        assembly,
        ISA::Type::I32,
        std::vector{int32_t(5), int32_t(5)}
    );
}

TEST(DUP, UI32) {
    const auto assembly = R"(
        push ui32 #5
        dup #0
        halt
    )";
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        assembly,
        ISA::Type::UI32,
        std::vector{uint32_t(5), uint32_t(5)}
    );
}

TEST(DUP, I64) {
    const auto assembly = R"(
        push i64 #5
        dup #0
        halt
    )";
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        assembly,
        ISA::Type::I64,
        std::vector{int64_t(5), int64_t(5)}
    );
}

TEST(DUP, UI64) {
    const auto assembly = R"(
        push ui64 #5
        dup #0
        halt
    )";
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        assembly,
        ISA::Type::UI64,
        std::vector{uint64_t(5), uint64_t(5)}
    );
}

TEST(DUP, F32) {
    const auto assembly = R"(
        push f32 #5.0
        dup #0
        halt
    )";
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        assembly,
        ISA::Type::F32,
        std::vector{5.0f, 5.0f}
    );
}

TEST(DUP, F64) {
    const auto assembly = R"(
        push f64 #5.0
        dup #0
        halt
    )";
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        assembly,
        ISA::Type::F64,
        std::vector{5.0, 5.0}
    );
}

TEST(DUP, PTR) {
    const auto assembly = R"(
        push ptr $x
        dup #0
        halt

    .data
    $x: i32 5
    )";
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        assembly,
        ISA::Type::PTR,
        std::vector{uint32_t(11), uint32_t(11)}
    );
}

TEST(DUP, I32_1) {
    const auto assembly = R"(
        push i32 #10
        push i32 #5
        dup #1
        halt
    )";
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        assembly,
        ISA::Type::I32,
        std::vector{int32_t(10), int32_t(5), int32_t(10)}
    );
}

TEST(DUP, I32_2) {
    const auto assembly = R"(
        push i32 #15
        push i32 #10
        push i32 #5
        dup #2
        halt
    )";
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        assembly,
        ISA::Type::I32,
        std::vector{int32_t(15), int32_t(5), int32_t(10), int32_t(15)}
    );
}

TEST(DUP, INVALID_UNDERFLOW_BY_ONE) {
    const auto assembly = R"(
        dup #0
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly, RuntimeErrorType::INTERNAL);
}

TEST(DUP, INVALID_UNDERFLOW_BY_TWO) {
    const auto assembly = R"(
        push i32 #5
        dup #1
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly, RuntimeErrorType::INTERNAL);
}
