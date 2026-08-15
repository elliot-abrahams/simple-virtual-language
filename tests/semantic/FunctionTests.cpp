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

TEST(FUNCTION, IMPLICIT_CONVERSION_INT_TO_FLOAT) {
    ASSERT_SEMANTICALLY_VALID(R"(
            float foo(float a) {
                return a;
            }

            float res = foo(5);
        )"
    );
}

TEST(FUNCTION, IMPLICIT_CONVERSION_MULTIPLE_INT_TO_FLOAT) {
    ASSERT_SEMANTICALLY_VALID(R"(
            float foo(float a, float b) {
                return a + b;
            }

            float res = foo(5, 6);
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

TEST(FUNCTION, INVALID_DUPLICATE_FUNCTION_SIGNATURE) {
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

TEST(FUNCTION, INVALID_AMBIGUOUS_FUNCTION_CALL) {
    ASSERT_THROWS_SEMANTIC_ERROR(R"(
            float add(float a, float b, float c) {
                return 3.0f;
            }
            float add(float a, int b, float c) {
                return 2.0f;
            }
            float add(int a, int b, float c) {
                return 1.0f;
            }
            float res = add(5, 5, 5);
        )"
    );
}

TEST(FUNCTION, INVALID_IMPLICIT_CONVERSION_FLOAT_TO_INT) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            void foo(int a) {}
            foo(5.5f);
        )"
    );
}