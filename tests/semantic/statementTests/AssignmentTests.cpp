#include <gtest/gtest.h>
#include "../SemanticTestUtils.h"

using namespace semanticTest;

TEST(STM_ASSIGN, INT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x;
            x = 5;
        )"
    );
}

TEST(STM_ASSIGN, IMPLICIT_CHAR_TO_INT) {
    ASSERT_SEMANTICALLY_VALID(R"(
            int x;
            x = 'a';
        )"
    );
}

TEST(STM_ASSIGN, FLOAT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            float x;
            x = 5.5f;
        )"
    );
}

TEST(STM_ASSIGN, IMPLICIT_INT_TO_FLOAT) {
    ASSERT_SEMANTICALLY_VALID(R"(
            float x;
            x = 5;
        )"
    );
}

TEST(STM_ASSIGN, IMPLICIT_CHAR_TO_FLOAT) {
    ASSERT_SEMANTICALLY_VALID(R"(
            float x;
            x = 'a';
        )"
    );
}

TEST(STM_ASSIGN, BOOL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            bool x;
            x = true;
        )"
    );
}

TEST(STM_ASSIGN, CHAR) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            char x;
            x = 'a';
        )"
    );
}

TEST(STM_ASSIGN, INT_ARRAY) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[] x;
            x = new int[3];
        )"
    );
}

TEST(STM_ASSIGN, FLOAT_ARRAY) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            float[] x;
            x = new float[3];
        )"
    );
}

TEST(STM_ASSIGN, BOOL_ARRAY) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            bool[] x;
            x = new bool[3];
        )"
    );
}

TEST(STM_ASSIGN, CHAR_ARRAY) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            char[] x;
            x = new char[3];
        )"
    );
}

TEST(STM_ASSIGN, ASSIGN_INT_ARRAY_ELEMENT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[] x = new int[5];
            x[0] = 5;
        )"
    );
}

TEST(STM_ASSIGN, ASSIGN_FLOAT_ARRAY_ELEMENT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            float[] x = new float[5];
            x[0] = 5.5f;
        )"
    );
}

TEST(STM_ASSIGN, ASSIGN_BOOL_ARRAY_ELEMENT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            bool[] x = new bool[5];
            x[0] = true;
        )"
    );
}

TEST(STM_ASSIGN, ASSIGN_CHAR_ARRAY_ELEMENT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            char[] x = new char[5];
            x[0] = 'a';
        )"
    );
}

TEST(STM_ASSIGN, ASSIGN_TWO_DIMENSIONAL_INT_ARRAY_ELEMENT) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[][] x = new int[5][1];
            x[0][1] = 5;
        )"
    );
}

TEST(STM_ASSIGN, ASSIGN_ONE_DIMENSIONAL_ARRAY_TO_TWO_DIMENSIONAL_INT_ARRAY) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int[][] x = new int[5][1];
            x[0] = new int[1];
        )"
    );
}

TEST(STM_ASSIGN, USE_INITIALISED_VAR) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            int y = x;
        )"
    );
}

TEST(STM_ASSIGN, PLUS_EQUAL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x += 1;
        )"
    );
}

TEST(STM_ASSIGN, PLUS_EQUAL_IMPLICIT_CONVERSION) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x += 'A';
        )"
    );
}

TEST(STM_ASSIGN, MINUS_EQUAL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x -= 1;
        )"
    );
}

TEST(STM_ASSIGN, MINUS_EQUAL_IMPLICIT_CONVERSION) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x -= 'A';
        )"
    );
}

TEST(STM_ASSIGN, MULTIPLY_EQUAL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x *= 1;
        )"
    );
}

TEST(STM_ASSIGN, MULTIPLY_EQUAL_IMPLICIT_CONVERSION) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x *= 'A';
        )"
    );
}

TEST(STM_ASSIGN, DIVIDE_EQUAL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            float x = 5;
            x /= 1;
        )"
    );
}

