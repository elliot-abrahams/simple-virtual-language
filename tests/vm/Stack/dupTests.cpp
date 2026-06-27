#include <gtest/gtest.h>
#include "../vmTestUtils.h"

TEST(DUP, I32) {
    const auto assembly = R"(
        push i32 #5
        dup
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
        dup
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
        dup
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
        dup
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
        dup
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
        dup
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
        dup
        halt

    .data
    $x: i32 5
    )";
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        assembly,
        ISA::Type::PTR,
        std::vector{uint32_t(9), uint32_t(9)}
    );
}

TEST(DUP, INVALID_UNDERFLOW) {
    const auto assembly = R"(
        dup
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}
