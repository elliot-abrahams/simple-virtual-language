#include <gtest/gtest.h>
#include "../SemanticTestUtils.h"

using namespace semanticTest;

TEST(EXPR_POSTFIX, INT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            int y = x;
        )"
    );
}

TEST(EXPR_POSTFIX, INT_TO_FLOAT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            float y = x;
        )"
    );
}

TEST(EXPR_POSTFIX, FLOAT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            float x = 5.5f;
            float y = x;
        )"
    );
}

TEST(EXPR_POSTFIX, BOOL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            bool x = false;
            bool y = x;
        )"
    );
}

TEST(EXPR_POSTFIX, ONE_DIMENSIONAL_INT_ARRAY_ZERO_INDICES) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[] x = new int[3];
            int[] y = x;
        )"
    );
}

TEST(EXPR_POSTFIX, ONE_DIMENSIONAL_FLOAT_ARRAY_ZERO_INDICES) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            float[] x = new float[3];
            float[] y = x;
        )"
    );
}

TEST(EXPR_POSTFIX, ONE_DIMENSIONAL_BOOL_ARRAY_ZERO_INDICES) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            bool[] x = new bool[3];
            bool[] y = x;
        )"
    );
}

TEST(EXPR_POSTFIX, ONE_DIMENSIONAL_INT_ARRAY_ONE_INDEX) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[] x = new int[3];
            int y = x[1];
        )"
    );
}

TEST(EXPR_POSTFIX, ONE_DIMENSIONAL_FLOAT_ARRAY_ONE_INDEX) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            float[] x = new float[3];
            float y = x[1];
        )"
    );
}

TEST(EXPR_POSTFIX, ONE_DIMENSIONAL_BOOL_ARRAY_ONE_INDEX) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            bool[] x = new bool[3];
            bool y = x[1];
        )"
    );
}

TEST(EXPR_POSTFIX, TWO_DIMENSIONAL_INT_ARRAY_ZERO_INDICES) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[][] x = new int[3][3];
            int[][] y = x;
        )"
    );
}

TEST(EXPR_POSTFIX, TWO_DIMENSIONAL_INT_ARRAY_ONE_INDEX) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[][] x = new int[3][3];
            int[] y = x[1];
        )"
    );
}

TEST(EXPR_POSTFIX, TWO_DIMENSIONAL_INT_ARRAY_TWO_INDICES) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[][] x = new int[3][3];
            int y = x[1][1];
        )"
    );
}

TEST(EXPR_POSTFIX, FUNCTIONAL_CALL_EXPR_RETURNING_ARRAY_ONE_INDEX) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[] foo() {
                return new int[3];
            }

            int x = foo()[1];
        )"
    );
}

TEST(EXPR_POSTFIX, NEW_EXPRESSION_ONE_INDEX) {
    ASSERT_SEMANTICALLY_VALID(
        "int x = new int[3]{1, 2, 3}[0];"
    );
}

TEST(EXPR_POSTFIX, INCREMENT_VARIABLE) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            int y = x++;
        )"
    );
}

TEST(EXPR_POSTFIX, DECREMENT_VARIABLE) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            int y = x--;
        )"
    );
}

TEST(EXPR_POSTFIX, INCREMENT_ARRAY_ELEMENT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[] arr = new int[3];
            int x = arr[1]++;
        )"
    );
}

TEST(EXPR_POSTFIX, DECREMENT_ARRAY_ELEMENT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[] arr = new int[3];
            int x = arr[1]--;
        )"
    );
}

TEST(EXPR_POSTFIX, INCREMENT_FUNCTION_CALL_RETURNING_ARRAY) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[] foo() { return new int[3]; }
            int x = foo()[1]++;
        )"
    );
}

