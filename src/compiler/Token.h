#ifndef SVM_TOKEN_H
#define SVM_TOKEN_H
#include <string>

enum class TokenKind {
    SEMI,
    LBR,
    RBR,
    EQUAL,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    INTEGER_TYPE,
    IDENTIFIER,
    INTEGER_LITERAL,
    END_OF_FILE
};

struct Token {
    TokenKind kind;
    std::string image;
    size_t line;
    size_t column;


    inline std::string kindToString() const {
        switch (this->kind) {
            case TokenKind::SEMI: return ";";
            case TokenKind::EQUAL: return "=";
            case TokenKind::INTEGER_TYPE: return "int";
            case TokenKind::IDENTIFIER: return "identifier";
            case TokenKind::INTEGER_LITERAL: return "integer literal";
            default: return "Unknown";
        }
    };
};



#endif //SVM_TOKEN_H