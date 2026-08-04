#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(CLE, I32_I32_TRUE) {
    const auto assembly = R"(
        push i32 #0
        push i32 #1
        cle
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(1));
}

TEST(CLE, I32_I32_FALSE) {
    const auto assembly = R"(
        push i32 #1
        push i32 #0
        cle
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(0));
}

TEST(CLE, UI32_UI32_TRUE) {
    const auto assembly = R"(
        push ui32 #10
        push ui32 #10
        cle
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(1));
}

TEST(CLE, UI32_UI32_FALSE) {
    const auto assembly = R"(
        push ui32 #100
        push ui32 #10
        cle
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(0));
}

TEST(CLE, I64_I64_TRUE) {
    const auto assembly = R"(
        push i64 #-100
        push i64 #-10
        cle
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(1));
}

TEST(CLE, I64_I64_FALSE) {
    const auto assembly = R"(
        push i64 #-10
        push i64 #-100
        cle
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(0));
}

TEST(CLE, UI64_UI64_TRUE) {
    const auto assembly = R"(
        push ui64 #0
        push ui64 #1
        cle
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(1));
}

TEST(CLE, UI64_UI64_FALSE) {
    const auto assembly = R"(
        push ui64 #1
        push ui64 #0
        cle
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(0));
}

TEST(CLE, F32_F32_TRUE) {
    const auto assembly = R"(
        push f32 #-0.5
        push f32 #0.5
        cle
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(1));
}

TEST(CLE, F32_F32_FALSE) {
    const auto assembly = R"(
        push f32 #0.5
        push f32 #-0.5
        cle
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(0));
}

TEST(CLE, F64_F64_TRUE) {
    const auto assembly = R"(
        push f64 #0.0
        push f64 #0.0
        cle
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(1));
}

TEST(CLE, F64_F64_FALSE) {
    const auto assembly = R"(
        push f64 #100.5
        push f64 #0.0
        cle
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(0));
}

TEST(CLE, PTR_PTR_TRUE) {
    const auto assembly = R"(
        push ptr $x
        push ptr $y
        cle
        halt

    .data
    $x: i32 5
    $y: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(1));
}

TEST(CLE, PTR_PTR_FALSE) {
    const auto assembly = R"(
        push ptr $y
        push ptr $x
        cle
        halt

    .data
    $x: i32 5
    $y: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(0));
}

TEST(CLE, INVALID_I32_UI32) {
    const auto assembly = R"(
        push i32 #5
        push ui32 #5
        cle
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CLE, INVALID_I64_UI64) {
    const auto assembly = R"(
        push i64 #5
        push ui64 #5
        cle
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CLE, INVALID_F32_F64) {
    const auto assembly = R"(
        push f32 #5.0
        push f64 #5.0
        cle
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CLE, INVALID_I32_PTR) {
    const auto assembly = R"(
        push i32 #5
        push ptr $x
        cle
        halt

    .data
    $x: i32 5
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CLE, INVALID_UNDERFLOW_BY_ONE) {
    const auto assembly = R"(
        push i32 #5
        cle
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CLE, INVALID_UNDERFLOW_BY_TWO) {
    const auto assembly = R"(
        cle
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}
