#ifndef SVM_TOKEN_H
#define SVM_TOKEN_H
#include <string>

enum class TokenKind {
    SEMI,
    COMMA,
    DOT,
    LBR,
    RBR,
    LCBR,
    RCBR,
    LSQBR,
    RSQBR,

    EQUAL,
    PLUS,
    INCREMENT,
    MINUS,
    DECREMENT,
    MULTIPLY,
    DIVIDE,
    INTEGER_DIVIDE,
    MODULO,

    LOGICAL_OR,
    LOGICAL_AND,
    LOGICAL_NOT,

    EQUAL_EQUAL,
    NOT_EQUAL,

    LESS_THAN,
    LESS_THAN_OR_EQUAL,
    GREATER_THAN,
    GREATER_THAN_OR_EQUAL,

    IF,
    ELSE,
    WHILE,
    CONTINUE,
    BREAK,
    RETURN,
    NEW,

    VOID_TYPE,
    INT_TYPE,
    FLOAT_TYPE,
    BOOL_TYPE,
    CHAR_TYPE,

    INT_LITERAL,
    FLOAT_LITERAL,
    BOOL_LITERAL,
    CHAR_LITERAL,

    IDENTIFIER,

    END_OF_FILE
};

struct Token {
    TokenKind kind;
    std::string image;
    size_t line;
    size_t column;
};

#endif //SVM_TOKEN_H