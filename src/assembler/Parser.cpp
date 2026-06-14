#include "Parser.h"

#include <iostream>
#include <limits>
#include <regex>


Parser::Parser() {}

std::optional<std::vector<AssemblerDefs::Statement>> Parser::parse(std::vector<AssemblerDefs::SVMAToken> &tokenStream) {
    std::vector<AssemblerDefs::Statement> statements;
    this->tokenStream = tokenStream;
    this->tokenIdx = 0;
    this->section = AssemblerDefs::Section::CODE;

    while (this->peek().type != AssemblerDefs::SVMATokenType::END_OF_FILE) {

        auto statement = this->parseToken();
        if (!statement.has_value()) {
            return std::nullopt;
        }
        statements.push_back(statement.value());
    }
    return statements;
}

std::optional<AssemblerDefs::Statement> Parser::parseToken() {

    switch (this->peek().type) {
        case AssemblerDefs::SVMATokenType::INSTRUCTION:
            return this->parseInstruction();
        case AssemblerDefs::SVMATokenType::LABEL_DEF:
            return this->parseLabelDef();
        case AssemblerDefs::SVMATokenType::METHOD_DEF:
            return this->parseMethodDef();
        case AssemblerDefs::SVMATokenType::DATA_START:
            return this->parseSectionStart();
    }

    if (this->section == AssemblerDefs::Section::DATA) {
        return this->parseData();
    }

    std::cerr << "Error found at Line " << this->peek().lineNumber << std::endl;
    std::cerr << "Unexpected token \'" << this->peek().value << "\'" << std::endl;

    return std::nullopt;
}

std::optional<AssemblerDefs::Statement> Parser::parseInstruction() {
    std::string instruction = this->peek().value;
    int lineNumber = this->peek().lineNumber;
    this->next();

    AssemblerDefs::Operand dataType;
    AssemblerDefs::Operand type;
    AssemblerDefs::Operand labelRef;
    AssemblerDefs::Operand immediate;

    // parse DATA_TYPE Token
    if (instruction == "out" ||
        instruction == "inn") {
        auto optionalDataType = this->parseDataType();
        if (!optionalDataType.has_value()) {
            return std::nullopt;
        }
        dataType = optionalDataType.value();
    }

    // parse TYPE Token
    if (instruction == "push" ||
        instruction == "load" ||
        instruction == "loadL" ||
        instruction == "conv") {

        auto optionalType = this->parseType();
        if (!optionalType.has_value()) {
            return std::nullopt;
        }
        type = optionalType.value();
    }

    // parse LABEL_REF Token
    if (instruction == "loadG" ||
        instruction == "storeG" ||
        instruction == "call" ||
        instruction == "jmp" ||
        instruction == "jez" ||
        instruction == "jnz") {

        auto optionalLabelRef = this->parseLabelRef();
        if (!optionalLabelRef.has_value()) {
            return std::nullopt;
        }
        labelRef = optionalLabelRef.value();
    }

    // parse IMMEDIATE Token
    if (instruction == "loadL" ||
        instruction == "storeL") {

        auto optionalImmediate = this->parseImmediate();
        if (!optionalImmediate.has_value()) {
            return std::nullopt;
        }
        immediate = optionalImmediate.value();
    }

    // check value is valid with given type (if operand consists of both type and value)
    if (!immediate.value.empty()) {
        if (!type.value.empty()) { // type
            if (!this->isValidFoType(type.value, immediate.value)) {
                this->handleValueOutOfRangeError(type.value, immediate.value, lineNumber);
                return std::nullopt;
            }
        } else if (!dataType.value.empty()) { // data type
            if (!this->isValidFoType(dataType.value, immediate.value)) {
                this->handleValueOutOfRangeError(dataType.value, immediate.value, lineNumber);
                return std::nullopt;
            }
        }
    }

    if (instruction == "nop") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "halt") return AssemblerDefs::Instruction{instruction, {}, lineNumber};

    //========================================================================================================
    // STACK
    //========================================================================================================

    if (instruction == "push") {
        if (this->peek().type == AssemblerDefs::SVMATokenType::LABEL_REF) {
            auto labelOperand = this->parseLabelRef().value();
            return AssemblerDefs::Instruction{instruction, {type, labelOperand}, lineNumber};
        }
        if (this->peek().type == AssemblerDefs::SVMATokenType::IMMEDIATE) {
            auto immediateOperand = this->parseImmediate().value();
            return AssemblerDefs::Instruction{instruction, {type, immediateOperand}, lineNumber};
        }
        this->handleUnexpectedTokenError({AssemblerDefs::SVMATokenType::LABEL_REF, AssemblerDefs::SVMATokenType::IMMEDIATE});
        return std::nullopt;
    }
    if (instruction == "pop") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "dup") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "swap") return AssemblerDefs::Instruction{instruction, {}, lineNumber};

    //========================================================================================================
    // MEMORY
    //========================================================================================================

    if (instruction == "load") return AssemblerDefs::Instruction{instruction, {type}, lineNumber};
    if (instruction == "loadG") return AssemblerDefs::Instruction{instruction, {labelRef}, lineNumber};
    if (instruction == "loadL") return AssemblerDefs::Instruction{instruction, {type, immediate}, lineNumber};
    if (instruction == "store") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "storeG") return AssemblerDefs::Instruction{instruction, {labelRef}, lineNumber};
    if (instruction == "storeL") return AssemblerDefs::Instruction{instruction, {immediate}, lineNumber};
    if (instruction == "alloc") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "free") return AssemblerDefs::Instruction{instruction, {}, lineNumber};

    //========================================================================================================
    // CONTROL
    //========================================================================================================

    if (instruction == "call") return AssemblerDefs::Instruction{instruction, {labelRef}, lineNumber};
    if (instruction == "ret") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "jmp") return AssemblerDefs::Instruction{instruction, {labelRef}, lineNumber};
    if (instruction == "jez") return AssemblerDefs::Instruction{instruction, {labelRef}, lineNumber};
    if (instruction == "jnz") return AssemblerDefs::Instruction{instruction, {labelRef}, lineNumber};

    //========================================================================================================
    // ARITHMETIC
    //========================================================================================================

    if (instruction == "add") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "sub") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "mul") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "div") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "mod") return AssemblerDefs::Instruction{instruction, {}, lineNumber};

    if (instruction == "not") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "and") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "orr") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "xor") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "shl") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "shr") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "sar") return AssemblerDefs::Instruction{instruction, {}, lineNumber};

    if (instruction == "ceq") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "cne") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "clt") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "cle") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "cgt") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "cge") return AssemblerDefs::Instruction{instruction, {}, lineNumber};

    //========================================================================================================
    // Other
    //========================================================================================================

    if (instruction == "out") return AssemblerDefs::Instruction{instruction, {dataType}, lineNumber};
    if (instruction == "in") return AssemblerDefs::Instruction{instruction, {dataType}, lineNumber};
    if (instruction == "conv") return AssemblerDefs::Instruction{instruction, {}, lineNumber};

    std::cerr << "Error found at Line " << lineNumber << std::endl;
    std::cerr << "Undefined instruction: " << instruction << std::endl;
    return std::nullopt;
}

