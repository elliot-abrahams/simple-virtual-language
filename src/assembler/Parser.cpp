//
// Created by erael on 04/05/2026.
//

#pragma once
#include "Parser.h"

#include <iostream>
#include <regex>


Parser::Parser() {}

std::optional<std::vector<Types::Statement>> Parser::parse(std::vector<Types::SVMAToken> &tokenStream) {
    std::vector<Types::Statement> statements;
    this->tokenStream = tokenStream;
    this->tokenIdx = 0;
    this->section = Types::Section::CODE;

    while (this->peek().type != Types::SVMATokenType::END_OF_FILE) {

        auto statement = this->parseToken();
        if (!statement.has_value()) {
            return std::nullopt;
        }
        statements.push_back(statement.value());
    }
    return statements;
}

std::optional<Types::Statement> Parser::parseToken() {

    switch (this->peek().type) {
        case Types::SVMATokenType::INSTRUCTION:
            return this->parseInstruction();
        case Types::SVMATokenType::LABEL_DEF:
            return this->parseLabelDef();
        case Types::SVMATokenType::METHOD_DEF:
            return this->parseMethodDef();
        case Types::SVMATokenType::DATA_START:
            return this->parseSectionStart();
    }

    if (this->section == Types::Section::DATA) {
        return this->parseData();
    }

    std::cerr << "Error found at Line " << this->peek().lineNumber << std::endl;
    std::cerr << "Unexpected token \'" << this->peek().value << "\'" << std::endl;

    return std::nullopt;
}

std::optional<Types::Statement> Parser::parseInstruction() {
    std::string instruction = this->peek().value;
    int lineNumber = this->peek().lineNumber;
    this->next();

    Types::Operand dataType;
    Types::Operand type;
    Types::Operand labelRef;
    Types::Operand immediate;

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
        instruction == "loadG" ||
        instruction == "loadL" ||
        instruction == "store" ||
        instruction == "storeG" ||
        instruction == "storeL" ||
        instruction == "add" ||
        instruction == "sub" ||
        instruction == "mul" ||
        instruction == "div" ||
        instruction == "mod" ||
        instruction == "not" ||
        instruction == "notB" ||
        instruction == "and" ||
        instruction == "orr" ||
        instruction == "xor" ||
        instruction == "shl" ||
        instruction == "shr" ||
        instruction == "ceq" ||
        instruction == "cne" ||
        instruction == "clt" ||
        instruction == "cle" ||
        instruction == "cgt" ||
        instruction == "cge" ||
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

    if (instruction == "nop") return Types::Instruction{instruction, {}, lineNumber};
    if (instruction == "halt") return Types::Instruction{instruction, {}, lineNumber};

    //========================================================================================================
    // STACK
    //========================================================================================================

    if (instruction == "push") {
        if (this->peek().type == Types::SVMATokenType::LABEL_REF) {
            auto labelOperand = this->parseLabelRef().value();
            return Types::Instruction{instruction, {type, labelOperand}, lineNumber};
        }
        if (this->peek().type == Types::SVMATokenType::IMMEDIATE) {
            auto immediateOperand = this->parseImmediate().value();
            return Types::Instruction{instruction, {type, immediateOperand}, lineNumber};
        }
        this->handleUnexpectedTokenError({Types::SVMATokenType::LABEL_REF, Types::SVMATokenType::IMMEDIATE});
        return std::nullopt;
    }
    if (instruction == "pop") return Types::Instruction{instruction, {}, lineNumber};
    if (instruction == "dup") return Types::Instruction{instruction, {}, lineNumber};
    if (instruction == "swap") return Types::Instruction{instruction, {}, lineNumber};

    //========================================================================================================
    // MEMORY
    //========================================================================================================

    if (instruction == "load") return Types::Instruction{instruction, {type}, lineNumber};
    if (instruction == "loadG") return Types::Instruction{instruction, {type, labelRef}, lineNumber};
    if (instruction == "loadL") return Types::Instruction{instruction, {type, immediate}, lineNumber};
    if (instruction == "store") return Types::Instruction{instruction, {type}, lineNumber};
    if (instruction == "storeG") return Types::Instruction{instruction, {type, labelRef}, lineNumber};
    if (instruction == "storeL") return Types::Instruction{instruction, {type, immediate}, lineNumber};
    if (instruction == "alloc") return Types::Instruction{instruction, {}, lineNumber};
    if (instruction == "free") return Types::Instruction{instruction, {}, lineNumber};

    //========================================================================================================
    // CONTROL
    //========================================================================================================

    if (instruction == "call") return Types::Instruction{instruction, {labelRef}, lineNumber};
    if (instruction == "ret") return Types::Instruction{instruction, {}, lineNumber};
    if (instruction == "jmp") return Types::Instruction{instruction, {labelRef}, lineNumber};
    if (instruction == "jez") return Types::Instruction{instruction, {labelRef}, lineNumber};
    if (instruction == "jnz") return Types::Instruction{instruction, {labelRef}, lineNumber};

    //========================================================================================================
    // ARITHMETIC
    //========================================================================================================

    if (instruction == "add") return Types::Instruction{instruction, {type}, lineNumber};
    if (instruction == "sub") return Types::Instruction{instruction, {type}, lineNumber};
    if (instruction == "mul") return Types::Instruction{instruction, {type}, lineNumber};
    if (instruction == "div") return Types::Instruction{instruction, {type}, lineNumber};
    if (instruction == "mod") return Types::Instruction{instruction, {type}, lineNumber};

    if (instruction == "not") return Types::Instruction{instruction, {type}, lineNumber};
    if (instruction == "notB") return Types::Instruction{instruction, {type}, lineNumber};
    if (instruction == "and") return Types::Instruction{instruction, {type}, lineNumber};
    if (instruction == "orr") return Types::Instruction{instruction, {type}, lineNumber};
    if (instruction == "xor") return Types::Instruction{instruction, {type}, lineNumber};
    if (instruction == "shl") return Types::Instruction{instruction, {type}, lineNumber};
    if (instruction == "shr") return Types::Instruction{instruction, {type}, lineNumber};

    if (instruction == "ceq") return Types::Instruction{instruction, {type}, lineNumber};
    if (instruction == "cne") return Types::Instruction{instruction, {type}, lineNumber};
    if (instruction == "clt") return Types::Instruction{instruction, {type}, lineNumber};
    if (instruction == "cle") return Types::Instruction{instruction, {type}, lineNumber};
    if (instruction == "cgt") return Types::Instruction{instruction, {type}, lineNumber};
    if (instruction == "cge") return Types::Instruction{instruction, {type}, lineNumber};

    //========================================================================================================
    // Other
    //========================================================================================================

    if (instruction == "out") return Types::Instruction{instruction, {dataType}, lineNumber};
    if (instruction == "in") return Types::Instruction{instruction, {dataType}, lineNumber};
    if (instruction == "conv") return Types::Instruction{instruction, {}, lineNumber};

    std::cerr << "Error found at Line " << lineNumber << std::endl;
    std::cerr << "Undefined instruction: " << instruction << std::endl;
    return std::nullopt;
}

