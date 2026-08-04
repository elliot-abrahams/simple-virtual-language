#include <gtest/gtest.h>
#include "TokeniserTestUtils.h"

using namespace tokeniserTest;

TEST(SYMBOL, SEMI) {
    ASSERT_TOKEN_EQ(";", TokenKind::SEMI, ";");
}

TEST(SYMBOL, COMMA) {
    ASSERT_TOKEN_EQ(",", TokenKind::COMMA, ",");
}

TEST(SYMBOL, LBR) {
    ASSERT_TOKEN_EQ("(", TokenKind::LBR, "(");
}

TEST(SYMBOL, RBR) {
    ASSERT_TOKEN_EQ(")", TokenKind::RBR, ")");
}

TEST(SYMBOL, LCBR) {
    ASSERT_TOKEN_EQ("{", TokenKind::LCBR, "{");
}

TEST(SYMBOL, RCBR) {
    ASSERT_TOKEN_EQ("}", TokenKind::RCBR, "}");
}

TEST(SYMBOL, EQUAL) {
    ASSERT_TOKEN_EQ("=", TokenKind::EQUAL, "=");
}

TEST(SYMBOL, PLUS) {
    ASSERT_TOKEN_EQ("+", TokenKind::PLUS, "+");
}

TEST(SYMBOL, MINUS) {
    ASSERT_TOKEN_EQ("-", TokenKind::MINUS, "-");
}

TEST(SYMBOL, MULTIPLY) {
    ASSERT_TOKEN_EQ("*", TokenKind::MULTIPLY, "*");
}

TEST(SYMBOL, DIVIDE) {
    ASSERT_TOKEN_EQ("/", TokenKind::DIVIDE, "/");
}

TEST(SYMBOL, INTEGER_DIVIDE) {
    ASSERT_TOKEN_EQ("//", TokenKind::INTEGER_DIVIDE, "//");
}

TEST(SYMBOL, MODULO) {
    ASSERT_TOKEN_EQ("%", TokenKind::MODULO, "%");
}

TEST(SYMBOL, LOGICAL_OR) {
    ASSERT_TOKEN_EQ("||", TokenKind::LOGICAL_OR, "||");
}

TEST(SYMBOL, LOGICAL_AND) {
    ASSERT_TOKEN_EQ("&&", TokenKind::LOGICAL_AND, "&&");
}

TEST(SYMBOL, LOGICAL_NOT) {
    ASSERT_TOKEN_EQ("!", TokenKind::LOGICAL_NOT, "!");
}

TEST(SYMBOL, EQUAL_EQUAL) {
    ASSERT_TOKEN_EQ("==", TokenKind::EQUAL_EQUAL, "==");
}

TEST(SYMBOL, NOT_EQUAL) {
    ASSERT_TOKEN_EQ("!=", TokenKind::NOT_EQUAL, "!=");
}

TEST(SYMBOL, LESS_THAN) {
    ASSERT_TOKEN_EQ("<", TokenKind::LESS_THAN, "<");
}

TEST(SYMBOL, LESS_THAN_OR_EQUAL) {
    ASSERT_TOKEN_EQ("<=", TokenKind::LESS_THAN_OR_EQUAL, "<=");
}

TEST(SYMBOL, GREATER_THAN) {
    ASSERT_TOKEN_EQ(">", TokenKind::GREATER_THAN, ">");
}

TEST(SYMBOL, GREATER_THAN_OR_EQUAL) {
    ASSERT_TOKEN_EQ(">=", TokenKind::GREATER_THAN_OR_EQUAL, ">=");
}