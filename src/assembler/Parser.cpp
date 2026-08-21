#include "Parser.h"

#include <iostream>
#include <limits>
#include <regex>


assembler::Parser::Parser() {}

std::optional<std::vector<AssemblerDefs::Statement>> assembler::Parser::parse(const std::vector<AssemblerDefs::SVMAToken> &tokenStream) {
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

std::optional<AssemblerDefs::Statement> assembler::Parser::parseToken() {

    switch (this->peek().type) {
        case AssemblerDefs::SVMATokenType::INSTRUCTION:
            return this->parseInstruction();
        case AssemblerDefs::SVMATokenType::LABEL_DEF: {
            if (this->section == AssemblerDefs::Section::CODE) {
                return this->parseLabelDef();
            }
            return this->parseData();
        }
        case AssemblerDefs::SVMATokenType::METHOD_DEF:
            return this->parseMethodDef();
        case AssemblerDefs::SVMATokenType::DIRECTIVE:
            return this->parseDirective();
        default: {
            if (this->section == AssemblerDefs::Section::CODE ||
                this->section == AssemblerDefs::Section::DATA
            ) {
                printError(std::string("Unexpected token \'" + this->peek().value + "\'"), this->peek().lineNumber);
                return std::nullopt;
            }

            if (this->section == AssemblerDefs::Section::DEBUG_SOURCE) {
                return this->parseDebugSource();
            }

            if (this->section == AssemblerDefs::Section::DEBUG_LINE_TABLE) {
                return this->parseDebugLine();
            }
        }
    }
    printError(std::string("Unexpected token \'" + this->peek().value + "\'"), this->peek().lineNumber);
    return std::nullopt;
}

std::optional<AssemblerDefs::Statement> assembler::Parser::parseInstruction() {
    std::string instruction = this->peek().value;
    int lineNumber = this->peek().lineNumber;
    this->next();

    AssemblerDefs::Operand type;
    AssemblerDefs::Operand labelRef;
    AssemblerDefs::Operand immediate;
    AssemblerDefs::Operand nativeRef;

    // parse TYPE Token
    if (instruction == "push" ||
        instruction == "load" ||
        instruction == "loadL" ||
        instruction == "conv") {

        auto optionalType = this->parseType();
        if (!optionalType.has_value()) {
            handleIncorrectInstructionOperand(instruction, AssemblerDefs::SVMATokenType::TYPE, lineNumber);
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
            handleIncorrectInstructionOperand(instruction, AssemblerDefs::SVMATokenType::LABEL_REF, lineNumber);
            return std::nullopt;
        }
        labelRef = optionalLabelRef.value();
    }

    // parse IMMEDIATE Token
    if (instruction == "loadL" ||
        instruction == "storeL") {

        auto optionalImmediate = this->parseImmediate();
        if (!optionalImmediate.has_value()) {
            handleIncorrectInstructionOperand(instruction, AssemblerDefs::SVMATokenType::IMMEDIATE, lineNumber);
            return std::nullopt;
        }
        immediate = optionalImmediate.value();
    }

    // parse NATIVE_REF Token
    if (instruction == "native") {
        auto optionalNativeRef = this->parseNativeRef();
        if (!optionalNativeRef.has_value()) {
            handleIncorrectInstructionOperand(instruction, AssemblerDefs::SVMATokenType::NATIVE_REF, lineNumber);
            return std::nullopt;
        }
        nativeRef = optionalNativeRef.value();
    }

    if (instruction == "push") {
        if (type.value != "ptr") {
            auto optionalImmediate = this->parseImmediate();
            if (!optionalImmediate.has_value()) {
                handleIncorrectInstructionOperand(instruction, AssemblerDefs::SVMATokenType::IMMEDIATE, lineNumber);
                return std::nullopt;
            }
            immediate = optionalImmediate.value();
        } else {
            auto optionalLabelRef = this->parseLabelRef();
            if (!optionalLabelRef.has_value()) {
                handleIncorrectInstructionOperand(instruction, AssemblerDefs::SVMATokenType::LABEL_REF, lineNumber);
                return std::nullopt;
            }
            labelRef = optionalLabelRef.value();
        }
    }

    // check value is valid with given type (if operand consists of both type and value)
    if (!immediate.value.empty() && !type.value.empty() && instruction != "loadL") {
        std::string value = immediate.value;
        if (value[0] == '#') {
            value = value.substr(1);
        }
        if (!checkAndHandleValueIsValidAsType(type.value, value, lineNumber)) {
            return std::nullopt;
        }
    }

    // check immediate of loadL / storeL is non-zero
    if (instruction == "loadL" || instruction == "storeL") {
        if (immediate.value == "#0") {
            printError(std::string("immediate value #0 is invalid for " + instruction), lineNumber);
            return std::nullopt;
        }
    }

    if (instruction == "nop") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "halt") return AssemblerDefs::Instruction{instruction, {}, lineNumber};

    //========================================================================================================
    // STACK
    //========================================================================================================

    if (instruction == "push") {
        if (type.value == "ptr") {
            return AssemblerDefs::Instruction{instruction, {type, labelRef}, lineNumber};
        }
        if (type.value != "ptr" && type.value != "str") {
            return AssemblerDefs::Instruction{instruction, {type, immediate}, lineNumber};
        }
        handleUnexpectedTokenError({AssemblerDefs::SVMATokenType::LABEL_REF, AssemblerDefs::SVMATokenType::IMMEDIATE}, this->peek(), lineNumber);
        return std::nullopt;
    }
    if (instruction == "pop") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "dup") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "swap") return AssemblerDefs::Instruction{instruction, {}, lineNumber};

    //========================================================================================================
    // MEMORY
    //========================================================================================================

    if (instruction == "load") return AssemblerDefs::Instruction{instruction, {type}, lineNumber};
    if (instruction == "loadB") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "loadG") return AssemblerDefs::Instruction{instruction, {labelRef}, lineNumber};
    if (instruction == "loadL") return AssemblerDefs::Instruction{instruction, {type, immediate}, lineNumber};
    if (instruction == "store") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "storeB") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "storeG") return AssemblerDefs::Instruction{instruction, {labelRef}, lineNumber};
    if (instruction == "storeL") return AssemblerDefs::Instruction{instruction, {immediate}, lineNumber};
    if (instruction == "alloc") return AssemblerDefs::Instruction{instruction, {}, lineNumber};
    if (instruction == "free") return AssemblerDefs::Instruction{instruction, {}, lineNumber};

    //========================================================================================================
    // CONTROL
    //========================================================================================================

    if (instruction == "call") return AssemblerDefs::Instruction{instruction, {labelRef}, lineNumber};
    if (instruction == "native") return AssemblerDefs::Instruction{instruction, {nativeRef}, lineNumber};
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

    if (instruction == "conv") return AssemblerDefs::Instruction{instruction, {type}, lineNumber};

    printError(std::string("Undefined instruction: " + instruction), lineNumber);
    return std::nullopt;
}

