#include "Lexer.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <regex>

#include "../include/Error.h"

assembler::Lexer::Lexer() : charIdx(0), lineNumber(0), columnNumber(0), reachedEndOfFile(false) {}

std::vector<assembler::SVMAToken> assembler::Lexer::lex(const std::filesystem::path* filePath) {
    std::fstream svmaFile;
    svmaFile.open(filePath->string(), std::ios::in); // read file

    if (!svmaFile.is_open()) {
        std::cerr << "could not open file" << std::endl;
        exit(EXIT_FAILURE);
    }

    this->filePath = filePath;

    char character;

    // copy characters from input into inputBuffer
    while (svmaFile >> std::noskipws >> character) {
        this->inputBuffer.push_back(character);
    }
    svmaFile.close();

    return this->buildTokenStream();
}

std::vector<assembler::SVMAToken> assembler::Lexer::lexString(const std::filesystem::path* testpath, const std::string& fileContent) {
    this->filePath = testpath;
    this->inputBuffer.assign(fileContent.begin(), fileContent.end());
    return this->buildTokenStream();
}

std::vector<assembler::SVMAToken> assembler::Lexer::buildTokenStream() {
    this->charIdx = 0;
    this->lineNumber = 1;
    this->columnNumber = 1;
    this->reachedEndOfFile = false;

    std::vector<SVMAToken> tokensStream;

    while (!this->reachedEndOfFile && this->charIdx < this->inputBuffer.size()) {

        this->skipWhitespace();

        if (this->charIdx >= this->inputBuffer.size() || this->peek() == ' ') {
            break;
        }

        switch (this->peek()) {
            case '\n':
                this->lineNumber++;
                this->columnNumber = 0;
                this->next();
                break;

            case ';':
                this->skipComment();
                break;

            default: {
                if (this->charIdx < this->inputBuffer.size()) {
                    tokensStream.push_back(this->lexToken());
                }
            }
        }
    }
    tokensStream.push_back(SVMAToken{SVMATokenType::END_OF_FILE, "EOF", this->lineNumber, this->columnNumber});
    return tokensStream;
}

assembler::SVMAToken assembler::Lexer::lexToken() {
    switch (this->peek()) {
        case '$':
            return this->lexLabel();
        case '#':
            return this->lexImmediate();
        case '.':
            return this->lexDirective();
        case '"':
            return this->lexString();
        default:
            char currentChar = this->peek();
            if (!std::isdigit(currentChar) && currentChar != '-') {
                return this->lexKeyWord();
            }
            if (currentChar == '0' && this->peekNext() == 'x') {
                return this->lexHex();
            }
            return this->lexNumber();
    }
}

void assembler::Lexer::next() {
    this->charIdx++;
    this->columnNumber++;
    if (this->charIdx >= this->inputBuffer.size()) {
        this->reachedEndOfFile = true;
    }
}

char assembler::Lexer::peek() const {
    return this->inputBuffer[this->charIdx];
}

char assembler::Lexer::peekNext() {
    if (this->charIdx + 1 == this->inputBuffer.size() - 1) {
        throw AssemblerError(
            *this->filePath,
            this->lineNumber,
            this->columnNumber,
            "unexpected 'EOF'"
        );
    }
    return this->inputBuffer[this->charIdx + 1];
}

assembler::SVMAToken assembler::Lexer::lexLabel() {
    const uint16_t labelColumn = this->columnNumber;
    // keep going through inputBuffer until reaching EOF, space, end of line, colon
    const std::string label = this->readUntilWhitespace();
    // LABEL_DEF
    if (label[label.size() - 1] == ':') {
        if (!isValidLabel(label.substr(0, label.size() - 1))) {
            this->throwInvalidLabel(label, labelColumn);
        }
        return SVMAToken{SVMATokenType::LABEL_DEF, label, this->lineNumber, labelColumn};
    }
    // LABEL_REF
    if (!isValidLabel(label)) {
        this->throwInvalidLabel(label, labelColumn);
    }
    return SVMAToken{SVMATokenType::LABEL_REF, label, this->lineNumber, labelColumn};
}

assembler::SVMAToken assembler::Lexer::lexNumber() {
    const uint16_t numberColumn = this->columnNumber;
    std::string number = this->readUntilWhitespace();
    return SVMAToken{SVMATokenType::NUMBER, number, this->lineNumber, numberColumn};
}

assembler::SVMAToken assembler::Lexer::lexHex() {
    const uint16_t hexColumn = this->columnNumber;
    const std::string hex = this->readUntilWhitespace();
    if (!isValidHex(hex)) {
        throw AssemblerError(
            *this->filePath,
            this->lineNumber,
            hexColumn,
            "invalid hexadecimal '" +
                hex + "'"
        );
    }
    return SVMAToken{SVMATokenType::HEX, hex, this->lineNumber, hexColumn};
}

assembler::SVMAToken assembler::Lexer::lexImmediate() {
    const uint16_t immediateColumn = this->columnNumber;
    const std::string immediate = this->readUntilWhitespace();
    return SVMAToken{SVMATokenType::IMMEDIATE, immediate, this->lineNumber, immediateColumn};
}

