#include <gtest/gtest.h>
#include "AssemblerTestsUtils.h"

using namespace assemblerTests;

TEST(METHOD, ENCODING) {
        ASSERT_BYTECODE_EQ(
    R"(
            def $foo:
                args 1
                locals 16
        )",
        std::vector<uint8_t>{
            0x18, // header -> data
            0x00,
            0x00,
            0x00,
            0x18, // header -> sources
            0x00,
            0x00,
            0x00,
            0x18, // header -> functions
            0x00,
            0x00,
            0x00,
            0x18, // header -> line_table
            0x00,
            0x00,
            0x00,
            0x18, // header -> end
            0x00,
            0x00,
            0x00,
            0x01, // args 1
            0x10, // locals 16
            0x00,
            0x00,
            0x00
        }
    );
}

TEST(METHOD, VALID_ARGS_AFTER_LOCALS) {
    ASSERT_NO_ASSEMBLER_ERROR(
        R"(
            def $foo:
                locals 0
                args 0
        )"
    );
}

TEST(METHOD, VALID_ARGS_LOCALS_MIN) {
    ASSERT_NO_ASSEMBLER_ERROR(
        R"(
            def $foo:
                args 0
                locals 0
        )"
    );
}

TEST(METHOD, VALID_ARGS_LOCALS_MAX) {
    ASSERT_NO_ASSEMBLER_ERROR(
        R"(
            def $foo:
                args 255
                locals 4294967295
        )"
    );
}

TEST(METHOD, INVALID_DUPLICATE_ARGS) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            def $foo:
                args 0
                args 0
                locals 0
        )"
    );
}

TEST(METHOD, INVALID_DUPLICATE_LOCALS) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            def $foo:
                args 0
                locals 0
                locals 0
        )"
    );
}

TEST(METHOD, INVALID_ARGS_MIN) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            def $foo:
                args -1
                locals 0
        )"
    );
}

TEST(METHOD, INVALID_ARGS_MAX) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            def $foo:
                args 256
                locals 0
        )"
    );
}

TEST(METHOD, INVALID_ARGS_DECIMAL) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            def $foo:
                args 1.5
                locals 0
        )"
    );
}

TEST(METHOD, INVALID_LOCALS_MIN) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            def $foo:
                args 0
                locals -1
        )"
    );
}

TEST(METHOD, INVALIDLOCALS_MAX) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            def $foo:
                args 0
                locals 4294967296
        )"
    );
}

TEST(METHOD, INVALIDLOCALS_DECIMAL) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            def $foo:
                args 0
                locals 1.5
        )"
    );
}

TEST(METHOD, INVALID_MISSING_LABEL_DEF) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            def
                args 1
                locals 2
        )"
    );
}

TEST(METHOD, INVALID_MISSING_ARGS_KW) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            def $foo
                1
                locals 2
        )"
    );
}

TEST(METHOD, INVALID_MISSING_ARGS) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            def $foo
                args
                locals 2
        )"
    );
}

TEST(METHOD, INVALID_MISSING_LOCALS_KW) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            def $foo
                args 1
                2
        )"
    );
}

TEST(METHOD, INVALID_MISSING_LOCALS) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            def $foo
                args 1
                locals
        )"
    );
}
