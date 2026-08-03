#include <gtest/gtest.h>
#include "../SemanticTestUtils.h"

using namespace semanticTest;

TEST(STM_FUNCTION_CALL, VOID_RETURN_TYPE) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            void foo() {

            }

            foo();
        )"
    );
}

TEST(STM_FUNCTION_CALL, INVALID_INT_RETURN_TYPE) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            int foo() {
                return 5;
            }

            foo();
        )"
    );
}

TEST(STM_FUNCTION_CALL, INVALID_FLOAT_RETURN_TYPE) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            float foo() {
                return 5.5f;
            }

            foo();
        )"
    );
}