std::optional<AssemblerDefs::Statement> assembler::Parser::parseLabelDef() {
    auto token = this->peek();
    this->next();
    return AssemblerDefs::Label{token.value, token.lineNumber};
}

std::optional<AssemblerDefs::Statement> assembler::Parser::parseData() {
    if (this->peek().type != AssemblerDefs::SVMATokenType::LABEL_DEF) {
        handleUnexpectedTokenError({AssemblerDefs::SVMATokenType::LABEL_DEF}, this->peek(), this->peek().lineNumber);
        return std::nullopt;
    }

    const auto labelDef = this->peek();
    this->next();
    const auto dataTypeToken = this->peek();

    this->next();
    const auto valueToken = this->peek();

    if (dataTypeToken.type != AssemblerDefs::SVMATokenType::DATA_TYPE &&
        dataTypeToken.type != AssemblerDefs::SVMATokenType::TYPE) {

        handleUnexpectedTokenError({AssemblerDefs::SVMATokenType::DATA_TYPE}, dataTypeToken, dataTypeToken.lineNumber);
        return std::nullopt;
    }

    if (valueToken.type != AssemblerDefs::SVMATokenType::NUMBER &&
        valueToken.type != AssemblerDefs::SVMATokenType::LABEL_REF &&
        valueToken.type != AssemblerDefs::SVMATokenType::STRING) {

        handleUnexpectedTokenError({
            AssemblerDefs::SVMATokenType::NUMBER,
            AssemblerDefs::SVMATokenType::LABEL_REF,
            AssemblerDefs::SVMATokenType::STRING,
        },
        valueToken, valueToken.lineNumber);

        return std::nullopt;
    }

    if (dataTypeToken.type == AssemblerDefs::SVMATokenType::TYPE) {
        if (!checkAndHandleValueIsValidAsType(dataTypeToken.value, valueToken.value, valueToken.lineNumber)) {
            return std::nullopt;
        }

    } else if (dataTypeToken.type == AssemblerDefs::SVMATokenType::DATA_TYPE) {
        if (!checkAndHandleValueIsValidAsDataType(dataTypeToken.value, valueToken.value, valueToken.lineNumber)) {
            return std::nullopt;
        }
    }
    auto data = AssemblerDefs::Data{labelDef.value, dataTypeToken.value, this->peek().value};
    this->next();
    return data;
}

