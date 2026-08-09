#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(LOADB_STOREB, I32) {
    const auto assembly = R"(
        push ui32 #50
        alloc
        push ui32 #40
        push i32 #64
        storeB
        push ui32 #40
        loadB
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(64));
}

TEST(LOADB_STOREB, UI32) {
    const auto assembly = R"(
        push ui32 #50
        alloc
        push ui32 #40
        push ui32 #64
        storeB
        push ui32 #40
        loadB
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(64));
}

TEST(LOADB_STOREB, I64) {
    const auto assembly = R"(
        push ui32 #50
        alloc
        push ui32 #40
        push i64 #64
        storeB
        push ui32 #40
        loadB
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(64));
}

TEST(LOADB_STOREB, UI64) {
    const auto assembly = R"(
        push ui32 #50
        alloc
        push ui32 #40
        push ui64 #64
        storeB
        push ui32 #40
        loadB
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(64));
}

TEST(LOADB_STOREB, TRUNCATE) {
    const auto assembly = R"(
        push ui32 #50
        alloc
        push ui32 #40
        push ui32 #257
        storeB
        push ui32 #40
        loadB
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(1));
}

TEST(LOADB_STOREB, INVALID_F32) {
    const auto assembly = R"(
        push ui32 #50
        alloc
        push ui32 #40
        push f32 #5.5
        storeB
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(LOADB_STOREB, INVALID_F64) {
    const auto assembly = R"(
        push ui32 #50
        alloc
        push ui32 #40
        push f64 #5.5
        storeB
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(LOADB_STOREB, INVALID_PTR) {
    const auto assembly = R"(
        push ui32 #50
        alloc
        push ui32 #40
        push ptr $x
        storeB
        halt

    .data
    $x: i32 5
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(LOADB_STOREB, INVALID_LOADB_UNDERFLOW) {
    const auto assembly = R"(
        loadB
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(LOADB_STOREB, INVALID_STOREB_UNDERFLOW_BY_ONE) {
    const auto assembly = R"(
        push ui32 #500
        storeB
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(LOADB_STOREB, INVALID_LOADB_UNDERFLOW_BY_TWO) {
    const auto assembly = R"(
        storeB
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}