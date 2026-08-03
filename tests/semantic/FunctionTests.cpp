#include <gtest/gtest.h>
#include "SemanticTestUtils.h"

using namespace semanticTest;

TEST(FUNCTION, FUNCTION_CALL_AFTER_DECLARATION) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int foo() {
                return 5;
            }

            int x = foo();
        )"
    );
}

TEST(FUNCTION, FUNCTION_CALL_BEFORE_DECLARATION) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = foo();

            int foo() {
                return 5;
            }
        )"
    );
}

TEST(FUNCTION, GLOBAL_VARIABLE_USED_AFTER_DECLARATION) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x;

            void foo() {
                x = 5;
            }
        )"
    );
}

TEST(FUNCTION, RECURSION) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int foo() {
                return foo();
            }
        )"
    );
}

TEST(FUNCTION, OVERLOADING) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int foo(int a) {
                return 5;
            }

            int foo() {
                return 6;
            }
        )"
    );
}

TEST(FUNCTION, INVALID_GLOBAL_VARIABLE_BEFORE_DECLARATION) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            void foo() {
                x = 5;
            }

            int x;
        )"
    );
}

TEST(FUNCTION, DUPLICATE_FUNCTION_SIGNATURE) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            int foo() {
                return 5;
            }

            int foo() {
                return 6;
            }
        )"
    );
}