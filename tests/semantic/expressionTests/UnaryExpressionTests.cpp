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