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
    FLOAT_TYPE,
    BOOL_TYPE,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    BOOL_LITERAL,
    IDENTIFIER,
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
            case TokenKind::LBR: return "(";
            case TokenKind::RBR: return ")";
            case TokenKind::EQUAL: return "=";
            case TokenKind::PLUS: return "+";
            case TokenKind::MINUS: return "-";
            case TokenKind::MULTIPLY: return "*";
            case TokenKind::DIVIDE: return "/";
            case TokenKind::MODULO: return "%";
            case TokenKind::INTEGER_TYPE: return "int";
            case TokenKind::FLOAT_TYPE: return "float";
            case TokenKind::BOOL_TYPE: return "bool";
            case TokenKind::INTEGER_LITERAL: return "integer literal";
            case TokenKind::FLOAT_LITERAL: return "float literal";
            case TokenKind::BOOL_LITERAL: return "bool literal";
            case TokenKind::IDENTIFIER: return "identifier";
            default: return "Unknown";
        }
    };
};



#endif //SVM_TOKEN_H