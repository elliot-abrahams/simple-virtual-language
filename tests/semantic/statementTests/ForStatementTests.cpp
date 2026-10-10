#include <gtest/gtest.h>
#include "../SemanticTestUtils.h"

using namespace semanticTest;

TEST(FOR, INT_RANGE_DECL) {
    ASSERT_SEMANTICALLY_VALID("for (int i : 0 .. 1) {}");
}

TEST(FOR, INT_RANGE_IDENTIFIER) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int i;
            for (i : 0 .. 1) {}
        )"
    );
}

TEST(FOR, INT_RANGE_IDENTIFIER_INITIALISED) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int i = 5;
            for (int i : i .. 1) {}
        )"
    );
}

TEST(FOR, INT_RANGE_IMPLICIT_CONVERSION_FROM_CHAR) {
    ASSERT_SEMANTICALLY_VALID("for (int i : 'a' .. 'b' : 'c') {}");
}

TEST(FOR, FLOAT_RANGE) {
    ASSERT_SEMANTICALLY_VALID("for (float i : 0.5f .. 10.0f : 0.5f) {}");
}

TEST(FOR, FLOAT_RANGE_IMPLICIT_CONVERSION_FROM_INT) {
    ASSERT_SEMANTICALLY_VALID("for (float i : 1 .. 10 : 4) {}");
}

TEST(FOR, FLOAT_RANGE_IMPLICIT_CONVERSION_FROM_CHAR) {
    ASSERT_SEMANTICALLY_VALID("for (float i : 'a' .. 'b' : 'c') {}");
}

TEST(FOR, CHAR_RANGE) {
    ASSERT_SEMANTICALLY_VALID("for (char i : 'a' .. 'z' : 'a') {}");
}

TEST(FOR, RANGE_VARIABLE_IN_SCOPE) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            for (int i : 0 .. 10) {
                print(i);
            }
        )"
    );
}

TEST(FOR, INT_ITERATE_DECL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[] arr = new int[3];
            for (int i : arr) {}
        )"
    );
}

TEST(FOR, INT_ARRAY_ITERATE_DECL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[][] arr = new int[3][3];
            for (int[] i : arr) {}
        )"
    );
}

TEST(FOR, INT_ITERATE_IDENTIFIER) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[] arr = new int[3];
            int i;
            for (i : arr) {}
        )"
    );
}

TEST(FOR, INT_ITERATE_IDENTIFIER_INITIALISED) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[] arr = new int[3];
            int i = 5;
            for (i : arr) {}
        )"
    );
}

TEST(FOR, FLOAT_ITERATE) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            float[] arr = new float[3];
            for (float i : arr) {}
        )"
    );
}

TEST(FOR, BOOL_ITERATE) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            bool[] arr = new bool[3];
            for (bool i : arr) {}
        )"
    );
}

TEST(FOR, CHAR_ITERATE) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            char[] arr = new char[3];
            for (char i : arr) {}
        )"
    );
}

TEST(FOR, ITERATE_NEW_EXPR) {
    ASSERT_SEMANTICALLY_VALID("for (int i : new int[3]) {}");
}

TEST(FOR, INVALID_RANGE_UNINITIALISED) {
    ASSERT_THROWS_SEMANTIC_ERROR("for (int i : i .. 1) {}");
}

TEST(FOR, INVALID_BOOL_RANGE) {
    ASSERT_THROWS_TYPE_ERROR("for (bool i : true .. false) {}");
}

TEST(FOR, INVALID_ARRAY_RANGE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int[] arr = new int[3];
            for (arr : 0 .. 10) {}
        )"
    );
}

TEST(FOR, INVALID_RANGE_START_TYPE) {
    ASSERT_THROWS_TYPE_ERROR("for (int i : 0.5f .. 1) {}");
}

TEST(FOR, INVALID_RANGE_END_TYPE) {
    ASSERT_THROWS_TYPE_ERROR("for (int i : 1 .. 10.5f) {}");
}

TEST(FOR, INVALID_RANGE_STEP_TYPE) {
    ASSERT_THROWS_TYPE_ERROR("for (int i : 1 .. 10 : 10.5f) {}");
}

TEST(FOR, INVALID_ITERATE_NON_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int num = 10;
            for (int i : num) {}
        )"
    );
}

TEST(FOR, INVALID_ITERATE_ELEMENT_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int[][] arr = new int[3][3];
            for (int i : arr) {}
        )"
    );
}

TEST(FOR, INVALID_VARIABLE_ACCESS_OUTSIDE_BODY) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            for (int i : 0 .. 10) {}
            print(i);
        )"
    );
}

/*
 *
*[  FAILED  ] FOR.INVALID_ARRAY_RANGE
[  FAILED  ] FOR.INVALID_RANGE_START_TYPE
[  FAILED  ] FOR.INVALID_RANGE_END_TYPE
[  FAILED  ] FOR.INVALID_RANGE_STEP_TYPE

 */