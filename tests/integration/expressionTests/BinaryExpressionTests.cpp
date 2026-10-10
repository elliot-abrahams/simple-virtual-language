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

TEST(EXPR_BINARY, PLUS_INT_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print(1 + 'A');",
        "66"
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

TEST(EXPR_BINARY, PLUS_FLOAT_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print(1.5f + 'A');",
        "66.5"
    );
}

TEST(EXPR_BINARY, PLUS_CHAR_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print('A' + 'A');",
        "130"
    );
}

TEST(EXPR_BINARY, PLUS_NEGATIVE) {
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

TEST(EXPR_BINARY, MINUS_INT_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print(1 - 'A');",
        "-64"
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

TEST(EXPR_BINARY, MINUS_FLOAT_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print(1.5f - 'A');",
        "-63.5"
    );
}

TEST(EXPR_BINARY, MINUS_CHAR_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print('C' - 'A');",
        "2"
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

TEST(EXPR_BINARY, MULTIPLY_INT_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print(2 * 'A');",
        "130"
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

TEST(EXPR_BINARY, MULTIPLY_FLOAT_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print(0.5f * 'A');",
        "32.5"
    );
}

TEST(EXPR_BINARY, MULTIPLY_CHAR_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print('!' * '!');",
        "1089"
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
        "1.0"
    );
}

TEST(EXPR_BINARY, DIVIDE_INT_FLOAT) {
    ASSERT_OUTPUT_EQ(
        "print(5 / 2.5f);",
        "2.0"
    );
}

TEST(EXPR_BINARY, DIVIDE_INT_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print(130 / 'A');",
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

TEST(EXPR_BINARY, DIVIDE_FLOAT_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print(130.0f / 'A');",
        "2.0"
    );
}

TEST(EXPR_BINARY, DIVIDE_CHAR_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print('A' / 'A');",
        "1.0"
    );
}

TEST(EXPR_BINARY, DIVIDE_INT_NEGATIVE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(-1 / -2);",
        "0.5"
    );
}

TEST(EXPR_BINARY, DIVIDE_INT_NEGATIVE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(1 / -2);",
        "-0.5"
    );
}

TEST(EXPR_BINARY, DIVIDE_INT_NEGATIVE_THREE) {
    ASSERT_OUTPUT_EQ(
        "print(-1 / 2);",
        "-0.5"
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

TEST(EXPR_BINARY, INTEGER_DIVIDE_INT_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print(130 // 'A');",
        "2"
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

TEST(EXPR_BINARY, INTEGER_DIVIDE_FLOAT_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print(135.0f // 'A');",
        "2"
    );
}

TEST(EXPR_BINARY, INTEGER_DIVIDE_CHAR_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print('A' // 'A');",
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
        "print(5 % 2.25f);",
        "0.5"
    );
}

TEST(EXPR_BINARY, MODULO_INT_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print(131 % 'A');",
        "1"
    );
}

TEST(EXPR_BINARY, MODULO_FLOAT_INT) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f % 1);",
        "0.5"
    );
}

TEST(EXPR_BINARY, MODULO_FLOAT_FLOAT) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f % 1.5f);",
        "1.0"
    );
}

TEST(EXPR_BINARY, MODULO_FLOAT_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print(130.5f % 'A');",
        "0.5"
    );
}

TEST(EXPR_BINARY, MODULO_CHAR_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print('A' % 'A');",
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

TEST(EXPR_BINARY, BITWISE_AND_INT_INT) {
    ASSERT_OUTPUT_EQ(
        "print(12 & 10);",
        "8"
    );
}

TEST(EXPR_BINARY, BITWISE_AND_INT_ZERO) {
    ASSERT_OUTPUT_EQ(
        "print(123 & 0);",
        "0"
    );
}

TEST(EXPR_BINARY, BITWISE_AND_INT_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
        "print(-1 & 10);",
        "10"
    );
}

TEST(EXPR_BINARY, BITWISE_AND_CHAR_INT) {
    ASSERT_OUTPUT_EQ(
        "print('A' & 15);",
        "1"
    );
}

TEST(EXPR_BINARY, BITWISE_AND_INT_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print(65 & 'A');",
        "65"
    );
}

TEST(EXPR_BINARY, BITWISE_AND_CHAR_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print('A' & 'B');",
        "64"
    );
}

TEST(EXPR_BINARY, BITWISE_OR_INT_INT) {
    ASSERT_OUTPUT_EQ(
        "print(12 | 10);",
        "14"
    );
}

