#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(SHL, I32_I32) {
    const auto assembly = R"(
        push i32 #4
        push i32 #2
        shl
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(16));
}

TEST(SHL, UI32_I32) {
    const auto assembly = R"(
        push ui32 #4
        push i32 #3
        shl
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(32));
}

TEST(SHL, I64_I32) {
    const auto assembly = R"(
        push i64 #5
        push i32 #2
        shl
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(20));
}

TEST(SHL, UI64_I32) {
    const auto assembly = R"(
        push ui64 #4
        push i32 #1
        shl
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(8));
}

TEST(SHL, I32_UI32) {
    const auto assembly = R"(
        push i32 #4
        push ui32 #1
        shl
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(8));
}

TEST(SHL, I32_I64) {
    const auto assembly = R"(
        push i32 #4
        push i64 #1
        shl
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(8));
}

TEST(SHL, I32_UI64) {
    const auto assembly = R"(
        push i32 #4
        push ui64 #1
        shl
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(8));
}

TEST(SHL, UI32_MASK_I32) {
    const auto assembly = R"(
        push ui32 #4
        push i32 #33
        shl
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(8));
}

TEST(SHL, UI32_MASK_I64) {
    const auto assembly = R"(
        push ui32 #4
        push i64 #4294967297
        shl
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(8));
}

TEST(SHL, UI32_NEGATIVE_MASK_I32) {
    const auto assembly = R"(
        push ui32 #4
        push i32 #-31
        shl
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(8));
}

TEST(SHL, NEGATIVE_I32_I32) {
    const auto assembly = R"(
        push i32 #-4
        push i32 #1
        shl
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(-8));
}

TEST(SHL, INVALID_F32_I32) {
    const auto assembly = R"(
        push f32 #5.0
        push i32 #5
        shl
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(SHL, INVALID_F64_I32) {
    const auto assembly = R"(
        push f64 #5.0
        push i32 #5
        shl
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(SHL, INVALID_PTR_I32) {
    const auto assembly = R"(
        push ptr $x
        push i32 #5
        shl
        halt

    .data
    $x: i32 5
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(SHL, INVALID_I32_F32) {
    const auto assembly = R"(
        push i32 #5
        push f32 #5.0
        shl
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(SHL, INVALID_I32_F64) {
    const auto assembly = R"(
        push i32 #5
        push f64 #5.0
        shl
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(SHL, INVALID_I32_PTR) {
    const auto assembly = R"(
        push i32 #5
        push ptr $x
        shl
        halt

    .data
    $x: i32 5
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(SHL, INVALID_UNDERFLOW_BY_ONE) {
    const auto assembly = R"(
        push i32 #5
        shl
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(SHL, INVALID_UNDERFLOW_BY_TWO) {
    const auto assembly = R"(
        shl
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}