#include <gtest/gtest.h>
#include "AssemblerTestsUtils.h"

using namespace assemblerTests;


TEST(DIRECTIVE, INVALID_DATA_DUPLICATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .data
            .data
        )"
    );
}

TEST(DIRECTIVE, INVALID_DATA_LOCATION) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .metadata
            .data
        )"
    );
}

TEST(DIRECTIVE, INVALID_METADATA_DUPLICATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .metadata
            .metadata
        )"
    );
}

TEST(DIRECTIVE, INVALID_METADATA_LOCATION) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .metadata
            .sources
            .metadata
        )"
    );
}

TEST(DIRECTIVE, INVALID_SOURCES_DUPLICATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .metadata
            .sources
            .sources
        )"
    );
}

TEST(DIRECTIVE, INVALID_SOURCES_LOCATION) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .sources
        )"
    );
}

TEST(DIRECTIVE, INVALID_FUNCTION_DUPLICATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .metadata
            .sources
            .functions
            .functions
        )"
    );
}

TEST(DIRECTIVE, INVALID_FUNCTION_LOCATION_ONE) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .metadata
            .functions
        )"
    );
}

TEST(DIRECTIVE, INVALID_FUNCTION_LOCATION_TWO) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .functions
        )"
    );
}

TEST(DIRECTIVE, INVALID_LINE_TABLE_DUPLICATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .metadata
            .sources
            .functions
            .line_table
            .line_table
        )"
    );
}

TEST(DIRECTIVE, INVALID_LINE_TABLE_LOCATION_ONE) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .metadata
            .sources
            .line_table
        )"
    );
}

TEST(DIRECTIVE, INVALID_LINE_TABLE_LOCATION_TWO) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .metadata
            .line_table
        )"
    );
}

TEST(DIRECTIVE, INVALID_LINE_TABLE_LOCATION_THREE) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .line_table
        )"
    );
}

TEST(DIRECTIVE, INVALID_DATA_IN_CODE_SECTION) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            $x: i32 5
        )"
    );
}

TEST(DIRECTIVE, INVALID_SOURCE_IN_DATA_SECTION) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .data
            0 "test.sv"
        )"
    );
}

TEST(DIRECTIVE, INVALID_FUNCTION_IN_SOURCE_SECTION) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .data
            .metadata
            .source
            0x00000000 0x00000000 0 "foo"
        )"
    );
}

TEST(DIRECTIVE, INVALID_LINE_TABLE_IN_FUNCTION_SECTION) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .data
            .metadata
            .source
            .function
            0x00000000 0x00000000 0 0 0
        )"
    );
}

TEST(DIRECTIVE, INVALID_CODE_IN_LINE_TABLE_SECTION) {
    ASSERT_THROWS_ASSEMBLER_ERROR(
        R"(
            .data
            .metadata
            .source
            .function
            .line_table
            pop
        )"
    );
}



