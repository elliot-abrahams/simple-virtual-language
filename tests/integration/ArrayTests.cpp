#include <gtest/gtest.h>
#include "IntegrationTestUtils.h"

using namespace integrationTests;

TEST(ARRAY, INT_ARRAY_1D) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[] arr = new int[3];
            print(arr[0]);
        )",
        "0"
    );
}

TEST(ARRAY, FLOAT_ARRAY_1D) {
    ASSERT_OUTPUT_EQ(
        R"(
            float[] arr = new float[3];
            print(arr[0]);
        )",
        "0.0"
    );
}

TEST(ARRAY, BOOL_ARRAY_1D) {
    ASSERT_OUTPUT_EQ(
        R"(
            bool[] arr = new bool[3];
            print(arr[0]);
        )",
        "false"
    );
}

TEST(ARRAY, INT_ARRAY_2D) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[][] arr = new int[3][3];
            print(arr[0][0]);
        )",
        "0"
    );
}

TEST(ARRAY, FLOAT_ARRAY_2D) {
    ASSERT_OUTPUT_EQ(
        R"(
            float[][] arr = new float[3][3];
            print(arr[0][0]);
        )",
        "0.0"
    );
}

TEST(ARRAY, BOOL_ARRAY_2D) {
    ASSERT_OUTPUT_EQ(
        R"(
            bool[][] arr = new bool[3][3];
            print(arr[0][0]);
        )",
        "false"
    );
}

TEST(ARRAY, INT_ARRAY_1D_INIT_ONE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[] arr = new int[3]{};
            print(arr[0]);
        )",
        "0"
    );
}

TEST(ARRAY, INT_ARRAY_1D_INIT_TWO) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[] arr = new int[3]{1};
            print(arr[0]);
            print(arr[1]);
            print(arr[2]);
        )",
        "100"
    );
}

TEST(ARRAY, INT_ARRAY_1D_INIT_THREE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[] arr = new int[3]{1, 2};
            print(arr[0]);
            print(arr[1]);
            print(arr[2]);
        )",
        "120"
    );
}

TEST(ARRAY, INT_ARRAY_1D_INIT_FOUR) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[] arr = new int[3]{1, 2, 3};
            print(arr[0]);
            print(arr[1]);
            print(arr[2]);
        )",
        "123"
    );
}

TEST(ARRAY, FLOAT_ARRAY_1D_INIT) {
    ASSERT_OUTPUT_EQ(
        R"(
            float[] arr = new float[3]{1.0f};
            print(arr[0]);
        )",
        "1.0"
    );
}

TEST(ARRAY, FLOAT_ARRAY_1D_INIT_IMPLICIT_CONVERSION) {
    ASSERT_OUTPUT_EQ(
        R"(
            float[] arr = new float[3]{1};
            print(arr[0]);
        )",
        "1.0"
    );
}

TEST(ARRAY, INT_ARRAY_2D_INIT) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[][] arr = new int[2][2]{
                {1, 2},
                {4, 5}
            };
            print(arr[0][0]);
            print(arr[0][1]);
            print(arr[1][0]);
            print(arr[1][1]);
        )",
        "1245"
    );
}

TEST(ARRAY, INT_ARRAY_6D_INIT) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[][][][][][] arr = new int[1][1][1][1][1][1]{{{{{{10}}}}}};
            print(arr[0][0][0][0][0][0]);
        )",
        "10"
    );
}

TEST(ARRAY, INT_ARRAY_1D_ASSIGN_ELEMENT) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[] arr = new int[3];

            arr[0] = 1;
            arr[1] = 3;
            arr[2] = 5;

            print(arr[0]);
            print(arr[1]);
            print(arr[2]);
        )",
        "135"
    );
}

TEST(ARRAY, INT_ARRAY_2D_ASSIGN_ELEMENT) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[][] arr = new int[2][2];

            arr[0][0] = 1;
            arr[0][1] = 3;
            arr[1][0] = 5;
            arr[1][1] = 7;

            print(arr[0][0]);
            print(arr[0][1]);
            print(arr[1][0]);
            print(arr[1][1]);
        )",
        "1357"
    );
}

TEST(ARRAY, INT_ARRAY_1D_ASSIGN_REFERENCE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[] arr = new int[2]{5, 6};
            int[] copy = arr;

            arr[0] = 9;

            print(arr[0]);
            print(arr[1]);
            print(copy[0]);
            print(copy[1]);
        )",
        "9696"
    );
}

