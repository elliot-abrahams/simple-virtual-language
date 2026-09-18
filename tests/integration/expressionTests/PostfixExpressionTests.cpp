#include <gtest/gtest.h>
#include "../IntegrationTestUtils.h"

using namespace integrationTests;

TEST(EXPR_POSTFIX, INCREMENT_VARIABLE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 5;
            print(x++);
            print(x);
        )",
        "56"
    );
}

TEST(EXPR_POSTFIX, DECREMENT_VARIABLE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 5;
            print(x--);
            print(x);
        )",
        "54"
    );
}

TEST(EXPR_POSTFIX, INCREMENT_ARRAY_ELEMENT) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[] arr = new int[1];
            print(arr[0]++);
            print(arr[0]);
        )",
        "01"
    );
}

TEST(EXPR_POSTFIX, DECREMENT_ARRAY_ELEMENT) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[] arr = new int[1];
            print(arr[0]--);
            print(arr[0]);
        )",
        "0-1"
    );
}
