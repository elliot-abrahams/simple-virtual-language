#include <gtest/gtest.h>
#include "../SemanticTestUtils.h"

using namespace semanticTest;

TEST(EXPR_STM, FUNCTION_CALL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            void foo() {}
            foo();
        )"
    );
}

TEST(EXPR_STM, VARIABLE_POST_INCREMENT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x++;
        )"
    );
}

TEST(EXPR_STM, VARIABLE_POST_DECREMENT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x--;
        )"
    );
}

TEST(EXPR_STM, VARIABLE_PRE_INCREMENT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            ++x;
        )"
    );
}

TEST(EXPR_STM, VARIABLE_PRE_DECREMENT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            --x;
        )"
    );
}

TEST(EXPR_STM, INVALID_FUNCTION_CALL_RETURNS_NON_VOID) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            int foo() { return 5; }
            foo();
        )"
    );
}

TEST(EXPR_STM, INVALID_VARIABLE) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            int x = 5;
            x;
        )"
    );
}

TEST(EXPR_STM, INVALID_INT_LITERAL) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        "5;"
    );
}

TEST(EXPR_STM, INVALID_FLOAT_LITERAL) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        "5.5f;"
    );
}

TEST(EXPR_STM, INVALID_BOOL_LITERAL) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        "true;"
    );
}

TEST(EXPR_STM, INVALID_NEW_EXPR) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        "new int[3];"
    );
}

TEST(EXPR_STM, INVALID_PAREN_EXPR) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            (5);
        )"
    );
}

TEST(EXPR_STM, INVALID_CAST_EXPR) {
    ASSERT_THROWS_SEMANTIC_ERROR("(int) 5.5f;");
}

TEST(EXPR_STM, INVALID_UNARY_EXPR_PLUS) {
    ASSERT_THROWS_SEMANTIC_ERROR("+5;");
}

TEST(EXPR_STM, INVALID_UNARY_EXPR_MINUS) {
    ASSERT_THROWS_SEMANTIC_ERROR("-5;");
}

TEST(EXPR_STM, INVALID_UNARY_EXPR_LOGICAL_NOT) {
    ASSERT_THROWS_SEMANTIC_ERROR("!5;");
}

TEST(EXPR_STM, INVALID_BINARY_EXPR) {
    ASSERT_THROWS_SEMANTIC_ERROR("1 + 2;");
}
