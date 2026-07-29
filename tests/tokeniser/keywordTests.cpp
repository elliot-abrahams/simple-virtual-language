#include <gtest/gtest.h>
#include "tokeniserTestUtils.h"

using namespace tokeniserTest;

TEST(KEYWORD, IF) {
    ASSERT_TOKEN_EQ("if", TokenKind::IF, "if");
}

TEST(KEYWORD, ELSE) {
    ASSERT_TOKEN_EQ("else", TokenKind::ELSE, "else");
}

TEST(KEYWORD, WHILE) {
    ASSERT_TOKEN_EQ("while", TokenKind::WHILE, "while");
}

TEST(KEYWORD, CONTINUE) {
    ASSERT_TOKEN_EQ("continue", TokenKind::CONTINUE, "continue");
}

TEST(KEYWORD, BREAK) {
    ASSERT_TOKEN_EQ("break", TokenKind::BREAK, "break");
}

TEST(KEYWORD, RETURN) {
    ASSERT_TOKEN_EQ("return", TokenKind::RETURN, "return");
}

TEST(KEYWORD, INT_TYPE) {
    ASSERT_TOKEN_EQ("int", TokenKind::INTEGER_TYPE, "int");
}

TEST(KEYWORD, FLOAT_TYPE) {
    ASSERT_TOKEN_EQ("float", TokenKind::FLOAT_TYPE, "float");
}

TEST(KEYWORD, BOOL_TYPE) {
    ASSERT_TOKEN_EQ("bool", TokenKind::BOOL_TYPE, "bool");
}

TEST(KEYWORD, VOID_TYPE) {
    ASSERT_TOKEN_EQ("void", TokenKind::VOID_TYPE, "void");
}