TEST(EXPR_BINARY, BITWISE_OR_INT_ZERO) {
    ASSERT_OUTPUT_EQ(
        "print(123 | 0);",
        "123"
    );
}

TEST(EXPR_BINARY, BITWISE_OR_INT_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
        "print(-1 | 10);",
        "-1"
    );
}

TEST(EXPR_BINARY, BITWISE_OR_CHAR_INT) {
    ASSERT_OUTPUT_EQ(
        "print('A' | 32);",
        "97"
    );
}

TEST(EXPR_BINARY, BITWISE_OR_CHAR_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print('A' | 'B');",
        "67"
    );
}

TEST(EXPR_BINARY, BITWISE_XOR_INT_INT) {
    ASSERT_OUTPUT_EQ(
        "print(12 ^ 10);",
        "6"
    );
}

TEST(EXPR_BINARY, BITWISE_XOR_EQUAL_VALUES) {
    ASSERT_OUTPUT_EQ(
        "print(42 ^ 42);",
        "0"
    );
}

TEST(EXPR_BINARY, BITWISE_XOR_INT_ZERO) {
    ASSERT_OUTPUT_EQ(
        "print(123 ^ 0);",
        "123"
    );
}

TEST(EXPR_BINARY, BITWISE_XOR_INT_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
        "print(-1 ^ 10);",
        "-11"
    );
}

TEST(EXPR_BINARY, BITWISE_XOR_CHAR_INT) {
    ASSERT_OUTPUT_EQ(
        "print('A' ^ 1);",
        "64"
    );
}

TEST(EXPR_BINARY, BITWISE_XOR_CHAR_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print('A' ^ 'B');",
        "3"
    );
}

TEST(EXPR_BINARY, LEFT_SHIFT_INT_INT) {
    ASSERT_OUTPUT_EQ(
        "print(1 << 3);",
        "8"
    );
}

TEST(EXPR_BINARY, LEFT_SHIFT_ZERO) {
    ASSERT_OUTPUT_EQ(
        "print(123 << 0);",
        "123"
    );
}

TEST(EXPR_BINARY, LEFT_SHIFT_MULTIPLE_BITS) {
    ASSERT_OUTPUT_EQ(
        "print(5 << 2);",
        "20"
    );
}

TEST(EXPR_BINARY, LEFT_SHIFT_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
        "print(-8 << 2);",
        "-32"
    );
}

TEST(EXPR_BINARY, LEFT_SHIFT_CHAR_INT) {
    ASSERT_OUTPUT_EQ(
        "print('A' << 1);",
        "130"
    );
}

TEST(EXPR_BINARY, LEFT_SHIFT_INT_CHAR) {
    ASSERT_OUTPUT_EQ(
        "print(1 << '\\0');",
        "1"
    );
}

TEST(EXPR_BINARY, ARITHMETIC_RIGHT_SHIFT_INT_INT) {
    ASSERT_OUTPUT_EQ(
        "print(16 >> 2);",
        "4"
    );
}

TEST(EXPR_BINARY, ARITHMETIC_RIGHT_SHIFT_ZERO) {
    ASSERT_OUTPUT_EQ(
        "print(123 >> 0);",
        "123"
    );
}

TEST(EXPR_BINARY, ARITHMETIC_RIGHT_SHIFT_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
        "print(-8 >> 1);",
        "-4"
    );
}

TEST(EXPR_BINARY, ARITHMETIC_RIGHT_SHIFT_NEGATIVE_ROUNDS_DOWN) {
    ASSERT_OUTPUT_EQ(
        "print(-7 >> 1);",
        "-4"
    );
}

TEST(EXPR_BINARY, ARITHMETIC_RIGHT_SHIFT_CHAR_INT) {
    ASSERT_OUTPUT_EQ(
        "print('A' >> 1);",
        "32"
    );
}

TEST(EXPR_BINARY, LOGICAL_RIGHT_SHIFT_INT_INT) {
    ASSERT_OUTPUT_EQ(
        "print(16 >>> 2);",
        "4"
    );
}

TEST(EXPR_BINARY, LOGICAL_RIGHT_SHIFT_ZERO) {
    ASSERT_OUTPUT_EQ(
        "print(123 >>> 0);",
        "123"
    );
}

