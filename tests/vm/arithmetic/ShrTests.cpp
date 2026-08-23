#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(SHR, I32_I32) {
    const auto assembly = R"(
        push i32 #4
        push i32 #2
        shr
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(1));
}

TEST(SHR, UI32_I32) {
    const auto assembly = R"(
        push ui32 #4
        push i32 #3
        shr
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(0));
}

TEST(SHR, I64_I32) {
    const auto assembly = R"(
        push i64 #5
        push i32 #2
        shr
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(1));
}

TEST(SHR, UI64_I32) {
    const auto assembly = R"(
        push ui64 #4
        push i32 #1
        shr
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(2));
}

TEST(SHR, I32_UI32) {
    const auto assembly = R"(
        push i32 #4
        push ui32 #1
        shr
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(2));
}

TEST(SHR, I32_I64) {
    const auto assembly = R"(
        push i32 #4
        push i64 #1
        shr
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(2));
}

TEST(SHR, I32_UI64) {
    const auto assembly = R"(
        push i32 #4
        push ui64 #1
        shr
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(2));
}

TEST(SHR, UI32_MASK_I32) {
    const auto assembly = R"(
        push ui32 #4
        push i32 #33
        shr
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(2));
}

TEST(SHR, UI32_MASK_I64) {
    const auto assembly = R"(
        push ui32 #4
        push i64 #4294967297
        shr
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(2));
}

TEST(SHR, UI32_NEGATIVE_MASK_I32) {
    const auto assembly = R"(
        push ui32 #4
        push i32 #-31
        shr
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(2));
}

TEST(SHR, NEGATIVE_I32_I32) {
    const auto assembly = R"(
        push i32 #-4
        push i32 #1
        shr
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(2147483646));
}

TEST(SHR, INVALID_F32_I32) {
    const auto assembly = R"(
        push f32 #5.0
        push i32 #5
        shr
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(SHR, INVALID_F64_I32) {
    const auto assembly = R"(
        push f64 #5.0
        push i32 #5
        shr
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(SHR, INVALID_PTR_I32) {
    const auto assembly = R"(
        push ptr $x
        push i32 #5
        shr
        halt

    .data
    $x: i32 5
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(SHR, INVALID_I32_F32) {
    const auto assembly = R"(
        push i32 #5
        push f32 #5.0
        shr
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(SHR, INVALID_I32_F64) {
    const auto assembly = R"(
        push i32 #5
        push f64 #5.0
        shr
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(SHR, INVALID_I32_PTR) {
    const auto assembly = R"(
        push i32 #5
        push ptr $x
        shr
        halt

    .data
    $x: i32 5
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(SHR, INVALID_UNDERFLOW_BY_ONE) {
    const auto assembly = R"(
        push i32 #5
        shr
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(SHR, INVALID_UNDERFLOW_BY_TWO) {
    const auto assembly = R"(
        shr
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}