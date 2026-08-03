#include <gtest/gtest.h>
#include "../semanticTestUtils.h"

using namespace semanticTest;

TEST(STM_WHILE, VALID) {
    ASSERT_SEMANTICALLY_VALID(R"(
            while (true) {}
        )"
    );
}

TEST(STM_WHILE, INVALID_INT_CONDITION) {
    ASSERT_THROWS_TYPE_ERROR(R"(
            while (3) {}
        )"
    );
}

TEST(STM_WHILE, INVALID_FLOAT_CONDITION) {
    ASSERT_THROWS_TYPE_ERROR(R"(
            while (3.5f) {}
        )"
    );
}