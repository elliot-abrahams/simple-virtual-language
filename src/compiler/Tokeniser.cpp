#include "Tokeniser.h"

#include <filesystem>
#include <iostream>

#include "../include/Error.h"


compiler::Tokeniser::Tokeniser(const std::string_view source, const std::filesystem::path* path) :
    source(source), path(path) {

    this->tokenBuffer.push_back(this->readToken());
}

Token compiler::Tokeniser::tok() {
    if (this->tokenBuffer.empty()) {
        this->tokenBuffer.push_back(this->readToken());
    }
    return this->tokenBuffer[0];
}

Token compiler::Tokeniser::lookAhead(const size_t n) {
    while (this->tokenBuffer.size() <= n) {
        tokenBuffer.push_back(this->readToken());
    }
    return this->tokenBuffer[n];
}

std::string compiler::Tokeniser::eat(const TokenKind& kind) {
    const Token token = this->tok();
    if (token.kind != kind) {
        throw SyntaxError(
            this->path->string(),
            token.line,
            token.column,
            "unexpected '" + token.image + "'"
        );
    }
    this->next();
    return token.image;
}

void compiler::Tokeniser::next() {
    if (this->tokenBuffer.empty()) {
        this->readToken();
    } else {
        // remove first token in buffer
        this->tokenBuffer.pop_front();
    }
}

