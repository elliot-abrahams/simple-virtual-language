#include "Tokeniser.h"

#include <filesystem>
#include <iostream>

#include "../include/Error.h"


compiler::Tokeniser::Tokeniser(const std::string_view source, const std::filesystem::path* path) :
    source(source), path(path) {

    this->next();
}

Token compiler::Tokeniser::tok() {
    return this->headToken;
}

std::string compiler::Tokeniser::eat(const TokenKind& kind) {
    const Token token = this->tok();
    if (token.kind != kind) {
        throw LexicalError(
            this->path->string(),
            this->line,
            this->column,
            "expecting " + token.kindToString()
        );
    }
    this->next();
    return token.image;
}

void compiler::Tokeniser::next() {
    this->readToken();
}

Token compiler::Tokeniser::readToken() {
    // skip whitespace
    while (std::isspace(this->source[this->current])) {
        if (this->source[this->current] == 10) {
            this->line++;
            this->column = 0;
        }
        this->advance();
    }

    // check reached end
    if (this->current == this->source.size()) {
        const Token token = Token{TokenKind::END_OF_FILE, " ", this->line, this->column};
        this->setHead(token);
        return token;
    }

    this->start = this->current;

    const char currentChar = this->source[this->current];

    switch (currentChar) {
        case ';': {
            const Token token = Token{TokenKind::SEMI, ";", this->line, this->column};
            this->advance();
            this->setHead(token);
            return token;
        }
        case '(': {
            const Token token = Token{TokenKind::LBR, "(", this->line, this->column};
            this->advance();
            this->setHead(token);
            return token;
        }
        case ')': {
            const Token token = Token{TokenKind::RBR, ")", this->line, this->column};
            this->advance();
            this->setHead(token);
            return token;
        }
        case '{': {
            const Token token = Token{TokenKind::LCBR, "{", this->line, this->column};
            this->advance();
            this->setHead(token);
            return token;
        }
        case '}': {
            const Token token = Token{TokenKind::RCBR, "}", this->line, this->column};
            this->advance();
            this->setHead(token);
            return token;
        }
        case '=': {
            if (this->current + 1 < this->source.size() &&
                this->source[this->current + 1] == '=') {

                const Token token = Token{TokenKind::EQUAL_EQUAL, "==", this->line, this->column};
                this->advance();
                this->advance();
                this->setHead(token);
                return token;
            }

            const Token token = Token{TokenKind::EQUAL, "=", this->line, this->column};
            this->advance();
            this->setHead(token);
            return token;
        }
        case '+': {
            const Token token = Token{TokenKind::PLUS, "+", this->line, this->column};
            this->advance();
            this->setHead(token);
            return token;
        }
        case '-': {
            const Token token = Token{TokenKind::MINUS, "-", this->line, this->column};
            this->advance();
            this->setHead(token);
            return token;
        }
        case '*': {
            const Token token = Token{TokenKind::MULTIPLY, "*", this->line, this->column};
            this->advance();
            this->setHead(token);
            return token;
        }
        case '/': {
            const Token token = Token{TokenKind::DIVIDE, "/", this->line, this->column};
            this->advance();
            this->setHead(token);
            return token;
        }
        case '%': {
            const Token token = Token{TokenKind::MODULO, "%", this->line, this->column};
            this->advance();
            this->setHead(token);
            return token;
        }
        case '|': {
            if (this->current + 1 < this->source.size() &&
                this->source[this->current + 1] == '|') {

                const Token token = Token{TokenKind::LOGICAL_OR, "||", this->line, this->column};
                this->advance();
                this->advance();
                this->setHead(token);
                return token;
            }
            throw LexicalError(
                this->path->string(),
                this->line,
                this->column,
                "invalid character '" + std::string(1, currentChar) + "'"
            );
        }
        case '&': {
            if (this->current + 1 < this->source.size() &&
                this->source[this->current + 1] == '&') {

                const Token token = Token{TokenKind::LOGICAL_AND, "&&", this->line, this->column};
                this->advance();
                this->advance();
                this->setHead(token);
                return token;
            }
            throw LexicalError(
                this->path->string(),
                this->line,
                this->column,
                "invalid character '" + std::string(1, currentChar) + "'"
            );
        }
        case '!': {
            if (this->current + 1 < this->source.size() &&
                this->source[this->current + 1] == '=') {

                const Token token = Token{TokenKind::NOT_EQUAL, "!=", this->line, this->column};
                this->advance();
                this->advance();
                this->setHead(token);
                return token;
            }

            const Token token = Token{TokenKind::LOGICAL_NOT, "!", this->line, this->column};
            this->advance();
            this->setHead(token);
            return token;
        }
        case '<': {
            if (this->current + 1 < this->source.size() &&
                this->source[this->current + 1] == '=') {

                const Token token = Token{TokenKind::LESS_THAN_OR_EQUAL, "<=", this->line, this->column};
                this->advance();
                this->advance();
                this->setHead(token);
                return token;
                }

            const Token token = Token{TokenKind::LESS_THAN, "<", this->line, this->column};
            this->advance();
            this->setHead(token);
            return token;
        }
        case '>': {
            if (this->current + 1 < this->source.size() &&
                this->source[this->current + 1] == '=') {

                const Token token = Token{TokenKind::GREATER_THAN_OR_EQUAL, ">=", this->line, this->column};
                this->advance();
                this->advance();
                this->setHead(token);
                return token;
                }

            const Token token = Token{TokenKind::GREATER_THAN, ">", this->line, this->column};
            this->advance();
            this->setHead(token);
            return token;
        }
        default: {

            if (std::isalpha(currentChar) || currentChar == '_') {
                while (this->current < this->source.size() &&
                      (std::isalnum(this->source[this->current]) || this->source[this->current] == '_')) {
                    this->advance();
                }

                const std::string image(this->source.substr(this->start, this->current - this->start));

                if (image == "if") {
                    const Token token = Token{TokenKind::IF, image, this->line, this->column};
                    this->setHead(token);
                    return token;
                }

                if (image == "else") {
                    const Token token = Token{TokenKind::ELSE, image, this->line, this->column};
                    this->setHead(token);
                    return token;
                }

                if (image == "int") {
                    const Token token = Token{TokenKind::INTEGER_TYPE, image, this->line, this->column};
                    this->setHead(token);
                    return token;
                }

                if (image == "float") {
                    const Token token = Token{TokenKind::FLOAT_TYPE, image, this->line, this->column};
                    this->setHead(token);
                    return token;
                }

                if (image == "bool") {
                    const Token token = Token{TokenKind::BOOL_TYPE, image, this->line, this->column};
                    this->setHead(token);
                    return token;
                }

                if (image == "true" || image == "false") {
                    const Token token = Token{TokenKind::BOOL_LITERAL, image, this->line, this->column};
                    this->setHead(token);
                    return token;
                }

                const auto token = Token{TokenKind::IDENTIFIER, image, this->line, this->column};
                this->setHead(token);
                return token;
            }

            if (std::isdigit(currentChar)) {
                // consume integer part
                while (this->current < this->source.size() && std::isdigit(this->source[this->current])) {
                    this->advance();
                }

                // check for a float
                if (this->current < this->source.size() && this->source[this->current] == '.') {
                    size_t decimalPosition = this->current;
                    this->advance();

                    // enforce digits after '.'
                    if (this->current >= this->source.size() || !std::isdigit(this->source[this->current])) {
                        throw LexicalError(
                            this->path->string(),
                            this->line,
                            this->column,
                            "expected digit after '.' in float literal"
                        );
                    }

                    // consume decimal part
                    while (this->current < this->source.size() && std::isdigit(this->source[this->current])) {
                        this->advance();
                    }

                    // enforce float literal ends with 'f'
                    if (this->current >= this->source.size() || this->source[this->current] != 'f') {
                        throw LexicalError(
                            this->path->string(),
                            this->line,
                            this->column,
                            "float literal must end with 'f'"
                        );
                    }

                    this->advance();
                    auto token = Token{TokenKind::FLOAT_LITERAL, std::string(source.substr(this->start, this->current - this->start - 1)), this->line, this->column};
                    this->setHead(token);
                    return token;
                }

                auto token = Token{TokenKind::INTEGER_LITERAL, std::string(source.substr(this->start, this->current - this->start)), this->line, this->column};
                this->setHead(token);
                return token;
            }

            throw LexicalError(
                this->path->string(),
                this->line,
                this->column,
                "invalid character '" + std::string(1, currentChar) + "'"
            );
        }
    }
}

void compiler::Tokeniser::advance() {
    this->current++;
    this->column++;
}

void compiler::Tokeniser::setHead(const Token &token) {
    this->headToken = token;
}

