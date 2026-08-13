#include <gtest/gtest.h>
#include "../IntegrationTestUtils.h"

using namespace integrationTests;

TEST(EXIT, INT) {
    ASSERT_EXIT_CODE_EQ(
        "exit(5);",
        5
    );
}

TEST(EXIT, INT_NEGATIVE) {
    ASSERT_EXIT_CODE_EQ(
        "exit(-5);",
        -5
    );
}
