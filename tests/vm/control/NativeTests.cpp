#include <gtest/gtest.h>
#include "../VmTestUtils.h"

TEST(NATIVE, EXIT_I32) {
    EXPECT_EXIT_CODE(
        R"(
            push i32 #5
            native exit
        )",
        5
    );
}

TEST(NATIVE, EXIT_I32_NEGATIVE) {
    EXPECT_EXIT_CODE(
        R"(
            push i32 #-5
            native exit
        )",
        -5
    );
}

TEST(NATIVE, EXIT_UI32) {
    EXPECT_EXIT_CODE(
        R"(
            push ui32 #5
            native exit
        )",
        5
    );
}

TEST(NATIVE, EXIT_I64) {
    EXPECT_EXIT_CODE(
        R"(
            push i64 #5
            native exit
        )",
        5
    );
}

TEST(NATIVE, EXIT_I64_NEGATIVE) {
    EXPECT_EXIT_CODE(
        R"(
            push i64 #-5
            native exit
        )",
        -5
    );
}

TEST(NATIVE, EXIT_UI64) {
    EXPECT_EXIT_CODE(
        R"(
            push ui64 #5
            native exit
        )",
        5
    );
}

TEST(NATIVE, EXIT_INVALID_F32) {
    EXPECT_VM_ERROR(
        R"(
            push f32 #5.0
            native exit
        )"
    );
}

TEST(NATIVE, EXIT_INVALID_F64) {
    EXPECT_VM_ERROR(
        R"(
            push f64 #5.0
            native exit
        )"
    );
}

TEST(NATIVE, EXIT_INVALID_PTR) {
    EXPECT_VM_ERROR(
        R"(
            push ptr $x
            native exit

        .data
        $x: i32 5
        )"
    );
}

TEST(NATIVE, EXIT_INVALID_UNDERFLOW) {
    EXPECT_VM_ERROR("native exit");
}

TEST(NATIVE, PRINT_I32) {
    EXPECT_CONSOLE_OUTPUT(
        R"(
            push i32 #5
            native print
            halt
        )",
        "5"
    );
}

TEST(NATIVE, PRINT_I32_NEGATIVE) {
    EXPECT_CONSOLE_OUTPUT(
        R"(
            push i32 #-5
            native print
            halt
        )",
        "-5"
    );
}

TEST(NATIVE, PRINT_UI32) {
    EXPECT_CONSOLE_OUTPUT(
        R"(
            push ui32 #5
            native print
            halt
        )",
        "5"
    );
}

TEST(NATIVE, PRINT_I64) {
    EXPECT_CONSOLE_OUTPUT(
        R"(
            push i64 #5
            native print
            halt
        )",
        "5"
    );
}

TEST(NATIVE, PRINT_I64_NEGATIVE) {
    EXPECT_CONSOLE_OUTPUT(
        R"(
            push i64 #-5
            native print
            halt
        )",
        "-5"
    );
}

TEST(NATIVE, PRINT_UI64) {
    EXPECT_CONSOLE_OUTPUT(
        R"(
            push ui64 #5
            native print
            halt
        )",
        "5"
    );
}

TEST(NATIVE, PRINT_F32) {
    EXPECT_CONSOLE_OUTPUT(
        R"(
            push f32 #5.5
            native print
            halt
        )",
        "5.5"
    );
}

TEST(NATIVE, PRINT_F32_NEGATIVE) {
    EXPECT_CONSOLE_OUTPUT(
        R"(
            push f32 #-5.5
            native print
            halt
        )",
        "-5.5"
    );
}

TEST(NATIVE, PRINT_F64) {
    EXPECT_CONSOLE_OUTPUT(
        R"(
            push f64 #5.5
            native print
            halt
        )",
        "5.5"
    );
}

TEST(NATIVE, PRINT_F64_NEGATIVE) {
    EXPECT_CONSOLE_OUTPUT(
        R"(
            push f64 #-5.5
            native print
            halt
        )",
        "-5.5"
    );
}

TEST(NATIVE, PRINT_INVALID_PTR) {
    EXPECT_VM_ERROR(
        R"(
            push ptr $x
            native print
            halt

            .data
            $x: i32 5
        )"
    );
}

TEST(NATIVE, PRINT_INVALID_UNDERFLOW) {
    EXPECT_VM_ERROR(
        R"(
            native print
            halt
        )"
    );
}

TEST(NATIVE, PRINT_STR_PTR) {
    EXPECT_CONSOLE_OUTPUT(
        R"(
            push ptr $msg
            native print_str
            halt

            .data
            $msg: str "hello"
        )",
        "hello"
    );
}

TEST(NATIVE, PRINT_STR_PTR_UTF_8) {
    EXPECT_CONSOLE_OUTPUT(
        R"(
            push ptr $msg
            native print_str
            halt

            .data
            $msg: str "hello😀"
        )",
        "hello😀"
    );
}

TEST(NATIVE, PRINT_STR_INVALID_I32) {
    EXPECT_VM_ERROR(
        R"(
            push i32 #5
            native print_str
            halt
        )"
    );
}

TEST(NATIVE, PRINT_STR_INVALID_UI32) {
    EXPECT_VM_ERROR(
        R"(
            push ui32 #5
            native print_str
            halt
        )"
    );
}

TEST(NATIVE, PRINT_STR_INVALID_I64) {
    EXPECT_VM_ERROR(
        R"(
            push i64 #5
            native print_str
            halt
        )"
    );
}

TEST(NATIVE, PRINT_STR_INVALID_UI64) {
    EXPECT_VM_ERROR(
        R"(
            push ui64 #5
            native print_str
            halt
        )"
    );
}

TEST(NATIVE, PRINT_STR_INVALID_F32) {
    EXPECT_VM_ERROR(
        R"(
            push f32 #5.5
            native print_str
            halt
        )"
    );
}

TEST(NATIVE, PRINT_STR_INVALID_F64) {
    EXPECT_VM_ERROR(
        R"(
            push f64 #5.5
            native print_str
            halt
        )"
    );
}

TEST(NATIVE, PRINT_STR_INVALID_UNDERFLOW) {
    EXPECT_VM_ERROR(
        R"(
            native print_str
            halt
        )"
    );
}
