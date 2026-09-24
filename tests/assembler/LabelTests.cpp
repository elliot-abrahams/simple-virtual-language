#include <gtest/gtest.h>
#include "AssemblerTestsUtils.h"

using namespace assemblerTests;

TEST(LABEL, VALID_LABEL_NAMES) {
    ASSERT_NO_ASSEMBLER_ERROR(
        R"(
            $label:
            $_label:
            $My2Label____123:
            $label()[,]],:
        )"
    );
}

TEST(LABEL, VALID_CODE_LABEL) {
    ASSERT_NO_ASSEMBLER_ERROR(
        R"(
            jmp $label

            $label:
        )"
    );
}

TEST(LABEL, VALID_METHOD_LABEL) {
    ASSERT_NO_ASSEMBLER_ERROR(
        R"(
            call $method

            def $method: args 0 locals 0
        )"
    );
}

TEST(LABEL, VALID_DATA_LABEL) {
    ASSERT_NO_ASSEMBLER_ERROR(
        R"(
            loadG $x

            .data
            $x: i32 5
        )"
    );
}

TEST(LABEL, VALID_BACKWARD_REFERENCE) {
    ASSERT_NO_ASSEMBLER_ERROR(
        R"(
            $label:

            jmp $label
        )"
    );
}

TEST(LABEL, VALID_DUPLICATE_LABELS_OF_DIFFERENT_TYPE) {
    ASSERT_NO_ASSEMBLER_ERROR(
        R"(
            $x:

            def $x: args 0 locals 0

            .data
            $x: i32 5
        )"
    );
}

TEST(LABEL, INVALID_LABEL_NAME_NUMBER) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$1label:");
}

TEST(LABEL, INVALID_LABEL_NAME_HYPHEN) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$-label:");
}

TEST(LABEL, INVALID_LABEL_NAME_DOT) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$.label:");
}

TEST(LABEL, INVALID_LABEL_NAME_SPACE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$ label:");
}

TEST(LABEL, INVALID_LABEL_MISSING_COLON) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label");
}

TEST(LABEL, INVALID_CODE_LABEL_USE) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            call $label

            $label:
        )"
    );
}

TEST(LABEL, INVALID_METHOD_LABEL_USE) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            loadG $method

            def $method: args 0 locals 0
        )"
    );
}

TEST(LABEL, INVALID_DATA_LABEL_USE) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            jmp $x

            .data
            $x: i32 5
        )"
    );
}

TEST(LABEL, INVALID_UNDEFINED_LABEL) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            jmp $x
        )"
    );
}

TEST(LABEL, INVALID_DUPLICATE_CODE_LABELS) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$x: $x:");
}

TEST(LABEL, INVALID_DUPLICATE_METHOD_LABELS) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            def $x: args 0 locals 0
            def $x: args 0 locals 0
        )"
    );
}

TEST(LABEL, INVALID_DUPLICATE_DATA_LABELS) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .data
            $x: i32 #5
            $x: i32 #5
        )"
    );
}