std::optional<AssemblerDefs::Statement> Parser::parseLabelDef() {
    auto token = this->peek();
    this->next();
    return AssemblerDefs::Label{token.value, token.lineNumber};
}

std::optional<AssemblerDefs::Statement> Parser::parseData() {
    auto dataTypetoken = this->peek();
    this->next();
    auto immediateToken = this->peek();

    if (dataTypetoken.type == AssemblerDefs::SVMATokenType::TYPE) {
        if (dataTypetoken.value == "i32" || dataTypetoken.value == "i64") {
            // enforce i32 / i64 is matched with an integer immediate
            if (!this->isNumberInteger(immediateToken)) {
                std::cerr << "Error found at Line " << this->peek().lineNumber << std::endl;
                std::cerr << "Expecting integer" << std::endl;
                return std::nullopt;
            }
        } else if (dataTypetoken.value == "ui32" || dataTypetoken.value == "u64") {
            // enforce ui32 / ui64 is matched with an unsigned integer immediate
            if (this->isNumberSigned(immediateToken) || !this->isNumberInteger(immediateToken)) {
                std::cerr << "Error found at Line " << this->peek().lineNumber << std::endl;
                std::cerr << "Expecting unsigned integer" << std::endl;
                return std::nullopt;
            }

        } else if (dataTypetoken.value == "ptr") {
            // enforce ptr type is matched with LABEL_REF
            if (this->peek().type == AssemblerDefs::SVMATokenType::LABEL_REF) {
                this->handleUnexpectedTokenError({AssemblerDefs::SVMATokenType::LABEL_REF});
                return std::nullopt;
            }
        }
    } else if (dataTypetoken.type == AssemblerDefs::SVMATokenType::DATA_TYPE) {
        // enforce str type is matched with STRING
        if (this->peek().type != AssemblerDefs::SVMATokenType::STRING) {
            this->handleUnexpectedTokenError({AssemblerDefs::SVMATokenType::STRING});
            return std::nullopt;
        }
    }
    auto data = AssemblerDefs::Data{dataTypetoken.value, this->peek().value};
    this->next();
    return data;
}

