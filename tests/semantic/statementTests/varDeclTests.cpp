#include <gtest/gtest.h>
#include "../semanticTestUtils.h"

using namespace semanticTest;

TEST(STM_VAR_DECL, INT_INIT) {
    ASSERT_SEMANTICALLY_VALID("int x = 5;");
}

TEST(STM_VAR_DECL, FLOAT_INIT) {
    ASSERT_SEMANTICALLY_VALID("float x = 5.5f;");
}

TEST(STM_VAR_DECL, BOOL_INIT) {
    ASSERT_SEMANTICALLY_VALID("bool x = true;");
}

TEST(STM_VAR_DECL, INVALID_ASSIGN_BOOL_TO_INT) {
    const auto testCode = R"(
        int x = true;
    )";
    ASSERT_THROWS_TYPE_ERROR(testCode);
}

TEST(STM_VAR_DECL, INVALID_ASSIGN_INT_TO_FLOAT) {
    const auto testCode = R"(
        float x = 5;
    )";
    ASSERT_THROWS_TYPE_ERROR(testCode);
}

TEST(STM_VAR_DECL, INVALID_ASSIGN_FLOAT_TO_BOOL) {
    const auto testCode = R"(
        bool x = 5.5f;
    )";
    ASSERT_THROWS_TYPE_ERROR(testCode);
}