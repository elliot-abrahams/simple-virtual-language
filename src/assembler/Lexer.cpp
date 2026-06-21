#include "Lexer.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <optional>
#include <regex>
#include <filesystem>

Lexer::Lexer() : charIdx(0), lineNumber(0), reachedEndOfFile(false) {
}

std::optional<std::vector<AssemblerDefs::SVMAToken>> Lexer::lex(const std::string &filePath) {
    std::fstream svmaFile;
    svmaFile.open(filePath, std::ios::in); // read file

    if (!svmaFile.is_open()) {
        std::cerr << "File " << filePath << " could not be opened." << std::endl;
        return std::nullopt;
    }

    char character;

    // copy characters from input into inputBuffer
    while (svmaFile >> std::noskipws >> character) {
        this->inputBuffer.push_back(character);
    }
    svmaFile.close();

    return this->buildTokenStream();
}

std::optional<std::vector<AssemblerDefs::SVMAToken>> Lexer::lexString(const std::string& fileContent) {
    char character;

    this->inputBuffer.assign(fileContent.begin(), fileContent.end());
    return this->buildTokenStream();
}

std::optional<std::vector<AssemblerDefs::SVMAToken>> Lexer::buildTokenStream() {
    this->charIdx = 0;
    this->lineNumber = 1;
    this->reachedEndOfFile = false;

    std::vector<AssemblerDefs::SVMAToken> tokensStream;

    while (!this->reachedEndOfFile && this->charIdx < this->inputBuffer.size()) {

        this->skipWhitespace();

        switch (this->peek()) {
            case '\n':
                this->lineNumber++;
                this->next();
                break;

            case ';':
                this->skipComment();
                break;

            default:
                if (!this->reachedEndOfFile) {
                    std::optional<AssemblerDefs::SVMAToken> token = this->lexToken();
                    if (!token.has_value()) {
                        return std::nullopt;
                    }
                    tokensStream.push_back(token.value());
                }
        }
    }
    tokensStream.push_back(AssemblerDefs::SVMAToken{AssemblerDefs::SVMATokenType::END_OF_FILE, "", this->lineNumber});
    return tokensStream;
}

std::optional<AssemblerDefs::SVMAToken> Lexer::lexToken() {
    switch (this->peek()) {
        case '$':
            return this->lexLabel();
        case '#':
            return this->lexImmediate();
        case '.':
            return this->lexDataStart();
        case '"':
            return this->lexString();
        default:
            if (std::isdigit(this->peek()) || this->peek() == '-') {
                return this->lexNumber();
            }
            return this->lexKeyWord();
    }
}

void Lexer::next() {
    this->charIdx++;
}

char Lexer::peek() const {
    return this->inputBuffer[this->charIdx];
}

std::optional<char> Lexer::peekNext() {
    if (this->charIdx + 1 == this->inputBuffer.size() - 1) {
        this->reachedEndOfFile = true;
        return std::nullopt;
    }
    return this->inputBuffer[this->charIdx + 1];
}

std::optional<AssemblerDefs::SVMAToken> Lexer::lexLabel() {
    // enforce label does not start with a number
    std::optional<char> nextChar = peekNext();
    if (nextChar.has_value() && std::isdigit(nextChar.value())) {
        return std::nullopt;
    }
    // keep going through inputBuffer until reaching EOF, space, end of line, colon
    const std::string label = this->readUntilWhitespace();
    // LABEL_DEF
    if (label[label.size() - 1] == ':') {
        if (!this->isValidLabel(label.substr(0, label.size() - 1))) {
            this->outputInvalidLabelError(label);
            return std::nullopt;
        }
        return AssemblerDefs::SVMAToken{AssemblerDefs::SVMATokenType::LABEL_DEF, label, this->lineNumber};
    }
    // LABEL_REF
    if (!this->isValidLabel(label)) {
        this->outputInvalidLabelError(label);
        return std::nullopt;
    }
    return AssemblerDefs::SVMAToken{AssemblerDefs::SVMATokenType::LABEL_REF, label, this->lineNumber};
}

std::optional<AssemblerDefs::SVMAToken> Lexer::lexNumber() {
    std::string number = this->readUntilWhitespace();
    if (!this->isValidNumber(number)) {
        this->outputLineNumberOfError();
        std::cerr << "Invalid number \' " << number << "\'" << std::endl;
        return std::nullopt;
    }
    return AssemblerDefs::SVMAToken{AssemblerDefs::SVMATokenType::NUMBER, number, this->lineNumber};
}

std::optional<AssemblerDefs::SVMAToken> Lexer::lexImmediate() {
    std::string immediate = this->readUntilWhitespace();
    if (!this->isValidImmediate(immediate)) {
        this->outputLineNumberOfError();
        std::cerr << "Invalid immediate \'" << immediate << "\'" << std::endl;
        return std::nullopt;
    }
    return AssemblerDefs::SVMAToken{AssemblerDefs::SVMATokenType::IMMEDIATE, immediate, this->lineNumber};
}

std::optional<AssemblerDefs::SVMAToken> Lexer::lexDataStart() {
    std::string dataStart = this->readUntilWhitespace();
    if (dataStart != ".data") {
        this->outputInvalidTokenError(dataStart);
        return std::nullopt;
    }
    return AssemblerDefs::SVMAToken{AssemblerDefs::SVMATokenType::DATA_START, "", this->lineNumber};
}

