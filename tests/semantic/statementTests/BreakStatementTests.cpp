#include <gtest/gtest.h>
#include "../SemanticTestUtils.h"

using namespace semanticTest;

TEST(STM_BREAK, WHILE) {
    ASSERT_SEMANTICALLY_VALID(R"(
            while (true) {
                break;
            }
        )"
    );
}

TEST(STM_BREAK, FOR) {
    ASSERT_SEMANTICALLY_VALID(R"(
            for (int i : 0 .. 10) {
                break;
            }
        )"
    );
}

TEST(STM_BREAK, INVALID_OUTSIDE_OF_LOOP) {
    ASSERT_THROWS_SEMANTIC_ERROR(R"(
            break;
        )"
    );
}