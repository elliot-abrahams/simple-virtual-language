#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(PUSH, I32) {
    const auto assembly = R"(
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(PUSH, UI32) {
    const auto assembly = R"(
        push ui32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(5));
}

TEST(PUSH, I64) {
    const auto assembly = R"(
        push i64 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(5));
}

TEST(PUSH, UI64) {
    const auto assembly = R"(
        push ui64 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(5));
}

TEST(PUSH, F32) {
    const auto assembly = R"(
        push f32 #5.0
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 5.0f);
}

TEST(PUSH, F64) {
    const auto assembly = R"(
        push f64 #5.0
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F64, 5.0);
}

TEST(PUSH, PTR) {
    const auto assembly = R"(
        push ptr $x
        halt

    .data
    $x: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::PTR, uint32_t(8));
}

