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
        case '=': {
            const Token token = Token{TokenKind::EQUAL, "=", this->line, this->column};
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

                if (image == "int") {
                    const Token token = Token{TokenKind::INTEGER_TYPE, image, this->line, this->column};
                    this->advance();
                    this->setHead(token);
                    return token;
                }

                const auto token = Token{TokenKind::IDENTIFIER, image, this->line, this->column};
                this->setHead(token);
                return token;
            }

            if (std::isdigit(currentChar)) {
                while (this->current < this->source.size() && std::isdigit(this->source[this->current])) {
                    this->advance();
                }

                auto token = Token{TokenKind::INTEGER_LITERAL, std::string(source.substr(this->start, this->current - this->start)), this->line, this->column};
                this->setHead(token);
                return token;
            }

            throw SyntaxError(
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

