#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(CONV, I32_TO_I32) {
    const auto assembly = R"(
        push i32 #5
        conv i32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(CONV, I32_TO_UI32) {
    const auto assembly = R"(
        push i32 #5
        conv ui32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(5));
}

TEST(CONV, I32_TO_UI32_WRAP) {
    const auto assembly = R"(
        push i32 #-1
        conv ui32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(4294967295));
}

TEST(CONV, I32_TO_I64) {
    const auto assembly = R"(
        push i32 #5
        conv i64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(5));
}

TEST(CONV, I32_TO_UI64) {
    const auto assembly = R"(
        push i32 #5
        conv ui64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(5));
}

TEST(CONV, I32_TO_UI64_WRAP) {
    const auto assembly = R"(
        push i32 #-1
        conv ui64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(18446744073709551615));
}

TEST(CONV, I32_TO_F32) {
    const auto assembly = R"(
        push i32 #5
        conv f32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 5.0f);
}

TEST(CONV, I32_TO_F32_PRECISION_LOSS) {
    const auto assembly = R"(
        push i32 #16777217
        conv f32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 16777216.0f);
}

TEST(CONV, I32_TO_F64) {
    const auto assembly = R"(
        push i32 #5
        conv f64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F64, 5.0);
}

TEST(CONV, I32_TO_PTR) {
    const auto assembly = R"(
        push i32 #5
        conv ptr
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::PTR, uint32_t(5));
}

TEST(CONV, UI32_TO_I32) {
    const auto assembly = R"(
        push ui32 #5
        conv i32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(CONV, UI32_TO_I32_WRAP) {
    const auto assembly = R"(
        push ui32 #4294967295
        conv i32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(-1));
}

TEST(CONV, UI32_TO_UI32) {
    const auto assembly = R"(
        push ui32 #5
        conv ui32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(5));
}

TEST(CONV, UI32_TO_I64) {
    const auto assembly = R"(
        push ui32 #5
        conv i64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(5));
}

TEST(CONV, UI32_TO_UI64) {
    const auto assembly = R"(
        push ui32 #5
        conv ui64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(5));
}

TEST(CONV, UI32_TO_F32) {
    const auto assembly = R"(
        push ui32 #5
        conv f32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 5.0f);
}

TEST(CONV, UI32_TO_F64) {
    const auto assembly = R"(
        push ui32 #5
        conv f64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F64, 5.0);
}

TEST(CONV, UI32_TO_PTR) {
    const auto assembly = R"(
        push ui32 #5
        conv ptr
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::PTR, uint32_t(5));
}

TEST(CONV, I64_TO_I32) {
    const auto assembly = R"(
        push i64 #5
        conv i32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(CONV, I64_TO_I32_WRAP) {
    const auto assembly = R"(
        push i64 #2147483648
        conv i32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(-2147483648));
}

TEST(CONV, I64_TO_UI32) {
    const auto assembly = R"(
        push i64 #5
        conv ui32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(5));
}

TEST(CONV, I64_TO_UI32_WRAP) {
    const auto assembly = R"(
        push i64 #-1
        conv ui32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(4294967295));
}

TEST(CONV, I64_TO_I64) {
    const auto assembly = R"(
        push i64 #5
        conv i64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(5));
}

TEST(CONV, I64_TO_UI64) {
    const auto assembly = R"(
        push i64 #5
        conv ui64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(5));
}

TEST(CONV, I64_TO_UI64_WRAP) {
    const auto assembly = R"(
        push i64 #-1
        conv ui64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(18446744073709551615));
}

TEST(CONV, I64_TO_F32) {
    const auto assembly = R"(
        push i64 #5
        conv f32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 5.0f);
}

TEST(CONV, I64_TO_F64) {
    const auto assembly = R"(
        push i64 #5
        conv f64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F64, 5.0);
}

TEST(CONV, I64_TO_PTR) {
    const auto assembly = R"(
        push i64 #5
        conv ptr
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::PTR, uint32_t(5));
}

TEST(CONV, UI64_TO_I32) {
    const auto assembly = R"(
        push ui64 #5
        conv i32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(CONV, UI64_TO_I32_WRAP) {
    const auto assembly = R"(
        push ui64 #18446744073709551615
        conv i32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(-1));
}

TEST(CONV, UI64_TO_UI32) {
    const auto assembly = R"(
        push ui64 #5
        conv ui32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(5));
}

TEST(CONV, UI64_TO_UI32_WRAP) {
    const auto assembly = R"(
        push ui64 #18446744073709551615
        conv ui32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(-1));
}

TEST(CONV, UI64_TO_I64) {
    const auto assembly = R"(
        push ui64 #5
        conv i64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(5));
}

TEST(CONV, UI64_TO_I64_WRAP) {
    const auto assembly = R"(
        push ui64 #18446744073709551615
        conv i64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(-1));
}

TEST(CONV, UI64_TO_UI64) {
    const auto assembly = R"(
        push ui64 #5
        conv ui64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(5));
}

TEST(CONV, UI64_TO_F32) {
    const auto assembly = R"(
        push ui64 #5
        conv f32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 5.0f);
}

TEST(CONV, UI64_TO_F64) {
    const auto assembly = R"(
        push ui64 #5
        conv f64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F64, 5.0);
}

TEST(CONV, UI64_TO_PTR) {
    const auto assembly = R"(
        push ui64 #5
        conv ptr
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::PTR, uint32_t(5));
}

