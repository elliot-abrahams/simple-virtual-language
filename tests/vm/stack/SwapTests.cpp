#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(SWAP, I32) {
    const auto assembly = R"(
        push i32 #5
        push i32 #10
        swap
        halt
    )";
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        assembly,
        ISA::Type::I32,
        std::vector{int32_t(5), int32_t(10)}
    );
}

TEST(SWAP, UI32) {
    const auto assembly = R"(
        push ui32 #5
        push ui32 #10
        swap
        halt
    )";
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        assembly,
        ISA::Type::UI32,
        std::vector{uint32_t(5), uint32_t(10)}
    );
}

TEST(SWAP, I64) {
    const auto assembly = R"(
        push i64 #5
        push i64 #10
        swap
        halt
    )";
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        assembly,
        ISA::Type::I64,
        std::vector{int64_t(5), int64_t(10)}
    );
}

TEST(SWAP, UI64) {
    const auto assembly = R"(
        push ui64 #5
        push ui64 #10
        swap
        halt
    )";
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        assembly,
        ISA::Type::UI64,
        std::vector{uint64_t(5), uint64_t(10)}
    );
}

TEST(SWAP, F32) {
    const auto assembly = R"(
        push f32 #5.0
        push f32 #10.0
        swap
        halt
    )";
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        assembly,
        ISA::Type::F32,
        std::vector{5.0f, 10.0f}
    );
}

TEST(SWAP, F64) {
    const auto assembly = R"(
        push f64 #5.0
        push f64 #10.0
        swap
        halt
    )";
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        assembly,
        ISA::Type::F64,
        std::vector{5.0, 10.0}
    );
}

TEST(SWAP, INVALID_UNDERFLOW_BY_ONE) {
    const auto assembly = R"(
        push i32 #5
        swap
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(SWAP, INVALID_UNDERFLOW_BY_TWO) {
    const auto assembly = R"(
        swap
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}