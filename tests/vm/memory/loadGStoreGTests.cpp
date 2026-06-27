#include <gtest/gtest.h>
#include "../vmTestUtils.h"

TEST(LOADG_STOREG, I32) {
    const auto assembly = R"(
        push i32 #5
        storeG $x
        loadG $x
        halt

    .data
    $x: i32 0
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(LOADG_STOREG, UI32) {
    const auto assembly = R"(
        push ui32 #5
        storeG $x
        loadG $x
        halt

    .data
    $x: ui32 0
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(5));
}

TEST(LOADG_STOREG, I64) {
    const auto assembly = R"(
        push i64 #5
        storeG $x
        loadG $x
        halt

    .data
    $x: i64 0
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(5));
}

TEST(LOADG_STOREG, UI64) {
    const auto assembly = R"(
        push ui64 #5
        storeG $x
        loadG $x
        halt

    .data
    $x: ui64 0
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(5));
}

TEST(LOADG_STOREG, F32) {
    const auto assembly = R"(
        push f32 #5.0
        storeG $x
        loadG $x
        halt

    .data
    $x: f32 0.0
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 5.0f);
}

TEST(LOADG_STOREG, F64) {
    const auto assembly = R"(
        push f64 #5.0
        storeG $x
        loadG $x
        halt

    .data
    $x: f64 0.0
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F64, 5.0);
}

TEST(LOADG_STOREG, PTR) {
    const auto assembly = R"(
        push ptr $x
        storeG $x
        loadG $x
        halt

    .data
    $x: ptr $y
    $y: i32 5 ; without this -> x would be equal to itself -> test would pass if storeG did nothing
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::PTR, uint32_t(18));
}

TEST(LOADG_STOREG, INVALID_STOREG_UNDERFLOW) {
    const auto assembly = R"(
        storeG $x
        halt

    .data
    $x: i32 5
    )";
    EXPECT_VM_ERROR(assembly);
}