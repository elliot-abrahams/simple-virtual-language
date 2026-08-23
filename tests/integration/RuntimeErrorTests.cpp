#include <gtest/gtest.h>
#include  "IntegrationTestUtils.h"


using namespace integrationTests;

TEST(RUNTIME_ERROR, DIVISION_BY_ZERO_ERROR_DIVIDE_INTEGER) {
    ASSERT_THROWS_RUNTIME_ERROR(
        R"(
            float x = 10 / 0;
        )",
        RuntimeErrorType::DIVISION_BY_ZERO,
        VMTestScenario::NONE
    );
}

TEST(RUNTIME_ERROR, DIVISION_BY_ZERO_ERROR_INTEGER_DIVIDE_INTEGER) {
    ASSERT_THROWS_RUNTIME_ERROR(
        R"(
            int x = 10 // 0;
        )",
        RuntimeErrorType::DIVISION_BY_ZERO,
        VMTestScenario::NONE
    );
}

TEST(RUNTIME_ERROR, DIVISION_BY_ZERO_ERROR_MODULO_INTEGER) {
    ASSERT_THROWS_RUNTIME_ERROR(
        R"(
            float x = 10 % 0;
        )",
        RuntimeErrorType::DIVISION_BY_ZERO,
        VMTestScenario::NONE
    );
}

TEST(RUNTIME_ERROR, DIVISION_BY_ZERO_ERROR_DIVIDE_FLOAT) {
    ASSERT_THROWS_RUNTIME_ERROR(
        R"(
            float x = 10 / 0.0f;
        )",
        RuntimeErrorType::DIVISION_BY_ZERO,
        VMTestScenario::NONE
    );
}

TEST(RUNTIME_ERROR, DIVISION_BY_ZERO_ERROR_INTEGER_DIVIDE_FLOAT) {
    ASSERT_THROWS_RUNTIME_ERROR(
        R"(
            int x = 10 // 0.0f;
        )",
        RuntimeErrorType::DIVISION_BY_ZERO,
        VMTestScenario::NONE
    );
}

TEST(RUNTIME_ERROR, DIVISION_BY_ZERO_ERROR_MODULO_FLOAT) {
    ASSERT_THROWS_RUNTIME_ERROR(
        R"(
            float x = 10 % 0.0f;
        )",
        RuntimeErrorType::DIVISION_BY_ZERO,
        VMTestScenario::NONE
    );
}

TEST(RUNTIME_ERROR, OUT_OF_RANGE_ERROR_FLOAT_TO_INT) {
    ASSERT_THROWS_RUNTIME_ERROR(
        R"(
            int x = (int)2147483648.0f;
        )",
        RuntimeErrorType::OUT_OF_RANGE,
        VMTestScenario::NONE
    );
}

TEST(RUNTIME_ERROR, OUT_OF_RANGE_ERROR_FLOAT_TO_INT_NEGATIVE) {
    ASSERT_THROWS_RUNTIME_ERROR(
        R"(
            int x = (int)-3000000000.0f;
        )", // -2147483649.0f rounds to -2147483648.0f when represented as float,
        // so I used a value clearly outside the int range.
        RuntimeErrorType::OUT_OF_RANGE,
        VMTestScenario::NONE
    );
}

TEST(RUNTIME_ERROR, STACK_OVERFLOW_ERROR) {
    ASSERT_THROWS_RUNTIME_ERROR(
        R"(
            void rec(int a) {
                rec(a);
            }
            rec(5);
        )",
        RuntimeErrorType::STACK_OVERFLOW,
        VMTestScenario::NEAR_CALL_STACK_OVERFLOW
    );
}
