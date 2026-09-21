#include <gtest/gtest.h>
#include "../IntegrationTestUtils.h"

using namespace integrationTests;

TEST(EXPR_STM, PRE_INCREMENT_INT_VARIABLE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 5;
            ++x;
            print(x);
        )",
        "6"
    );
}

TEST(EXPR_STM, POST_INCREMENT_INT_VARIABLE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 5;
            x++;
            print(x);
        )",
        "6"
    );
}

TEST(EXPR_STM, PRE_DECREMENT_INT_VARIABLE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 5;
            --x;
            print(x);
        )",
        "4"
    );
}

TEST(EXPR_STM, POST_DECREMENT_INT_VARIABLE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 5;
            x--;
            print(x);
        )",
        "4"
    );
}

TEST(EXPR_STM, PRE_INCREMENT_FLOAT_VARIABLE) {
    ASSERT_OUTPUT_EQ(
        R"(
            float x = 5.5f;
            ++x;
            print(x);
        )",
        "6.5"
    );
}

TEST(EXPR_STM, POST_INCREMENT_FLOAT_VARIABLE) {
    ASSERT_OUTPUT_EQ(
        R"(
            float x = 5.5f;
            x++;
            print(x);
        )",
        "6.5"
    );
}

TEST(EXPR_STM, PRE_DECREMENT_FLOAT_VARIABLE) {
    ASSERT_OUTPUT_EQ(
        R"(
            float x = 5.5f;
            --x;
            print(x);
        )",
        "4.5"
    );
}

TEST(EXPR_STM, POST_DECREMENT_FLOAT_VARIABLE) {
    ASSERT_OUTPUT_EQ(
        R"(
            float x = 5.5f;
            x--;
            print(x);
        )",
        "4.5"
    );
}

TEST(EXPR_STM, PRE_INCREMENT_ARRAY_ELEMENT) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[] arr = new int[1];
            ++arr[0];
            print(arr[0]);
        )",
        "1"
    );
}

TEST(EXPR_STM, POST_INCREMENT_ARRAY_ELEMENT) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[] arr = new int[1];
            arr[0]++;
            print(arr[0]);
        )",
        "1"
    );
}

TEST(EXPR_STM, PRE_DECREMENT_ARRAY_ELEMENT) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[] arr = new int[1];
            --arr[0];
            print(arr[0]);
        )",
        "-1"
    );
}

TEST(EXPR_STM, POST_DECREMENT_ARRAY_ELEMENT) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[] arr = new int[1];
            arr[0]--;
            print(arr[0]);
        )",
        "-1"
    );
}