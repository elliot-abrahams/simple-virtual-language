#include <gtest/gtest.h>
#include "../../vmTestUtils.h"

TEST(CEQ, I32_I32_TRUE) {
    const auto assembly = R"(
        push i32 #0
        push i32 #0
        ceq
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(1));
}

TEST(CEQ, I32_I32_FALSE) {
    const auto assembly = R"(
        push i32 #1
        push i32 #0
        ceq
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(0));
}

TEST(CEQ, UI32_UI32_TRUE) {
    const auto assembly = R"(
        push ui32 #10
        push ui32 #10
        ceq
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(1));
}

TEST(CEQ, UI32_UI32_FALSE) {
    const auto assembly = R"(
        push ui32 #10
        push ui32 #100
        ceq
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(0));
}

TEST(CEQ, I64_I64_TRUE) {
    const auto assembly = R"(
        push i64 #-10
        push i64 #-10
        ceq
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(1));
}

TEST(CEQ, I64_I64_FALSE) {
    const auto assembly = R"(
        push i64 #-10
        push i64 #-100
        ceq
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(0));
}

TEST(CEQ, UI64_UI64_TRUE) {
    const auto assembly = R"(
        push ui64 #0
        push ui64 #0
        ceq
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(1));
}

TEST(CEQ, UI64_UI64_FALSE) {
    const auto assembly = R"(
        push ui64 #1
        push ui64 #0
        ceq
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(0));
}

TEST(CEQ, F32_F32_TRUE) {
    const auto assembly = R"(
        push f32 #0.0
        push f32 #0.0
        ceq
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(1));
}

TEST(CEQ, F32_F32_FALSE) {
    const auto assembly = R"(
        push f32 #1.0
        push f32 #0.0
        ceq
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(0));
}

TEST(CEQ, F64_F64_TRUE) {
    const auto assembly = R"(
        push f64 #-0.5
        push f64 #-0.5
        ceq
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(1));
}

TEST(CEQ, F64_F64_FALSE) {
    const auto assembly = R"(
        push f64 #-0.1
        push f64 #100.5
        ceq
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(0));
}

TEST(CEQ, PTR_PTR_TRUE) {
    const auto assembly = R"(
        push ptr $x
        push ptr $x
        ceq
        halt

    .data
    $x: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(1));
}

TEST(CEQ, PTR_PTR_FALSE) {
    const auto assembly = R"(
        push ptr $x
        push ptr $y
        ceq
        halt

    .data
    $x: i32 5
    $y: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(0));
}

TEST(CEQ, INVALID_I32_UI32) {
    const auto assembly = R"(
        push i32 #5
        push ui32 #5
        ceq
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CEQ, INVALID_I64_UI64) {
    const auto assembly = R"(
        push i64 #5
        push ui64 #5
        ceq
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CEQ, INVALID_F32_F64) {
    const auto assembly = R"(
        push f32 #5.0
        push f64 #5.0
        ceq
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CEQ, INVALID_I32_PTR) {
    const auto assembly = R"(
        push i32 #5
        push ptr $x
        ceq
        halt

    .data
    $x: i32 5
    )";
    EXPECT_VM_ERROR(assembly);
}