TEST(ARRAY, INT_ARRAY_2D_ASSIGN_REFERENCE) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[][] arr = new int[2][2]{{5, 6}, {7, 8}};
            int[] arr2 = arr[0];

            arr2[0] = 9;

            print(arr[0][0]);
            print(arr[0][1]);
            print(arr2[0]);
            print(arr2[1]);
        )",
        "9696"
    );
}

TEST(ARRAY, INT_ARRAY_FUNCTION_ARGUMENT) {
    ASSERT_OUTPUT_EQ(
        R"(
            int foo(int[] x) {
                return x[0];
            }
            print(foo( new int[1]{5} ));
        )",
        "5"
    );
}

TEST(ARRAY, INT_ARRAY_FUNCTION_RETURN) {
    ASSERT_OUTPUT_EQ(
        R"(
            int[] foo() {
                return new int[2]{5, 6};
            }
            print(foo()[0]);
        )",
        "5"
    );
}

TEST(ARRAY, NEW_EXPESSION_WITH_INDEX) {
    ASSERT_OUTPUT_EQ(
        "print(new int[3]{1, 2, 3}[1]);",
        "2"
    );
}

TEST(ARRAY, INVALID_NEGATIVE_ARRAY_SIZE) {
    ASSERT_THROWS_RUNTIME_ERROR(
        R"(
            int[] arr = new int[-1];
        )",
        RuntimeErrorType::EXPLICIT_NEGATIVE_ARRAY_LENGTH,
        VMTestScenario::NONE
    );
}

TEST(ARRAY, INVALID_1D_ARRAY_INITIALISER_HAS_TOO_MANY_ELEMENTS) {
    ASSERT_THROWS_RUNTIME_ERROR(
        R"(
            int[] arr = new int[3]{1, 2, 3, 4};
        )",
        RuntimeErrorType::EXPLICIT_ARRAY_INITIALISER_LENGTH,
        VMTestScenario::NONE
    );
}

TEST(ARRAY, INVALID_2D_ARRAY_INITIALISER_HAS_TOO_MANY_ELEMENTS_ONE) {
    ASSERT_THROWS_RUNTIME_ERROR(
        R"(
            int[][] arr = new int[3][3]{{}, {}, {}, {}};
        )",
        RuntimeErrorType::EXPLICIT_ARRAY_INITIALISER_LENGTH,
        VMTestScenario::NONE
    );
}

TEST(ARRAY, INVALID_2D_ARRAY_INITIALISER_HAS_TOO_MANY_ELEMENTS_TWO) {
    ASSERT_THROWS_RUNTIME_ERROR(
        R"(
            int[][] arr = new int[3][3]{{1, 2, 3, 4}};
        )",
        RuntimeErrorType::EXPLICIT_ARRAY_INITIALISER_LENGTH,
        VMTestScenario::NONE
    );
}

TEST(ARRAY, INVALID_ARRAY_ACCESS_INDEX_OUT_OF_RANGE_ONE) {
    ASSERT_THROWS_RUNTIME_ERROR(
        R"(
            int[] arr = new int[3];
            print(arr[-1]);
        )",
        RuntimeErrorType::EXPLICIT_ARRAY_INDEX_OUT_OF_RANGE,
        VMTestScenario::NONE
    );
}

TEST(ARRAY, INVALID_ARRAY_ACCESS_INDEX_OUT_OF_RANGE_TWO) {
    ASSERT_THROWS_RUNTIME_ERROR(
        R"(
            int[] arr = new int[3];
            print(arr[3]);
        )",
        RuntimeErrorType::EXPLICIT_ARRAY_INDEX_OUT_OF_RANGE,
        VMTestScenario::NONE
    );
}

TEST(ARRAY, INVALID_ARRAY_ASSIGN_INDEX_OUT_OF_RANGE_ONE) {
    ASSERT_THROWS_RUNTIME_ERROR(
        R"(
            int[] arr = new int[3];
            arr[-1] = 4;
        )",
        RuntimeErrorType::EXPLICIT_ARRAY_INDEX_OUT_OF_RANGE,
        VMTestScenario::NONE
    );
}

TEST(ARRAY, INVALID_ARRAY_ASSIGN_INDEX_OUT_OF_RANGE_TWO) {
    ASSERT_THROWS_RUNTIME_ERROR(
        R"(
            int[] arr = new int[3];
            arr[3] = 4;
        )",
        RuntimeErrorType::EXPLICIT_ARRAY_INDEX_OUT_OF_RANGE,
        VMTestScenario::NONE
    );
}
