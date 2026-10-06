#include <gtest/gtest.h>
#include "../IntegrationTestUtils.h"

using namespace integrationTests;

TEST(PRINTLN, INT) {
    ASSERT_OUTPUT_EQ(
        "println(5);",
        "5\n"
    );
}

TEST(PRINTLN, INT_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
        "println(-5);",
        "-5\n"
    );
}

TEST(PRINTLN, FLOAT) {
    ASSERT_OUTPUT_EQ(
    "println(5.5f);",
        "5.5\n"
    );
}

TEST(PRINTLN, FLOAT_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
    "println(-5.5f);",
        "-5.5\n"
    );
}

TEST(PRINTLN, BOOL_TRUE) {
    ASSERT_OUTPUT_EQ(
        "println(true);",
        "true\n"
    );
}

TEST(PRINTLN, BOOL_FALSE) {
    ASSERT_OUTPUT_EQ(
        "println(false);",
        "false\n"
    );
}

TEST(PRINTLN, CHAR) {
    ASSERT_OUTPUT_EQ(
        "println('a');",
        "a\n"
    );
}

TEST(PRINTLN, CHAR_ESCAPE_SEQUENCE) {
    ASSERT_OUTPUT_EQ(
        "println('\\t');",
        "\t\n"
    );
}