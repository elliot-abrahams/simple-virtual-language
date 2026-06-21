#include <gtest/gtest.h>
#include "../../vmTestUtils.h"

TEST(NOT, I32) {
    const auto assembly = R"(
        push i32 #5
        not
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(-6));
}

TEST(NOT, UI32) {
    const auto assembly = R"(
        push ui32 #5
        not
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(4294967290));
}

TEST(NOT, I64) {
    const auto assembly = R"(
        push i64 #5
        not
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(-6));
}

TEST(NOT, UI64) {
    const auto assembly = R"(
        push ui64 #5
        not
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(18446744073709551610));
}

TEST(NOT, INVALID_F32) {
    const auto assembly = R"(
        push f32 #5.0
        not
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(NOT, INVALID_F64) {
    const auto assembly = R"(
        push f64 #5.0
        not
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}