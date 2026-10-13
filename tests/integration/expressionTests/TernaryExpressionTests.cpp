#include <gtest/gtest.h>
#include "../IntegrationTestUtils.h"

using namespace integrationTests;

TEST(TERNARY_EXPR, INT_TRUE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = true ? 1 : 2;
            print(x);
        )",
        "1"
    );
}

TEST(TERNARY_EXPR, INT_FALSE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = false ? 1 : 2;
            print(x);
        )",
        "2"
    );
}

TEST(TERNARY_EXPR, FLOAT_TRUE) {
    ASSERT_OUTPUT_EQ(
        R"(
            float x = true ? 1.5f : 2.5f;
            print(x);
        )",
        "1.5"
    );
}

TEST(TERNARY_EXPR, FLOAT_FALSE) {
    ASSERT_OUTPUT_EQ(
        R"(
            float x = false ? 1.5f : 2.5f;
            print(x);
        )",
        "2.5"
    );
}

TEST(TERNARY_EXPR, BOOL_TRUE) {
    ASSERT_OUTPUT_EQ(
        R"(
            bool x = true ? true : false;
            print(x);
        )",
        "true"
    );
}

TEST(TERNARY_EXPR, BOOL_FALSE) {
    ASSERT_OUTPUT_EQ(
        R"(
            bool x = false ? true : false;
            print(x);
        )",
        "false"
    );
}

TEST(TERNARY_EXPR, CHAR_TRUE) {
    ASSERT_OUTPUT_EQ(
        R"(
            char x = true ? 'a' : 'b';
            print(x);
        )",
        "a"
    );
}

TEST(TERNARY_EXPR, CHAR_FALSE) {
    ASSERT_OUTPUT_EQ(
        R"(
            char x = false ? 'a' : 'b';
            print(x);
        )",
        "b"
    );
}

TEST(TERNARY_EXPR, IMPLICIT_INT_TO_FLOAT_TRUE) {
    ASSERT_OUTPUT_EQ(
        R"(
            float x = true ? 1.5f : 1;
            print(x);
        )",
        "1.5"
    );
}

TEST(TERNARY_EXPR, IMPLICIT_INT_TO_FLOAT_FALSE) {
    ASSERT_OUTPUT_EQ(
        R"(
            float x = false ? 1 : 1.5f;
            print(x);
        )",
        "1.5"
    );
}

TEST(TERNARY_EXPR, IMPLICIT_CHAR_TO_INT_TRUE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = true ? 'a' : 1;
            print(x);
        )",
        "97"
    );
}

TEST(TERNARY_EXPR, IMPLICIT_CHAR_TO_INT_FALSE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int x = false ? 1 : 'a';
            print(x);
        )",
        "97"
    );
}

TEST(TERNARY_EXPR, IMPLICIT_CHAR_TO_FLOAT_TRUE) {
    ASSERT_OUTPUT_EQ(
        R"(
            float x = true ? 'a' : 1.5f;
            print(x);
        )",
        "97.0"
    );
}

TEST(TERNARY_EXPR, IMPLICIT_CHAR_TO_FLOAT_FALSE) {
    ASSERT_OUTPUT_EQ(
        R"(
            float x = false ? 1.5f : 'a';
            print(x);
        )",
        "97.0"
    );
}

