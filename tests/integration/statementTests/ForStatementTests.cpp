#include <gtest/gtest.h>
#include "../IntegrationTestUtils.h"

using namespace integrationTests;

TEST(FOR, RANGE_INT) {
    ASSERT_OUTPUT_EQ(
        R"(
            for (int i : 0 .. 4) {
                print(i);
            }
        )",
        "0123"
    );
}

TEST(FOR, RANGE_STEP_INT_ONE) {
    ASSERT_OUTPUT_EQ(
        R"(
            for (int i : 0 .. 4 : 2) {
                print(i);
            }
        )",
        "02"
    );
}

TEST(FOR, RANGE_STEP_INT_TWO) {
    ASSERT_OUTPUT_EQ(
        R"(
            for (int i : 0 .. 5 : 2) {
                print(i);
            }
        )",
        "024"
    );
}

TEST(FOR, RANGE_NEGATIVE_STEP_INT_DECL) {
    ASSERT_OUTPUT_EQ(
        R"(
            for (int i : 4 .. 0 : -1) {
                print(i);
            }
        )",
        "4321"
    );
}

TEST(FOR, RANGE_INT_OUTER_SCOPE_DECL) {
    ASSERT_OUTPUT_EQ(
        R"(
            int i;
            for (i : 0 .. 4) {
                print(i);
            }
            print(i);
        )",
        "01233"
    );
}

TEST(FOR, RANGE_INT_INITIALISED) {
    ASSERT_OUTPUT_EQ(
        R"(
            int i = 5;
            for (i : 0 .. 4) {
                print(i);
            }
            print(i);
        )",
        "01233"
    );
}

TEST(FOR, RANGE_IMPLICIT_CHAR_TO_INT) {
    ASSERT_OUTPUT_EQ(
        R"(
            for (int i : 'a' .. 'd') {
                print(i);
            }
        )",
        "979899"
    );
}

TEST(FOR, RANGE_STEP_IMPLICIT_CHAR_TO_INT) {
    ASSERT_OUTPUT_EQ(
        R"(
            for (int i : 'a' .. 'd' : (char)2) {
                print(i);
            }
        )",
        "9799"
    );
}

TEST(FOR, RANGE_FLOAT) {
    ASSERT_OUTPUT_EQ(
        R"(
            for (float i : 0.5f .. 2.2f) {
                print(i);
            }
        )",
        "0.51.5"
    );
}

TEST(FOR, RANGE_STEP_FLOAT) {
    ASSERT_OUTPUT_EQ(
        R"(
            for (float i : 0.5f .. 2.2f : 0.5f) {
                print(i);
            }
        )",
        "0.51.01.52.0"
    );
}

TEST(FOR, RANGE_IMPLICIT_INT_TO_FLOAT) {
    ASSERT_OUTPUT_EQ(
        R"(
            for (float i : 1 .. 3) {
                print(i);
            }
        )",
        "1.02.0"
    );
}

TEST(FOR, RANGE_STEP_IMPLICIT_INT_TO_FLOAT) {
    ASSERT_OUTPUT_EQ(
        R"(
            for (float i : 1 .. 5 : 2) {
                print(i);
            }
        )",
        "1.03.0"
    );
}

TEST(FOR, RANGE_CHAR) {
    ASSERT_OUTPUT_EQ(
        R"(
            for (char c : 'A' .. 'D') {
                print(c);
            }
        )",
        "ABC"
    );
}

TEST(FOR, RANGE_STEP_CHAR) {
    ASSERT_OUTPUT_EQ(
        R"(
            for (char c : 'A' .. 'E' : (char)2) {
                print(c);
            }
        )",
        "AC"
    );
}

TEST(FOR, ITERATOR_INT) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[] arr = new int[4]{2, 4, 6, 8};
            for (int i : arr) {
                print(i);
            }
        )",
        "2468"
    );
}

TEST(FOR, ITERATOR_FLOAT) {
    ASSERT_OUTPUT_EQ(
        R"(
            float[] arr = new float[4]{2.0f, 4.0f, 6.0f, 8.0f};
            for (float i : arr) {
                print(i);
            }
        )",
        "2.04.06.08.0"
    );
}

TEST(FOR, ITERATOR_BOOL) {
    ASSERT_OUTPUT_EQ(
        R"(
            bool[] arr = new bool[3]{true, false, false};
            for (bool i : arr) {
                print(i);
            }
        )",
        "truefalsefalse"
    );
}

TEST(FOR, ITERATOR_CHAR) {
    ASSERT_OUTPUT_EQ(
        R"(
            char[] arr = new char[6]{'H', 'e', 'l', 'l', 'o', '!'};
            for (char i : arr) {
                print(i);
            }
        )",
        "Hello!"
    );
}

TEST(FOR, ITERATOR_INT_2D) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[][] arr = new int[3][3]{
                            { 9, 8, 7 },
                            { 6, 5, 4 },
                            { 3, 2, 1 }};
            for (int[] i : arr) {
                for (int j : i) {
                    print(j);
                }
            }
        )",
        "987654321"
    );
}