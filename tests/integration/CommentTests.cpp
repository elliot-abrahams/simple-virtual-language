#include <gtest/gtest.h>
#include "IntegrationTestUtils.h"

using namespace integrationTests;

TEST(COMMENT, SINGLE_LINE_COMMENT) {
    ASSERT_OUTPUT_EQ(
        R"(
            #### comment
            int x = 10;
            # x = 5;
            print(x); # print result
        )",
        "10"
    );
}