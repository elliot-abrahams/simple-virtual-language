#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(ADDRL, ARG) {
    EXPECT_OPERAND_VM_STACK_EQ(
        R"(
            call $test
            halt

            def $test:
                args 1
                locals 0

                addrL #1
                ret
        )",
        ISA::Type::PTR,
        uint32_t(UINT32_MAX - 16)
    );
}

TEST(ADDRL, LOCAL) {
    EXPECT_OPERAND_VM_STACK_EQ(
        R"(
            call $test
            halt

            def $test:
                args 1
                locals 0

                addrL #-5
                ret
        )",
        ISA::Type::PTR,
        uint32_t(UINT32_MAX - 40)
    );
}

TEST(AARL, INVALID_ARG_OUT_OF_BOUNDS) {
    EXPECT_RUNTIME_ERROR(
    R"(
            call $test
            halt

            def $test:
                args 10
                locals 10

                addrL #50
                ret
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(AARL, INVALID_LOCALS_OUT_OF_BOUNDS) {
    EXPECT_RUNTIME_ERROR(
    R"(
            call $test
            halt

            def $test:
                args 10
                locals 10

                addrL #-50
                ret
        )",
        RuntimeErrorType::INTERNAL
    );
}

TEST(ADDRL, INVALID_NO_STACK_FRAME) {
    EXPECT_RUNTIME_ERROR(
        R"(
            addrL #1
            halt
        )",
        RuntimeErrorType::INTERNAL
    );
}