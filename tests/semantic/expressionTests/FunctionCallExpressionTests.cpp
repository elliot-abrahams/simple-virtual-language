#include <gtest/gtest.h>
#include "../SemanticTestUtils.h"

using namespace semanticTest;

TEST(EXPR_FUNCTION_CALL, INT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int foo() {
                return 5;
            }

            int x = foo();
        )"
    );
}

TEST(EXPR_FUNCTION_CALL, INT_ARRAY) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[] foo() {
                return new int[3];
            }

            int[] x = foo();
        )"
    );
}

TEST(EXPR_FUNCTION_CALL, INT_BINARY_EXPR) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int foo() {
                return 5;
            }

            int x = foo() + 1;
        )"
    );
}

TEST(EXPR_FUNCTION_CALL, FLOAT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            float foo() {
                return 5.5f;
            }

            float x = foo();
        )"
    );
}

TEST(EXPR_FUNCTION_CALL, FLOAT_ARRAY) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            float[] foo() {
                return new float[3];
            }

            float[] x = foo();
        )"
    );
}

TEST(EXPR_FUNCTION_CALL, FLOAT_IMPLICIT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            float foo() {
                return 5;
            }

            float x = foo();
        )"
    );
}

TEST(EXPR_FUNCTION_CALL, FLOAT_BINARY_EXPR) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            float foo() {
                return 5.5f;
            }

            float x = foo() + 1.5f;
        )"
    );
}

TEST(EXPR_FUNCTION_CALL, BOOL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            bool foo() {
                return true;
            }

            bool x = foo();
        )"
    );
}

TEST(EXPR_FUNCTION_CALL, BOOL_ARRAY) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            bool[] foo() {
                return new bool[3];
            }

            bool[] x = foo();
        )"
    );
}

TEST(EXPR_FUNCTION_CALL, BOOL_BINARY_EXPR) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            bool foo() {
                return false;
            }

            bool x = foo() && true;
        )"
    );
}

TEST(EXPR_FUNCTION_CALL, RETURN_FUNCTION_CALL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int foo() {
                return 5;
            }

            int bar() {
                return foo();
            }

            int x = bar();
        )"
    );
}

TEST(EXPR_FUNCTION_CALL, INVALID_RETURN_WRONG_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int foo() {
                return 5.5f;
            }
        )"
    );
}

TEST(EXPR_FUNCTION_CALL, INVALID_RETURN_WRONG_ARRAY_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int[] foo() {
                return new float[3];
            }
        )"
    );
}

TEST(EXPR_FUNCTION_CALL, INVALID_RETURN_WRONG_ARRAY_DIMENSION_ONE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int[] foo() {
                return new int[3][3];
            }
        )"
    );
}

TEST(EXPR_FUNCTION_CALL, INVALID_RETURN_WRONG_ARRAY_DIMENSION_TWO) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int[] foo() {
                return 5;
            }
        )"
    );
}

TEST(EXPR_FUNCTION_CALL, INVALID_VOID_RETURN_TYPE_USED_IN_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            void foo() {}

            int x = foo();
        )"
    );
}

TEST(EXPR_FUNCTION_CALL, INVALID_WRONG_ARGUMENT_TYPE_ONE) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            int foo(int a) {
                return 5;
            }

            int x = foo(true);
        )"
    );
}

TEST(EXPR_FUNCTION_CALL, INVALID_WRONG_ARGUMENT_TYPE_TWO) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            int foo(int a) {
                return 5;
            }
            int[] arr = new int[3];
            int x = foo(arr);
        )"
    );
}

TEST(EXPR_FUNCTION_CALL, INVALID_WRONG_ARGUMENT_TYPE_THREE) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            int foo(int[] a) {
                return a[0];
            }

            int x = foo(4);
        )"
    );
}

TEST(EXPR_FUNCTION_CALL, INVALID_WRONG_NUMBER_OF_ARGUMENTS) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            int foo(int a) {
                return 5;
            }

            int x = foo();
        )"
    );
}

TEST(EXPR_FUNCTION_CALL, INVALID_RETURN_FUNCTION_CALL) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            bool foo() {
                return true;
            }

            int bar() {
                return foo();
            }

            int x = bar();
        )"
    );
}