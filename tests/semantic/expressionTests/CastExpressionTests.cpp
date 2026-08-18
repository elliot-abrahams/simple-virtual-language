#include <gtest/gtest.h>
#include "../SemanticTestUtils.h"

using namespace semanticTest;

TEST(EXPR_CAST, INT_TO_FLOAT) {
    ASSERT_SEMANTICALLY_VALID("float x = (float) 5;");
}

TEST(EXPR_CAST, FLOAT_TO_INT) {
    ASSERT_SEMANTICALLY_VALID("int x = (int) 5.5f;");
}

TEST(EXPR_CAST, INVALID_INT_TO_BOOL) {
    ASSERT_THROWS_TYPE_ERROR("bool x = (bool) 5;");
}

TEST(EXPR_CAST, INVALID_BOOL_TO_INT) {
    ASSERT_THROWS_TYPE_ERROR("int x = (int) true;");
}

TEST(EXPR_CAST, INVALID_FLOAT_TO_BOOL) {
    ASSERT_THROWS_TYPE_ERROR("bool x = (bool) 5.5f;");
}

TEST(EXPR_CAST, INVALID_BOOL_TO_FLOAT) {
    ASSERT_THROWS_TYPE_ERROR("float x = (float) true;");
}