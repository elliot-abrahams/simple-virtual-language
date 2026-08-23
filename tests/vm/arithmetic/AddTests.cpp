#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(ADD, I32_I32) {
    const auto assembly = R"(
        push i32 #5
        push i32 #5
        add
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(10));
}

TEST(ADD, I32_PTR) {
    const auto assembly = R"(
        push i32 #5
        push ptr $x
        add
        halt

    .data
    $x: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::PTR, uint32_t(20));
}

TEST(ADD, UI32_UI32) {
    const auto assembly = R"(
        push ui32 #5
        push ui32 #5
        add
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(10));
}

TEST(ADD, UI32_PTR) {
    const auto assembly = R"(
        push ui32 #5
        push ptr $x
        add
        halt

    .data
    $x: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::PTR, uint32_t(20));
}

TEST(ADD, I64_I64) {
    const auto assembly = R"(
        push i64 #5
        push i64 #5
        add
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(10));
}

TEST(ADD, UI64_UI64) {
    const auto assembly = R"(
        push ui64 #5
        push ui64 #5
        add
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(10));
}

TEST(ADD, F32_F32) {
    const auto assembly = R"(
        push f32 #5.75
        push f32 #5.75
        add
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 11.5f);
}

TEST(ADD, F64_F64) {
    const auto assembly = R"(
        push f64 #5.75
        push f64 #5.75
        add
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F64, 11.5);
}

TEST(ADD, PTR_I32) {
    const auto assembly = R"(
        push ptr $x
        push i32 #5
        add
        halt

    .data
    $x: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::PTR, uint32_t(20));
}

TEST(ADD, PTR_UI32) {
    const auto assembly = R"(
        push ptr $x
        push ui32 #5
        add
        halt

    .data
    $x: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::PTR, uint32_t(20));
}

TEST(ADD, I32_0_0) {
    const auto assembly = R"(
        push i32 #0
        push i32 #0
        add
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(0));
}

TEST(ADD, I32_NEGATIVE) {
    const auto assembly = R"(
        push i32 #-1
        push i32 #-2
        add
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(-3));
}

TEST(ADD, I32_WRAP) {
    const auto assembly = R"(
        push i32 #2147483647
        push i32 #1
        add
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(-2147483648));
}

TEST(ADD, UI32_WRAP) {
    const auto assembly = R"(
        push ui32 #4294967295
        push ui32 #1
        add
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(0));
}

TEST(ADD, I64_WRAP) {
    const auto assembly = R"(
        push i64 #9223372036854775807
        push i64 #1
        add
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(-9223372036854775808));
}

TEST(ADD, UI64_WRAP) {
    const auto assembly = R"(
        push ui64 #18446744073709551615
        push ui64 #1
        add
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(0));
}

TEST(ADD, F32_PRECISION_LOSS) {
    const auto assembly = R"(
        push f32 #16777216.0
        push f32 #1.0
        add
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 16777216.0f);
}

TEST(ADD, INVALID_I32_F32) {
    const auto assembly = R"(
        push i32 #5
        push f32 #2.0
        add
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(ADD, INVALID_PTR_PTR) {
    const auto assembly = R"(
        push ptr $x
        push ptr $x
        add
        halt

    .data
    $x: i32 5
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(ADD, INVALID_PTR_I64) {
    const auto assembly = R"(
        push ptr $x
        push i64 #5
        add
        halt

    .data
    $x: i32 5
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(ADD, INVALID_UI64_F64) {
    const auto assembly = R"(
        push ui64 #10
        push f64 #2.0
        add
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(ADD, INVALID_UNDERFLOW_BY_ONE) {
    const auto assembly = R"(
        push i32 #5
        add
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(ADD, INVALID_UNDERFLOW_BY_TWO) {
    const auto assembly = R"(
        add
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}
