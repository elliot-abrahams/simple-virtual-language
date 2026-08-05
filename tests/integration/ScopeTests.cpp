#include <gtest/gtest.h>
#include "IntegrationTestUtils.h"

using namespace integrationTests;

TEST(SCOPE, INNER_SCOPE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 5;
            {
                int y = 10;
                print(y);
            }
            print(x);
        )",
        "105"
    );
}

TEST(SCOPE, VARIALBE_SHADOWING) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 5;
            {
                int x = 10;
                print(x);
            }
            print(x);
        )",
        "105"
    );
}

TEST(SCOPE, NESTED_SCOPE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 1;
            {
                int y = 2;
                {
                    int z = 3;
                    print(x);
                    print(y);
                    print(z);
                }
            }
        )",
        "123"
    );
}