#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(JEZ, I32_ZERO) {
    const auto assembly = R"(
        push i32 #0
        jez $jump
        push f32 #8.0 ; this should be jumped over
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(JEZ, I32_NOT_ZERO) {
    const auto assembly = R"(
        push i32 #1
        jez $jump
        push f32 #8.0
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 8.0f);
}

TEST(JEZ, UI32_ZERO) {
    const auto assembly = R"(
        push ui32 #0
        jez $jump
        push f32 #8.0 ; this should be jumped over
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(JEZ, UI32_NOT_ZERO) {
    const auto assembly = R"(
        push ui32 #1
        jez $jump
        push f32 #8.0
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 8.0f);
}

TEST(JEZ, I64_ZERO) {
    const auto assembly = R"(
        push i64 #0
        jez $jump
        push f32 #8.0 ; this should be jumped over
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(JEZ, I64_NOT_ZERO) {
    const auto assembly = R"(
        push i64 #1
        jez $jump
        push f32 #8.0
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 8.0f);
}

TEST(JEZ, UI64_ZERO) {
    const auto assembly = R"(
        push ui64 #0
        jez $jump
        push f32 #8.0 ; this should be jumped over
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(JEZ, UI64_NOT_ZERO) {
    const auto assembly = R"(
        push ui64 #1
        jez $jump
        push f32 #8.0
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 8.0f);
}

TEST(JEZ, F32_ZERO) {
    const auto assembly = R"(
        push f32 #0.0
        jez $jump
        push f32 #8.0 ; this should be jumped over
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(JEZ, F32_NOT_ZERO) {
    const auto assembly = R"(
        push f32 #0.5
        jez $jump
        push f32 #8.0
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 8.0f);
}

TEST(JEZ, F64_ZERO) {
    const auto assembly = R"(
        push f64 #0.0
        jez $jump
        push f32 #8.0 ; this should be jumped over
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(JEZ, F64_NOT_ZERO) {
    const auto assembly = R"(
        push f64 #0.5
        jez $jump
        push f32 #8.0
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 8.0f);
}

TEST(JEZ, PTR_ZERO) {
    const auto assembly = R"(
        push ptr $x
        push i32 #33
        sub
        jez $jump
        push f32 #8.0 ; this should be jumped over
        halt

    $jump:
        push i32 #5
        halt

    .data
    $x: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(JEZ, PTR_NOT_ZERO) {
    const auto assembly = R"(
        push ptr $x
        jez $jump
        push f32 #8.0
        halt

    $jump:
        push i32 #5
        halt

    .data
    $x: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 8.0f);
}

TEST(JEZ, INVALID_UNDERFLOW) {
    const auto assembly = R"(
        jez $l
        halt
    $l:
    )";
    EXPECT_VM_ERROR(assembly);
}