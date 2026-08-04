#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(LOADL_STOREL, I32_LOCAL) {
    const auto assembly = R"(
        call $method
        halt

    def $method:
        args    0
        locals  1

        push i32 #5
        storeL #-1
        loadL i32 #-1

        ret
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(LOADL_STOREL, UI32_LOCAL) {
    const auto assembly = R"(
        call $method
        halt

    def $method:
        args    0
        locals  1

        push ui32 #5
        storeL #-1
        loadL ui32 #-1

        ret
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(5));
}

TEST(LOADL_STOREL, I64_LOCAL) {
    const auto assembly = R"(
        call $method
        halt

    def $method:
        args    0
        locals  1

        push i64 #5
        storeL #-1
        loadL i64 #-1

        ret
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(5));
}

TEST(LOADL_STOREL, UI64_LOCAL) {
    const auto assembly = R"(
        call $method
        halt

    def $method:
        args    0
        locals  1

        push ui64 #5
        storeL #-1
        loadL ui64 #-1

        ret
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(5));
}

TEST(LOADL_STOREL, F32_LOCAL) {
    const auto assembly = R"(
        call $method
        halt

    def $method:
        args    0
        locals  1

        push f32 #5.0
        storeL #-1
        loadL f32 #-1

        ret
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 5.0f);
}

TEST(LOADL_STOREL, F64_LOCAL) {
    const auto assembly = R"(
        call $method
        halt

    def $method:
        args    0
        locals  1

        push f64 #5.0
        storeL #-1
        loadL f64 #-1

        ret
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F64, 5.0);
}

TEST(LOADL_STOREL, PTR_LOCAL) {
    const auto assembly = R"(
        call $method
        halt

    def $method:
        args    0
        locals  1

        push ptr $x
        storeL #-1
        loadL ptr #-1

        ret

    .data
    $x: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::PTR, uint32_t(30));
}

TEST(LOADL_STOREL, I32_ARG) {
    const auto assembly = R"(
        push i32 #0
        call $method
        halt

    def $method:
        args    1
        locals  0

        push i32 #5
        storeL #1
        loadL i32 #1

        ret
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I32, int32_t(5));
}

TEST(LOADL_STOREL, UI32_ARG) {
    const auto assembly = R"(
        push ui32 #0
        call $method
        halt

    def $method:
        args    1
        locals  0

        push ui32 #5
        storeL #1
        loadL ui32 #1

        ret
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI32, uint32_t(5));
}

TEST(LOADL_STOREL, I64_ARG) {
    const auto assembly = R"(
        push i64 #0
        call $method
        halt

    def $method:
        args    1
        locals  0

        push i64 #5
        storeL #1
        loadL i64 #1

        ret
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::I64, int64_t(5));
}

TEST(LOADL_STOREL, UI64_ARG) {
    const auto assembly = R"(
        push ui64 #0
        call $method
        halt

    def $method:
        args    1
        locals  0

        push ui64 #5
        storeL #1
        loadL ui64 #1

        ret
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::UI64, uint64_t(5));
}

TEST(LOADL_STOREL, F32_ARG) {
    const auto assembly = R"(
        push f32 #0.0
        call $method
        halt

    def $method:
        args    1
        locals  0

        push f32 #5.0
        storeL #1
        loadL f32 #1

        ret
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F32, 5.0f);
}

TEST(LOADL_STOREL, F64_ARG) {
    const auto assembly = R"(
        push f64 #0.0
        call $method
        halt

    def $method:
        args    1
        locals  0

        push f64 #5.0
        storeL #1
        loadL f64 #1

        ret
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::F64, 5.0);
}

TEST(LOADL_STOREL, PTR_ARG) {
    const auto assembly = R"(
        push ptr $y
        call $method
        halt

    def $method:
        args    1
        locals  0

        push ptr $x
        storeL #1
        loadL ptr #1

        ret

    .data
    $x: i32 5
    $y: i32 5
    )";
    EXPECT_OPERAND_VM_STACK_EQ(assembly, ISA::Type::PTR, uint32_t(36));
}

TEST(LOADL_STOREL, INVALID_LOADL_OUT_OF_BOUNDS_LOCAL) {
    const auto assembly = R"(
        call $method
        halt

    def $method:
        args    0
        locals  1

        loadL i32 #-2

        ret
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(LOADL_STOREL, INVALID_LOADL_OUT_OF_BOUNDS_ARG) {
    const auto assembly = R"(
        push i32 #5
        call $method
        halt

    def $method:
        args    1
        locals  0

        loadL i32 #2

        ret
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(LOADL_STOREL, INVALID_STOREL_OUT_OF_BOUNDS_LOCAL) {
    const auto assembly = R"(
        call $method
        halt

    def $method:
        args    0
        locals  1

        push i32 #5
        storeL #-2

        ret
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(LOADL_STOREL, INVALID_STOREL_OUT_OF_BOUNDS_ARG) {
    const auto assembly = R"(
        push i32 #5
        call $method
        halt

    def $method:
        args    1
        locals  0

        push i32 #5
        storeL #2

        ret
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(LOADL_STOREL, INVALID_STOREL_UNDERFLOW) {
    const auto assembly = R"(
        call $method
        halt

    def $method:
        args    0
        locals  1

        storeL #1

        ret
    )";
    EXPECT_VM_ERROR(assembly);
}
