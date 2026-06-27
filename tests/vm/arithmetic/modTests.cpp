#include <filesystem>
#include <gtest/gtest.h>
#include "../vmTestUtils.h"

TEST(MOD, I32_I32) {
    const auto assembly = R"(
        push i32 #8
        push i32 #3
        mod
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(2));
}

TEST(MOD, UI32_UI32) {
    const auto assembly = R"(
        push ui32 #8
        push ui32 #3
        mod
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(2));
}

TEST(MOD, I64_I64) {
    const auto assembly = R"(
        push i64 #8
        push i64 #3
        mod
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(2));
}

TEST(MOD, UI64_UI64) {
    const auto assembly = R"(
        push ui64 #8
        push ui64 #3
        mod
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(2));
}

TEST(MOD, F32_F32) {
    const auto assembly = R"(
        push f32 #8.0
        push f32 #2.5
        mod
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 0.5f);
}

TEST(MOD, F64_F64) {
    const auto assembly = R"(
        push f64 #8.0
        push f64 #2.5
        mod
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F64, 0.5);
}

TEST(MOD, I32_NEGATIVE) {
    const auto assembly = R"(
        push i32 #-8
        push i32 #3
        mod
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(-2));
}

TEST(MOD, INVALID_I32_ZERO) {
    const auto assembly = R"(
        push i32 #5
        push i32 #0
        mod
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(MOD, INVALID_PTR_PTR) {
    const auto assembly = R"(
        push ptr $x
        push ptr $x
        mod
        halt

    .data
    $x: i32 5
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(MOD, INVALID_I32_F32) {
    const auto assembly = R"(
        push i32 #5
        push f32 #2.0
        mod
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(MOD, INVALID_UI64_F64) {
    const auto assembly = R"(
        push ui64 #5
        push f64 #2.0
        mod
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(MOD, INVALID_PTR_I64) {
    const auto assembly = R"(
        push ptr $x
        push i64 #5
        mod
        halt

    .data
    $x: i32 5
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(MOD, INVALID_UNDERFLOW_BY_ONE) {
    const auto assembly = R"(
        push i32 #5
        mod
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(MOD, INVALID_UNDERFLOW_BY_TWO) {
    const auto assembly = R"(
        mod
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}