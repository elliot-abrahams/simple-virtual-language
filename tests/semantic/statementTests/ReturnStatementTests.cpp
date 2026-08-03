#include <gtest/gtest.h>
#include "../SemanticTestUtils.h"

using namespace semanticTest;

TEST(STM_RETURN, WITH_EXPRESSION) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int foo() {
                return 5;
            }
        )"
    );
}

TEST(STM_RETURN, WITHOUT_EXPRESSION) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            void foo() {
                return;
            }
        )"
    );
}

TEST(STM_RETURN, IF_RETURNS_ON_ALL_PATHS) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int foo(bool a) {
                if (a) {
                    return 5;
                } else {
                    return 5;
                }
            }
        )"
    );
}

TEST(STM_RETURN, ELSE_IF_RETURNS_ON_ALL_PATHS) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int foo(bool a, bool b) {
                if (a) {
                    return 5;
                } else if (b) {
                    return 5;
                } else {
                    return 5;
                }
            }
        )"
    );
}



TEST(STM_RETURN, INVALID_WITH_EXPRESSION) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            void foo() {
                return 5;
            }
        )"
    );
}

TEST(STM_RETURN, INVALID_WITHOUT_EXPRESSION) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            int foo() {
                return;
            }
        )"
    );
}

TEST(STM_RETURN, INVALID_IF_DOESNT_RETURN_ON_ALL_PATHS) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            int foo(bool a) {
                if (a) {
                    return 5;
                }
            }
        )"
    );
}

TEST(STM_RETURN, INVALID_ELSE_IF_DOES_NOT_RETURN_ON_ALL_PATHS) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            int foo(bool a, bool b) {
                if (a) {
                    return 5;
                } else if (b) {
                    return 5;
                }
            }
        )"
    );
}

TEST(STM_RETURN, INVALID_WHILE_DOES_NOT_RETURN_ON_ALL_PATHS) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            int foo() {
                while (true) {
                    return 5;
                }
            }
        )"
    );
}