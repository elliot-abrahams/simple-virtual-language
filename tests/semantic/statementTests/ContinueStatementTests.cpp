#include <gtest/gtest.h>
#include "../SemanticTestUtils.h"

using namespace semanticTest;

TEST(STM_CONTINUE, WHILE) {
    ASSERT_SEMANTICALLY_VALID(R"(
            while (true) {
                continue;
            }
        )"
    );
}

TEST(STM_CONTINUE, FOR) {
    ASSERT_SEMANTICALLY_VALID(R"(
            for (int i : 0 .. 10) {
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