std::optional<AssemblerDefs::Statement> Parser::parseMethodDef() {
    AssemblerDefs::SVMAToken numberOfArgsToken;
    AssemblerDefs::SVMAToken numberOfLocalsToken;

    this->next();
    if (this->peek().type != AssemblerDefs::SVMATokenType::LABEL_DEF) {
        this->handleUnexpectedTokenError({AssemblerDefs::SVMATokenType::LABEL_DEF});
        return std::nullopt;
    }
    std::string methodName = this->peek().value;
    int lineNumber = this->peek().lineNumber;
    this->next();
    while (this->peek().type == AssemblerDefs::SVMATokenType::METHOD_METADATA_FIELD) {
        if (this->peek().value == "args") {
            this->next();
            numberOfArgsToken = this->peek();
            if (numberOfArgsToken.type != AssemblerDefs::SVMATokenType::NUMBER) {
                this->handleUnexpectedTokenError({AssemblerDefs::SVMATokenType::NUMBER});
                return std::nullopt;
            }
            // enforce number of args is an unsigned integer
            if (!isNumberInteger(numberOfArgsToken) || isNumberSigned(numberOfArgsToken)) {
                std::cerr << "Error found at Line " << this->peek().lineNumber << std::endl;
                std::cerr << "Expecting unsigned integer" << std::endl;
                return std::nullopt;
            }
            // enforce number of args is not higher than 255 (max number of args)
            if (stoi(numberOfArgsToken.value) > 255) {
                std::cerr << "Error found at Line " << this->peek().lineNumber << std::endl;
                std::cerr << "Number of method arguments out of range (0 - 255)" << std::endl;
                return std::nullopt;
            }
            this->next();
            continue;
        }
        if (this->peek().value == "locals") {
            this->next();
            numberOfLocalsToken = this->peek();
            if (numberOfLocalsToken.type != AssemblerDefs::SVMATokenType::NUMBER) {
                this->handleUnexpectedTokenError({AssemblerDefs::SVMATokenType::NUMBER});
                return std::nullopt;
            }
            // enforce number of locals is an unsigned integer
            if (!isNumberInteger(numberOfLocalsToken) || isNumberSigned(numberOfLocalsToken)) {
                std::cerr << "Error found at Line " << this->peek().lineNumber << std::endl;
                std::cerr << "Expecting unsigned integer" << std::endl;
                return std::nullopt;
            }
            this->next();
        }
    }

    return AssemblerDefs::MethodDef{
        methodName,
        static_cast<uint8_t>(stoi(numberOfArgsToken.value)),
        static_cast<uint32_t>(stoi(numberOfLocalsToken.value)),
        lineNumber
    };
}

std::optional<AssemblerDefs::Operand> Parser::parseType() {
    return this->parseOperand(AssemblerDefs::SVMATokenType::TYPE);
}

std::optional<AssemblerDefs::Operand> Parser::parseDataType() {
    return this->parseOperand(AssemblerDefs::SVMATokenType::DATA_TYPE);
}

std::optional<AssemblerDefs::Operand> Parser::parseImmediate() {
    return this->parseOperand(AssemblerDefs::SVMATokenType::IMMEDIATE);
}

std::optional<AssemblerDefs::Operand> Parser::parseLabelRef() {
    return this->parseOperand(AssemblerDefs::SVMATokenType::LABEL_REF);
}

std::optional<AssemblerDefs::Operand> Parser::parseOperand(const AssemblerDefs::SVMATokenType tokenType) {
    auto token = this->peek();
    if (this->peek().type != tokenType) {
        return std::nullopt;
    }
    this->next();
    return AssemblerDefs::Operand{this->mapTokenTypeToOperandType(tokenType), token.value};
}

std::optional<AssemblerDefs::Statement> Parser::parseSectionStart() {
    if (this->section == AssemblerDefs::Section::DATA) {
        std::cerr << "Error found at Line " << this->peek().lineNumber << std::endl;
        std::cerr << "Duplicate section declaration" << std::endl;
        this->next();
        return std::nullopt;
    }
    this->next();
    this->section = AssemblerDefs::Section::DATA;
    return AssemblerDefs::Section::DATA;
}

