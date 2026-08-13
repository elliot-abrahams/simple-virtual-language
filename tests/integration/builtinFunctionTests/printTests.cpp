#include <gtest/gtest.h>
#include "../IntegrationTestUtils.h"

using namespace integrationTests;

TEST(PRINT, INT) {
    ASSERT_OUTPUT_EQ(
        "print(5);",
        "5"
    );
}

TEST(PRINT, INT_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
    "print(-5);",
        "-5"
    );
}

TEST(PRINT, FLOAT) {
    ASSERT_OUTPUT_EQ(
    "print(5.5f);",
        "5.5"
    );
}

TEST(PRINT, FLOAT_NEGATIVE) {
    ASSERT_OUTPUT_EQ(
    "print(-5.5f);",
        "-5.5"
    );
}

TEST(PRINT, BOOL_TRUE) {
    ASSERT_OUTPUT_EQ(
        "print(true);",
        "true"
    );
}

TEST(PRINT, BOOL_FALSE) {
    ASSERT_OUTPUT_EQ(
        "print(false);",
        "false"
    );
}