#include <gtest/gtest.h>
#include "../../vmTestUtils.h"

TEST(SAR, I32_I32) {
    const auto assembly = R"(
        push i32 #4
        push i32 #2
        sar
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(1));
}

TEST(SAR, UI32_I32) {
    const auto assembly = R"(
        push ui32 #4
        push i32 #3
        sar
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(0));
}

TEST(SAR, I64_I32) {
    const auto assembly = R"(
        push i64 #5
        push i32 #2
        sar
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(1));
}

TEST(SAR, UI64_I32) {
    const auto assembly = R"(
        push ui64 #4
        push i32 #1
        sar
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(2));
}

TEST(SAR, I32_UI32) {
    const auto assembly = R"(
        push i32 #4
        push ui32 #1
        sar
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(2));
}

TEST(SAR, I32_I64) {
    const auto assembly = R"(
        push i32 #4
        push i64 #1
        sar
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(2));
}

TEST(SAR, I32_UI64) {
    const auto assembly = R"(
        push i32 #4
        push ui64 #1
        sar
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(2));
}

TEST(SAR, UI32_MASK_I32) {
    const auto assembly = R"(
        push ui32 #4
        push i32 #33
        sar
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(2));
}

TEST(SAR, UI32_MASK_I64) {
    const auto assembly = R"(
        push ui32 #4
        push i64 #4294967297
        sar
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(2));
}

TEST(SAR, UI32_NEGATIVE_MASK_I32) {
    const auto assembly = R"(
        push ui32 #4
        push i32 #-31
        sar
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(2));
}

TEST(SAR, NEGATIVE_I32_I32) {
    const auto assembly = R"(
        push i32 #-4
        push i32 #1
        sar
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(-2));
}

TEST(SAR, INVALID_F32_I32) {
    const auto assembly = R"(
        push f32 #5.0
        push i32 #5
        sar
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(SAR, INVALID_F64_I32) {
    const auto assembly = R"(
        push f64 #5.0
        push i32 #5
        sar
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(SAR, INVALID_PTR_I32) {
    const auto assembly = R"(
    $x:
        push ptr $x
        push i32 #5
        sar
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(SAR, INVALID_I32_F32) {
    const auto assembly = R"(
        push i32 #5
        push f32 #5.0
        sar
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(SAR, INVALID_I32_F64) {
    const auto assembly = R"(
        push i32 #5
        push f64 #5.0
        sar
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(SAR, INVALID_I32_PTR) {
    const auto assembly = R"(
    $x:
        push i32 #5
        push ptr $x
        sar
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}