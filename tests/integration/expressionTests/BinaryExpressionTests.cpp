#include <gtest/gtest.h>
#include "../IntegrationTestUtils.h"

using namespace integrationTests;

TEST(EXPR_BINARY, PLUS_INT_INT) {
    ASSERT_OUTPUT_EQ(
        "print(1 + 2);",
        "3"
    );
}

TEST(EXPR_BINARY, PLUS_INT_FLOAT) {
    ASSERT_OUTPUT_EQ(
        "print(1 + 2.5f);",
        "3.5"
    );
}

TEST(EXPR_BINARY, PLUS_FLOAT_INT) {
    ASSERT_OUTPUT_EQ(
        "print(2.5f + 1);",
        "3.5"
    );
}

TEST(EXPR_BINARY, PLUS_FLOAT_FLOAT) {
    ASSERT_OUTPUT_EQ(
        "print(1.5f + 2.5f);",
        "4.0"
    );
}

TEST(EXPR_BINARY, PLUS_INT_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
        "print(-1 + -2);",
        "-3"
    );
}

TEST(EXPR_BINARY, MINUS_INT_INT) {
    ASSERT_OUTPUT_EQ(
        "print(1 - 2);",
        "-1"
    );
}

TEST(EXPR_BINARY, MINUS_INT_FLOAT) {
    ASSERT_OUTPUT_EQ(
        "print(1 - 2.5f);",
        "-1.5"
    );
}

TEST(EXPR_BINARY, MINUS_FLOAT_INT) {
    ASSERT_OUTPUT_EQ(
        "print(2.5f - 1);",
        "1.5"
    );
}

TEST(EXPR_BINARY, MINUS_FLOAT_FLOAT) {
    ASSERT_OUTPUT_EQ(
        "print(1.5f - 2.5f);",
        "-1.0"
    );
}

TEST(EXPR_BINARY, MINUS_INT_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
        "print(-1 - -2);",
        "1"
    );
}

TEST(EXPR_BINARY, MULTIPLY_INT_INT) {
    ASSERT_OUTPUT_EQ(
        "print(1 * 2);",
        "2"
    );
}

TEST(EXPR_BINARY, MULTIPLY_INT_FLOAT) {
    ASSERT_OUTPUT_EQ(
        "print(1 * 2.5f);",
        "2.5"
    );
}

TEST(EXPR_BINARY, MULTIPLY_FLOAT_INT) {
    ASSERT_OUTPUT_EQ(
        "print(2.5f * 1);",
        "2.5"
    );
}

TEST(EXPR_BINARY, MULTIPLY_FLOAT_FLOAT) {
    ASSERT_OUTPUT_EQ(
        "print(1.5f * 2.5f);",
        "3.75"
    );
}

TEST(EXPR_BINARY, MUTLIPLY_INT_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
        "print(-1 * -2);",
        "2"
    );
}

TEST(EXPR_BINARY, DIVIDE_INT_INT) {
    ASSERT_OUTPUT_EQ(
        "print(2 / 2);",
        "1"
    );
}

TEST(EXPR_BINARY, DIVIDE_INT_FLOAT) {
    ASSERT_OUTPUT_EQ(
        "print(5.0f / 2.5f);",
        "2.0"
    );
}

TEST(EXPR_BINARY, DIVIDE_FLOAT_INT) {
    ASSERT_OUTPUT_EQ(
        "print(2.5f / 1);",
        "2.5"
    );
}

TEST(EXPR_BINARY, DIVIDE_FLOAT_FLOAT) {
    ASSERT_OUTPUT_EQ(
        "print(3.75f / 1.5f);",
        "2.5"
    );
}

TEST(EXPR_BINARY, DIVIDE_INT_NEGATIVE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(-1 / -2);",
        "1.5"
    );
}

TEST(EXPR_BINARY, DIVIDE_INT_NEGATIVE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(1 / -2);",
        "-1.5"
    );
}

TEST(EXPR_BINARY, DIVIDE_INT_NEGATIVE_THREE) {
    ASSERT_OUTPUT_EQ(
        "print(-1 / 2);",
        "-1.5"
    );
}

TEST(EXPR_BINARY, INTEGER_DIVIDE_INT_INT) {
    ASSERT_OUTPUT_EQ(
        "print(4 // 2);",
        "2"
    );
}

TEST(EXPR_BINARY, INTEGER_DIVIDE_INT_FLOAT) {
    ASSERT_OUTPUT_EQ(
        "print(4 // 2.5f);",
        "1"
    );
}

TEST(EXPR_BINARY, INTEGER_DIVIDE_FLOAT_INT) {
    ASSERT_OUTPUT_EQ(
        "print(4.5f // 2);",
        "2"
    );
}

TEST(EXPR_BINARY, INTEGER_DIVIDE_FLOAT_FLOAT) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f // 5.2f);",
        "1"
    );
}

