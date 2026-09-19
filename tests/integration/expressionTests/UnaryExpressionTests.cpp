#include <gtest/gtest.h>
#include "../IntegrationTestUtils.h"

using namespace integrationTests;

TEST(EXPR_UNARY, PLUS_INT) {
    ASSERT_OUTPUT_EQ(
        "print(+5);",
        "5"
    );
}

TEST(EXPR_UNARY, MINUS_INT) {
    ASSERT_OUTPUT_EQ(
        "print(-5);",
        "-5"
    );
}

TEST(EXPR_UNARY, PLUS_FLOAT) {
    ASSERT_OUTPUT_EQ(
        "print(+5.5f);",
        "5.5"
    );
}

TEST(EXPR_UNARY, MINUS_FLOAT) {
    ASSERT_OUTPUT_EQ(
        "print(-5.0f);",
        "-5.0"
    );
}

TEST(EXPR_UNARY, LOGICAL_NOT_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(!true);",
        "false"
    );
}

TEST(EXPR_UNARY, LOGICAL_NOT_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(!false);",
        "true"
    );
}

TEST(EXPR_UNARY, INCREMENT_INT_VARIABLE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 5;
            print(x);
            print(++x);
            print(x);
        )",
        "566"
    );
}

TEST(EXPR_UNARY, DECREMENT_INT_VARIABLE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 5;
            print(x);
            print(--x);
            print(x);
        )",
        "544"
    );
}

TEST(EXPR_UNARY, INCREMENT_FLOAT_VARIABLE) {
    ASSERT_OUTPUT_EQ(
        R"(
            float x = 5.5f;
            print(x);
            print(++x);
            print(x);
        )",
        "5.56.56.5"
    );
}

TEST(EXPR_UNARY, DECREMENT_FLOAT_VARIABLE) {
    ASSERT_OUTPUT_EQ(
        R"(
            float x = 5.5f;
            print(x);
            print(--x);
            print(x);
        )",
        "5.54.54.5"
    );
}

TEST(EXPR_UNARY, INCREMENT_ARRAY_ELEMENT) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[] arr = new int[1];
            print(arr[0]);
            print(++arr[0]);
            print(arr[0]);
        )",
        "011"
    );
}

TEST(EXPR_UNARY, DECREMENT_ARRAY_ELEMENT) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[] arr = new int[1];
            print(arr[0]);
            print(--arr[0]);
            print(arr[0]);
        )",
        "0-1-1"
    );
}