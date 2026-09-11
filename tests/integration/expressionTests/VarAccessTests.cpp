#include <gtest/gtest.h>

#include "../IntegrationTestUtils.h"

using namespace integrationTests;

TEST(EXPR_VAR_ACCESS, INT) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 5;
            print(x);
        )",
        "5"
    );
}

TEST(EXPR_VAR_ACCESS, FLOAT) {
    ASSERT_OUTPUT_EQ(
        R"(
            float x = 5.5f;
            print(x);
        )",
        "5.5"
    );
}

TEST(EXPR_VAR_ACCESS, BOOL) {
    ASSERT_OUTPUT_EQ(
        R"(
            bool x = true;
            print(x);
        )",
        "true"
    );
}
