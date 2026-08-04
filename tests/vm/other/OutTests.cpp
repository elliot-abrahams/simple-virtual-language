#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(OUT, OUTPUT_I32) {
    const auto assembly = R"(
        push i32 #5
        out i32
        halt
    )";
    EXPECT_CONSOLE_OUTPUT(assembly, "5\n");
}

TEST(OUT, OUTPUT_I32_NEGATIVE) {
    const auto assembly = R"(
        push i32 #-5
        out i32
        halt
    )";
    EXPECT_CONSOLE_OUTPUT(assembly, "-5\n");
}

TEST(OUT, OUTPUT_UI32) {
    const auto assembly = R"(
        push ui32 #5
        out ui32
        halt
    )";
    EXPECT_CONSOLE_OUTPUT(assembly, "5\n");
}

TEST(OUT, OUTPUT_I64) {
    const auto assembly = R"(
        push i64 #5
        out i64
        halt
    )";
    EXPECT_CONSOLE_OUTPUT(assembly, "5\n");
}

TEST(OUT, OUTPUT_I64_NEGATIVE) {
    const auto assembly = R"(
        push i64 #-5
        out i64
        halt
    )";
    EXPECT_CONSOLE_OUTPUT(assembly, "-5\n");
}

TEST(OUT, OUTPUT_UI64) {
    const auto assembly = R"(
        push ui64 #5
        out ui64
        halt
    )";
    EXPECT_CONSOLE_OUTPUT(assembly, "5\n");
}

TEST(OUT, OUTPUT_F32) {
    const auto assembly = R"(
        push f32 #5.5
        out f32
        halt
    )";
    EXPECT_CONSOLE_OUTPUT(assembly, "5.5\n");
}

TEST(OUT, OUTPUT_F32_NEGATIVE) {
    const auto assembly = R"(
        push f32 #-5.5
        out f32
        halt
    )";
    EXPECT_CONSOLE_OUTPUT(assembly, "-5.5\n");
}

TEST(OUT, OUTPUT_F64) {
    const auto assembly = R"(
        push f64 #5.5
        out f64
        halt
    )";
    EXPECT_CONSOLE_OUTPUT(assembly, "5.5\n");
}

TEST(OUT, OUTPUT_F64_NEGATIVE) {
    const auto assembly = R"(
        push f64 #-5.5
        out f64
        halt
    )";
    EXPECT_CONSOLE_OUTPUT(assembly, "-5.5\n");
}

TEST(OUT, OUTPUT_STR) {
    const auto assembly = R"(
        push ptr $msg
        out str
        halt

    .data
    $msg: str "Hello"
    )";
    EXPECT_CONSOLE_OUTPUT(assembly, "Hello\n");
}

TEST(OUT, INVALID_OUTPUT_PTR) {
    const auto assembly = R"(
        push ptr $x
        out ptr

    .data
    $x: i32 5
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(OUT, INVALID_OUTPUT_STR) {
    const auto assembly = R"(
        push i32 #5
        out str
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}

TEST(OUT, UNDERFLOW) {
    const auto assembly = R"(
        out i32
        halt
    )";
    EXPECT_VM_ERROR(assembly);
}