TEST(EXPR_BINARY, LOGICAL_RIGHT_SHIFT_NEGATIVE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(-1 >>> 1);",
        "2147483647"
    );
}

TEST(EXPR_BINARY, LOGICAL_RIGHT_SHIFT_NEGATIVE_VALUE) {
    ASSERT_OUTPUT_EQ(
        "print(-128 >>> 1);",
        "2147483584"
    );
}

TEST(EXPR_BINARY, LOGICAL_RIGHT_SHIFT_CHAR_INT) {
    ASSERT_OUTPUT_EQ(
        "print('A' >>> 1);",
        "32"
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

TEST(EXPR_BINARY, EQUAL_EQUAL_INT_INT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5 == 5);",
        "true"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_INT_INT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5 == -5);",
        "false"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_INT_FLOAT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5 == 5.0f);",
        "true"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_INT_FLOAT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5 == 5.1f);",
        "false"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_INT_CHAR_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(65 == 'A');",
        "true"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_INT_CHAR_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5 == 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_FLOAT_INT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5.0f == 5);",
        "true"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_FLOAT_INT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f == 5);",
        "false"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_FLOAT_FLOAT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f == 5.5f);",
        "true"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_FLOAT_FLOAT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f == 5.25f);",
        "false"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_FLOAT_CHAR_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(65.0f == 'A');",
        "true"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_FLOAT_CHAR_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f == 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_BOOL_BOOL_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(true == true);",
        "true"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_BOOL_BOOL_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(true == false);",
        "false"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_CHAR_CHAR_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print('A' == 'A');",
        "true"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_CHAR_CHAR_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print('C' == 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_INT_INT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5 != -5);",
        "true"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_INT_INT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5 != 5);",
        "false"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_INT_FLOAT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5 != 5.5f);",
        "true"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_INT_FLOAT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5 != 5.0f);",
        "false"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_INT_CHAR_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5 != 'A');",
        "true"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_INT_CHAR_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(65 != 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_FLOAT_INT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f != 5);",
        "true"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_FLOAT_INT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5.0f != 5);",
        "false"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_FLOAT_FLOAT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f != 5.25f);",
        "true"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_FLOAT_FLOAT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f != 5.5f);",
        "false"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_FLOAT_CHAR_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(6.0f != 'A');",
        "true"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_FLOAT_CHAR_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(65.0f != 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_BOOL_BOOL_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(true != false);",
        "true"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_BOOL_BOOL_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(true != true);",
        "false"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_CHAR_CHAR_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print('A' != 'B');",
        "true"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_CHAR_CHAR_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print('A' != 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_INT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5 < 10);",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_INT_FALSE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5 < 5);",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_INT_FALSE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(10 < 5);",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_FLOAT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5 < 5.1f);",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_FLOAT_FALSE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5 < 4.9f);",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_FLOAT_FALSE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5 < 5.0f);",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_CHAR_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(64 < 'A');",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_CHAR_FALSE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(66 < 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_CHAR_FALSE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(65 < 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_FLOAT_INT_TRUE) {
    ASSERT_OUTPUT_EQ("print(5.5f < 6);",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_FLOAT_INT_FALSE_ONE) {
    ASSERT_OUTPUT_EQ("print(5.5f < 5);",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_FLOAT_INT_FALSE_TWO) {
    ASSERT_OUTPUT_EQ("print(5.0f < 5);",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_FLOAT_FLOAT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f < 10.5f);",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_FLOAT_FLOAT_FALSE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f < 5.5f);",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_FLOAT_FLOAT_FALSE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(10.5f < 5.5f);",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_FLOAT_CHAR_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(64.0f < 'A');",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_FLOAT_CHAR_FALSE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(66.0f < 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_FLOAT_CHAR_FALSE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(65.0f < 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_CHAR_CHAR_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print('A' < 'B');",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_CHAR_CHAR_FALSE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print('B' < 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_CHAR_CHAR_FALSE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print('A' < 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_INT_INT_TRUE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5 <= 10);",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_INT_INT_TRUE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5 <= 5);",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_INT_INT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(10 <= 5);",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_INT_FLOAT_TRUE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5 <= 5.1f);",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_INT_FLOAT_TRUE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5 <= 5.0f);",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_INT_FLOAT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5 <= 4.0f);",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_INT_CHAR_TRUE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(64 <= 'A');",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_INT_CHAR_TRUE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(65 <= 'A');",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_INT_CHAR_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(66 <= 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_FLOAT_INT_TRUE_ONE) {
    ASSERT_OUTPUT_EQ("print(5.5f <= 10);",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_FLOAT_INT_TRUE_TWO) {
    ASSERT_OUTPUT_EQ("print(10.0f <= 10);",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_FLOAT_INT_FALSE) {
    ASSERT_OUTPUT_EQ("print(5.5f <= 5);",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_FLOAT_FLOAT_TRUE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f <= 10.5f);",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_FLOAT_FLOAT_TRUE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f <= 5.5f);",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_FLOAT_FLOAT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(10.5f <= 5.5f);",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_FLOAT_CHAR_TRUE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f <= 'A');",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_FLOAT_CHAR_TRUE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(65.0f <= 'A');",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_FLOAT_CHAR_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(65.5f <= 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_CHAR_CHAR_TRUE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print('A' <= 'B');",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_CHAR_CHAR_TRUE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print('A' <= 'A');",
        "true"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_CHAR_CHAR_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print('B' <= 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_INT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(10 > 5);",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_INT_FALSE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5 > 5);",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_INT_FALSE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5 > 10);",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_FLOAT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(5 > 4.1f);",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_FLOAT_FALSE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5 > 5.1f);",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_FLOAT_FALSE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5 > 5.0f);",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_CHAR_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(66 > 'A');",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_CHAR_FALSE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(65 > 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_CHAR_FALSE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(64 > 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_FLOAT_INT_TRUE) {
    ASSERT_OUTPUT_EQ("print(5.5f > 5);",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_FLOAT_INT_FALSE_ONE) {
    ASSERT_OUTPUT_EQ("print(4.5f > 5);",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_FLOAT_INT_FALSE_TWO) {
    ASSERT_OUTPUT_EQ("print(5.0f > 5);",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_FLOAT_FLOAT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(10.5f > 5.5f);",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_FLOAT_FLOAT_FALSE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f > 5.5f);",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_FLOAT_FLOAT_FALSE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f > 10.5f);",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_FLOAT_CHAR_TRUE) {
    ASSERT_OUTPUT_EQ("print(66.5f > 'A');",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_FLOAT_CHAR_FALSE_ONE) {
    ASSERT_OUTPUT_EQ("print(61.0f > 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_FLOAT_CHAR_FALSE_TWO) {
    ASSERT_OUTPUT_EQ("print(65.0f > 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_CHAR_CHAR_TRUE) {
    ASSERT_OUTPUT_EQ("print('B' > 'A');",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_CHAR_CHAR_FALSE_ONE) {
    ASSERT_OUTPUT_EQ("print('A' > 'B');",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_CHAR_CHAR_FALSE_TWO) {
    ASSERT_OUTPUT_EQ("print('A' > 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_INT_INT_TRUE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(10 >= 5);",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_INT_INT_TRUE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5 >= 5);",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_INT_INT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5 >= 10);",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_INT_FLOAT_TRUE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(5 >= 4.1f);",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_INT_FLOAT_TRUE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5 >= 5.0f);",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_INT_FLOAT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5 >= 8.1f);",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_INT_CHAR_TRUE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(66 >= 'A');",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_INT_CHAR_TRUE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(65 >= 'A');",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_INT_CHAR_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(64 >= 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_FLOAT_INT_TRUE_ONE) {
    ASSERT_OUTPUT_EQ("print(4.5f >= 2);",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_FLOAT_INT_TRUE_TWO) {
    ASSERT_OUTPUT_EQ("print(5.0f >= 5);",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_FLOAT_INT_FALSE) {
    ASSERT_OUTPUT_EQ("print(4.5f >= 5);",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_FLOAT_FLOAT_TRUE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(10.5f >= 5.5f);",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_FLOAT_FLOAT_TRUE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f >= 5.5f);",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_FLOAT_FLOAT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f >= 10.5f);",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_FLOAT_CHAR_TRUE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print(100.5f >= 'A');",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_FLOAT_CHAR_TRUE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print(65.0f >= 'A');",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_FLOAT_CHAR_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(5.5f >= 'A');",
        "false"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_CHAR_CHAR_TRUE_ONE) {
    ASSERT_OUTPUT_EQ(
        "print('B' >= 'A');",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_CHAR_CHAR_TRUE_TWO) {
    ASSERT_OUTPUT_EQ(
        "print('A' >= 'A');",
        "true"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_CHAR_CHAR_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print('A' >= 'B');",
        "false"
    );
}