TEST(STM_ASSIGN, DIVIDE_EQUAL_IMPLICIT_CONVERSION) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            float x = 5;
            x /= 'A';
        )"
    );
}

TEST(STM_ASSIGN, INTEGER_DIVIDE_EQUAL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x //= 1;
        )"
    );
}

TEST(STM_ASSIGN, INTEGER_DIVIDE_EQUAL_IMPLICIT_CONVERSION_ONE) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x //= 'A';
        )"
    );
}

TEST(STM_ASSIGN, INTEGER_DIVIDE_EQUAL_IMPLICIT_CONVERSION_TWO) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            float x = 5.5f;
            x //= 5.5f;
        )"
    );
}

TEST(STM_ASSIGN, MODULO_EQUAL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x &= 1;
        )"
    );
}

TEST(STM_ASSIGN, MODULO_EQUAL_IMPLICIT_CONVERSION) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x &= 'A';
        )"
    );
}

TEST(STM_ASSIGN, BITWISE_OR_EQUAL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x |= 1;
        )"
    );
}

TEST(STM_ASSIGN, BITWISE_OR_EQUAL_IMPLICIT_CONVERSION) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x |= 'A';
        )"
    );
}

TEST(STM_ASSIGN, BITEISE_XOR_EQUAL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x ^= 1;
        )"
    );
}

TEST(STM_ASSIGN, BITEISE_XOR_EQUAL_IMPLICIT_CONVERSION) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x ^= 'A';
        )"
    );
}

TEST(STM_ASSIGN, BITWISE_AND_EQUAL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x &= 1;
        )"
    );
}

TEST(STM_ASSIGN, BITWISE_AND_EQUAL_IMPLICIT_CONVERSION) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x &= 'A';
        )"
    );
}

TEST(STM_ASSIGN, LEFT_SHIFT_EQUAL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x <<= 1;
        )"
    );
}

TEST(STM_ASSIGN, LEFT_SHIFT_EQUAL_IMPLICIT_CONVERSION) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x <<= 'A';
        )"
    );
}

TEST(STM_ASSIGN, ARITHMETIC_RIGHT_SHIFT_EQUAL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x >>= 1;
        )"
    );
}

TEST(STM_ASSIGN, ARITHMETIC_RIGHT_SHIFT_EQUAL_IMPLICIT_CONVERSION) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x >>= 'A';
        )"
    );
}

TEST(STM_ASSIGN, LOGICAL_RIGHT_SHIFT_EQUAL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x >>>= 1;
        )"
    );
}

TEST(STM_ASSIGN, LOGICAL_RIGHT_SHIFT_EQUAL_IMPLICIT_CONVERSION) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            int x = 5;
            x >>>= 'A';
        )"
    );
}

TEST(STM_ASSIGN, LOGICAL_OR_EQUAL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            bool x = true;
            x ||= true;
        )"
    );
}

TEST(STM_ASSIGN, LOGICAL_AND_EQUAL) {
    ASSERT_SEMANTICALLY_VALID(
        R"(
            bool x = true;
            x &&= true;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_BOOL_TO_INT) {
    ASSERT_THROWS_TYPE_ERROR(R"(
            int x;
            x = true;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_FLOAT_TO_BOOL) {
    ASSERT_THROWS_TYPE_ERROR(R"(
            bool x;
            x = 5.5f;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_FLOAT_TO_CHAR) {
    ASSERT_THROWS_TYPE_ERROR(R"(
            char x;
            x = 5.5f;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_INT_TO_INT_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int[] x;
            x = 5;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_FLOAT_TO_FLOAT_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            float[] x;
            x = 5.5f;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_BOOL_TO_BOOL_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            bool[] x;
            x = true;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_CHAR_TO_CHAR_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            char[] x;
            x = 'a';
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_INT_ARRAY_TO_INT) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int x;
            x = new int[3];
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_FLOAT_ARRAY_TO_FLOAT) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            float x;
            x = new float[3];
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_BOOL_ARRAY_TO_BOOL) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            bool x;
            x = new bool[3];
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_CHAR_ARRAY_TO_CHAR) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            char x;
            x = new char[3];
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_INT_ELEMENT_TO_BOOL_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            bool[] x = new bool[5];
            x[0] = 5;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ASSIGN_BOOL_ELEMENT_TO_FLOAT_ARRAY) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            float[] x = new float[5];
            x[0] = false;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_USE_VARIABLE_BEFORE_INITIALISED) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            int x;
            int y = x;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_USE_OF_ARRAY_VARIABLE_BEFORE_INITIALISED) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            int[] x;
            int y = x[1];
        )"
    );
}

