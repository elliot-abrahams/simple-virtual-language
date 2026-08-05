#include <gtest/gtest.h>
#include "../IntegrationTestUtils.h"

using namespace integrationTests;

TEST(EXPR_UNARY, PLUS_INT) {
    ASSERT_OUTPUT_EQ(
        "print(+5);",
        "5"
    );
}

TEST(EXPR_UNARY, MINUS_INT) {
    ASSERT_OUTPUT_EQ(
        "print(-5);",
        "-5"
    );
}

TEST(EXPR_UNARY, PLUS_FLOAT) {
    ASSERT_OUTPUT_EQ(
        "print(+5.5f);",
        "5.5"
    );
}

TEST(EXPR_UNARY, MINUS_FLOAT) {
    ASSERT_OUTPUT_EQ(
        "print(-5.0f);",
        "-5.0"
    );
}

TEST(EXPR_UNARY, LOGICAL_NOT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(!true);",
        "false"
    );
}

TEST(EXPR_UNARY, LOGICAL_NOT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(!false);",
        "true"
    );
}