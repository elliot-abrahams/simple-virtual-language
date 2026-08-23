#include <filesystem>
#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(DIV, I32_I32) {
    const auto assembly = R"(
        push i32 #6
        push i32 #3
        div
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(2));
}

TEST(DIV, UI32_UI32) {
    const auto assembly = R"(
        push ui32 #6
        push ui32 #3
        div
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(2));
}

TEST(DIV, I64_I64) {
    const auto assembly = R"(
        push i64 #6
        push i64 #3
        div
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(2));
}

TEST(DIV, UI64_UI64) {
    const auto assembly = R"(
        push ui64 #6
        push ui64 #3
        div
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(2));
}

TEST(DIV, F32_F32) {
    const auto assembly = R"(
        push f32 #5.0
        push f32 #2.5
        div
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 2.0f);
}

TEST(DIV, F64_F64) {
    const auto assembly = R"(
        push f64 #5.0
        push f64 #2.5
        div
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F64, 2.0);
}

TEST(DIV, I32_NEGATIVE) {
    const auto assembly = R"(
        push i32 #-8
        push i32 #2
        div
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(-4));
}

TEST(DIV, INVALID_PTR_PTR) {
    const auto assembly = R"(
        push ptr $x
        push ptr $x
        div
        halt

    .data
    $x: i32 5
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(DIV, INVALID_I32_ZERO) {
    const auto assembly = R"(
        push i32 #5
        push i32 #0
        div
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(DIV, INVALID_I32_F32) {
    const auto assembly = R"(
        push i32 #5
        push f32 #2.0
        div
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(DIV, INVALID_UI64_F64) {
    const auto assembly = R"(
        push ui64 #5
        push f64 #2.0
        div
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(DIV, INVALID_PTR_I64) {
    const auto assembly = R"(
        push ptr $x
        push i64 #5
        div
        halt

    .data
    $x: i32 5
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(DIV, INVALID_UNDERFLOW_BY_ONE) {
    const auto assembly = R"(
        push i32 #5
        div
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(DIV, INVALID_UNDERFLOW_BY_TWO) {
    const auto assembly = R"(
        div
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}