#include <gtest/gtest.h>
#include "../IntegrationTestUtils.h"

using namespace integrationTests;

TEST(EXPR_BINARY, INT_ADDITION) {
    ASSERT_OUTPUT_EQ(
        "print(1 + 2);",
        "3"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INT_ADDITION_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(-1 + 2);",
        "1"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INT_ADDITION_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(1 + -2);",
        "-1"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INT_ADDITION_THREE) {
    ASSERT_OUTPUT_EQ(
        "print(-1 + -2);",
        "-3"
    );
}

TEST(EXPR_BINARY, INT_SUBTRACTION) {
    ASSERT_OUTPUT_EQ(
        "print(2 - 1);",
        "1"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INT_SUBTRACTION_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(-2 - 1);",
        "-3"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INT_SUBTRACTION_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(2 - -1);",
        "3"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INT_SUBTRACTION_THREE) {
    ASSERT_OUTPUT_EQ(
        "print(-2 - -1);",
        "-1"
    );
}

TEST(EXPR_BINARY, INT_MULTIPLICATION_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5 * 3);",
        "15"
    );
}

TEST(EXPR_BINARY, INT_MULTIPLICATION_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(3 * 5);",
        "15"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INT_MULTIPLICATION_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(-3 * 5);",
        "-15"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INT_MULTIPLICATION_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(3 * -5);",
        "-15"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INT_MULTIPLICATION_THREE) {
    ASSERT_OUTPUT_EQ(
        "print(-3 * -5);",
        "15"
    );
}

TEST(EXPR_BINARY, INT_DIVISION) {
    ASSERT_OUTPUT_EQ(
        "print(10 / 4);",
        "2.5"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INT_DIVISION_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(-10 / 4);",
        "-2.5"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INT_DIVISION_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(10 / -4);",
        "-2.5"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INT_DIVISION_THREE) {
    ASSERT_OUTPUT_EQ(
        "print(-10 / -4);",
        "2.5"
    );
}

TEST(EXPR_BINARY, FLOAT_DIVISION) {
    ASSERT_OUTPUT_EQ(
        "print(12.5f / 2.5f);",
        "5.0"
    );
}

TEST(EXPR_BINARY, NEGATIVE_FLOAT_DIVISION_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(-12.5f / 2.5f);",
        "-5.0"
    );
}

TEST(EXPR_BINARY, NEGATIVE_FLOAT_DIVISION_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(12.5f / -2.5f);",
        "-5.0"
    );
}

TEST(EXPR_BINARY, NEGATIVE_FLOAT_DIVISION_THREE) {
    ASSERT_OUTPUT_EQ(
        "print(-12.5f / -2.5f);",
        "5.0"
    );
}

TEST(EXPR_BINARY, INT_FLOAT_DIVISION) {
    ASSERT_OUTPUT_EQ(
        "print(20 / 2.5f);",
        "8.0"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INT_FLOAT_DIVISION_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(-20 / 2.5f);",
        "-8.0"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INT_FLOAT_DIVISION_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(20 / -2.5f);",
        "-8.0"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INT_FLOAT_DIVISION_THREE) {
    ASSERT_OUTPUT_EQ(
        "print(-20 / -2.5f);",
        "8.0"
    );
}

TEST(EXPR_BINARY, FLOAT_INT_DIVISION) {
    ASSERT_OUTPUT_EQ(
        "print(12.5f / 10);",
        "1.25"
    );
}

TEST(EXPR_BINARY, NEGATIVE_FLOAT_INT_DIVISION_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(-12.5f / 10);",
        "-1.25"
    );
}

TEST(EXPR_BINARY, NEGATIVE_FLOAT_INT_DIVISION_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(12.5f / -10);",
        "-1.25"
    );
}

TEST(EXPR_BINARY, NEGATIVE_FLOAT_INT_DIVISION_THREE) {
    ASSERT_OUTPUT_EQ(
        "print(-12.5f / -10);",
        "1.25"
    );
}

TEST(EXPR_BINARY, INTEGER_DIVISION) {
    ASSERT_OUTPUT_EQ(
        "print(10 // 5);",
        "2"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INTEGER_DIVISION_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(-10 // 5);",
        "-2"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INTEGER_DIVISION_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(10 // -5);",
        "-2"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INTEGER_DIVISION_THREE) {
    ASSERT_OUTPUT_EQ(
        "print(-10 // -5);",
        "2"
    );
}

TEST(EXPR_BINARY, INT_INT_MODULO_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5 % 2);",
        "1"
    );
}

TEST(EXPR_BINARY, INT_INT_MODULO_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5 % 5);",
        "0"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INT_INT_MODULO_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(-5 % 2);",
        "-1"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INT_INT_MODULO_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5 % -2);",
        "1"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INT_INT_MODULO_THREE) {
    ASSERT_OUTPUT_EQ(
        "print(-5 % -2);",
        "-1"
    );
}

TEST(EXPR_BINARY, FLOAT_INT_MODULO_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f % 2);",
        "1.5"
    );
}

TEST(EXPR_BINARY, FLOAT_INT_MODULO_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f % 5);",
        "0.5"
    );
}

TEST(EXPR_BINARY, NEGATIVE_FLOAT_INT_MODULO_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(-5.5f % 2);",
        "-1.5"
    );
}

TEST(EXPR_BINARY, NEGATIVE_FLOAT_INT_MODULO_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f % -2);",
        "1.5"
    );
}

TEST(EXPR_BINARY, NEGATIVE_FLOAT_INT_MODULO_THREE) {
    ASSERT_OUTPUT_EQ(
        "print(-5.5f % -2);",
        "-1.5"
    );
}

TEST(EXPR_BINARY, INT_FLOAT_MODULO_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5 % 2.5f);",
        "0.0"
    );
}

TEST(EXPR_BINARY, INT_FLOAT_MODULO_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5 % 1.5f);",
        "0.5"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INT_FLOAT_MODULO_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(-5 % 1.5f);",
        "-0.5"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INT_FLOAT_MODULO_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5 % -1.5f);",
        "0.5"
    );
}

TEST(EXPR_BINARY, NEGATIVE_INT_FLOAT_MODULO_THREE) {
    ASSERT_OUTPUT_EQ(
        "print(-5 % -1.5f);",
        "-0.5"
    );
}

TEST(EXPR_BINARY, FLOAT_FLOAT_MODULO_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f % 2.5f);",
        "0.5"
    );
}

TEST(EXPR_BINARY, FLOAT_FLOAT_MODULO_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5.75f % 1.5f);",
        "1.25"
    );
}

TEST(EXPR_BINARY, NEGATIVE_FLOAT_FLOAT_MODULO_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(-5.75f % 1.5f);",
        "-1.25"
    );
}

TEST(EXPR_BINARY, NEGATIVE_FLOAT_FLOAT_MODULO_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5.75f % -1.5f);",
        "1.25"
    );
}

TEST(EXPR_BINARY, NEGATIVE_FLOAT_FLOAT_MODULO_THREE) {
    ASSERT_OUTPUT_EQ(
        "print(-5.75f % -1.5f);",
        "-1.25"
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