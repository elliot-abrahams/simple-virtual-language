#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(MUL, I32_I32) {
    const auto assembly = R"(
        push i32 #2
        push i32 #3
        mul
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(6));
}

TEST(MUL, UI32_UI32) {
    const auto assembly = R"(
        push ui32 #2
        push ui32 #3
        mul
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(6));
}

TEST(MUL, I64_I64) {
    const auto assembly = R"(
        push i64 #2
        push i64 #3
        mul
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(6));
}

TEST(MUL, UI64_UI64) {
    const auto assembly = R"(
        push ui64 #2
        push ui64 #3
        mul
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(6));
}

TEST(MUL, F32_F32) {
    const auto assembly = R"(
        push f32 #5.5
        push f32 #2.0
        mul
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 11.0f);
}

TEST(MUL, F64_F64) {
    const auto assembly = R"(
        push f64 #5.5
        push f64 #2.0
        mul
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F64, 11.0);
}

TEST(MUL, I32_NEGATIVE) {
    const auto assembly = R"(
        push i32 #-2
        push i32 #3
        mul
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(-6));
}

TEST(MUL, INVALID_UNDERFLOW_BY_ONE) {
    const auto assembly = R"(
        push i32 #5
        mul
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(MUL, INVALID_UNDERFLOW_BY_TWO) {
    const auto assembly = R"(
        mul
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}