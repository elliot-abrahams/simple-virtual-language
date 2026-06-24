#include <filesystem>
#include <gtest/gtest.h>
#include "../../vmTestUtils.h"

TEST(SUB, I32_I32) {
    const auto assembly = R"(
        push i32 #5
        push i32 #3
        sub
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(2));
}

TEST(SUB, UI32_UI32) {
    const auto assembly = R"(
        push ui32 #5
        push ui32 #3
        sub
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(2));
}

TEST(SUB, I64_I64) {
    const auto assembly = R"(
        push i64 #5
        push i64 #3
        sub
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(2));
}

TEST(SUB, UI64_UI64) {
    const auto assembly = R"(
        push ui64 #5
        push ui64 #3
        sub
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(2));
}

TEST(SUB, F32_F32) {
    const auto assembly = R"(
        push f32 #5.5
        push f32 #3.5
        sub
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 2.0f);
}

TEST(SUB, F64_F64) {
    const auto assembly = R"(
        push f64 #5.5
        push f64 #3.5
        sub
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F64, 2.0);
}

TEST(SUB, PTR_PTR) {
    const auto assembly = R"(
        push ptr $x
        push ptr $x
        sub
        halt

    .data
    $x: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(0));
}

TEST(SUB, PTR_I32) {
    const auto assembly = R"(
        push ptr $x
        push i32 #1
        sub
        halt

    .data
    $x: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::PTR, uint32_t(14));
}

TEST(SUB, PTR_UI32) {
    const auto assembly = R"(
        push ptr $x
        push ui32 #1
        sub
        halt

    .data
    $x: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::PTR, uint32_t(14));
}

TEST(SUB, I32_NEGATIVE) {
    const auto assembly = R"(
        push i32 #5
        push i32 #-3
        sub
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(8));
}

TEST(SUB, I32_WRAP) {
    const auto assembly = R"(
        push i32 #-2147483648
        push i32 #1
        sub
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(2147483647));
}

TEST(SUB, UI32_WRAP) {
    const auto assembly = R"(
        push ui32 #0
        push ui32 #1
        sub
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(4294967295));
}

TEST(SUB, I64_WRAP) {
    const auto assembly = R"(
        push i64 #-9223372036854775808
        push i64 #1
        sub
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(9223372036854775807));
}

TEST(SUB, UI64_WRAP) {
    const auto assembly = R"(
        push ui64 #0
        push ui64 #1
        sub
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(18446744073709551615));
}

TEST(SUB, INVALID_I32_F32) {
    const auto assembly = R"(
        push i32 #5
        push f32 #2.0
        sub
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(SUB, INVALID_UI64_F64) {
    const auto assembly = R"(
        push ui64 #5
        push f64 #2.0
        sub
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(SUB, INVALID_PTR_I64) {
    const auto assembly = R"(
        push ptr $x
        push i64 #5
        sub
        halt

    .data
    $x: i32 5
    )";
    EXPECT_VM_ERROR(assembly);
}