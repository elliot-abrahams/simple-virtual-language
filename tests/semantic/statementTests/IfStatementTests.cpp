#include <gtest/gtest.h>
#include "../SemanticTestUtils.h"

using namespace semanticTest;

TEST(STM_IF, VALID) {
    ASSERT_SEMANTICALLY_VALID(R"(
            if (true) {}
        )"
    );
}

TEST(STM_IF, INVALID_INT_CONDITION) {
    ASSERT_THROWS_TYPE_ERROR(R"(
            if (3) {}
        )"
    );
}

TEST(STM_IF, INVALID_FLOAT_CONDITION) {
    ASSERT_THROWS_TYPE_ERROR(R"(
            if (3.5f) {}
        )"
    );
}