#include <gtest/gtest.h>
#include "../vmTestUtils.h"

TEST(JNZ, I32_ZERO) {
    const auto assembly = R"(
        push i32 #0
        jnz $jump
        push f32 #8.0
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 8.0f);
}

TEST(JNZ, I32_NOT_ZERO) {
    const auto assembly = R"(
        push i32 #1
        jnz $jump
        push f32 #8.0 ; this should be jumped over
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(JNZ, UI32_ZERO) {
    const auto assembly = R"(
        push ui32 #0
        jnz $jump
        push f32 #8.0
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 8.0f);
}

TEST(JNZ, UI32_NOT_ZERO) {
    const auto assembly = R"(
        push ui32 #1
        jnz $jump
        push f32 #8.0 ; this should be jumped over
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(JNZ, I64_ZERO) {
    const auto assembly = R"(
        push i64 #0
        jnz $jump
        push f32 #8.0
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 8.0f);
}

TEST(JNZ, I64_NOT_ZERO) {
    const auto assembly = R"(
        push i64 #1
        jnz $jump
        push f32 #8.0 ; this should be jumped over
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(JNZ, UI64_ZERO) {
    const auto assembly = R"(
        push ui64 #0
        jnz $jump
        push f32 #8.0
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 8.0f);
}

TEST(JNZ, UI64_NOT_ZERO) {
    const auto assembly = R"(
        push ui64 #1
        jnz $jump
        push f32 #8.0 ; this should be jumped over
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(JNZ, F32_ZERO) {
    const auto assembly = R"(
        push f32 #0.0
        jnz $jump
        push f32 #8.0
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 8.0f);
}

TEST(JNZ, F32_NOT_ZERO) {
    const auto assembly = R"(
        push f32 #0.5
        jnz $jump
        push f32 #8.0 ; this should be jumped over
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(JNZ, F64_ZERO) {
    const auto assembly = R"(
        push f64 #0.0
        jnz $jump
        push f32 #8.0
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 8.0f);
}

TEST(JNZ, F64_NOT_ZERO) {
    const auto assembly = R"(
        push f64 #0.5
        jnz $jump
        push f32 #8.0 ; this should be jumped over
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(JNZ, PTR_ZERO) {
    const auto assembly = R"(
        push ptr $x
        push i32 #33
        sub
        jnz $jump
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

TEST(JNZ, PTR_NOT_ZERO) {
    const auto assembly = R"(
        push ptr $x
        jnz $jump
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

TEST(JNZ, INVALID_UNDERFLOW) {
    const auto assembly = R"(
        jnz $l
        halt
    $l:
    )";
    EXPECT_VM_ERROR(assembly);
}