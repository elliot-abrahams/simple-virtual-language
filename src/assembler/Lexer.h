//
// Created by erael on 03/05/2026.
//

#ifndef SIMPLE_VM_LEXER_H
#define SIMPLE_VM_LEXER_H

#pragma once
#include "Types.h"
#include <optional>
#include <string>
#include <vector>


class Lexer {

public:
    Lexer();

    std::optional<std::vector<Types::SVMAToken>> lex(const std::string &filePath);

private:
    std::optional<std::vector<Types::SVMAToken>> buildTokenStream();

    std::optional<Types::SVMAToken> lexToken();
    std::optional<Types::SVMAToken> lexLabel();
    std::optional<Types::SVMAToken> lexNumber();
    std::optional<Types::SVMAToken> lexImmediate();
    std::optional<Types::SVMAToken> lexDataStart();
    std::optional<Types::SVMAToken> lexChar();
    std::optional<Types::SVMAToken> lexString();
    std::optional<Types::SVMAToken> lexKeyWord();

    void next();
    char peek() const;
    std::optional<char> peekNext();
    std::string readUntilWhitespace();
    std::string readChar();
    std::string readString();
    void skipWhitespace();
    void skipComment();

    static bool isValidLabel(const std::string& s);
    static bool isValidNumber(const std::string& s);
    static bool isValidImmediate(const std::string& s);
    static bool isValidChar(const std::string& s);
    static bool isValidString(const std::string& s);

    void outputInvalidTokenError(const std::string& word) const;
    void outputInvalidLabelError(const std::string& word) const;
    void outputLineNumberOfError() const;

    std::vector<char> inputBuffer;
    int charIdx;
    int lineNumber;
    bool reachedEndOfFile;
};


#endif //SIMPLE_VM_LEXER_H