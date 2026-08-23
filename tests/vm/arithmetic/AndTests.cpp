#include <filesystem>
#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(AND, I32_I32) {
    const auto assembly = R"(
        push i32 #6
        push i32 #4
        and
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(4));
}

TEST(AND, UI32_UI32) {
    const auto assembly = R"(
        push ui32 #6
        push ui32 #4
        and
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(4));
}

TEST(AND, I64_I64) {
    const auto assembly = R"(
        push i64 #6
        push i64 #4
        and
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(4));
}

TEST(AND, UI64_UI64) {
    const auto assembly = R"(
        push ui64 #6
        push ui64 #4
        and
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(4));
}

TEST(AND, INVALID_F32_F32) {
    const auto assembly = R"(
        push f32 #8.0
        push f32 #2.5
        and
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(AND, INVALID_F64_F64) {
    const auto assembly = R"(
        push f64 #8.0
        push f64 #2.5
        and
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(AND, INVALID_PTR_PTR) {
    const auto assembly = R"(
        push ptr $x
        push ptr $x
        and
        halt

    .data
    $x: i32 5
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(AND, INVALID_I32_F32) {
    const auto assembly = R"(
        push i32 #5
        push f32 #2.0
        and
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(AND, INVALID_UI64_F64) {
    const auto assembly = R"(
        push ui64 #5
        push f64 #2.0
        and
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(AND, INVALID_PTR_I64) {
    const auto assembly = R"(
        push ptr $x
        push i64 #5
        and
        halt

    .data
    $x: i32 5
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(AND, INVALID_UNDERFLOW_BY_ONE) {
    const auto assembly = R"(
        push i32 #5
        and
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(AND, INVALID_UNDERFLOW_BY_TWO) {
    const auto assembly = R"(
        and
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}