std::optional<Types::Statement> Parser::parseLabelDef() {
    auto token = this->peek();
    this->next();
    return Types::Label{token.value, token.lineNumber};
}

std::optional<Types::Statement> Parser::parseData() {
    auto dataTypetoken = this->peek();
    this->next();
    auto immediateToken = this->peek();

    if (dataTypetoken.type == Types::SVMATokenType::TYPE) {
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
            if (this->peek().type == Types::SVMATokenType::LABEL_REF) {
                this->handleUnexpectedTokenError({Types::SVMATokenType::LABEL_REF});
                return std::nullopt;
            }

        } else if (dataTypetoken.value == "char") {
            // enforce char type is matched with CHAR
            if (this->peek().type != Types::SVMATokenType::CHAR) {
                this->handleUnexpectedTokenError({Types::SVMATokenType::CHAR});
                return std::nullopt;
            }
        }
    } else if (dataTypetoken.type == Types::SVMATokenType::DATA_TYPE) {
        // enforce str type is matched with STRING
        if (this->peek().type != Types::SVMATokenType::STRING) {
            this->handleUnexpectedTokenError({Types::SVMATokenType::STRING});
            return std::nullopt;
        }
    }
    auto data = Types::Data{dataTypetoken.value, this->peek().value};
    this->next();
    return data;
}

