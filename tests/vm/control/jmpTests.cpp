#include <gtest/gtest.h>
#include "../vmTestUtils.h"

TEST(JMP, JUMP) {
    const auto assembly = R"(
        jmp $jump
        push f32 #8.0 ; this should be jumped over
        halt

    $jump:
        push i32 #5
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}