assembler::SVMAToken assembler::Lexer::lexDirective() {
    const uint16_t directiveColumn = this->columnNumber;
    const std::string directive = this->readUntilWhitespace();
    if (directive != ".data" &&
        directive != ".metadata" &&
        directive != ".sources" &&
        directive != ".functions" &&
        directive != ".line_table"
    ) {
        throw AssemblerError(
            *this->filePath,
            this->lineNumber,
            directiveColumn,
            "invalid directive '" +
                directive + "'"
        );
    }
    return SVMAToken{SVMATokenType::DIRECTIVE, directive, this->lineNumber, directiveColumn};
}

assembler::SVMAToken assembler::Lexer::lexString() {
    const uint16_t stringColumn = this->columnNumber;
    const std::string string = this->readString();
    if (!isValidString(string)) {
        throw AssemblerError(
            *this->filePath,
            this->lineNumber,
            stringColumn,
            "invalid string '" +
                 string + "'"
        );
    }
    return SVMAToken{SVMATokenType::STRING, string, this->lineNumber, stringColumn};
}

assembler::SVMAToken assembler::Lexer::lexKeyWord() {
    const uint16_t keywordColumn = this->columnNumber;
    const std::string keyword = this->readUntilWhitespace();

    // TYPE Token
    if (type.find(keyword) != type.end()) {
        return SVMAToken{SVMATokenType::TYPE, keyword, this->lineNumber, keywordColumn};
    }
    // DATA_TYPE Token
    if (dataType.find(keyword) != dataType.end()) {
        return SVMAToken{SVMATokenType::DATA_TYPE, keyword, this->lineNumber, keywordColumn};
    }
    // METHOD_DEF Token
    if (keyword == "def") {
        return SVMAToken{SVMATokenType::METHOD_DEF, "", this->lineNumber,  keywordColumn};
    }
    // METHOD_METADATA
    if (method_metadata_fields.find(keyword) != method_metadata_fields.end()) {
        return SVMAToken{SVMATokenType::METHOD_METADATA_FIELD, keyword, this->lineNumber, keywordColumn};
    }
    // INSTRUCTION
    if (opcode.find(keyword) != opcode.end()) {
        return SVMAToken{SVMATokenType::INSTRUCTION, keyword, this->lineNumber, keywordColumn};
    }
    // NATIVE_REF
    if (nativeRef.find(keyword) != nativeRef.end()) {
        return SVMAToken{SVMATokenType::NATIVE_REF, keyword, this->lineNumber, keywordColumn};
    }
    // ERROR_REF
    if (errorRef.find(keyword) != errorRef.end()) {
        return SVMAToken{SVMATokenType::ERROR_REF, keyword, this->lineNumber, keywordColumn};
    }
    throw AssemblerError(
        *this->filePath,
        this->lineNumber,
        keywordColumn,
        "invalid keyword '" +
             keyword + "'"
    );
}

std::string assembler::Lexer::readUntilWhitespace() {
    std::string word;

    if (this->reachedEndOfFile || this->charIdx >= this->inputBuffer.size()) {
        return word;
    }

    while (this->charIdx < this->inputBuffer.size() && !std::isspace(this->peek())) {
        word += this->peek();
        this->next();
    }
    return word;
}

std::string assembler::Lexer::readString() {
    std::string string;
    string += this->peek();
    this->next();
    // keep consuming chars until " (without escape character) appears in string
    while (!(this->peek() == '"' && this->inputBuffer.at(this->charIdx - 1) != '\\') && this->charIdx < this->inputBuffer.size()) {
        string += this->peek();
        this->next();
        if (this->charIdx == this->inputBuffer.size() - 1) {
            this->reachedEndOfFile = true;
            break;
        }
    }
    if (this->charIdx == this->inputBuffer.size()) {
        return string;
    }
    string += this->peek();
    this->next();
    return string;
}

void assembler::Lexer::skipWhitespace() {
    while (this->charIdx + 1 < this->inputBuffer.size() && this->inputBuffer[this->charIdx] == ' ') {
        this->next();
    }
}

void assembler::Lexer::skipComment() {
    while (!this->reachedEndOfFile && this->peek() != '\n') {
        this->next();
    }
    this->lineNumber++;
    this->next();
}

bool assembler::Lexer::isValidLabel(const std::string& s) {
    return std::regex_match(s, std::regex{R"(\$[a-zA-Z_][a-zA-Z0-9_()\[\],]*)"});
}

bool assembler::Lexer::isValidHex(const std::string& s) {
    return std::regex_match(s, std::regex{R"(0x[0-9A-Fa-f]+)"});
}

bool assembler::Lexer::isValidString(const std::string &s) {
    if (s.size() < 2 ||
        s[0] != '"' ||
        s[s.size() - 1] != '"'
    ) {
        return false;
    }
    return true;
}

void assembler::Lexer::throwInvalidLabel(const std::string& label, const uint16_t columnNumber) const {
    throw AssemblerError(
        *this->filePath,
        this->lineNumber,
        columnNumber,
        "invalid label '" +
            label +
            "'"
    );
}
