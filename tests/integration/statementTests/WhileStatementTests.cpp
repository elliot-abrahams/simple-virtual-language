#include <gtest/gtest.h>
#include "../IntegrationTestUtils.h"

using namespace integrationTests;

TEST(STM_WHILE, RUN_LOOP) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 0;
            while (x <= 2) {
                print(x);
                x = x + 1;
            }
            print(0);
        )",
        "0120"
    );
}

TEST(STM_WHILE, SKIP_LOOP) {
    ASSERT_OUTPUT_EQ(
        R"(
            while (false) {
                print(5);
            }
            print(0);
        )",
        "0"
    );
}

TEST(STM_WHILE, BREAK) {
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

TEST(STM_WHILE, CONTINUE) {
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