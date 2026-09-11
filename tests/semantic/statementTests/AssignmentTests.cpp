#include <gtest/gtest.h>
#include "../SemanticTestUtils.h"

using namespace semanticTest;

TEST(STM_ASSIGN, INT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x;
            x = 5;
        )"
    );
}

TEST(STM_ASSIGN, FLOAT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            float x;
            x = 5.5f;
        )"
    );
}

TEST(STM_ASSIGN, IMPLICIT_INT_TO_FLOAT) {
    ASSERT_SEMANTICALLY_VALID(R"(
            float x;
            x = 5;
        )"
    );
}

TEST(STM_ASSIGN, BOOL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            bool x;
            x = true;
        )"
    );
}

TEST(STM_ASSIGN, INT_ARRAY) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[] x;
            x = new int[3];
        )"
    );
}

TEST(STM_ASSIGN, FLOAT_ARRAY) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            float[] x;
            x = new float[3];
        )"
    );
}

TEST(STM_ASSIGN, BOOL_ARRAY) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            bool[] x;
            x = new bool[3];
        )"
    );
}

TEST(STM_ASSIGN, ASSIGN_INT_ARRAY_ELEMENT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[] x = new int[5];
            x[0] = 5;
        )"
    );
}

TEST(STM_ASSIGN, ASSIGN_FLOAT_ARRAY_ELEMENT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            float[] x = new float[5];
            x[0] = 5.5f;
        )"
    );
}

TEST(STM_ASSIGN, ASSIGN_BOOL_ARRAY_ELEMENT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            bool[] x = new bool[5];
            x[0] = true;
        )"
    );
}

TEST(STM_ASSIGN, ASSIGN_TWO_DIMENSIONAL_INT_ARRAY_ELEMENT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[][] x = new int[5][1];
            x[0][1] = 5;
        )"
    );
}

TEST(STM_ASSIGN, ASSIGN_ONE_DIMENSIONAL_ARRAY_TO_TWO_DIMENSIONAL_INT_ARRAY) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[][] x = new int[5][1];
            x[0] = new int[1];
        )"
    );
}

TEST(STM_ASSIGN, USE_INITIALISED_VAR) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            int y = x;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_BOOL_TO_INT) {
    ASSERT_THROWS_TYPE_ERROR(R"(
            int x;
            x = true;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_FLOAT_TO_BOOL) {
    ASSERT_THROWS_TYPE_ERROR(R"(
            bool x;
            x = 5.5f;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_INT_TO_INT_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int[] x;
            x = 5;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_FLOAT_TO_FLOAT_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            float[] x;
            x = 5.5f;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_BOOL_TO_BOOL_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            bool[] x;
            x = true;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_INT_ARRAY_TO_INT) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int x;
            x = new int[3];
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_FLOAT_ARRAY_TO_FLOAT) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            float x;
            x = new float[3];
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_BOOL_ARRAY_TO_BOOL) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            bool x;
            x = new bool[3];
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_INT_ELEMENT_TO_BOOL_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            bool[] x = new bool[5];
            x[0] = 5;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_BOOL_ELEMENT_TO_FLOAT_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            float[] x = new float[5];
            x[0] = false;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_USE_VARIABLE_BEFORE_INITIALISED) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            int x;
            int y = x;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_USE_OF_ARRAY_VARIABLE_BEFORE_INITIALISED) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            int[] x;
            int y = x[1];
        )"
    );
}

TEST(STM_ASSIGN, INVALID_USE_OF_ARRAY_ACCESS_BEFORE_INITIALISED) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            int[] x;
            x[0] = 5;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_USE_VARIABLE_BEFORE_DECLARATION) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            x = 5;
        )"
    );
}