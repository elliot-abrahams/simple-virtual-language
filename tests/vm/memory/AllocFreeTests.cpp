#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(ALLOC_FREE, ALLOCATE_BLOCK) {
    const auto assembly = R"(
        push ui32 #4
        alloc
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::PTR, uint32_t(12));
}

TEST(ALLOC_FREE, ZERO_INITIALISE) {
    const auto assembly = R"(
        push ui32 #4
        alloc
        load ui32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(0));
}

TEST(ALLOC_FREE, ALLOCATE_TWO_CONSECUTIVE_BLOCKS) {
    const auto assembly = R"(
        push ui32 #4
        alloc
        push ui32 #4
        alloc
        halt
    )";
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        assembly,
        ISA::Type::PTR,
        std::vector{uint32_t(27), uint32_t(19)}
    );
}

TEST(ALLOC_FREE, ALLOCATE_AFTER_FREE) {
    const auto assembly = R"(
        push ui32 #4
        alloc
        push ui32 #4
        alloc
        swap
        dup
        free
        push ui32 #4
        alloc
        halt
    )";
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        assembly,
        ISA::Type::PTR,
        std::vector{uint32_t(29), uint32_t(29), uint32_t(37)}
    );
}

TEST(ALLOC_FREE, INVALID_ALLOCATE_ZERO_BYTES) {
    const auto assembly = R"(
        push ui32 #0
        alloc
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(ALLOC_FREE, INVALUD_ALLOC_UNDERFLOW) {
    const auto assembly = R"(
        alloc
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(ALLOC_FREE, INVALID_STACK_OVERFLOW) {
    const auto assembly = R"(
        push ui32 #4294967290
        alloc
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}

TEST(ALLOC_FREE, INVALID_FREE_UNDERFLOW) {
    const auto assembly = R"(
        free
        halt
    )";
    EXPECT_INTERNAL_RUNTIME_ERROR(assembly);
}