TEST(CONV, F32_TO_I32) {
    const auto assembly = R"(
        push f32 #5.0
        conv i32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(CONV, F32_TO_I32_TRUNCATE) {
    const auto assembly = R"(
        push f32 #5.9
        conv i32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(CONV, F32_TO_UI32) {
    const auto assembly = R"(
        push f32 #5.0
        conv ui32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(5));
}

TEST(CONV, F32_TO_I64) {
    const auto assembly = R"(
        push f32 #5.0
        conv i64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(5));
}

TEST(CONV, F32_TO_UI64) {
    const auto assembly = R"(
        push f32 #5.0
        conv ui64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(5));
}

TEST(CONV, F32_TO_F32) {
    const auto assembly = R"(
        push f32 #5.0
        conv f32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 5.0f);
}

TEST(CONV, F32_TO_F64) {
    const auto assembly = R"(
        push f32 #5.0
        conv f64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F64, 5.0);
}

TEST(CONV, F64_TO_I32) {
    const auto assembly = R"(
        push f64 #5.0
        conv i32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(CONV, F64_TO_I32_TRUNCATE) {
    const auto assembly = R"(
        push f64 #5.9
        conv i32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(CONV, F64_TO_I32_TRUNCATE_NEGATIVE) {
    const auto assembly = R"(
        push f64 #-5.9
        conv i32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(-5));
}

TEST(CONV, F64_TO_UI32) {
    const auto assembly = R"(
        push f64 #5.0
        conv ui32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(5));
}

TEST(CONV, F64_TO_I64) {
    const auto assembly = R"(
        push f64 #5.0
        conv i64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(5));
}

TEST(CONV, F64_TO_UI64) {
    const auto assembly = R"(
        push f64 #5.0
        conv ui64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(5));
}

TEST(CONV, F64_TO_F32) {
    const auto assembly = R"(
        push f64 #5.0
        conv f32
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 5.0f);
}

TEST(CONV, F64_TO_F64) {
    const auto assembly = R"(
        push f64 #5.0
        conv f64
        halt
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F64, 5.0);
}

TEST(CONV, PTR_TO_I32) {
    const auto assembly = R"(
        push ptr $x
        conv i32
        halt

    .data
    $x: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(10));
}

TEST(CONV, PTR_TO_I32_WRAP) {
    const auto assembly = R"(
        push ptr $x
        push ui32 #4294967277
        add
        dup
        conv i32
        halt

    .data
    $x: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(-1));
}

TEST(CONV, PTR_TO_UI32) {
    const auto assembly = R"(
        push ptr $x
        conv ui32
        halt

    .data
    $x: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(10));
}

TEST(CONV, PTR_TO_UI32_WRAP) {
    const auto assembly = R"(
        push ptr $x
        push ui32 #4294967277
        add
        dup
        conv ui32
        halt

    .data
    $x: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(-1));
}

TEST(CONV, PTR_TO_I64) {
    const auto assembly = R"(
        push ptr $x
        conv i64
        halt

    .data
    $x: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(10));
}

TEST(CONV, PTR_TO_UI64) {
    const auto assembly = R"(
        push ptr $x
        conv ui64
        halt

    .data
    $x: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(10));
}

TEST(CONV, PTR_TO_PTR) {
    const auto assembly = R"(
        push ptr $x
        conv ptr
        halt

    .data
    $x: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::PTR, uint32_t(10));
}


TEST(CONV, INVALID_I32_TO_PTR_NEGATIVE) {
    const auto assembly = R"(
        push i32 #-1
        conv ptr
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CONV, INVALID_I64_TO_PTR_NEGATIVE) {
    const auto assembly = R"(
        push i64 #-1
        conv ptr
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CONV, INVALID_I64_TO_PTR_OUT_OF_RANGE) {
    const auto assembly = R"(
        push i64 #4294967300
        conv ptr
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CONV, INVALID_UI64_TO_PTR_OUT_OF_RANGE) {
    const auto assembly = R"(
        push ui64 #4294967300
        conv ptr
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CONV, INVALID_F32_TO_PTR) {
    const auto assembly = R"(
        push f32 #5.0
        conv ptr
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CONV, INVALID_F64_TO_UI64_OUT_OF_RANGE) {
    const auto assembly = R"(
        push f64 #-5.9
        conv ui64
        halt
    )";
    EXPECT_VM_ERROR(assembly);
    }

TEST(CONV, INVALID_F64_TO_PTR) {
    const auto assembly = R"(
        push f64 #5.0
        conv ptr
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CONV, INVALID_F32_TO_I32_OUT_OF_RANGE) {
    const auto assembly = R"(
        push f32 #2147483648.0
        conv i32
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CONV, INVALID_F32_TO_UI32_OUT_OF_RANGE) {
    const auto assembly = R"(
        push f32 #-5.9
        conv ui32
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CONV, INVALID_F32_TO_UI64_OUT_OF_RANGE) {
    const auto assembly = R"(
        push f32 #-5.9
        conv ui64
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CONV, INVALID_F64_TO_I32_OUT_OF_RANGE) {
    const auto assembly = R"(
        push f32 #2147483648.0
        conv i32
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CONV, INVALID_F64_TO_UI32_OUT_OF_RANGE) {
    const auto assembly = R"(
        push f64 #-5.9
        conv ui32
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CONV, INVALID_PTR_TO_F32) {
    const auto assembly = R"(
        push ptr $x
        conv f32
        halt

    .data
    $x: i32 5
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CONV, INVALID_PTR_TO_F64) {
    const auto assembly = R"(
        push ptr $x
        conv f32
        halt

    .data
    $x: i32 5
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(CONV, INVALID_UNDERFLOW) {
    const auto assembly = R"(
        conv i32
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}