Token compiler::Tokeniser::readToken() {
    // traverse input until a token is read or an error is thrown
    while (true) {
        // skip whitespace
        while (this->current < this->source.size() &&
           std::isspace(static_cast<unsigned char>(this->source[this->current]))) {
            if (this->source[this->current] == '\n') {
                this->line++;
                this->column = 0;
            }
            this->advance();
        }

        // check reached end
        if (this->current == this->source.size()) {
            return Token{TokenKind::END_OF_FILE, "End of File", this->line, this->column};
        }

        this->start = this->current;

        const char currentChar = this->source[this->current];

        switch (currentChar) {
            case '#' : { // single line comment
                const Token token = Token{};
                // skip until newline
                while (this->current < this->source.size() &&
                    this->source[this->current] != '\n' &&
                    this->source[this->current] != '\r'
                ) {
                    if (this->source[this->current] == '\n') {
                        this->line++;
                        this->column = 0;
                    }
                    this->advance();
                }
                break;
            }
            case ';': {
                const Token token = Token{TokenKind::SEMI, ";", this->line, this->column};
                this->advance();
                return token;
            }
            case ':': {
                const Token token = Token{TokenKind::COLON, ":", this->line, this->column};
                this->advance();
                return token;
            }
            case ',': {
                const Token token = Token{TokenKind::COMMA, ",", this->line, this->column};
                this->advance();
                return token;
            }
            case '.': {
                if (this->current + 1 < this->source.size() &&
                    this->source[this->current + 1] == '.') {

                    const Token token = Token{TokenKind::DOT_DOT, "..", this->line, this->column};
                    this->advance();
                    this->advance();
                    return token;
                }
                const Token token = Token{TokenKind::DOT, ".", this->line, this->column};
                this->advance();
                return token;
            }
            case '(': {
                const Token token = Token{TokenKind::LBR, "(", this->line, this->column};
                this->advance();
                return token;
            }
            case ')': {
                const Token token = Token{TokenKind::RBR, ")", this->line, this->column};
                this->advance();
                return token;
            }
            case '{': {
                const Token token = Token{TokenKind::LCBR, "{", this->line, this->column};
                this->advance();
                return token;
            }
            case '}': {
                const Token token = Token{TokenKind::RCBR, "}", this->line, this->column};
                this->advance();
                return token;
            }
            case '[': {
                const Token token = Token{TokenKind::LSQBR, "[", this->line, this->column};
                this->advance();
                return token;
            }
            case ']': {
                const Token token = Token{TokenKind::RSQBR, "]", this->line, this->column};
                this->advance();
                return token;
            }
            case '\'': {
                const auto line = this->line;
                const auto column = this->column;
                std::string image = "'";
                this->advance();
                if (this->source[this->current] == '\\') {
                    image += "\\";
                    this->advance();
                    switch (const char escapeChar = this->source[this->current]) {
                        case 'n':
                        case 't':
                        case 'r':
                        case '0':
                        case '\\':
                        case '\'':
                            image += escapeChar;
                            this->advance();
                            break;

                        default:
                            throw SyntaxError(
                                *this->path,
                                this->line,
                                this->column,
                                "invalid escape character '" +
                                    std::string(1, escapeChar) +
                                    "'"
                            );
                    }
                } else {
                    const auto start = this->current;
                    this->advanceUtf8CodePoint();
                    image += this->source.substr(start, this->current - start);
                }
                if (this->source[this->current] != '\'') {
                    this->throwUnexpectedCharError(this->source[this->current]);
                }
                image += "\'";
                this->advance();
                return Token{TokenKind::CHAR_LITERAL, image, line, column};
            }
            case '=': {
                if (this->current + 1 < this->source.size() &&
                    this->source[this->current + 1] == '=') {

                    const Token token = Token{TokenKind::EQUAL_EQUAL, "==", this->line, this->column};
                    this->advance();
                    this->advance();
                    return token;
                }

                const Token token = Token{TokenKind::EQUAL, "=", this->line, this->column};
                this->advance();
                return token;
            }
            case '+': {
                if (this->current + 1 < this->source.size() &&
                    this->source[this->current + 1] == '+') {

                    const Token token = Token{TokenKind::INCREMENT, "++", this->line, this->column};
                    this->advance();
                    this->advance();
                    return token;
                }
                const Token token = Token{TokenKind::PLUS, "+", this->line, this->column};
                this->advance();
                return token;
            }
            case '-': {
                if (this->current + 1 < this->source.size() &&
                    this->source[this->current + 1] == '-') {

                    const Token token = Token{TokenKind::DECREMENT, "--", this->line, this->column};
                    this->advance();
                    this->advance();
                    return token;
                }
                const Token token = Token{TokenKind::MINUS, "-", this->line, this->column};
                this->advance();
                return token;
            }
            case '*': {
                const Token token = Token{TokenKind::MULTIPLY, "*", this->line, this->column};
                this->advance();
                return token;
            }
            case '/': {
                if (this->current + 1 < this->source.size() &&
                    this->source[this->current + 1] == '/') {

                    const Token token = Token{TokenKind::INTEGER_DIVIDE, "//", this->line, this->column};
                    this->advance();
                    this->advance();
                    return token;
                }
                const Token token = Token{TokenKind::DIVIDE, "/", this->line, this->column};
                this->advance();
                return token;
            }
            case '%': {
                const Token token = Token{TokenKind::MODULO, "%", this->line, this->column};
                this->advance();
                return token;
            }
            case '|': {
                if (this->current + 1 < this->source.size() &&
                    this->source[this->current + 1] == '|') {

                    const Token token = Token{TokenKind::LOGICAL_OR, "||", this->line, this->column};
                    this->advance();
                    this->advance();
                    return token;
                }
                this->throwUnexpectedCharError('|');
            }
            case '&': {
                if (this->current + 1 < this->source.size() &&
                    this->source[this->current + 1] == '&') {

                    const Token token = Token{TokenKind::LOGICAL_AND, "&&", this->line, this->column};
                    this->advance();
                    this->advance();
                    return token;
                }
                this->throwUnexpectedCharError('&');
            }
            case '!': {
                if (this->current + 1 < this->source.size() &&
                    this->source[this->current + 1] == '=') {

                    const Token token = Token{TokenKind::NOT_EQUAL, "!=", this->line, this->column};
                    this->advance();
                    this->advance();
                    return token;
                }

                const Token token = Token{TokenKind::LOGICAL_NOT, "!", this->line, this->column};
                this->advance();
                return token;
            }
            case '<': {
                if (this->current + 1 < this->source.size() &&
                    this->source[this->current + 1] == '=') {

                    const Token token = Token{TokenKind::LESS_THAN_OR_EQUAL, "<=", this->line, this->column};
                    this->advance();
                    this->advance();
                    return token;
                }

                const Token token = Token{TokenKind::LESS_THAN, "<", this->line, this->column};
                this->advance();
                return token;
            }
            case '>': {
                if (this->current + 1 < this->source.size() &&
                    this->source[this->current + 1] == '=') {

                    const Token token = Token{TokenKind::GREATER_THAN_OR_EQUAL, ">=", this->line, this->column};
                    this->advance();
                    this->advance();
                    return token;
                }

                const Token token = Token{TokenKind::GREATER_THAN, ">", this->line, this->column};
                this->advance();
                return token;
            }
            default: {

                if (std::isalpha(currentChar) || currentChar == '_') {
                    while (this->current < this->source.size() &&
                          (std::isalnum(this->source[this->current]) || this->source[this->current] == '_')) {
                        this->advance();
                    }

                    // parse keyword or identifier
                    const std::string image(this->source.substr(this->start, this->current - this->start));

                    if (image == "if") return Token{TokenKind::IF, image, this->line, this->column - 2};
                    if (image == "else") return Token{TokenKind::ELSE, image, this->line, this->column - 4};
                    if (image == "while") return Token{TokenKind::WHILE, image, this->line, this->column - 5};
                    if (image == "for") return Token{TokenKind::FOR, image, this->line, this->column - 3};
                    if (image == "continue") return Token{TokenKind::CONTINUE, image, this->line, this->column - 8};
                    if (image == "break") return Token{TokenKind::BREAK, image, this->line, this->column - 5};
                    if (image == "return") return Token{TokenKind::RETURN, image, this->line, this->column - 6};
                    if (image == "new") return Token{TokenKind::NEW, image, this->line, this->column - 3};

                    if (image == "void") return Token{TokenKind::VOID_TYPE, image, this->line, this->column - 4};
                    if (image == "int") return Token{TokenKind::INT_TYPE, image, this->line, this->column - 3};
                    if (image == "float") return Token{TokenKind::FLOAT_TYPE, image, this->line, this->column - 5};
                    if (image == "bool") return Token{TokenKind::BOOL_TYPE, image, this->line, this->column - 4};
                    if (image == "char") return Token{TokenKind::CHAR_TYPE, image, this->line, this->column - 4};

                    if (image == "true") return Token{TokenKind::BOOL_LITERAL, image, this->line, this->column - 4};
                    if (image == "false") return Token{TokenKind::BOOL_LITERAL, image, this->line, this->column - 5};

                    if (image.substr(0,2) == "__") {
                        throw SyntaxError(
                            *this->path,
                            this->line,
                            this->column - 1, // '-1' because this->column points to the char following this keyword token
                            "identifiers starting with '__' are reserved"
                        );
                    }

                    return Token{TokenKind::IDENTIFIER, image, this->line, this->column - image.size()};
                }

                if (std::isdigit(currentChar)) {
                    // consume integer part
                    while (this->current < this->source.size() && std::isdigit(this->source[this->current])) {
                        this->advance();
                    }

                    // check for a float
                    // if DOT is followed by a DOT, create INT_LITERAL token
                    if ((this->current < this->source.size() && this->source[this->current] == '.') &&
                        (this->current + 1 < this->source.size() && this->source[this->current + 1] != '.')
                    ) {
                        this->advance();

                        // enforce digits after '.'
                        if (this->current >= this->source.size() || !std::isdigit(this->source[this->current])) {
                            throw SyntaxError(
                                *this->path,
                                this->line,
                                this->column - 1,
                                "expected digit after '.'"
                            );
                        }

                        // consume decimal part
                        while (this->current < this->source.size() && std::isdigit(this->source[this->current])) {
                            this->advance();
                        }

                        // enforce float literal ends with 'f'
                        if (this->current >= this->source.size() || this->source[this->current] != 'f') {
                            throw SyntaxError(
                                *this->path,
                                this->line,
                                this->column - 1,
                                "float value must end with 'f'"
                            );
                        }

                        this->advance();
                        const std::string image = std::string(source.substr(this->start, this->current - this->start - 1));
                        auto token = Token{TokenKind::FLOAT_LITERAL, image, this->line, this->column - (image.size() + 1)};
                        return token;
                    }

                    const std::string image = std::string(source.substr(this->start, this->current - this->start));
                    auto token = Token{TokenKind::INT_LITERAL, image, this->line, this->column - image.size()};
                    return token;
                }
                this->throwUnexpectedCharError(currentChar);
            }
        }
    }
}