void Parser::next() {
    this->tokenIdx++;
}

AssemblerDefs::SVMAToken Parser::peek() {
    return this->tokenStream[this->tokenIdx];
}

AssemblerDefs::SVMAToken Parser::peekNext() {
    return this->tokenStream[this->tokenIdx + 1];
}

bool Parser::isValidFoType(const std::string& type, const std::string& value) {
    try {
        std::string valueToCheck;
        if (value[0] == '#') {
            valueToCheck = value.substr(1);
        }

        if (type == "i32") {
            std::stoi(valueToCheck);
        } else if (type == "ui32") {
            std::stoul(valueToCheck);
        } else if (type == "i64") {
            std::stoll(valueToCheck);
        } else if (type == "ui64") {
            std::stoull(valueToCheck);
        } else if (type == "f32") {
            std::stof(valueToCheck);
        } else if (type == "f64") {
            std::stod(valueToCheck);
        }

    } catch (const std::invalid_argument& e) {
        return false;
    } catch (const std::out_of_range& e) {
        return false;
    }
    return true;
}

void Parser::handleValueOutOfRangeError(const std::string& dataType, const std::string& data, const int& lineNumber) const {
    std::cerr << "Error found at Line " << lineNumber << std::endl;
    std::cerr << data << " out of range for " << dataType << std::endl;
}

bool Parser::isNumberInteger(const AssemblerDefs::SVMAToken& token) {
    return std::regex_match(token.value, std::regex("-?[0-9]*"));
}

bool Parser::isNumberSigned(const AssemblerDefs::SVMAToken& token) {
    return std::regex_match(token.value, std::regex("-[0-9]+(.[0-9]+)?"));
}

AssemblerDefs::OperandType Parser::mapTokenTypeToOperandType(AssemblerDefs::SVMATokenType tokenType) {
    switch (tokenType) {
        case AssemblerDefs::SVMATokenType::IMMEDIATE: return AssemblerDefs::OperandType::IMMEDIATE;
        case AssemblerDefs::SVMATokenType::TYPE: return AssemblerDefs::OperandType::TYPE;
        case AssemblerDefs::SVMATokenType::DATA_TYPE: return AssemblerDefs::OperandType::DATA_TYPE;
        case AssemblerDefs::SVMATokenType::LABEL_REF: return AssemblerDefs::OperandType::LABEL_REF;
    }
}

void Parser::handleUnexpectedTokenError(const std::vector<AssemblerDefs::SVMATokenType> &expectingTypes) {
    std::cerr << "Error found at Line " << this->peek().lineNumber << std::endl;
    std::cerr << "Expecting ";
    for (int i = 0; i < expectingTypes.size(); i++) {
        if (i + 1 == expectingTypes.size()) std::cerr << tokenTypeToString(expectingTypes.at(i));
        else std::cerr << tokenTypeToString(expectingTypes.at(i)) << " / ";
    }
    std::cerr << ", found " << tokenTypeToString(this->peek().type) << std::endl;
}

std::string Parser::tokenTypeToString(AssemblerDefs::SVMATokenType tokenType) {
    std::string s;
    switch (tokenType) {
        case AssemblerDefs::SVMATokenType::DATA_START: s = "SECTION_START"; break;
        case AssemblerDefs::SVMATokenType::INSTRUCTION: s = "INSTRUCTION"; break;
        case AssemblerDefs::SVMATokenType::TYPE: s = "TYPE"; break;
        case AssemblerDefs::SVMATokenType::DATA_TYPE: s = "DATA_TYPE"; break;
        case AssemblerDefs::SVMATokenType::NUMBER: s = "NUMBER"; break;
        case AssemblerDefs::SVMATokenType::IMMEDIATE: s = "IMMEDIATE"; break;
        case AssemblerDefs::SVMATokenType::STRING: s = "STRING"; break;
        case AssemblerDefs::SVMATokenType::LABEL_REF: s = "LABEL_REF"; break;
        case AssemblerDefs::SVMATokenType::LABEL_DEF: s = "LABEL_DEF"; break;
        case AssemblerDefs::SVMATokenType::METHOD_DEF: s = "METHOD_DEF"; break;
        case AssemblerDefs::SVMATokenType::METHOD_METADATA_FIELD : s = "METHOD_METADATA_FIELD"; break;
    }
    return s;
}
