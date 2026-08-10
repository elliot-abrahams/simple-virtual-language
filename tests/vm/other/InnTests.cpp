#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(INN, READ_I32) {
    const auto assembly = R"(
        inn i32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ_WITH_CONSOLE_INPUT(
        assembly,
        "5\n",
        ISA::Type::I32,
        int32_t(5)
    );
}

TEST(INN, READ_I32_NEGATIVE) {
    const auto assembly = R"(
        inn i32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ_WITH_CONSOLE_INPUT(
        assembly,
        "-5\n",
        ISA::Type::I32,
        int32_t(-5)
    );
}

TEST(INN, READ_UI32) {
    const auto assembly = R"(
        inn ui32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ_WITH_CONSOLE_INPUT(
        assembly,
        "5\n",
        ISA::Type::UI32,
        uint32_t(5)
    );
}

TEST(INN, READ_I64) {
    const auto assembly = R"(
        inn i64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ_WITH_CONSOLE_INPUT(
        assembly,
        "5\n",
        ISA::Type::I64,
        int64_t(5)
    );
}

TEST(INN, READ_I64_NEGATIVE) {
    const auto assembly = R"(
        inn i64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ_WITH_CONSOLE_INPUT(
        assembly,
        "-5\n",
        ISA::Type::I64,
        int64_t(-5)
    );
}

TEST(INN, READ_UI64) {
    const auto assembly = R"(
        inn ui64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ_WITH_CONSOLE_INPUT(
        assembly,
        "5\n",
        ISA::Type::UI64,
        uint64_t(5)
    );
}

TEST(INN, READ_F32_DECIMAL_INPUT) {
    const auto assembly = R"(
        inn f32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ_WITH_CONSOLE_INPUT(
        assembly,
        "5.0\n",
        ISA::Type::F32,
        5.0f
    );
}

TEST(INN, READ_F32) {
    const auto assembly = R"(
        inn f32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ_WITH_CONSOLE_INPUT(
        assembly,
        "5\n",
        ISA::Type::F32,
        5.0f
    );
}

TEST(INN, READ_F32_NEGATIVE) {
    const auto assembly = R"(
        inn f32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ_WITH_CONSOLE_INPUT(
        assembly,
        "-5\n",
        ISA::Type::F32,
        -5.0f
    );
}

TEST(INN, READ_F64_DECIMAL_INPUT) {
    const auto assembly = R"(
        inn f64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ_WITH_CONSOLE_INPUT(
        assembly,
        "5\n",
        ISA::Type::F64,
        5.0
    );
}

TEST(INN, READ_F64) {
    const auto assembly = R"(
        inn f64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ_WITH_CONSOLE_INPUT(
        assembly,
        "5.0\n",
        ISA::Type::F64,
        5.0
    );
}

TEST(INN, READ_F64_NEGATIVE) {
    const auto assembly = R"(
        inn f64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ_WITH_CONSOLE_INPUT(
        assembly,
        "-5.0\n",
        ISA::Type::F64,
        -5.0
    );
}

TEST(INN, READ_STR) {
    const auto assembly = R"(
        inn str
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ_WITH_CONSOLE_INPUT(
        assembly,
        "hello\n",
        ISA::Type::PTR,
        uint32_t(7)
    );
}

TEST(INN, READ_WRITE_STR) {
    const auto assembly = R"(
        inn str
        out
        halt
    )";
    EXPECT_CONSOLE_OUTPUT_WITH_CONSOLE_INPUT(
        assembly,
        "hello",
        "hello"
    );
}

TEST(INN, READ_WRITE_STR_UTF_8) {
    const auto assembly = R"(
        inn str
        out
        halt
    )";
    EXPECT_CONSOLE_OUTPUT_WITH_CONSOLE_INPUT(
        assembly,
        "hello😀",
        "hello😀"
    );
}


TEST(INN, INVALID_READ_I32_OUT_OF_RANGE) {
    const auto assembly = R"(
        inn i32
        halt
    )";
    EXPECT_VM_ERROR_WITH_CONSOLE_INPUT(
        assembly,
        "10000000000000000000000000000000"
    );
}

TEST(INN, INVALID_READ_UI32_OUT_OF_RANGE) {
    const auto assembly = R"(
        inn ui32
        halt
    )";
    EXPECT_VM_ERROR_WITH_CONSOLE_INPUT(
        assembly,
        "10000000000000000000000000000000"
    );
}

TEST(INN, INVALID_READ_I64_OUT_OF_RANGE) {
    const auto assembly = R"(
        inn i64
        halt
    )";
    EXPECT_VM_ERROR_WITH_CONSOLE_INPUT(
        assembly,
        "10000000000000000000000000000000"
    );
}

TEST(INN, INVALID_READ_UI64_OUT_OF_RANGE) {
    const auto assembly = R"(
        inn ui64
        halt
    )";
    EXPECT_VM_ERROR_WITH_CONSOLE_INPUT(
        assembly,
        "10000000000000000000000000000000"
    );
}

TEST(INN, INVALID_READ_F32_OUT_OF_RANGE) {
    const auto assembly = R"(
        inn f32
        halt
    )";
    EXPECT_VM_ERROR_WITH_CONSOLE_INPUT(
        assembly,
        "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    );
}

TEST(INN, INVALID_READ_F64_OUT_OF_RANGE) {
    const auto assembly = R"(
        inn f64
        halt
    )";
    EXPECT_VM_ERROR_WITH_CONSOLE_INPUT(
        assembly,
        "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    );
}

TEST(INN, INVALID_READ_PTR) {
    const auto assembly = R"(
        inn ptr
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}