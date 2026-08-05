#include <gtest/gtest.h>
#include "../IntegrationTestUtils.h"

using namespace integrationTests;

TEST(STM_IF, IF_TRUE) {
    ASSERT_OUTPUT_EQ(
        R"(
            if (true) {
                print(1);
            }
            print(2);
        )",
        "12"
    );
}

TEST(STM_IF, IF_FALSE) {
    ASSERT_OUTPUT_EQ(
        R"(
            if (false) {
                print(1);
            }
            print(2);
        )",
        "2"
    );
}

TEST(STM_IF, IF_ELSE_TRUE) {
    ASSERT_OUTPUT_EQ(
        R"(
            if (true) {
                print(1);
            } else {
                print(2);
            }
            print(3);
        )",
        "13"
    );
}

TEST(STM_IF, IF_ELSE_FALSE) {
    ASSERT_OUTPUT_EQ(
        R"(
            if (false) {
                print(1);
            } else {
                print(2);
            }
            print(3);
        )",
        "23"
    );
}

TEST(STM_IF, IF_ELSE_IF_ELSE_TRUE_TRUE) {
    ASSERT_OUTPUT_EQ(
        R"(
            if (true) {
                print(1);
            } else if (true) {
                print(2);
            } else {
                print(3);
            }
            print(4);
        )",
        "14"
    );
}

TEST(STM_IF, IF_ELSE_IF_ELSE_TRUE_FALSE) {
    ASSERT_OUTPUT_EQ(
        R"(
            if (true) {
                print(1);
            } else if (false) {
                print(2);
            } else {
                print(3);
            }
            print(4);
        )",
        "14"
    );
}

TEST(STM_IF, IF_ELSE_IF_ELSE_FALSE_TRUE) {
    ASSERT_OUTPUT_EQ(
        R"(
            if (false) {
                print(1);
            } else if (true) {
                print(2);
            } else {
                print(3);
            }
            print(4);
        )",
        "24"
    );
}

TEST(STM_IF, IF_ELSE_IF_ELSE_FALSE_FALSE) {
    ASSERT_OUTPUT_EQ(
        R"(
            if (false) {
                print(1);
            } else if (false) {
                print(2);
            } else {
                print(3);
            }
            print(4);
        )",
        "34"
    );
}