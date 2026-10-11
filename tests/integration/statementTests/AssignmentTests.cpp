#include <gtest/gtest.h>
#include "../IntegrationTestUtils.h"

using namespace integrationTests;

TEST(STM_ASSIGN, INT) {
    ASSERT_OUTPUT_EQ(
        R"(
            int i = 5;
            print(i);
        )",
        "5"
    );
}

TEST(STM_ASSIGN, IMPLICIT_CHAR_TO_INT) {
    ASSERT_OUTPUT_EQ(
        R"(
            int i = 'A';
            print(i);
        )",
        "65"
    );
}

TEST(STM_ASSIGN, FLOAT) {
    ASSERT_OUTPUT_EQ(
        R"(
            float f = 5.0f;
            print(f);
        )",
        "5.0"
    );
}

TEST(STM_ASSIGN, IMPLICIT_INT_TO_FLOAT) {
    ASSERT_OUTPUT_EQ(
        R"(
            float f = 5;
            print(f);
        )",
        "5.0"
    );
}

TEST(STM_ASSIGN, IMPLICIT_CHAR_TO_FLOAT) {
    ASSERT_OUTPUT_EQ(
        R"(
            float f = 'A';
            print(f);
        )",
        "65.0"
    );
}

TEST(STM_ASSIGN, BOOL) {
    ASSERT_OUTPUT_EQ(
        R"(
            bool b = true;
            print(b);
        )",
        "true"
    );
}

TEST(STM_ASSIGN, CHAR) {
    ASSERT_OUTPUT_EQ(
        R"(
            char c = 'h';
            print(c);
        )",
        "h"
    );
}

TEST(STM_ASSIGN, PLUS_EQUAL) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 10;
            x += 5;
            print(x);
        )",
        "15"
    );
}

TEST(STM_ASSIGN, PLUS_EQUAL_ARRAY) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[] arr = new int[1]{10};
            arr[0] += 5;
            print(arr[0]);
        )",
        "15"
    );
}

TEST(STM_ASSIGN, PLUS_EQUAL_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 10;
            x += -5;
            print(x);
        )",
        "5"
    );
}

TEST(STM_ASSIGN, PLUS_EQUAL_IMPLICIT_CONVERSION) {
    ASSERT_OUTPUT_EQ(
        R"(
            float x = 10.5f;
            x += 5;
            print(x);
        )",
        "15.5"
    );
}

TEST(STM_ASSIGN, MINUS_EQUAL) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 10;
            x -= 5;
            print(x);
        )",
        "5"
    );
}

TEST(STM_ASSIGN, PLUS_MINUS_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = -10;
            x -= -5;
            print(x);
        )",
        "-5"
    );
}

TEST(STM_ASSIGN, PLUS_MINUS_IMPLICIT_CONVERSION) {
    ASSERT_OUTPUT_EQ(
        R"(
            float x = 10.5f;
            x -= 5;
            print(x);
        )",
        "5.5"
    );
}

TEST(STM_ASSIGN, MULTIPLY_EQUAL) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 10;
            x *= 5;
            print(x);
        )",
        "50"
    );
}

TEST(STM_ASSIGN, MULTIPLY_EQUAL_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 10;
            x *= -5;
            print(x);
        )",
        "-50"
    );
}

TEST(STM_ASSIGN, MULTIPLY_EQUAL_IMPLICIT_CONVERSION) {
    ASSERT_OUTPUT_EQ(
        R"(
            float x = 0.5f;
            x *= 5;
            print(x);
        )",
        "2.5"
    );
}

TEST(STM_ASSIGN, DIVIDE_EQUAL) {
    ASSERT_OUTPUT_EQ(
        R"(
            float x = 10.0f;
            x /= 2.0f;
            print(x);
        )",
        "5.0"
    );
}

TEST(STM_ASSIGN, DIVIDE_EQUAL_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
        R"(
            float x = 10.0f;
            x /= -2.0f;
            print(x);
        )",
        "-5.0"
    );
}

TEST(STM_ASSIGN, PLUS_DIVIDE_IMPLICIT_CONVERSION) {
    ASSERT_OUTPUT_EQ(
        R"(
            float x = 25.0f;
            x /= 2;
            print(x);
        )",
        "12.5"
    );
}

TEST(STM_ASSIGN, INTEGER_DIVIDE_EQUAL) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 10;
            x //= 2;
            print(x);
        )",
        "5"
    );
}

TEST(STM_ASSIGN, INTEGER_DIVIDE_EQUAL_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 10;
            x //= -2.0f;
            print(x);
        )",
        "-5"
    );
}

TEST(STM_ASSIGN, INTEGER_DIVIDE_IMPLICIT_CONVERSION_ONE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 10;
            x //= 5.5f;
            print(x);
        )",
        "1"
    );
}

TEST(STM_ASSIGN, INTEGER_DIVIDE_IMPLICIT_CONVERSION_TWO) {
    ASSERT_OUTPUT_EQ(
        R"(
            float x = 10.2f;
            x //= 5;
            print(x);
        )",
        "2.0"
    );
}

TEST(STM_ASSIGN, MODULO_EQUAL) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 10;
            x %= 4;
            print(x);
        )",
        "2"
    );
}

