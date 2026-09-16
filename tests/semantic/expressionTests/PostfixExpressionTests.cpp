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