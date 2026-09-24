#include <gtest/gtest.h>
#include "AssemblerTestsUtils.h"

using namespace assemblerTests;

TEST(METADATA, ENCODING) {
    ASSERT_BYTECODE_EQ(
        R"(
halt

def $foo():
        args 0
        locals 0

        ret

.data

.metadata

    .sources
            0    "examples\test.sv"

    .functions
        ;     start          end       source    name
        0x00000001    0x00000007        0        "foo"

    .line_table
        ;     start          end       source       line       column
        0x00000001    0x00000006        0             1        1
        0x00000006    0x00000007        0             1       13

        )",
        std::vector<uint8_t>{
            0x1a, // header -> data
            0x00,
            0x00,
            0x00,
            0x1a, // header -> sources
            0x00,
            0x00,
            0x00,
            0x30, // header -> functions
            0x00,
            0x00,
            0x00,
            0x41, // header -> line_table
            0x00,
            0x00,
            0x00,
            0x61, // header -> end
            0x00,
            0x00,
            0x00,
            0x01, // halt
            0x00, // args 0
            0x00, // locals 0
            0x00,
            0x00,
            0x00,
            0x15, // ret
            0x00, // source id
            0x00,
            0x10, // length of "examples\test.sv"
            0x00,
            0x00,
            0x00,
            0x65, // 'e'
            0x78, // 'x'
            0x61, // 'a'
            0x6d, // 'm'
            0x70, // 'p'
            0x6c, // 'l'
            0x65, // 'e'
            0x73, // 's'
            0x5c, // '\'
            0x74, // 't'
            0x65, // 'e'
            0x73, // 's'
            0x74, // 't'
            0x2e, // '.'
            0x73, // 's'
            0x76, // 'v'
            0x01, // function start address
            0x00,
            0x00,
            0x00,
            0x07, // function end address
            0x00,
            0x00,
            0x00,
            0x00, // function source
            0x00,
            0x03, // length of 'foo'
            0x00,
            0x00,
            0x00,
            0x66, // 'f'
            0x6f, // 'o'
            0x6f, // 'o'
            0x01, // line_table start address
            0x00,
            0x00,
            0x00,
            0x06, // line_table end address
            0x00,
            0x00,
            0x00,
            0x00, // line_table source
            0x00,
            0x01, // line table line number
            0x00,
            0x00,
            0x00,
            0x01, // line table column number
            0x00,
            0x06, // line_table start address
            0x00,
            0x00,
            0x00,
            0x07, // line_table end address
            0x00,
            0x00,
            0x00,
            0x00, // line_table source
            0x00,
            0x01, // line table line number
            0x00,
            0x00,
            0x00,
            0x0d, // line table column number
            0x00
        }
    );
}

TEST(METADATA, INVALID_SOURCE_MISSING_ID) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .metadata
            .sources "f"
        )"
    );
}

TEST(METADATA, INVALID_SOURCE_MISSING_FILE) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .metadata
            .sources 0
        )"
    );
}

TEST(METADATA, INVALID_FUNCTION_MISSING_ADDRESS) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .metadata
            .sources
            .functions 0x00000000 0 "f"
        )"
    );
}

TEST(METADATA, INVALID_FUNCTION_MISSING_SOURCE) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .metadata
            .sources
            .functions 0x00000000 0x00000000 "f"
        )"
    );
}

TEST(METADATA, INVALID_FUNCTION_MISSING_NAME) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .metadata
            .sources
            .functions 0x00000000 0x00000000 0
        )"
    );
}

TEST(METADATA, INVALID_lINE_TABLE_MISSING_ADDRESS) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .metadata
            .sources
            .functions
            .line_table 0x00000000 0 0 0
        )"
    );
}

TEST(METADATA, INVALID_lINE_TABLE_MISSING_SOURCE) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .metadata
            .sources
            .functions
            .line_table 0x00000000 0x00000000 0 0
        )"
    );
}