std::optional<AssemblerDefs::Statement> assembler::Parser::parseMethodDef() {
    AssemblerDefs::SVMAToken numberOfArgsToken;
    AssemblerDefs::SVMAToken numberOfLocalsToken;

    this->next();
    if (this->peek().type != AssemblerDefs::SVMATokenType::LABEL_DEF) {
        handleUnexpectedTokenError({AssemblerDefs::SVMATokenType::LABEL_DEF}, this->peek(), this->peek().lineNumber);
        return std::nullopt;
    }
    const std::string methodName = this->peek().value;
    const int lineNumber = this->peek().lineNumber;
    this->next();
    while (this->peek().type == AssemblerDefs::SVMATokenType::METHOD_METADATA_FIELD) {
        if (this->peek().value == "args") {
            this->next();
            numberOfArgsToken = this->peek();
            if (numberOfArgsToken.type != AssemblerDefs::SVMATokenType::NUMBER) {
                handleUnexpectedTokenError({AssemblerDefs::SVMATokenType::NUMBER}, numberOfArgsToken, lineNumber);
                return std::nullopt;
            }
            // enforce number of args is an unsigned integer
            if (!isNumberInteger(numberOfArgsToken.value) || isNumberSigned(numberOfArgsToken.value)) {
                printError("Expecting unsigned integer", this->peek().lineNumber);
                return std::nullopt;
            }
            // enforce number of args is not higher than 255 (max number of args)
            if (stoi(numberOfArgsToken.value) > 255) {
                printError("Number of method arguments out of range (0 - 255)", this->peek().lineNumber);
                return std::nullopt;
            }
            this->next();
            continue;
        }
        if (this->peek().value == "locals") {
            this->next();
            numberOfLocalsToken = this->peek();
            if (numberOfLocalsToken.type != AssemblerDefs::SVMATokenType::NUMBER) {
                handleUnexpectedTokenError({AssemblerDefs::SVMATokenType::NUMBER}, numberOfLocalsToken, lineNumber);
                return std::nullopt;
            }
            // enforce number of locals is an unsigned integer
            if (!isNumberInteger(numberOfLocalsToken.value) || isNumberSigned(numberOfLocalsToken.value)) {
                printError("Expecting unsigned integer", this->peek().lineNumber);
                return std::nullopt;
            }
            this->next();
        }
    }

    return AssemblerDefs::MethodDef{
        methodName,
        static_cast<uint8_t>(stoul(numberOfArgsToken.value)),
        static_cast<uint32_t>(stoul(numberOfLocalsToken.value)),
        lineNumber
    };
}

std::optional<AssemblerDefs::Operand> assembler::Parser::parseType() {
    return this->parseOperand(AssemblerDefs::SVMATokenType::TYPE);
}


std::optional<AssemblerDefs::Operand> assembler::Parser::parseImmediate() {
    return this->parseOperand(AssemblerDefs::SVMATokenType::IMMEDIATE);
}

std::optional<AssemblerDefs::Operand> assembler::Parser::parseLabelRef() {
    return this->parseOperand(AssemblerDefs::SVMATokenType::LABEL_REF);
}

std::optional<AssemblerDefs::Operand> assembler::Parser::parseNativeRef() {
    return this->parseOperand(AssemblerDefs::SVMATokenType::NATIVE_REF);
}

std::optional<AssemblerDefs::Operand> assembler::Parser::parseOperand(const AssemblerDefs::SVMATokenType tokenType) {
    auto token = this->peek();
    if (this->peek().type != tokenType) {
        return std::nullopt;
    }
    this->next();
    return AssemblerDefs::Operand{mapTokenTypeToOperandType(tokenType), token.value};
}

