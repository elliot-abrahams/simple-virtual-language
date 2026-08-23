#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(POP, I32) {
    const auto assembly = R"(
        push i32 #5
        push f32 #-600.0
        pop
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(POP, UI32) {
    const auto assembly = R"(
        push ui32 #5
        push f32 #-600.0
        pop
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(5));
}

TEST(POP, I64) {
    const auto assembly = R"(
        push i64 #5
        push f32 #-600.0
        pop
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(5));
}

TEST(POP, UI64) {
    const auto assembly = R"(
        push ui64 #5
        push f32 #-600.0
        pop
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(5));
}

TEST(POP, F32) {
    const auto assembly = R"(
        push f32 #5.0
        push i32 #6
        pop
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 5.0f);
}

TEST(POP, F64) {
    const auto assembly = R"(
        push f64 #5.0
        push i32 #6
        pop
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F64, 5.0);
}

TEST(POP, PTR) {
    const auto assembly = R"(
        push ptr $x
        push i32 #6
        pop
        halt

    .data
    $x: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::PTR, uint32_t(15));
}

TEST(POP, INVLAID_UNDERFLOW) {
    const auto assembly = R"(
        pop
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}