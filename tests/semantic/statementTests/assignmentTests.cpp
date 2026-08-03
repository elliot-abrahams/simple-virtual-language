#include <gtest/gtest.h>
#include "../semanticTestUtils.h"

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

TEST(STM_ASSIGN, BOOL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            bool x;
            x = true;
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

TEST(STM_ASSIGN, INVALID_ASSIGN_INT_TO_FLOAT) {
    ASSERT_THROWS_TYPE_ERROR(R"(
            float x;
            x = 5;
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

TEST(STM_ASSIGN, INVALID_USE_VARIABLE_BEFORE_INITIALISED) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            int x;
            int y = x;
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