std::optional<AssemblerDefs::Statement> assembler::Parser::parseDirective() {
    if (this->peek().value == ".data") {
        if (this->section != AssemblerDefs::Section::CODE) {
            printError("Invalid '.data' directive", this->peek().lineNumber);
            return std::nullopt;
        }
        this->next();
        this->section = AssemblerDefs::Section::DATA;
        return AssemblerDefs::Section::DATA;
    }

    if (this->peek().value == ".debug") {
        if (this->section != AssemblerDefs::Section::DATA &&
            this->section != AssemblerDefs::Section::CODE
        ) {
            printError("Invalid '.debug' directive", this->peek().lineNumber);
            return std::nullopt;
        }
        this->next();
        this->section = AssemblerDefs::Section::DEBUG;
        return AssemblerDefs::Section::DEBUG;
    }

    if (this->peek().value == ".source") {
        if (this->section != AssemblerDefs::Section::DEBUG) {
            printError("Invalid '.source' directive", this->peek().lineNumber);
            return std::nullopt;
        }
        this->next();
        this->section = AssemblerDefs::Section::DEBUG_SOURCE;
        return AssemblerDefs::Section::DEBUG_SOURCE;
    }

    if (this->peek().value == ".line_table") {
        if (this->section != AssemblerDefs::Section::DEBUG_SOURCE) {
            printError("Invalid '.line_table' directive", this->peek().lineNumber);
            return std::nullopt;
        }
        this->next();
        this->section = AssemblerDefs::Section::DEBUG_LINE_TABLE;
        return AssemblerDefs::Section::DEBUG_LINE_TABLE;
    }
}

std::optional<AssemblerDefs::DebugSource> assembler::Parser::parseDebugSource() {
    const auto sourceIdToken = this->peek();

    if (sourceIdToken.type != AssemblerDefs::SVMATokenType::NUMBER) {
        handleUnexpectedTokenError({AssemblerDefs::SVMATokenType::NUMBER}, sourceIdToken, sourceIdToken.lineNumber);
        return std::nullopt;
    }
    // enforce sourceId can be represented as uint16_t
    if (!fitsUint16(sourceIdToken.value)) {
        printError("Invalid value for debug source id \'" + sourceIdToken.value + "\"", this->peek().lineNumber);
        return std::nullopt;
    }
    const uint16_t sourceId = static_cast<uint16_t>(std::stoul(sourceIdToken.value));

    this->next();

    const auto sourcePathToken = this->peek();

    if (sourcePathToken.type != AssemblerDefs::SVMATokenType::STRING) {
        handleUnexpectedTokenError({AssemblerDefs::SVMATokenType::STRING}, sourcePathToken, sourcePathToken.lineNumber);
        return std::nullopt;
    }

    this->next();

    return AssemblerDefs::DebugSource{
        sourceId,
        sourcePathToken.value
    };
}

