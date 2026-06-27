#include <gtest/gtest.h>
#include "../vmTestUtils.h"

TEST(HALT, HALTS_PROGRAM) {
    const auto assembly = R"(
        push i32 #5
        halt
        push f32 #5.0
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}
