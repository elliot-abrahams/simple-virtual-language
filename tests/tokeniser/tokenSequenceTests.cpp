#include <gtest/gtest.h>
#include "tokeniserTestUtils.h"

using namespace tokeniserTest;

TEST(SEQUENCE, DOUBLE_IDENTIFIER) {
    ASSERT_TOKEN_SEQUENCE_EQ(
        "abc def",
        {TokenKind::IDENTIFIER, TokenKind::IDENTIFIER},
        {"abc", "def"}
    );
}

TEST(SEQUENCE, IDENTIFIER_BETWEEN_EXCLAMATION_MARK) {
    ASSERT_TOKEN_SEQUENCE_EQ(
        "abc!def",
        {TokenKind::IDENTIFIER, TokenKind::LOGICAL_NOT, TokenKind::IDENTIFIER},
        {"abc", "!", "def"}
    );
}

TEST(SEQUENCE, IF_LBR_IDENTIFIER_RBR) {
    ASSERT_TOKEN_SEQUENCE_EQ(
        "if(x)",
        {TokenKind::IF, TokenKind::LBR, TokenKind::IDENTIFIER, TokenKind::RBR},
        {"if", "(", "x", ")"}
    );
}