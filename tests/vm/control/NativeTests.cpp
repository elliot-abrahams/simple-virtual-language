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