void compiler::Tokeniser::advance() {
    this->current++;
    this->column++;
}

void compiler::Tokeniser::advance(size_t n) {
    this->current += n;
    this->column += n;
}

void compiler::Tokeniser::advanceUtf8CodePoint() {
    const auto first = static_cast<unsigned char>(this->source[this->current]);

    std::size_t length;

    if (first <= 0x7F) {
        length = 1;
    } else if ((first & 0xE0) == 0xC0) {
        length = 2;
    } else if ((first & 0xF0) == 0xE0) {
        length = 3;
    } else if ((first & 0xF8) == 0xF0) {
        length = 4;
    } else {
        throw SyntaxError(
            *this->path,
            this->line,
            this->column,
            "invalid UTF-8 sequence"
        );
    }

    for (std::size_t i = 1; i < length; ++i) {
        const auto byte = static_cast<unsigned char>(this->source[this->current + i]);

        if ((byte & 0xC0) != 0x80) {
            throw SyntaxError(
                *this->path,
                this->line,
                this->column,
                "invalid UTF-8 sequence"
            );
        }
    }

    this->advance(length);
}

void compiler::Tokeniser::throwUnexpectedCharError(const char character) const {
    throw SyntaxError(
        *this->path,
        this->line,
        this->column,
        "unexpected '" + std::string(1, character) + "'"
    );
}