std::optional<AssemblerDefs::DebugLine> assembler::Parser::parseDebugLine() {
    // parse start Address
    const auto startAddressToken = this->peek();
    if (startAddressToken.type != AssemblerDefs::SVMATokenType::HEX) {
        handleUnexpectedTokenError({AssemblerDefs::SVMATokenType::HEX}, startAddressToken, startAddressToken.lineNumber);
        return std::nullopt;
    }

    // enforce startAddress can be represented as uint32_t
    if (!fitsUint32(startAddressToken.value)) {
        printError("Invalid value for debug startAddress \'" + startAddressToken.value + "\"", this->peek().lineNumber);
        return std::nullopt;
    }
    size_t pos;
    const uint32_t startAddress = static_cast<uint32_t>(std::stoul(startAddressToken.value, &pos, 16)); // convert hex to uint16_t

    this->next();

    // parse end Address
    const auto endAddressToken = this->peek();
    if (endAddressToken.type != AssemblerDefs::SVMATokenType::HEX) {
        handleUnexpectedTokenError({AssemblerDefs::SVMATokenType::HEX}, endAddressToken, endAddressToken.lineNumber);
        return std::nullopt;
    }

    // enforce endAddress can be represented as uint32_t
    if (!fitsUint32(startAddressToken.value)) {
        printError("Invalid value for debug endAddress \'" + endAddressToken.value + "\"", this->peek().lineNumber);
        return std::nullopt;
    }
    const uint32_t endAddress = static_cast<uint32_t>(std::stoul(endAddressToken.value, &pos, 16)); // convert hex to uint16_t

    this->next();

    // parse source id
    const auto sourceIdToken = this->peek();
    if (sourceIdToken.type != AssemblerDefs::SVMATokenType::NUMBER) {
        handleUnexpectedTokenError({AssemblerDefs::SVMATokenType::NUMBER}, sourceIdToken, sourceIdToken.lineNumber);
        return std::nullopt;
    }

    // enforce sourceId can be represented as uint16_t
    if (!fitsUint16(sourceIdToken.value)) {
        printError("Invalid value for debug source id \'" + sourceIdToken.value + "\"", this->peek().lineNumber);
        return std::nullopt;
    }
    const uint16_t sourceId = static_cast<uint16_t>(std::stoul(sourceIdToken.value));

    this->next();

    // parse line number
    const auto lineNumberToken = this->peek();
    if (lineNumberToken.type != AssemblerDefs::SVMATokenType::NUMBER) {
        handleUnexpectedTokenError({AssemblerDefs::SVMATokenType::NUMBER}, lineNumberToken, lineNumberToken.lineNumber);
        return std::nullopt;
    }

    // enforce lineNumber can be represented as uint32_t
    if (!fitsUint32(lineNumberToken.value)) {
        printError("Invalid value for debug line number \'" + lineNumberToken.value + "\"", this->peek().lineNumber);
        return std::nullopt;
    }
    const uint32_t lineNumber = static_cast<uint32_t>(std::stoul(lineNumberToken.value));

    this->next();

    // parse column number
    const auto columnNumberToken = this->peek();
    if (columnNumberToken.type != AssemblerDefs::SVMATokenType::NUMBER) {
        handleUnexpectedTokenError({AssemblerDefs::SVMATokenType::NUMBER}, columnNumberToken, columnNumberToken.lineNumber);
        return std::nullopt;
    }
    // enforce sourceId can be represented as uint16_t
    if (!fitsUint16(columnNumberToken.value)) {
        printError("Invalid value for debug column \'" + columnNumberToken.value + "\"", this->peek().lineNumber);
        return std::nullopt;
    }
    const uint16_t columnNumber = static_cast<uint16_t>(std::stoul(columnNumberToken.value));

    this->next();

    return AssemblerDefs::DebugLine{
        startAddress,
        endAddress,
        sourceId,
        lineNumber,
        columnNumber
    };
}

void assembler::Parser::next() {
    this->tokenIdx++;
}

AssemblerDefs::SVMAToken assembler::Parser::peek() {
    return this->tokenStream[this->tokenIdx];
}

AssemblerDefs::SVMAToken assembler::Parser::peekNext() {
    return this->tokenStream[this->tokenIdx + 1];
}

bool assembler::Parser::checkAndHandleValueIsValidAsType(const std::string type, const std::string &value, const int& lineNumber) {
    if (type == "i32" || type == "i64") {
        // enforce i32 / i64 is matched with an integer
        if (!isNumberInteger(value)) {
            printError("Expecting integer", lineNumber);
            return false;
        }
        return true;
    }
    if (type == "ui32" || type == "ui64") {
        // enforce ui32 / ui64 is matched with an unsigned integer
        if (!isNumberInteger(value) || isNumberSigned(value)) {
            printError("Expecting unsigned integer", lineNumber);
            return false;
        }
        return true;
    }
    if (type == "f32" || type == "f64") {
        // enforce f32 / f64 is matched with a decimal value
        if (!isNumberDecimal(value)) {
            printError("Expecting decimal", lineNumber);
            return false;
        }
        return true;
    }
    if (type == "ptr") {
        // enforce ptr type is matched with LABEL_REF
        if (value[0] != '$' || value[value.size() - 1] == ':') {
            printError("Expecting Label Reference", lineNumber);
            return false;
        }
        return true;
    }
    return false;
}

bool assembler::Parser::checkAndHandleValueIsValidAsDataType(const std::string type, const std::string &value, const int &lineNumber) {
    if (type == "str") {
        if (value[0] != '"' || value[value.size() - 1] != '"') {
            printError("Expecting String", lineNumber);
            return false;
        }
        return true;
    }
    return false;
}

bool assembler::Parser::isNumberInteger(const std::string& value) {
    return std::regex_match(value, std::regex("-?[0-9]*"));
}

bool assembler::Parser::isNumberDecimal(const std::string& value) {
    return std::regex_match(value, std::regex(R"(-?[0-9]+\.[0-9]+)"));
}

