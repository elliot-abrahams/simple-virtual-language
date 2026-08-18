#include <gtest/gtest.h>
#include "../IntegrationTestUtils.h"

using namespace integrationTests;

TEST(EXPR_CAST, INT_TO_FLOAT) {
    ASSERT_OUTPUT_EQ(
        "print((float) 5);",
        "5.0"
    );
}

TEST(EXPR_CAST, FLOAT_TO_INT) {
    ASSERT_OUTPUT_EQ(
        "print((int) 5.0f);",
        "5"
    );
}