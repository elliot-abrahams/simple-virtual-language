#include <gtest/gtest.h>
#include "../SemanticTestUtils.h"

using namespace semanticTest;

TEST(EXPR_UNARY, PLUS) {
    ASSERT_SEMANTICALLY_VALID("int x = +5;");
}

TEST(EXPR_UNARY, MINUS) {
    ASSERT_SEMANTICALLY_VALID("int x = -5;");
}

TEST(EXPR_UNARY, LOGICAL_NOT) {
    ASSERT_SEMANTICALLY_VALID("bool x = !true;");
}

TEST(EXPR_UNARY, INCREMENT_INT_VARIABLE) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            int y = ++x;
        )"
    );
}

TEST(EXPR_UNARY, DECREMENT_INT_VARIABLE) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            int y = --x;
        )"
    );
}

TEST(EXPR_UNARY, INCREMENT_FLOAT_VARIABLE) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            float x = 5.5f;
            float y = ++x;
        )"
    );
}

TEST(EXPR_UNARY, DECREMENT_FLOAT_VARIABLE) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            float x = 5.5f;
            float y = --x;
        )"
    );
}

TEST(EXPR_UNARY, INCREMENT_ARRAY_ELEMENT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[] arr = new int[3];
            int x = ++arr[1];
        )"
    );
}

TEST(EXPR_UNARY, DECREMENT_ARRAY_ELEMENT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[] arr = new int[3];
            int x = --arr[1];
        )"
    );
}

TEST(EXPR_UNARY, INCREMENT_FUNCTION_CALL_RETURNING_ARRAY) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[] foo() { return new int[3]; }
            int x = ++foo()[1];
        )"
    );
}

TEST(EXPR_UNARY, DECREMENT_FUNCTION_CALL_RETURNING_ARRAY) {
    ASSERT_SEMANTICALLY_VALID(
    R"(
            int[] foo() { return new int[3]; }
            int x = --foo()[1];
        )"
    );
}

TEST(EXPR_UNARY, INVALID_BOOL_PLUS) {
    ASSERT_THROWS_TYPE_ERROR("bool x = +true;");
}

TEST(EXPR_UNARY, INVALID_BOOL_MINUS) {
    ASSERT_THROWS_TYPE_ERROR("bool x = -true;");
}

TEST(EXPR_UNARY, INVALID_INT_LOGICAL_NOT) {
    ASSERT_THROWS_TYPE_ERROR("bool x = !5;");
}

TEST(EXPR_UNARY, INVALID_FLOAT_LOGICAL_NOT) {
    ASSERT_THROWS_TYPE_ERROR("bool x = !5.5f;");
}

TEST(EXPR_UNARY, INCREMENT_INT) {
    ASSERT_THROWS_TYPE_ERROR(
        "int x = ++5;"
    );
}

TEST(EXPR_UNARY, DECREMENT_INT) {
    ASSERT_THROWS_TYPE_ERROR(
        "int x = --5;"
    );
}

TEST(EXPR_UNARY, INCREMENT_FLOAT) {
    ASSERT_THROWS_TYPE_ERROR(
        "float x = ++5.5f;"
    );
}

TEST(EXPR_UNARY, DECREMENT_FLOAT) {
    ASSERT_THROWS_TYPE_ERROR(
        "float x = --5.5f;"
    );
}

TEST(EXPR_UNARY, INCREMENT_BOOL) {
    ASSERT_THROWS_TYPE_ERROR(
        "int x = ++true;"
    );
}

TEST(EXPR_UNARY, DECREMENT_BOOL) {
    ASSERT_THROWS_TYPE_ERROR(
        "int x = --true;"
    );
}

TEST(EXPR_UNARY, INVALID_INCREMENT_BOOL_VARIABLE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            bool x = true;
            bool y = ++x;
        )"
    );
}

TEST(EXPR_UNARY, INVALID_DECREMENT_BOOL_VARIABLE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            bool x = true;
            bool y = --x;
        )"
    );
}

TEST(EXPR_UNARY, INVALID_INCREMENT_NEW_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
        "int x = ++(new int[3]);"
    );
}

TEST(EXPR_UNARY, INVALID_DECREMENT_NEW_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
        "int x = --(new int[3]);"
    );
}

TEST(EXPR_UNARY, INVALID_INCREMENT_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
    R"(
            int[] arr = new int[3];
            int x = ++arr;
        )"
    );
}

TEST(EXPR_UNARY, INVALID_DECREMENT_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int[] arr = new int[3];
            int x = --arr;
        )"
    );
}

TEST(EXPR_UNARY, INVALID_INCREMENT_NESTED_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int[][] arr = new int[3][3];
            int x = ++arr[1];
        )"
    );
}

TEST(EXPR_UNARY, INVALID_DECREMENT_NESED_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int[][] arr = new int[3][3];
            int x = --arr[1];
        )"
    );
}

TEST(EXPR_UNARY, INVALID_INCREMENT_NESTED_POSTFIX_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
    R"(
            int x = 5;
            int y = ++(x++);
        )"
    );
}

TEST(EXPR_UNARY, INVALID_DECREMENT_NESED_POSTFIX_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int x = 5;
            int y = --(x--);
        )"
    );
}

TEST(EXPR_UNARY, INVALID_INCREMENT_UNARY_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
    R"(
            int x = 5;
            int y = ++(-5);
        )"
    );
}

TEST(EXPR_UNARY, INVALID_DECREMENT_UNARY_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
    R"(
            int x = 5;
            int y = --(-5);
        )"
    );
}

TEST(EXPR_UNARY, INVALID_INCREMENT_BINARY_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
    R"(
            int x = 5;
            int y = ++(1 + 2);
        )"
    );
}

TEST(EXPR_UNARY, INVALID_DECREMENT_BINARY_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
    R"(
            int x = 5;
            int y = --(1 + 2);
        )"
    );
}

TEST(EXPR_UNARY, INVALID_INCREMENT_FUNCTION_CALL_RETURNING_INT) {
    ASSERT_THROWS_TYPE_ERROR(
    R"(
            int foo() { return 5; }
            int x = ++foo();
        )"
    );
}

TEST(EXPR_UNARY, INVALID_DECREMENT_FUNCTION_CALL_RETURNING_INT) {
    ASSERT_THROWS_TYPE_ERROR(
    R"(
            int foo() { return 5; }
            int x = --foo();
        )"
    );
}

TEST(EXPR_UNARY, INVALID_INCREMENT_CAST_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            float x = 5.5f;
            int y = ++((int) x);
        )"
    );
}

TEST(EXPR_UNARY, INVALID_DECREMENT_CAST_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            float x = 5.5f;
            int y = --((int) x);
        )"
    );
}