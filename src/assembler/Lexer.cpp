#include "Lexer.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <optional>
#include <regex>
#include <filesystem>

assembler::Lexer::Lexer() : charIdx(0), lineNumber(0), reachedEndOfFile(false) {
}

std::optional<std::vector<AssemblerDefs::SVMAToken>> assembler::Lexer::lex(const std::string &filePath) {
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

std::optional<std::vector<AssemblerDefs::SVMAToken>> assembler::Lexer::lexString(const std::string& fileContent) {
    char character;

    this->inputBuffer.assign(fileContent.begin(), fileContent.end());
    return this->buildTokenStream();
}

std::optional<std::vector<AssemblerDefs::SVMAToken>> assembler::Lexer::buildTokenStream() {
    this->charIdx = 0;
    this->lineNumber = 1;
    this->reachedEndOfFile = false;

    std::vector<AssemblerDefs::SVMAToken> tokensStream;

    while (!this->reachedEndOfFile && this->charIdx < this->inputBuffer.size()) {

        this->skipWhitespace();

        if (this->charIdx >= this->inputBuffer.size() || this->peek() == ' ') {
            break;
        }

        switch (this->peek()) {
            case '\n':
                this->lineNumber++;
                this->next();
                break;

            case ';':
                this->skipComment();
                break;

            default:
                if (this->charIdx < this->inputBuffer.size()) {
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

std::optional<AssemblerDefs::SVMAToken> assembler::Lexer::lexToken() {
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
    if (this->charIdx >= this->inputBuffer.size()) {
        this->reachedEndOfFile = true;
    }
}

char assembler::Lexer::peek() const {
    return this->inputBuffer[this->charIdx];
}

std::optional<char> assembler::Lexer::peekNext() {
    if (this->charIdx + 1 == this->inputBuffer.size() - 1) {
        return std::nullopt;
    }
    return this->inputBuffer[this->charIdx + 1];
}

std::optional<AssemblerDefs::SVMAToken> assembler::Lexer::lexLabel() {
    // enforce label does not start with a number
    std::optional<char> nextChar = peekNext();
    if (nextChar.has_value() && std::isdigit(nextChar.value())) {
        return std::nullopt;
    }
    // keep going through inputBuffer until reaching EOF, space, end of line, colon
    const std::string label = this->readUntilWhitespace();
    // LABEL_DEF
    if (label[label.size() - 1] == ':') {
        if (!isValidLabel(label.substr(0, label.size() - 1))) {
            this->outputInvalidLabelError(label);
            return std::nullopt;
        }
        return AssemblerDefs::SVMAToken{AssemblerDefs::SVMATokenType::LABEL_DEF, label, this->lineNumber};
    }
    // LABEL_REF
    if (!isValidLabel(label)) {
        this->outputInvalidLabelError(label);
        return std::nullopt;
    }
    return AssemblerDefs::SVMAToken{AssemblerDefs::SVMATokenType::LABEL_REF, label, this->lineNumber};
}

std::optional<AssemblerDefs::SVMAToken> assembler::Lexer::lexNumber() {
    std::string number = this->readUntilWhitespace();
    if (!isValidNumber(number)) {
        this->outputLineNumberOfError();
        std::cerr << "Invalid number \' " << number << "\'" << std::endl;
        return std::nullopt;
    }
    return AssemblerDefs::SVMAToken{AssemblerDefs::SVMATokenType::NUMBER, number, this->lineNumber};
}

std::optional<AssemblerDefs::SVMAToken> assembler::Lexer::lexHex() {
    const std::string hex = this->readUntilWhitespace();
    if (!isValidHex(hex)) {
        this->outputLineNumberOfError();
        std::cerr << "Invalid hexadecimal number \'" << hex << "\'" << std::endl;
        return std::nullopt;
    }
    return AssemblerDefs::SVMAToken{AssemblerDefs::SVMATokenType::HEX, hex, this->lineNumber};
}

std::optional<AssemblerDefs::SVMAToken> assembler::Lexer::lexImmediate() {
    const std::string immediate = this->readUntilWhitespace();
    if (!isValidImmediate(immediate)) {
        this->outputLineNumberOfError();
        std::cerr << "Invalid immediate \'" << immediate << "\'" << std::endl;
        return std::nullopt;
    }
    return AssemblerDefs::SVMAToken{AssemblerDefs::SVMATokenType::IMMEDIATE, immediate, this->lineNumber};
}

std::optional<AssemblerDefs::SVMAToken> assembler::Lexer::lexDirective() {
    const std::string directive = this->readUntilWhitespace();
    if (directive != ".data" &&
        directive != ".debug" &&
        directive != ".source" &&
        directive != ".line_table"
    ) {
        this->outputInvalidTokenError(directive);
        return std::nullopt;
    }
    return AssemblerDefs::SVMAToken{AssemblerDefs::SVMATokenType::DIRECTIVE, directive, this->lineNumber};
}

std::optional<AssemblerDefs::SVMAToken> assembler::Lexer::lexString() {
    const std::string string = this->readString();
    if (!isValidString(string)) {
        this->outputLineNumberOfError();
        if (this->charIdx >= this->inputBuffer.size()) {
            std::cerr << "Unterminated string" << std::endl;
        } else {
            std::cerr << "Invalid string \'" << string << "\'" << std::endl;
        }
        return std::nullopt;
    }
    return AssemblerDefs::SVMAToken{AssemblerDefs::SVMATokenType::STRING, string, this->lineNumber};
}

std::optional<AssemblerDefs::SVMAToken> assembler::Lexer::lexKeyWord() {
    const std::string keyword = this->readUntilWhitespace();

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
    // NATIVE_REF
    if (AssemblerDefs::nativeRef.find(keyword) != AssemblerDefs::nativeRef.end()) {
        return AssemblerDefs::SVMAToken{AssemblerDefs::SVMATokenType::NATIVE_REF, keyword, this->lineNumber};
    }
    this->outputInvalidTokenError(keyword);
    return std::nullopt;
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

std::string assembler::Lexer::readChar() {
    std::string character = "'";
    this->next();
    for (int i = 0; i < 2; i++) {
        character += this->peek();
        this->next();
        if (this->reachedEndOfFile) {
            break;
        }
    }
    return character;
}

std::string assembler::Lexer::readString() {
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
    return std::regex_match(s, std::regex{R"(\$[a-zA-Z_][a-zA-Z0-9_(),]*)"});
}

bool assembler::Lexer::isValidNumber(const std::string& s) {
    return std::regex_match(s, std::regex{R"(-?[0-9]+(\.[0-9]+)?)"});
}

bool assembler::Lexer::isValidHex(const std::string& s) {
    return std::regex_match(s, std::regex{R"(0x[0-9A-Fa-f]+)"});
}

bool assembler::Lexer::isValidImmediate(const std::string& s) {
    return std::regex_match(s, std::regex{R"(#-?[0-9]+(\.[0-9]+)?)"});
}

bool assembler::Lexer::isValidString(const std::string &s) {
    return s.size() > 2;
}

void assembler::Lexer::outputInvalidTokenError(const std::string& word) const {
    this->outputLineNumberOfError();
    std::cerr << "Invalid SVMA Token \'" << word << "\'\n" << std::endl;
}

void assembler::Lexer::outputInvalidLabelError(const std::string &word) const {
    this->outputLineNumberOfError();
    std::cerr << "Invalid label \'" << word << "\'\n" << std::endl;
}

void assembler::Lexer::outputLineNumberOfError() const {
    std::cerr << "Error found at Line " << this->lineNumber << std::endl;
}
