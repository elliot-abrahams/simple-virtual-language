#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(CGT, I32_I32_TRUE) {
    const auto assembly = R"(
        push i32 #1
        push i32 #0
        cgt
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(1));
}

TEST(CGT, I32_I32_FALSE) {
    const auto assembly = R"(
        push i32 #0
        push i32 #0
        cgt
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(0));
}

TEST(CGT, UI32_UI32_TRUE) {
    const auto assembly = R"(
        push ui32 #100
        push ui32 #10
        cgt
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(1));
}

TEST(CGT, UI32_UI32_FALSE) {
    const auto assembly = R"(
        push ui32 #10
        push ui32 #100
        cgt
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(0));
}

TEST(CGT, I64_I64_TRUE) {
    const auto assembly = R"(
        push i64 #-10
        push i64 #-100
        cgt
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(1));
}

TEST(CGT, I64_I64_FALSE) {
    const auto assembly = R"(
        push i64 #-100
        push i64 #-10
        cgt
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(0));
}

TEST(CGT, UI64_UI64_TRUE) {
    const auto assembly = R"(
        push ui64 #1
        push ui64 #0
        cgt
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(1));
}

TEST(CGT, UI64_UI64_FALSE) {
    const auto assembly = R"(
        push ui64 #0
        push ui64 #1
        cgt
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(0));
}

TEST(CGT, F32_F32_TRUE) {
    const auto assembly = R"(
        push f32 #0.5
        push f32 #-0.5
        cgt
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(1));
}

TEST(CGT, F32_F32_FALSE) {
    const auto assembly = R"(
        push f32 #-0.5
        push f32 #0.5
        cgt
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(0));
}

TEST(CGT, F64_F64_TRUE) {
    const auto assembly = R"(
        push f64 #100.5
        push f64 #0.0
        cgt
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(1));
}

TEST(CGT, F64_F64_FALSE) {
    const auto assembly = R"(
        push f64 #0.0
        push f64 #0.0
        cgt
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(0));
}

TEST(CGT, PTR_PTR_TRUE) {
    const auto assembly = R"(
        push ptr $y
        push ptr $x
        cgt
        halt

    .data
    $x: i32 5
    $y: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(1));
}

TEST(CGT, PTR_PTR_FALSE) {
    const auto assembly = R"(
        push ptr $x
        push ptr $y
        cgt
        halt

    .data
    $x: i32 5
    $y: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(0));
}

TEST(CGT, INVALID_I32_UI32) {
    const auto assembly = R"(
        push i32 #5
        push ui32 #5
        cgt
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CGT, INVALID_I64_UI64) {
    const auto assembly = R"(
        push i64 #5
        push ui64 #5
        cgt
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CGT, INVALID_F32_F64) {
    const auto assembly = R"(
        push f32 #5.0
        push f64 #5.0
        cgt
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CGT, INVALID_I32_PTR) {
    const auto assembly = R"(
        push i32 #5
        push ptr $x
        cgt
        halt

    .data
    $x: i32 5
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CGT, INVALID_UNDERFLOW_BY_ONE) {
    const auto assembly = R"(
        push i32 #5
        cgt
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CGT, INVALID_UNDERFLOW_BY_TWO) {
    const auto assembly = R"(
        cgt
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}
