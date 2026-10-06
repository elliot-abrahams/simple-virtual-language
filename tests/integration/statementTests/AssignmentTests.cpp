#include <gtest/gtest.h>
#include "../IntegrationTestUtils.h"

using namespace integrationTests;

TEST(STM_ASSIGN, INT) {
    ASSERT_OUTPUT_EQ(
        R"(
            int i = 5;
            print(i);
        )",
        "5"
    );
}

TEST(STM_ASSIGN, IMPLICIT_CHAR_TO_INT) {
    ASSERT_OUTPUT_EQ(
        R"(
            int i = 'A';
            print(i);
        )",
        "65"
    );
}

TEST(STM_ASSIGN, FLOAT) {
    ASSERT_OUTPUT_EQ(
        R"(
            float f = 5.0f;
            print(f);
        )",
        "5.0"
    );
}

TEST(STM_ASSIGN, IMPLICIT_INT_TO_FLOAT) {
    ASSERT_OUTPUT_EQ(
        R"(
            float f = 5;
            print(f);
        )",
        "5.0"
    );
}

TEST(STM_ASSIGN, IMPLICIT_CHAR_TO_FLOAT) {
    ASSERT_OUTPUT_EQ(
        R"(
            float f = 'A';
            print(f);
        )",
        "65.0"
    );
}

TEST(STM_ASSIGN, BOOL) {
    ASSERT_OUTPUT_EQ(
        R"(
            bool b = true;
            print(b);
        )",
        "true"
    );
}

TEST(STM_ASSIGN, CHAR) {
    ASSERT_OUTPUT_EQ(
        R"(
            char c = 'h';
            print(c);
        )",
        "h"
    );
}

TEST(STM_ASSIGN, INT_ASSIGN_WITH_VARIABLE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = 5;
            int y = x;
            print(y);
        )",
        "5"
    );
}

TEST(STM_ASSIGN, MUTIPLE_VARIABLES) {
    ASSERT_OUTPUT_EQ(
        R"(
            int a = 5;
            int b = 8;
            print(a);
            print(b);
        )",
        "58"
    );
}