std::optional<AssemblerDefs::SVMAToken> Lexer::lexString() {
    std::string string = this->readString();
    if (!this->isValidString(string)) {
        this->outputLineNumberOfError();
        if (this->reachedEndOfFile) {
            std::cerr << "Unterminated string" << std::endl;
        } else {
            std::cerr << "Invalid string \'" << string << "\'" << std::endl;
        }
        return std::nullopt;
    }
    return AssemblerDefs::SVMAToken{AssemblerDefs::SVMATokenType::STRING, string, this->lineNumber};
}

std::optional<AssemblerDefs::SVMAToken> Lexer::lexKeyWord() {
    std::string keyword = this->readUntilWhitespace();

    // TYPE Token
    if (AssemblerDefs::type.find(keyword) != AssemblerDefs::type.end()) {
        return AssemblerDefs::SVMAToken{AssemblerDefs::SVMATokenType::TYPE, keyword, this->lineNumber};
    }
    // DATA_TYPE Token
    if (AssemblerDefs::dataType.find(keyword) != AssemblerDefs::dataType.end()) {
        return AssemblerDefs::SVMAToken{AssemblerDefs::SVMATokenType::DATA_TYPE, keyword, this->lineNumber};
    }
    // METHOD_DEF Token
    if (keyword == "def") {
        return AssemblerDefs::SVMAToken{AssemblerDefs::SVMATokenType::METHOD_DEF, "", this->lineNumber};
    }
    // METHOD_METADATA
    if (AssemblerDefs::method_metadata_fields.find(keyword) != AssemblerDefs::method_metadata_fields.end()) {
        return AssemblerDefs::SVMAToken{AssemblerDefs::SVMATokenType::METHOD_METADATA_FIELD, keyword, this->lineNumber};
    }
    // INSTRUCTION
    if (AssemblerDefs::opcode.find(keyword) != AssemblerDefs::opcode.end()) {
        return AssemblerDefs::SVMAToken{AssemblerDefs::SVMATokenType::INSTRUCTION, keyword, this->lineNumber};
    }
    this->outputInvalidTokenError(keyword);
    return std::nullopt;
}

std::string Lexer::readUntilWhitespace() {
    std::string word;
    while (this->peek() != ' ' && this->peek() != '\n') {
        word += this->peek();
        if (this->charIdx == this->inputBuffer.size() - 1) {
            this->reachedEndOfFile = true;
            break;
        }
        this->next();
    }
    return word;
}

std::string Lexer::readChar() {
    std::string character = "'";
    this->next();
    for (int i = 0; i < 2; i++) {
        character += this->peek();
        this->next();
        if (this->charIdx == this->inputBuffer.size()) {
            this->reachedEndOfFile = true;
            break;
        }
    }
    return character;
}

std::string Lexer::readString() {
    std::string string = "\"";
    this->next();
    // keep consuming chars until " (without escape character) appears in string
    while (!(this->peek() == '"' && this->inputBuffer.at(this->charIdx - 1) != '\\')) {
        string += this->peek();
        this->next();
        if (this->charIdx == this->inputBuffer.size() - 1) {
            this->reachedEndOfFile = true;
            break;
        }
    }
    string += this->peek();
    this->next();
    return string;
}

void Lexer::skipWhitespace() {
    while (this->charIdx + 1 < this->inputBuffer.size() && this->inputBuffer[this->charIdx] == ' ') {
        this->next();
    }
    // reached end of file
    if (this->charIdx == this->inputBuffer.size() - 1) {
        this->reachedEndOfFile = true;
    }
}

void Lexer::skipComment() {
    while (this->charIdx + 1 < this->inputBuffer.size() && this->inputBuffer[this->charIdx] != '\n') {
        this->next();
    }
    // reached end of file
    if (this->charIdx == this->inputBuffer.size() - 1) {
        this->reachedEndOfFile = true;
    }
    this->lineNumber++;
    this->next();
}

bool Lexer::isValidLabel(const std::string& s) {
    return std::regex_match(s, std::regex{R"(\$[a-zA-Z_][a-zA-Z0-9_]*)"});
}

bool Lexer::isValidNumber(const std::string& s) {
    return std::regex_match(s, std::regex{R"(-?[0-9]+(\.[0-9]+)?)"});
}

bool Lexer::isValidImmediate(const std::string& s) {
    return std::regex_match(s, std::regex{R"(#-?[0-9]+(\.[0-9]+)?)"});
}

bool Lexer::isValidString(const std::string &s) {
    return s.size() > 2;
}

void Lexer::outputInvalidTokenError(const std::string& word) const {
    this->outputLineNumberOfError();
    std::cerr << "Invalid SVMA Token \'" << word << "\'\n" << std::endl;
}

void Lexer::outputInvalidLabelError(const std::string &word) const {
    this->outputLineNumberOfError();
    std::cerr << "Invalid label \'" << word << "\'\n" << std::endl;
}

void Lexer::outputLineNumberOfError() const {
    std::cerr << "Error found at Line " << this->lineNumber << std::endl;
}
