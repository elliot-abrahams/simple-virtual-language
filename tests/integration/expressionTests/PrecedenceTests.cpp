#include <gtest/gtest.h>
#include "../IntegrationTestUtils.h"

using namespace integrationTests;

TEST(EXPR_PRECEDENCE, LOGICAL_AND_BEFORE_LOGICAL_OR_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(false && false || true);",
        "true"
    );
}

TEST(EXPR_PRECEDENCE, LOGICAL_AND_BEFORE_LOGICAL_OR_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(true || false && false);",
        "true"
    );
}

TEST(EXPR_PRECEDENCE, EQUALITY_BEFORE_LOGICAL_AND_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(false == false && false);",
        "false"
    );
}

TEST(EXPR_PRECEDENCE, EQUALITY_BEFORE_LOGICAL_AND_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(false && false == false);",
        "false"
    );
}

TEST(EXPR_PRECEDENCE, COMPARISON_BEFORE_EQUALITY_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5 > 2 == false);",
        "false"
    );
}

TEST(EXPR_PRECEDENCE, COMPARISON_BEFORE_EQUALITY_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(false == 2 > 5);",
        "true"
    );
}

TEST(EXPR_PRECEDENCE, ADDITIVE_BEFORE_COMPARISON_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5 + 8 > 8);",
        "true"
    );
}

TEST(EXPR_PRECEDENCE, ADDITIVE_BEFORE_COMPARISON_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5 > 8 + 8);",
        "false"
    );
}

TEST(EXPR_PRECEDENCE, MULTIPLICATIVE_BEFORE_ADDITIVE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(3 * 2 + 4);",
        "10"
    );
}

TEST(EXPR_PRECEDENCE, MULTIPLICATIVE_BEFORE_ADDITIVE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(3 + 2 * 4);",
        "11"
    );
}

TEST(EXPR_PRECEDENCE, UNARY_BEFORE_MULTIPLICATIVE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(-1 * 2);",
        "-2"
    );
}

TEST(EXPR_PRECEDENCE, UNARY_BEFORE_MULTIPLICATIVE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(1 * -2);",
        "-2"
    );
}

TEST(EXPR_PRECEDENCE, PAREN) {
    ASSERT_OUTPUT_EQ(
        "print((5 + 5) * 2);",
        "20"
    );
}