TEST(STM_ASSIGN, MODULO_EQUAL_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 10;
            x %= -3;
            print(x);
        )",
        "1"
    );
}

TEST(STM_ASSIGN, MODULO_EQUAL_IMPLICIT_CONVERSION) {
    ASSERT_OUTPUT_EQ(
        R"(
            float x = 25.0f;
            x %= 10;
            print(x);
        )",
        "5.0"
    );
}

TEST(STM_ASSIGN, BITWISE_OR_EQUAL) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 5;
            x |= 12;
            print(x);
        )",
        "13"
    );
}

TEST(STM_ASSIGN, BITWISE_OR_EQUAL_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 12;
            x |= -5;
            print(x);
        )",
        "-1"
    );
}

TEST(STM_ASSIGN, BITWISE_OR_EQUAL_IMPLICIT_CONVERSION) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 25;
            x |= 'A';
            print(x);
        )",
        "89"
    );
}

TEST(STM_ASSIGN, BITWISE_XOR_EQUAL) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 5;
            x ^= 12;
            print(x);
        )",
        "9"
    );
}

TEST(STM_ASSIGN, BITWISE_XOR_EQUAL_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 12;
            x ^= -5;
            print(x);
        )",
        "-9"
    );
}

TEST(STM_ASSIGN, BITWISE_XOR_EQUAL_IMPLICIT_CONVERSION) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 25;
            x ^= 'A';
            print(x);
        )",
        "88"
    );
}

TEST(STM_ASSIGN, BITWISE_AND_EQUAL) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 5;
            x &= 12;
            print(x);
        )",
        "4"
    );
}

TEST(STM_ASSIGN, BITWISE_AND_EQUAL_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 12;
            x &= -5;
            print(x);
        )",
        "8"
    );
}

TEST(STM_ASSIGN, BITWISE_AND_EQUAL_IMPLICIT_CONVERSION) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 25;
            x &= 'A';
            print(x);
        )",
        "1"
    );
}

TEST(STM_ASSIGN, LEFT_SHIFT_EQUAL) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 7;
            x <<= 1;
            print(x);
        )",
        "14"
    );
}

TEST(STM_ASSIGN, LEFT_SHIT_EQUAL_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 7;
            x <<= -1;
            print(x);
        )",
        "-2147483648"
    );
}

TEST(STM_ASSIGN, LEFT_SHIFT_EQUAL_IMPLICIT_CONVERSION) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 20000;
            x <<= '\n';
            print(x);
        )",
        "20480000"
    );
}

TEST(STM_ASSIGN, ARITHMETIC_RIGHT_SHIFT_EQUAL) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 7;
            x >>= 1;
            print(x);
        )",
        "3"
    );
}

TEST(STM_ASSIGN, ARITHMETIC_RIGHT_SHIFT_EQUAL_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = -7;
            x >>= 1;
            print(x);
        )",
        "-4"
    );
}

TEST(STM_ASSIGN, ARITHMETIC_RIGHT_SHIFT_EQUAL_IMPLICIT_CONVERSION) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 20000;
            x >>= '\n';
            print(x);
        )",
        "19"
    );
}

TEST(STM_ASSIGN, LOGICAL_RIGHT_SHIFT_EQUAL) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 7;
            x >>>= 1;
            print(x);
        )",
        "3"
    );
}

TEST(STM_ASSIGN, LOGICAL_RIGHT_SHIFT_EQUAL_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = -7;
            x >>>= 1;
            print(x);
        )",
        "2147483644"
    );
}

TEST(STM_ASSIGN, LOGICAL_RIGHT_SHIFT_SHIFT_EQUAL_IMPLICIT_CONVERSION) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 20000;
            x >>>= '\n';
            print(x);
        )",
        "19"
    );
}

TEST(STM_ASSIGN, LOGICAL_OR_EQUAL_FALSE) {
    ASSERT_OUTPUT_EQ(
        R"(
            bool x = false;
            x ||= false;
            print(x);
        )",
        "false"
    );
}

TEST(STM_ASSIGN, LOGICAL_OR_EQUAL_TRUE) {
    ASSERT_OUTPUT_EQ(
        R"(
            bool x = false;
            x ||= true;
            print(x);
        )",
        "true"
    );
}

TEST(STM_ASSIGN, LOGICAL_AND_EQUAL_FALSE) {
    ASSERT_OUTPUT_EQ(
        R"(
            bool x = false;
            x &&= true;
            print(x);
        )",
        "false"
    );
}

TEST(STM_ASSIGN, LOGICAL_AND_EQUAL_TRUE) {
    ASSERT_OUTPUT_EQ(
        R"(
            bool x = true;
            x &&= true;
            print(x);
        )",
        "true"
    );
}

TEST(STM_ASSIGN, INT_ASSIGN_WITH_VARIABLE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 5;
            int y = x;
            print(y);
        )",
        "5"
    );
}

TEST(STM_ASSIGN, MUTIPLE_VARIABLES) {
    ASSERT_OUTPUT_EQ(
        R"(
            int a = 5;
            int b = 8;
            print(a);
            print(b);
        )",
        "58"
    );
}