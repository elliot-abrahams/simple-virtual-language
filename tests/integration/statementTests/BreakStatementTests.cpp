#include <gtest/gtest.h>
#include "../IntegrationTestUtils.h"

using namespace integrationTests;

TEST(STM_BREAK, WHILE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 0;
            while (true) {
                x = 5;
                break;
                x = 10;
            }
            print(x);
        )",
        "5"
    );
}

TEST(STM_BREAK, FOR) {
    ASSERT_OUTPUT_EQ(
        R"(
            int i;
            for (i : 0 .. 10) {
                if (i == 2) {
                    break;
                }
            }
            print(i);
        )",
        "2"
    );
}