std::optional<Types::Statement> Parser::parseMethodDef() {
    Types::SVMAToken numberOfArgsToken;
    Types::SVMAToken numberOfLocalsToken;

    this->next();
    if (this->peek().type != Types::SVMATokenType::LABEL_DEF) {
        this->handleUnexpectedTokenError({Types::SVMATokenType::LABEL_DEF});
        return std::nullopt;
    }
    std::string methodName = this->peek().value;
    int lineNumber = this->peek().lineNumber;
    this->next();
    while (this->peek().type == Types::SVMATokenType::METHOD_METADATA_FIELD) {
        if (this->peek().value == "args") {
            this->next();
            numberOfArgsToken = this->peek();
            if (numberOfArgsToken.type != Types::SVMATokenType::NUMBER) {
                this->handleUnexpectedTokenError({Types::SVMATokenType::NUMBER});
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
            if (numberOfLocalsToken.type != Types::SVMATokenType::NUMBER) {
                this->handleUnexpectedTokenError({Types::SVMATokenType::NUMBER});
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

    return Types::MethodDef{
        methodName,
        static_cast<uint8_t>(stoi(numberOfArgsToken.value)),
        static_cast<uint32_t>(stoi(numberOfLocalsToken.value)),
        lineNumber
    };
}

std::optional<Types::Operand> Parser::parseType() {
    return this->parseOperand(Types::SVMATokenType::TYPE);
}

std::optional<Types::Operand> Parser::parseDataType() {
    return this->parseOperand(Types::SVMATokenType::DATA_TYPE);
}

std::optional<Types::Operand> Parser::parseImmediate() {
    return this->parseOperand(Types::SVMATokenType::IMMEDIATE);
}

std::optional<Types::Operand> Parser::parseLabelRef() {
    return this->parseOperand(Types::SVMATokenType::LABEL_REF);
}

std::optional<Types::Operand> Parser::parseOperand(const Types::SVMATokenType tokenType) {
    auto token = this->peek();
    if (this->peek().type != tokenType) {
        return std::nullopt;
    }
    this->next();
    return Types::Operand{this->mapTokenTypeToOperandType(tokenType), token.value};
}

std::optional<Types::Statement> Parser::parseSectionStart() {
    if (this->section == Types::Section::DATA) {
        std::cerr << "Error found at Line " << this->peek().lineNumber << std::endl;
        std::cerr << "Duplicate section declaration" << std::endl;
        this->next();
        return std::nullopt;
    }
    this->next();
    this->section = Types::Section::DATA;
    return Types::Section::DATA;
}

void Parser::next() {
    this->tokenIdx++;
}

Types::SVMAToken Parser::peek() {
    return this->tokenStream[this->tokenIdx];
}

Types::SVMAToken Parser::peekNext() {
    return this->tokenStream[this->tokenIdx + 1];
}

bool Parser::isNumberInteger(const Types::SVMAToken& token) {
    return std::regex_match(token.value, std::regex("-?[0-9]*"));
}

bool Parser::isNumberSigned(const Types::SVMAToken& token) {
    return std::regex_match(token.value, std::regex("-[0-9]+(.[0-9]+)?"));
}

Types::OperandType Parser::mapTokenTypeToOperandType(Types::SVMATokenType tokenType) {
    switch (tokenType) {
        case Types::SVMATokenType::IMMEDIATE: return Types::OperandType::IMMEDIATE;
        case Types::SVMATokenType::TYPE: return Types::OperandType::TYPE;
        case Types::SVMATokenType::DATA_TYPE: return Types::OperandType::DATA_TYPE;
        case Types::SVMATokenType::LABEL_REF: return Types::OperandType::LABEL_REF;
    }
}

void Parser::handleUnexpectedTokenError(const std::vector<Types::SVMATokenType> &expectingTypes) {
    std::cerr << "Error found at Line " << this->peek().lineNumber << std::endl;
    std::cerr << "Expecting ";
    for (int i = 0; i < expectingTypes.size(); i++) {
        if (i + 1 == expectingTypes.size()) std::cerr << tokenTypeToString(expectingTypes.at(i));
        else std::cerr << tokenTypeToString(expectingTypes.at(i)) << " / ";
    }
    std::cerr << ", found " << tokenTypeToString(this->peek().type) << std::endl;
}

std::string Parser::tokenTypeToString(Types::SVMATokenType tokenType) {
    std::string s;
    switch (tokenType) {
        case Types::SVMATokenType::DATA_START: s = "SECTION_START"; break;
        case Types::SVMATokenType::INSTRUCTION: s = "INSTRUCTION"; break;
        case Types::SVMATokenType::TYPE: s = "TYPE"; break;
        case Types::SVMATokenType::DATA_TYPE: s = "DATA_TYPE"; break;
        case Types::SVMATokenType::NUMBER: s = "NUMBER"; break;
        case Types::SVMATokenType::IMMEDIATE: s = "IMMEDIATE"; break;
        case Types::SVMATokenType::CHAR: s = "CHAR"; break;
        case Types::SVMATokenType::STRING: s = "STRING"; break;
        case Types::SVMATokenType::LABEL_REF: s = "LABEL_REF"; break;
        case Types::SVMATokenType::LABEL_DEF: s = "LABEL_DEF"; break;
        case Types::SVMATokenType::METHOD_DEF: s = "METHOD_DEF"; break;
        case Types::SVMATokenType::METHOD_METADATA_FIELD : s = "METHOD_METADATA_FIELD"; break;
    }
    return s;
}
