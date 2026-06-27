#include <filesystem>
#include <gtest/gtest.h>
#include "../vmTestUtils.h"

TEST(ORR, I32_I32) {
    const auto assembly = R"(
        push i32 #6
        push i32 #5
        orr
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(7));
}

TEST(ORR, UI32_UI32) {
    const auto assembly = R"(
        push ui32 #6
        push ui32 #5
        orr
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(7));
}

TEST(ORR, I64_I64) {
    const auto assembly = R"(
        push i64 #6
        push i64 #5
        orr
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(7));
}

TEST(ORR, UI64_UI64) {
    const auto assembly = R"(
        push ui64 #6
        push ui64 #5
        orr
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(7));
}

TEST(ORR, INVALID_F32_F32) {
    const auto assembly = R"(
        push f32 #8.0
        push f32 #2.5
        orr
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(ORR, INVALID_F64_F64) {
    const auto assembly = R"(
        push f64 #8.0
        push f64 #2.5
        orr
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(ORR, INVALID_PTR_PTR) {
    const auto assembly = R"(
        push ptr $x
        push ptr $x
        orr
        halt

    .data
    $x: i32 5
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(ORR, INVALID_I32_F32) {
    const auto assembly = R"(
        push i32 #5
        push f32 #2.0
        orr
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(ORR, INVALID_UI64_F64) {
    const auto assembly = R"(
        push ui64 #5
        push f64 #2.0
        orr
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(ORR, INVALID_PTR_I64) {
    const auto assembly = R"(
        push ptr $x
        push i64 #5
        orr
        halt

    .data
    $x: i32 5
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(ORR, INVALID_UNDERFLOW_BY_ONE) {
    const auto assembly = R"(
        push i32 #5
        orr
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(ORR, INVALID_UNDERFLOW_BY_TWO) {
    const auto assembly = R"(
        orr
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}