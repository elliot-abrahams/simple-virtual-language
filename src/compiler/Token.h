#ifndef SVM_TOKEN_H
#define SVM_TOKEN_H
#include <string>

enum class TokenKind {
    SEMI,
    COMMA,
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

    INT_TYPE,
    FLOAT_TYPE,
    BOOL_TYPE,
    VOID_TYPE,

    INT_LITERAL,
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
            case TokenKind::COMMA: return ",";
            case TokenKind::LBR: return "(";
            case TokenKind::RBR: return ")";
            case TokenKind::LCBR: return "{";
            case TokenKind::RCBR: return "}";
            case TokenKind::LSQBR: return "[";
            case TokenKind::RSQBR: return "]";

            case TokenKind::EQUAL: return "=";
            case TokenKind::PLUS: return "+";
            case TokenKind::INCREMENT: return "++";
            case TokenKind::MINUS: return "-";
            case TokenKind::DECREMENT: return "--";
            case TokenKind::MULTIPLY: return "*";
            case TokenKind::DIVIDE: return "/";
            case TokenKind::INTEGER_DIVIDE: return "//";
            case TokenKind::MODULO: return "%";

            case TokenKind::LOGICAL_OR: return "||";
            case TokenKind::LOGICAL_AND: return "&&";
            case TokenKind::LOGICAL_NOT: return "!";

            case TokenKind::EQUAL_EQUAL: return "==";
            case TokenKind::NOT_EQUAL: return "!=";

            case TokenKind::LESS_THAN: return "<";
            case TokenKind::LESS_THAN_OR_EQUAL: return "<=";
            case TokenKind::GREATER_THAN: return ">";
            case TokenKind::GREATER_THAN_OR_EQUAL: return ">=";

            case TokenKind::IF: return "if";
            case TokenKind::ELSE: return "else";
            case TokenKind::WHILE: return "while";
            case TokenKind::CONTINUE: return "continue";
            case TokenKind::BREAK: return "break";
            case TokenKind::RETURN: return "return";
            case TokenKind::NEW: return "new";

            case TokenKind::INT_TYPE: return "int";
            case TokenKind::FLOAT_TYPE: return "float";
            case TokenKind::BOOL_TYPE: return "bool";
            case TokenKind::VOID_TYPE: return "void";

            case TokenKind::INT_LITERAL: return "integer literal";
            case TokenKind::FLOAT_LITERAL: return "float literal";
            case TokenKind::BOOL_LITERAL: return "bool literal";

            case TokenKind::IDENTIFIER: return "identifier";
            case TokenKind::END_OF_FILE: return "end of file";
            default: return "Unknown";
        }
    };
};

#endif //SVM_TOKEN_H