TEST(STM_ASSIGN, INVALID_USE_OF_ARRAY_ACCESS_BEFORE_INITIALISED) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            int[] x;
            x[0] = 5;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_USE_VARIABLE_BEFORE_DECLARATION) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            x = 5;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_PLUS_EQUAL_EXPR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            bool x = true;
            x += 5;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_PLUS_EQUAL_VAR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int x = 5;
            x += true;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_MINUS_EQUAL_EXPR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            bool x = true;
            x -= 5;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_MINUS_EQUAL_VAR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int x = 5;
            x -= true;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_MULTIPLY_EQUAL_EXPR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            bool x = true;
            x *= 5;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_MULTIPLY_EQUAL_VAR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int x = 5;
            x *= true;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_DIVIDE_EQUAL_EXPR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            bool x = true;
            x /= 5;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_DIVIDE_EQUAL_VAR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int x = 5;
            x /= true;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_INTEGER_DIVIDE_EQUAL_EXPR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            bool x = true;
            x //= 5;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_INTEGER_DIVIDE_EQUAL_VAR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int x = 5;
            x //= true;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_MODULO_EQUAL_EXPR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            bool x = true;
            x &= 5;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_MODULO_EQUAL_VAR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int x = 5;
            x &= true;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_BITWISE_OR_EQUAL_EXPR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int x = 5;
            x |= 5.5f;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_BITWISE_OR_EQUAL_VAR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            float x = 5.5f;
            x |= 5;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_BITWISE_XOR_EQUAL_EXPR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int x = 5;
            x ^= 5.5f;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_BITWISE_XOR_EQUAL_VAR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            float x = 5.5f;
            x ^= 5;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_BITWISE_AND_EQUAL_EXPR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int x = 5;
            x &= 5.5f;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_BITWISE_AND_EQUAL_VAR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            float x = 5.5f;
            x &= 5;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_LEFT_SHIFT_EQUAL_EXPR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int x = 5;
            x <<= 5.5f;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_LEFT_SHIFT_EQUAL_VAR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            float x = 5.5f;
            x <<= 5;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ARITHMETIC_RIGHT_SHIFT_EQUAL_EXPR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int x = 5;
            x >>= 5.5f;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_ARITHMETIC_RIGHT_SHIFT_EQUAL_VAR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            float x = 5.5f;
            x >>= 5;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_LOGICAL_RIGHT_SHIFT_EQUAL_EXPR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int x = 5;
            x >>>= 5.5f;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_LOGICAL_RIGHT_SHIFT_EQUAL_VAR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            float x = 5.5f;
            x >>>= 5;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_LOGICAL_OR_EQUAL_EXPR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int x = 5;
            x ||= true;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_LOGICAL_OR_EQUAL_VAR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            bool x = true;
            x ||= 5;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_LOGICAL_AND_EQUAL_EXPR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int x = 5;
            x &&= true;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_LOGICAL_AND_EQUAL_VAR_TYPE) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            bool x = true;
            x &&= 5;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_COMPLEX_ARRAY_EXPR) {
    ASSERT_THROWS_TYPE_ERROR(
        R"(
            int[] arr = new int[3];
            arr += 1;
        )"
    );
}

TEST(STM_ASSIGN, INVALID_COMPLEX_UNINITIALISED) {
    ASSERT_THROWS_SEMANTIC_ERROR(
        R"(
            int x;
            x += 1;
        )"
    );
}