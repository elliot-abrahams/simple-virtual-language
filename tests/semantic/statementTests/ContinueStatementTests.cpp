#include <gtest/gtest.h>
#include "../SemanticTestUtils.h"

using namespace semanticTest;

TEST(STM_CONTINUE, VALID) {
    ASSERT_SEMANTICALLY_VALID(R"(
            while (true) {
                continue;
            }
        )"
    );
}

TEST(STM_CONTINUE, INVALID_OUTSIDE_OF_LOOP) {
    ASSERT_THROWS_SEMANTIC_ERROR(R"(
            continue;
        )"
    );
}