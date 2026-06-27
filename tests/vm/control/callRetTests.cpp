#include <gtest/gtest.h>
#include "../vmTestUtils.h"
#include "../../../src/vm/memory/MemoryManager.h"

TEST(CALL_RET, CALL_RETURNS_TO_CALLER) {
    const auto assembly = R"(
        call $method
        push i32 #5
        halt

    def $method:
        args 0
        locals 0

        push ui32 #0
        ret
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(CALL_RET, CALL_PUSHES_RETURN_VALUE) {
    const auto assembly = R"(
        call $method
        halt

    def $method:
        args 0
        locals 0

        push i32 #5
        ret
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(CALL_RET, CALL_PASSES_MULTIPLE_ARGUMENTS) {
    const auto assembly = R"(
        push i32 #2
        push i32 #4
        push i32 #6
        call $method

    def $method:
        args 3
        locals 0

        loadL i32 #1
        loadL i32 #2
        loadL i32 #3
        halt
    )";
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        assembly,
        ISA::Type::I32,
        std::vector{int32_t(6), int32_t(4), int32_t(2)}
    );
}

TEST(CALL_RET, NESTED_CALL) {
    const auto assembly = R"(
        push i32 #5
        call $method1
        halt

    def $method1:
        args 1
        locals 0

        loadL i32 #1
        push i32 #10
        call $method2
        ret

    def $method2:
        args 1
        locals 0

        loadL i32 #1
        ret
    )";
    EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
        assembly,
        ISA::Type::I32,
        std::vector{int32_t(10), int32_t(5)}
    );
}

TEST(CALL_RET, LOCAL_ZERO_INITIALISE) {
    const auto assembly = R"(
        call $method
        halt

    def $method:
        args 0
        locals 1

        loadL i32 #-1
        ret
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(0));
}

TEST(CALL_RET, INVALID_RET_UNDERFLOW) {
    const auto assembly = R"(
        ret
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CALL_RET, IVALID_OVERFLOW) {
    const auto assembly = R"(
        call $method
        halt

    def $method:
        args 0
        locals 4294967294

        call $method
        ret
    )";
    EXPECT_VM_ERROR(assembly);
}