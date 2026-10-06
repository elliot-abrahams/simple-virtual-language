#include <gtest/gtest.h>
#include "../IntegrationTestUtils.h"

using namespace integrationTests;


TEST(EXPR_CAST, INT_TO_INT) {
    ASSERT_OUTPUT_EQ(
        "print((int) 5);",
        "5"
    );
}

TEST(EXPR_CAST, INT_TO_FLOAT) {
    ASSERT_OUTPUT_EQ(
        "print((float) 5);",
        "5.0"
    );
}

TEST(EXPR_CAST, INT_TO_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print((char) 65);",
        "A"
    );
}

TEST(EXPR_CAST, FLOAT_TO_INT) {
    ASSERT_OUTPUT_EQ(
        "print((int) 5.0f);",
        "5"
    );
}

TEST(EXPR_CAST, FLOAT_TO_FLOAT) {
    ASSERT_OUTPUT_EQ(
        "print((float) 5.0f);",
        "5.0"
    );
}

TEST(EXPR_CAST, FLOAT_TO_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print((char) 65.0f);",
        "A"
    );
}

TEST(EXPR_CAST, BOOL_TO_BOOL) {
    ASSERT_OUTPUT_EQ(
        "print((bool) true);",
        "true"
    );
}

TEST(EXPR_CAST, CHAR_TO_INT) {
    ASSERT_OUTPUT_EQ(
        "print((int) 'A');",
        "65"
    );
}

TEST(EXPR_CAST, CHAR_TO_FLOAT) {
    ASSERT_OUTPUT_EQ(
        "print((float) 'A');",
        "65.0"
    );
}

TEST(EXPR_CAST, CHAR_TO_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print((char) 'A');",
        "A"
    );
}