bool assembler::Parser::isNumberSigned(const std::string& value) {
    return std::regex_match(value, std::regex("-[0-9]+(.[0-9]+)?"));
}

bool assembler::Parser::fitsUint16(const std::string &s) {
    try {
        size_t pos;
        const long long value = std::stoll(s, &pos);

        return pos == s.size() &&
            value >= 0 &&
            value <= std::numeric_limits<uint16_t>::max();

    } catch (const std::out_of_range& e) {
        return false;
    }
}

bool assembler::Parser::fitsUint32(const std::string &s) {
    try {
        size_t pos;
        const long long value = std::stoll(s, &pos, 0);

        return pos == s.size() &&
            value >= 0 &&
            value <= std::numeric_limits<uint32_t>::max();

    } catch (const std::out_of_range& e) {
        return false;
    }
}

AssemblerDefs::OperandType assembler::Parser::mapTokenTypeToOperandType(const AssemblerDefs::SVMATokenType tokenType) {
    switch (tokenType) {
        case AssemblerDefs::SVMATokenType::IMMEDIATE: return AssemblerDefs::OperandType::IMMEDIATE;
        case AssemblerDefs::SVMATokenType::TYPE: return AssemblerDefs::OperandType::TYPE;
        case AssemblerDefs::SVMATokenType::DATA_TYPE: return AssemblerDefs::OperandType::DATA_TYPE;
        case AssemblerDefs::SVMATokenType::LABEL_REF: return AssemblerDefs::OperandType::LABEL_REF;
        case AssemblerDefs::SVMATokenType::NATIVE_REF: return AssemblerDefs::OperandType::NATIVE_REF;
    }
}

void assembler::Parser::printError(const std::string &msg, const int &lineNumber) {
    std::cerr << "Error found at Line " << lineNumber << std::endl;
    std::cerr << msg << std::endl;
}

void assembler::Parser::handleUnexpectedTokenError(const std::vector<AssemblerDefs::SVMATokenType> &expectingTypes, const AssemblerDefs::SVMAToken& actualType, const int& lineNumber) {
    printError("Expecting: ", lineNumber);
    for (int i = 0; i < expectingTypes.size(); i++) {
        if (i + 1 == expectingTypes.size()) std::cerr << tokenTypeToString(expectingTypes.at(i));
        else std::cerr << tokenTypeToString(expectingTypes.at(i)) << " / ";
    }
    std::cerr << ",\nfound:\n" << tokenTypeToString(actualType.type) << std::endl;
}

void assembler::Parser::handleIncorrectInstructionOperand(const std::string &instructionMnemonic, const AssemblerDefs::SVMATokenType expectedType, const int &lineNumber) {
    printError(std::string("Incorrect operand for instruction \'" + instructionMnemonic + "\'"), lineNumber);
    std::cerr << "Expecting operand: " << tokenTypeToString(expectedType) << std::endl;
}

std::string assembler::Parser::tokenTypeToString(const AssemblerDefs::SVMATokenType tokenType) {
    std::string s;
    switch (tokenType) {
        case AssemblerDefs::SVMATokenType::DIRECTIVE: s = "DIRECTIVE"; break;
        case AssemblerDefs::SVMATokenType::INSTRUCTION: s = "INSTRUCTION"; break;
        case AssemblerDefs::SVMATokenType::TYPE: s = "TYPE"; break;
        case AssemblerDefs::SVMATokenType::DATA_TYPE: s = "DATA_TYPE"; break;
        case AssemblerDefs::SVMATokenType::NUMBER: s = "NUMBER"; break;
        case AssemblerDefs::SVMATokenType::IMMEDIATE: s = "IMMEDIATE"; break;
        case AssemblerDefs::SVMATokenType::HEX: s = "HEX"; break;
        case AssemblerDefs::SVMATokenType::STRING: s = "STRING"; break;
        case AssemblerDefs::SVMATokenType::LABEL_REF: s = "LABEL_REF"; break;
        case AssemblerDefs::SVMATokenType::LABEL_DEF: s = "LABEL_DEF"; break;
        case AssemblerDefs::SVMATokenType::METHOD_DEF: s = "METHOD_DEF"; break;
        case AssemblerDefs::SVMATokenType::METHOD_METADATA_FIELD : s = "METHOD_METADATA_FIELD"; break;
        case AssemblerDefs::SVMATokenType::NATIVE_REF: s = "NATIVE_REF"; break;
    }
    return s;
}
