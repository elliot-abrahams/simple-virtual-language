//
// Created by erael on 03/05/2026.
//

#ifndef SIMPLE_VM_LEXER_H
#define SIMPLE_VM_LEXER_H

#pragma once
#include "AssemblerDefs.h"
#include <optional>
#include <string>
#include <vector>


class Lexer {

public:
    Lexer();

    std::optional<std::vector<AssemblerDefs::SVMAToken>> lex(const std::string &filePath);

private:
    std::optional<std::vector<AssemblerDefs::SVMAToken>> buildTokenStream();

    std::optional<AssemblerDefs::SVMAToken> lexToken();
    std::optional<AssemblerDefs::SVMAToken> lexLabel();
    std::optional<AssemblerDefs::SVMAToken> lexNumber();
    std::optional<AssemblerDefs::SVMAToken> lexImmediate();
    std::optional<AssemblerDefs::SVMAToken> lexDataStart();
    std::optional<AssemblerDefs::SVMAToken> lexChar();
    std::optional<AssemblerDefs::SVMAToken> lexString();
    std::optional<AssemblerDefs::SVMAToken> lexKeyWord();

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