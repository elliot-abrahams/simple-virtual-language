#include <gtest/gtest.h>
#include "../IntegrationTestUtils.h"

using namespace integrationTests;

TEST(STM_CONTINUE, WHILE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 0;
            while (x <= 3) {
                print(x);
                x = x + 1;
                continue;
                x = x + 1;
            }
        )",
        "0123"
    );
}

TEST(STM_CONTINUE, FOR) {
    ASSERT_OUTPUT_EQ(
        R"(
            int j = 10;
            for (int i : 0 .. 3) {
                continue;
                j++;
            }
            print(j);
        )",
        "10"
    );
}