#include <gtest/gtest.h>
#include "semanticTestUtils.h"

using namespace semanticTest;

TEST(SCOPE, INNER_SCOPE_ACCESS_OUTER_SCOPE_VARIABLE) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            {
                int y = x;
            }
        )"
    );
}

TEST(SCOPE, INNER_SCOPE_SHADOWS_OUTER_SCOPE_VARIABLE) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            {
                int x = 10;
            }
        )"
    );
}

TEST(SCOPE, USE_INITIALISED_VARIABLE_WITHIN_BLOCK) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            {
                int x = 5;
                int y = x;
            }
        )"
    );
}

TEST(SCOPE, INVALID_VARIABLE_NOT_VISIBLE_OUTSIDE_OF_SCOPE_OF_DECL) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            {
                int x = 5;
            }
            x = 7;
        )"
    );
}

TEST(SCOPE, INVALID_DUPLICATE_VARIABLE_DECL) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            int x;
            int x;
        )"
    );
}

TEST(SCOPE, INVALID_DUPLICATE_VARIABLE_DECL_WITHIN_BLOCK) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            {
                int x;
                int x;
            }
        )"
    );
}

TEST(SCOPE, INVALID_USING_UNDECLARED_VARIABLE) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            x = 5;
        )"
    );
}

TEST(SCOPE, INVALID_USING_VARIABLE_BEFORE_DECLARATION) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            x = 5;
            int x;
        )"
    );
}