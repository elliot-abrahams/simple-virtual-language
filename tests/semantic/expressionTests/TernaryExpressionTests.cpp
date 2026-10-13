#include <gtest/gtest.h>
#include "../SemanticTestUtils.h"

using namespace semanticTest;

TEST(TERNARY_EXPR, INT_INT) {
    ASSERT_SEMANTICALLY_VALID("int x = true ? 1 : 1;");
}

TEST(TERNARY_EXPR, INT_FLOAT) {
    ASSERT_SEMANTICALLY_VALID("float x = true ? 1 : 5.5f;");
}

TEST(TERNARY_EXPR, INT_CHAR) {
    ASSERT_SEMANTICALLY_VALID("int x = true ? 1 : 'a';");
}

TEST(TERNARY_EXPR, FLOAT_INT) {
    ASSERT_SEMANTICALLY_VALID("float x = true ? 5.5f : 1;");
}

TEST(TERNARY_EXPR, FLOAT_FLOAT) {
    ASSERT_SEMANTICALLY_VALID("float x = true ? 5.5f : 5.5f;");
}

TEST(TERNARY_EXPR, FLOAT_CHAR) {
    ASSERT_SEMANTICALLY_VALID("float x = true ? 5.5f : 'a';");
}

TEST(TERNARY_EXPR, BOOL_BOOL) {
    ASSERT_SEMANTICALLY_VALID("bool x = true ? true : false;");
}

TEST(TERNARY_EXPR, CHAR_INT) {
    ASSERT_SEMANTICALLY_VALID("int x = true ? 'a' : 1;");
}

TEST(TERNARY_EXPR, CHAR_FLOAT) {
    ASSERT_SEMANTICALLY_VALID("float x = true ? 'a' : 5.5f;");
}

TEST(TERNARY_EXPR, CHAR_CHAR) {
    ASSERT_SEMANTICALLY_VALID("char x = true ? 'a' : 'a';");
}

TEST(TERNARY_EXPR, INVALID_INT_BOOL) {
    ASSERT_THROWS_TYPE_ERROR("int x = true ? 1 : false;");
}

TEST(TERNARY_EXPR, INVALID_FLOAT_BOOL) {
    ASSERT_THROWS_TYPE_ERROR("int x = true ? 5.5f : false;");
}

TEST(TERNARY_EXPR, INVALID_BOOL_INT) {
    ASSERT_THROWS_TYPE_ERROR("int x = true ? false : 1;");
}

TEST(TERNARY_EXPR, INVALID_BOOL_FLOAT) {
    ASSERT_THROWS_TYPE_ERROR("int x = true ? false : 5.5f;");
}

TEST(TERNARY_EXPR, INVALID_BOOL_CHAR) {
    ASSERT_THROWS_TYPE_ERROR("int x = true ? false : 'a';");
}

TEST(TERNARY_EXPR, INVALID_CHAR_BOOL) {
    ASSERT_THROWS_TYPE_ERROR("int x = true ? 'a' : false;");
}

TEST(TERNARY_EXPR, INVALID_INT_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(R"(
        int[] arr = new int[3];
        int x = true ? 1 : arr;
    )");
}

TEST(TERNARY_EXPR, INVALID_ARRAY_INT) {
    ASSERT_THROWS_TYPE_ERROR(R"(
        int[] arr = new int[3];
        int x = true ? arr : 1;
    )");
}

TEST(TERNARY_EXPR, INVALID_CONITION_INT) {
    ASSERT_THROWS_TYPE_ERROR("int x = 1 ? 1 : 1;");
}

TEST(TERNARY_EXPR, INVALID_CONITION_FLOAT) {
    ASSERT_THROWS_TYPE_ERROR("int x = 1.5f ? 1 : 1;");
}

TEST(TERNARY_EXPR, INVALID_CONITION_CHAR) {
    ASSERT_THROWS_TYPE_ERROR("int x = 'A' ? 1 : 1;");
}