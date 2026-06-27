#include <gtest/gtest.h>
#include "../vmTestUtils.h"

TEST(LOAD_STORE, I32) {
    const auto assembly = R"(
        push ui32 #500
        push i32 #5
        store
        push ui32 #500
        load i32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(LOAD_STORE, UI32) {
    const auto assembly = R"(
        push ui32 #500
        push ui32 #5
        store
        push ui32 #500
        load ui32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(5));
}

TEST(LOAD_STORE, I64) {
    const auto assembly = R"(
        push ui32 #500
        push i64 #5
        store
        push ui32 #500
        load i64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(5));
}

TEST(LOAD_STORE, UI64) {
    const auto assembly = R"(
        push ui32 #500
        push ui64 #5
        store
        push ui32 #500
        load ui64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(5));
}

TEST(LOAD_STORE, F32) {
    const auto assembly = R"(
        push ui32 #500
        push f32 #5.0
        store
        push ui32 #500
        load f32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 5.0f);
}

TEST(LOAD_STORE, F64) {
    const auto assembly = R"(
        push ui32 #500
        push f64 #5.0
        store
        push ui32 #500
        load f64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F64, 5.0);
}

TEST(LOAD_STORE, PTR) {
    const auto assembly = R"(
        push ui32 #500
        push ptr $x
        store
        push ui32 #500
        load ptr
        halt

    .data
    $x: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::PTR, uint32_t(23));
}

TEST(LOAD_STORE, INVALID_LOAD_UNDERFLOW) {
    const auto assembly = R"(
        load i32
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(LOAD_STORE, INVALID_STORE_UNDERFLOW_BY_ONE) {
    const auto assembly = R"(
        push ui32 #500
        store
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(LOAD_STORE, INVALID_STORE_UNDERFLOW_BY_TWO) {
    const auto assembly = R"(
        store
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}