TEST(EXPR_POSTFIX, DECREMENT_FUNCTION_CALL_RETURNING_ARRAY) {
    ASSERT_SEMANTICALLY_VALID(
    R"(
            int[] foo() { return new int[3]; }
            int x = foo()[1]--;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_FLOAT_TO_INT) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            float x = 5.5f;
            int y = x;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_INT_TO_BOOL) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int x = 5;
            bool y = x;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_BOOL_TO_FLOAT) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            bool x = true;
            float y = x;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_INT_TO_INT_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int x = 5;
            int[] y = x;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_FLOAT_TO_FLOAT_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            float x = 5;
            float[] y = x;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_BOOL_TO_BOOL_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            bool x = 5;
            bool[] y = x;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_INT_ARRAY_TO_INT) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int[] x = new int[3];
            int y = x;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_FLOAT_ARRAY_TO_FLOAT) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            float[] x = new float[3];
            float y = x;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_BOOL_ARRAY_TO_BOOL) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            bool[] x = new bool[3];
            bool y = x;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_INT_ARRAY_WITH_ONE_INDICEX) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int x = 5;
            int y = x[1];
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_ONE_DIMENSIONAL_INT_ARRAY_WITH_TWO_INDICES) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int[] x = new int[3];
            int y = x[1][1];
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_FLOAT_INDEX_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int[] x = new int[3];
            int y = x[5.5f];
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_BOOL_INDEX_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int[] x = new int[3];
            int y = x[false];
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_INCREMENT_INT) {
    ASSERT_THROWS_TYPE_ERROR(
        "int x = 5++;"
    );
}

TEST(EXPR_POSTFIX, INVALID_INCREMENT_FLOAT) {
    ASSERT_THROWS_TYPE_ERROR(
            "float x = 5.5f++;"
    );
}

TEST(EXPR_POSTFIX, INVALID_DECREMENT_INT) {
    ASSERT_THROWS_TYPE_ERROR(
        "int x = 5--;"
    );
}

TEST(EXPR_POSTFIX, INVALID_DECREMENT_FLOAT) {
    ASSERT_THROWS_TYPE_ERROR(
        "float x = 5.5f--;"
    );
}

TEST(EXPR_POSTFIX, INVALID_INCREMENT_BOOL) {
    ASSERT_THROWS_TYPE_ERROR(
        "int x = true++;"
    );
}

TEST(EXPR_POSTFIX, INVALID_DECREMENT_BOOL) {
    ASSERT_THROWS_TYPE_ERROR(
        "int x = true--;"
    );
}

TEST(EXPR_POSTFIX, INVALID_INCREMENT_NEW_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
        "int x = new int[3]++;"
    );
}

TEST(EXPR_POSTFIX, INVALID_DECREMENT_NEW_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
        "int x = new int[3]--;"
    );
}

TEST(EXPR_POSTFIX, INVALID_INCREMENT_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
    R"(
            int[] arr = new int[3];
            int x = arr++;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_DECREMENT_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int[] arr = new int[3];
            int x = arr--;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_INCREMENT_NESTED_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
    R"(
            int[][] arr = new int[3][3];
            int x = arr[1]++;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_DECREMENT_NESED_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int[][] arr = new int[3][3];
            int x = arr[1]--;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_INCREMENT_NESTED_POSTFIX_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
    R"(
            int x = 5;
            int y = (x++)++;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_DECREMENT_NESED_POSTFIX_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int x = 5;
            int y = (x--)--;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_INCREMENT_UNARY_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
    R"(
            int x = 5;
            int y = (-5)++;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_DECREMENT_UNARY_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
    R"(
            int x = 5;
            int y = (-5)--;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_INCREMENT_BINARY_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
    R"(
            int x = 5;
            int y = (1 + 2)++;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_DECREMENT_BINARY_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
    R"(
            int x = 5;
            int y = (1 + 2)--;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_INCREMENT_FUNCTION_CALL_RETURNING_INT) {
    ASSERT_THROWS_TYPE_ERROR(
    R"(
            int foo() { return 5; }
            int x = foo()++;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_DECREMENT_FUNCTION_CALL_RETURNING_INT) {
    ASSERT_THROWS_TYPE_ERROR(
    R"(
            int foo() { return 5; }
            int x = foo()--;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_INCREMENT_CAST_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            float x = 5.5f;
            int y = ((int) x)++;
        )"
    );
}

TEST(EXPR_POSTFIX, INVALID_DECREMENT_CAST_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            float x = 5.5f;
            int y = ((int) x)--;
        )"
    );
}