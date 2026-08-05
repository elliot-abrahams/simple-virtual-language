#include <gtest/gtest.h>
#include "IntegrationTestUtils.h"

using namespace integrationTests;

TEST(FUNCTION, RETURN_VOID) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 0;
            void foo() {
                x = 10;
            }
            foo();
            print(x);
        )",
        "10"
    );
}

TEST(FUNCTION, RETURN_INT) {
    ASSERT_OUTPUT_EQ(
        R"(
            int foo() {
                return 5;
            }
            print(foo());
        )",
        "5"
    );
}

TEST(FUNCTION, RETURN_FLOAT) {
    ASSERT_OUTPUT_EQ(
        R"(
            float foo() {
                return 5.5f;
            }
            print(foo());
        )",
        "5.5"
    );
}

TEST(FUNCTION, RETURN_BOOL) {
    ASSERT_OUTPUT_EQ(
        R"(
            bool foo() {
                return false;
            }
            print(foo());
        )",
        "false"
    );
}

TEST(FUNCTION, RETURN_INT_WITH_PARAMETER) {
    ASSERT_OUTPUT_EQ(
        R"(
            int foo(int a) {
                return a;
            }
            print(foo(6));
        )",
        "6"
    );
}

TEST(FUNCTION, RETURN_VOID_WITH_PARAMETERS) {
    ASSERT_OUTPUT_EQ(
        R"(
            void foo(int a, float b) {
                print(a);
                print(b);
            }
            foo(5, 6.5f);
        )",
        "56.5"
    );
}

TEST(FUNCTION, RETURN_WITH_VOID_RETURN_TYPE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 0;
            void foo() {
                x = 5;
                return;
                x = 10;
            }
            foo();
            print(x);
        )",
        "5"
    );
}

TEST(FUNCTION, NESTED_CALL) {
    ASSERT_OUTPUT_EQ(
        R"(
            int add(int a, int b) {
                return a + b;
            }
            print(add(add(1, 2), 3));
        )",
        "6"
    );
}

TEST(FUNCTION, RECURSION_RETURN_VOID) {
    ASSERT_OUTPUT_EQ(
        R"(
            void countdown(int n) {
                print(n);

                if (n > 0) {
                    countdown(n - 1);
                }
            }

            countdown(4);
        )",
        "43210"
    );
}

TEST(FUNCTION, RECURSION_RETURN_INT) {
    ASSERT_OUTPUT_EQ(
        R"(
            int factorial(int n) {
                if (n == 0) {
                    return 1;
                }

                return n * factorial(n - 1);
            }

            print(factorial(5));
        )",
        "120"
    );
}

TEST(FUNCTION, OVERLOADING) {
    ASSERT_OUTPUT_EQ(
        R"(
            int foo(int a) {
                return 5;
            }

            int foo(float a) {
                return 6;
            }

            print(foo(1));
            print(foo(1.5f));
        )",
        "56"
    );
}