TEST(EXPR_BINARY, INTEGER_DIVIDE_INT_NEGATIVE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(-4 // 2);",
        "-2"
    );
}

TEST(EXPR_BINARY, INTEGER_DIVIDE_INT_NEGATIVE_TW) {
    ASSERT_OUTPUT_EQ(
        "print(4 // -2);",
        "-2"
    );
}

TEST(EXPR_BINARY, INTEGER_DIVIDE_INT_NEGATIVE_THREE) {
    ASSERT_OUTPUT_EQ(
        "print(-4 // -2);",
        "2"
    );
}

TEST(EXPR_BINARY, MODULO_INT_INT) {
    ASSERT_OUTPUT_EQ(
        "print(10 % 3);",
        "1"
    );
}

TEST(EXPR_BINARY, MODULO_INT_FLOAT) {
    ASSERT_OUTPUT_EQ(
        "print(5 % 2.5f);",
        "0"
    );
}

TEST(EXPR_BINARY, MODULO_INT_NEGATIVE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(-5 % 2);",
        "-1"
    );
}

TEST(EXPR_BINARY, MODULO_INT_NEGATIVE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5 % -2);",
        "1"
    );
}

TEST(EXPR_BINARY, MODULO_INT_NEGATIVE_THREE) {
    ASSERT_OUTPUT_EQ(
        "print(-5 % -2);",
        "-1"
    );
}

TEST(EXPR_BINARY, LOGICAL_OR_FALSE_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(false || false);",
        "false"
    );
}

TEST(EXPR_BINARY, LOGICAL_OR_TRUE_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(true || false);",
        "true"
    );
}

TEST(EXPR_BINARY, LOGICAL_OR_FALSE_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(false || true);",
        "true"
    );
}

TEST(EXPR_BINARY, LOGICAL_OR_TRUE_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(true || true);",
        "true"
    );
}

TEST(EXPR_BINARY, LOGICAL_AND_FALSE_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(false && false);",
        "false"
    );
}

TEST(EXPR_BINARY, LOGICAL_AND_TRUE_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(true && false);",
        "false"
    );
}

TEST(EXPR_BINARY, LOGICAL_AND_FALSE_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(false && true);",
        "false"
    );
}

TEST(EXPR_BINARY, LOGICAL_AND_TRUE_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(true && true);",
        "true"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_INT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5 == 5);",
        "true"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_INT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5 == -5);",
        "false"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_FLOAT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f == 5.5f);",
        "true"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_FLOAT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f == 5.25f);",
        "false"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_INT_FLOAT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5 == 5.0f);",
        "true"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_FLOAT_INT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f == 5);",
        "false"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_BOOL_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(true == true);",
        "true"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_BOOL_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(true == false);",
        "false"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_INT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5 != -5);",
        "true"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_INT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5 != 5);",
        "false"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_FLOAT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f != 5.25f);",
        "true"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_FLOAT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f != 5.5f);",
        "false"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_INT_FLOAT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5 != 5.5f);",
        "true"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_FLOAT_INT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5.0f != 5);",
        "false"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_BOOL_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(true != false);",
        "true"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_BOOL_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(true != true);",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5 < 10);",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_FALSE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5 < 5);",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_FALSE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(10 < 5);",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_FLOAT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f < 10.5f);",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_FLOAT_FALSE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f < 5.5f);",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_FLOAT_FALSE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(10.5f < 5.5f);",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_FLOAT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5 < 5.1f);",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_FLOAT_INT_FALSE) {
    ASSERT_OUTPUT_EQ("print(5.5f < 5);",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_INT_TRUE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5 <= 10);",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_INT_TRUE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5 <= 5);",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_INT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(10 <= 5);",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_FLOAT_TRUE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f <= 10.5f);",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_FLOAT_TRUE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f <= 5.5f);",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_FLOAT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(10.5f <= 5.5f);",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_INT_FLOAT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5 <= 5.1f);",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_FLOAT_INT_FALSE) {
    ASSERT_OUTPUT_EQ("print(5.5f <= 5);",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(10 > 5);",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_FALSE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5 > 5);",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_FALSE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5 > 10);",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_FLOAT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(10.5f > 5.5f);",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_FLOAT_FALSE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f > 5.5f);",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_FLOAT_FALSE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f > 10.5f);",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_FLOAT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5 > 4.1f);",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_FLOAT_INT_FALSE) {
    ASSERT_OUTPUT_EQ("print(4.5f > 5);",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_INT_TRUE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(10 >= 5);",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_INT_TRUE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5 >= 5);",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_INT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5 >= 10);",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_FLOAT_TRUE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(10.5f >= 5.5f);",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_FLOAT_TRUE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f >= 5.5f);",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_FLOAT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f >= 10.5f);",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_INT_FLOAT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5 >= 4.1f);",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_FLOAT_INT_FALSE) {
    ASSERT_OUTPUT_EQ("print(4.5f >= 5);",
        "false"
    );
}