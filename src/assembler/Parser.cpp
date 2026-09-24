#include "Parser.h"

#include <cmath>
#include <limits>
#include <regex>

#include "../include/Error.h"


assembler::Parser::Parser() {}

std::vector<assembler::Statement> assembler::Parser::parse(const std::filesystem::path* filepath, const std::vector<SVMAToken> &tokenStream) {
    std::vector<Statement> statements;
    this->tokenStream = tokenStream;
    this->tokenIdx = 0;
    this->section = Section::CODE;
    this->filepath = filepath;

    while (this->peek().type != SVMATokenType::END_OF_FILE) {
        statements.push_back(this->parseToken());
    }
    return statements;
}

assembler::Statement assembler::Parser::parseToken() {

    switch (this->peek().type) {
        case SVMATokenType::INSTRUCTION:
            return this->parseInstruction();
        case SVMATokenType::LABEL_DEF: {
            if (this->section == Section::CODE) {
                return this->parseLabelDef();
            }
            return this->parseData();
        }
        case SVMATokenType::METHOD_DEF:
            return this->parseMethodDef();
        case SVMATokenType::DIRECTIVE:
            return this->parseDirective();
        default: {
            if (this->section == Section::CODE ||
                this->section == Section::DATA
            ) {
                this->throwUnexpectedTokenError(this->peek());
            }

            if (this->section == Section::METADATA_SOURCE) {
                return this->parseSourceMetadata();
            }

            if (this->section == Section::METADATA_FUNCTION) {
                return this->parseFunctionMetadata();
            }

            if (this->section == Section::METADATA_LINE_TABLE) {
                return this->parseLineTableMetadata();
            }
        }
    }
    this->throwUnexpectedTokenError(this->peek());
}

assembler::Statement assembler::Parser::parseInstruction() {
    const auto instruction = this->peek();
    const auto opcode = instruction.value;
    this->next();

    Operand type;
    Operand labelRef;
    Operand immediate;
    Operand nativeRef;
    Operand errorRef;

    // parse TYPE Token
    if (opcode == "push" ||
        opcode == "load" ||
        opcode == "loadL" ||
        opcode == "conv") {

        type = this->parseOperand(SVMATokenType::TYPE);
    }

    // parse LABEL_REF Token
    if (opcode == "loadG" ||
        opcode == "storeG" ||
        opcode == "call" ||
        opcode == "jmp" ||
        opcode == "jez" ||
        opcode == "jnz") {

        labelRef = this->parseOperand(SVMATokenType::LABEL_REF);
    }

    // parse IMMEDIATE Token
    if (opcode == "dup" ||
        opcode == "rotD" ||
        opcode == "rotU" ||
        opcode == "loadL" ||
        opcode == "storeL" ||
        opcode == "addrL"
    ) {
        immediate = this->parseOperand(SVMATokenType::IMMEDIATE);
    }

    // parse NATIVE_REF Token
    if (opcode == "native") {
        nativeRef = this->parseOperand(SVMATokenType::NATIVE_REF);
    }

    // parse ERROR_REF Token
    if (opcode == "throw") {
        errorRef = this->parseOperand(SVMATokenType::ERROR_REF);
    }

    if (opcode == "push") {
        if (type.value != "ptr") {
            immediate = this->parseOperand(SVMATokenType::IMMEDIATE);
        } else {
            labelRef = this->parseOperand(SVMATokenType::LABEL_REF);
        }
    }

    // check value is valid with given type (if operand consists of both type and value)
    if (!immediate.value.empty() && !type.value.empty() && opcode != "loadL") {
        std::string value = immediate.value;
        this->checkAndHandleValueIsValidAsType(type.value, immediate.value, immediate.line, immediate.column);
    }

    // check immediate of loadL / storeL / addrL is non-zero and value is within bounds
    // check immediate of dup / rotD / rotU is not decimal
    if (opcode == "loadL" || opcode == "storeL" || opcode == "addrL") {
        this->checkAndHandleValueIsValidAsType("slot", immediate.value, immediate.line, immediate.column);
    } else if (opcode == "dup" || opcode == "rotD" || opcode == "rotU") {
        this->checkAndHandleValueIsValidAsType("stack", immediate.value, immediate.line, immediate.column);
    }

    if (opcode == "nop") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "halt") return Instruction{opcode, {}, instruction.line, instruction.column};

    //========================================================================================================
    // STACK
    //========================================================================================================

    if (opcode == "push") {
        if (type.value == "ptr") {
            return Instruction{opcode, {type, labelRef}, instruction.line, instruction.column};
        }
        if (type.value != "ptr" && type.value != "str") {
            return Instruction{opcode, {type, immediate}, instruction.line, instruction.column};
        }
        this->throwUnexpectedTokenError(this->peek());
    }
    if (opcode == "pop") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "dup") return Instruction{opcode, {immediate}, instruction.line, instruction.column};
    if (opcode == "swap") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "rotD") return Instruction{opcode, {immediate}, instruction.line, instruction.column};
    if (opcode == "rotU") return Instruction{opcode, {immediate}, instruction.line, instruction.column};

    //========================================================================================================
    // MEMORY
    //========================================================================================================

    if (opcode == "load") return Instruction{opcode, {type}, instruction.line, instruction.column};
    if (opcode == "loadB") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "loadG") return Instruction{opcode, {labelRef}, instruction.line, instruction.column};
    if (opcode == "loadL") return Instruction{opcode, {type, immediate}, instruction.line, instruction.column};
    if (opcode == "store") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "storeB") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "storeG") return Instruction{opcode, {labelRef}, instruction.line, instruction.column};
    if (opcode == "storeL") return Instruction{opcode, {immediate}, instruction.line, instruction.column};
    if (opcode == "addrL") return Instruction{opcode, {immediate}, instruction.line, instruction.column};
    if (opcode == "alloc") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "free") return Instruction{opcode, {}, instruction.line, instruction.column};

    //========================================================================================================
    // CONTROL
    //========================================================================================================

    if (opcode == "call") return Instruction{opcode, {labelRef}, instruction.line, instruction.column};
    if (opcode == "native") return Instruction{opcode, {nativeRef}, instruction.line, instruction.column};
    if (opcode == "ret") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "jmp") return Instruction{opcode, {labelRef}, instruction.line, instruction.column};
    if (opcode == "jez") return Instruction{opcode, {labelRef}, instruction.line, instruction.column};
    if (opcode == "jnz") return Instruction{opcode, {labelRef}, instruction.line, instruction.column};

    //========================================================================================================
    // ARITHMETIC
    //========================================================================================================

    if (opcode == "add") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "sub") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "mul") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "div") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "mod") return Instruction{opcode, {}, instruction.line, instruction.column};

    if (opcode == "not") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "and") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "orr") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "xor") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "shl") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "shr") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "sar") return Instruction{opcode, {}, instruction.line, instruction.column};

    if (opcode == "ceq") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "cne") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "clt") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "cle") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "cgt") return Instruction{opcode, {}, instruction.line, instruction.column};
    if (opcode == "cge") return Instruction{opcode, {}, instruction.line, instruction.column};

    //========================================================================================================
    // Other
    //========================================================================================================

    if (opcode == "conv") return Instruction{opcode, {type}, instruction.line, instruction.column};
    if (opcode == "throw") return Instruction{opcode, {errorRef}, instruction.line, instruction.column};

    this->throwUnexpectedTokenError(instruction);
}

assembler::Statement assembler::Parser::parseLabelDef() {
    auto token = this->peek();
    this->next();
    return Label{token.value, token.line, token.column};
}

assembler::Statement assembler::Parser::parseData() {
    if (this->peek().type != SVMATokenType::LABEL_DEF) {
        this->throwUnexpectedTokenError(this->peek());
    }

    const auto labelDef = this->peek();
    this->next();
    const auto dataTypeToken = this->peek();

    this->next();
    const auto valueToken = this->peek();

    if (dataTypeToken.type != SVMATokenType::DATA_TYPE &&
        dataTypeToken.type != SVMATokenType::TYPE) {

        this->throwUnexpectedTokenError(this->peek());
    }

    if (valueToken.type != SVMATokenType::NUMBER &&
        valueToken.type != SVMATokenType::LABEL_REF &&
        valueToken.type != SVMATokenType::STRING
    ) {
        this->throwUnexpectedTokenError(valueToken);
    }

    if (dataTypeToken.type == SVMATokenType::TYPE) {
        checkAndHandleValueIsValidAsType(dataTypeToken.value, valueToken.value, valueToken.line, valueToken.column);
    } else if (dataTypeToken.type == SVMATokenType::DATA_TYPE) {
        checkAndHandleValueIsValidAsDataType(dataTypeToken.value, valueToken.value, valueToken.line, valueToken.column);
    }
    auto data = Data{Label{labelDef.value, labelDef.line, labelDef.column}, dataTypeToken.value, this->peek().value, valueToken.line};
    this->next();
    return data;
}

assembler::Statement assembler::Parser::parseMethodDef() {
    SVMAToken numberOfArgsToken;
    SVMAToken numberOfLocalsToken;

    bool argsParsed = false;
    bool localsParsed = false;

    this->next();
    if (this->peek().type != SVMATokenType::LABEL_DEF) {
        this->throwUnexpectedTokenError(this->peek());
    }
    const auto methodLabel = this->peek();
    const uint32_t lineNumber = this->peek().line;
    this->next();
    while (this->peek().type == SVMATokenType::METHOD_METADATA_FIELD) {
        if (this->peek().value == "args") {
            if (argsParsed) {
                this->throwUnexpectedTokenError(this->peek());
            }
            argsParsed = true;
            this->next();
            numberOfArgsToken = this->peek();
            if (numberOfArgsToken.type != SVMATokenType::NUMBER) {
                this->throwUnexpectedTokenError(numberOfArgsToken);
            }
            // enforce number of arguments to be an unsigned integer and representable as uint8_t
            const int32_t parsedNumberOfArgs = std::stoi(numberOfArgsToken.value);
            if (!isInteger(numberOfArgsToken.value) ||
                isSigned(numberOfArgsToken.value) ||
                parsedNumberOfArgs > std::numeric_limits<uint8_t>::max() ||
                parsedNumberOfArgs < 0
            ) {
                this->throwMethodMetadataOutOfRange(numberOfArgsToken.value, numberOfArgsToken.line, numberOfArgsToken.column, true);
            }
            this->next();
            continue;
        }
        if (this->peek().value == "locals") {
            if (localsParsed) {
                this->throwUnexpectedTokenError(this->peek());
            }
            localsParsed = true;
            this->next();
            numberOfLocalsToken = this->peek();
            if (numberOfLocalsToken.type != SVMATokenType::NUMBER) {
                this->throwUnexpectedTokenError(numberOfLocalsToken);
            }
            // enforce number of locals to be an unsigned integer and representable as uint32_t
            const int64_t parsedNumberOfLocals = std::stoll(numberOfLocalsToken.value);
            if (!isInteger(numberOfLocalsToken.value) || isSigned(numberOfLocalsToken.value) ||
                parsedNumberOfLocals > std::numeric_limits<uint32_t>::max() ||
                parsedNumberOfLocals < 0
            ) {
                this->throwMethodMetadataOutOfRange(numberOfLocalsToken.value, numberOfLocalsToken.line, numberOfLocalsToken.column, false);
            }
            this->next();
        }
    }

    return MethodDef{
        Label{methodLabel.value, methodLabel.line, methodLabel.column},
        static_cast<uint8_t>(stoul(numberOfArgsToken.value)),
        static_cast<uint32_t>(stoul(numberOfLocalsToken.value)),
        lineNumber
    };
}

assembler::Operand assembler::Parser::parseOperand(const SVMATokenType tokenType) {
    auto token = this->peek();
    if (this->peek().type != tokenType) {
        this->throwUnexpectedTokenError(token);
    }
    this->next();
    return Operand{mapTokenTypeToOperandType(tokenType), token.value, token.line, token.column};
}

assembler::Statement assembler::Parser::parseDirective() {
    const auto directive = this->peek();
    if (directive.value == ".data") {
        if (this->section != Section::CODE) this->throwUnexpectedTokenError(directive);

        this->next();
        this->section = Section::DATA;
        return Section::DATA;
    }

    if (directive.value == ".metadata") {
        if (this->section != Section::DATA &&
            this->section != Section::CODE)
                this->throwUnexpectedTokenError(directive);

        this->next();
        this->section = Section::METADATA;
        return Section::METADATA;
    }

    if (directive.value == ".sources") {
        if (this->section != Section::METADATA) this->throwUnexpectedTokenError(directive);

        this->next();
        this->section = Section::METADATA_SOURCE;
        return Section::METADATA_SOURCE;
    }

    if (directive.value == ".functions") {
        if (this->section != Section::METADATA_SOURCE) this->throwUnexpectedTokenError(directive);

        this->next();
        this->section = Section::METADATA_FUNCTION;
        return Section::METADATA_FUNCTION;
    }

    if (directive.value == ".line_table") {
        if (this->section != Section::METADATA_FUNCTION) this->throwUnexpectedTokenError(directive);

        this->next();
        this->section = Section::METADATA_LINE_TABLE;
        return Section::METADATA_LINE_TABLE;
    }
}

assembler::SourceMetadata assembler::Parser::parseSourceMetadata() {
    const uint16_t sourceId = this->parseSourceId();

    const auto sourcePathToken = this->peek();

    if (sourcePathToken.type != SVMATokenType::STRING) {
        this->throwUnexpectedTokenError(sourcePathToken);
    }

    this->next();

    return SourceMetadata{
        sourceId,
        sourcePathToken.value
    };
}

assembler::FunctionMetadata assembler::Parser::parseFunctionMetadata() {
    const uint32_t startAddress = this->parseAddress();
    const uint32_t endAddress = this->parseAddress();
    const uint16_t sourceId = this->parseSourceId();

    // parse function name
    const auto functionNameToken = this->peek();

    if (functionNameToken.type != SVMATokenType::STRING) {
        this->throwUnexpectedTokenError(functionNameToken);
    }

    this->next();

    return FunctionMetadata{
        startAddress,
        endAddress,
        sourceId,
        functionNameToken.value
    };
}

assembler::LineTableMetadata assembler::Parser::parseLineTableMetadata() {
    const uint32_t startAddress = this->parseAddress();
    const uint32_t endAddress = this->parseAddress();
    const uint16_t sourceId = this->parseSourceId();

    // parse line number
    const auto lineNumberToken = this->peek();
    if (lineNumberToken.type != SVMATokenType::NUMBER) {
        this->throwUnexpectedTokenError(lineNumberToken);
    }

    // enforce lineNumber can be represented as uint32_t
    if (!fitsUint32(lineNumberToken.value)) {
        throw AssemblerError(
            *this->filepath,
            lineNumberToken.line,
            lineNumberToken.column,
            "invalid line number '" +
                lineNumberToken.value +
                "'"
        );
    }
    const uint32_t lineNumber = static_cast<uint32_t>(std::stoul(lineNumberToken.value));

    this->next();

    // parse column number
    const auto columnNumberToken = this->peek();
    if (columnNumberToken.type != SVMATokenType::NUMBER) {
        this->throwUnexpectedTokenError(columnNumberToken);
    }
    // enforce sourceId can be represented as uint16_t
    if (!fitsUint16(columnNumberToken.value)) {
        throw AssemblerError(
            *this->filepath,
            columnNumberToken.line,
            columnNumberToken.column,
            "invalid column number '" +
                columnNumberToken.value +
                "'"
        );
    }
    const uint16_t columnNumber = static_cast<uint16_t>(std::stoul(columnNumberToken.value));

    this->next();

    return LineTableMetadata{
        startAddress,
        endAddress,
        sourceId,
        lineNumber,
        columnNumber
    };
}

uint32_t assembler::Parser::parseAddress() {
    // parse address
    const auto address = this->peek();
    if (address.type != SVMATokenType::HEX) {
        this->throwUnexpectedTokenError(address);
    }

    // enforce address can be represented as uint32_t
    if (!fitsUint32(address.value)) {
        throw AssemblerError(
            *this->filepath,
            address.line,
            address.column,
            "invalid address '" +
                address.value +
                "'"
        );
    }
    size_t pos;
    this->next();
    return static_cast<uint32_t>(std::stoul(address.value, &pos, 16)); // convert hex to uint16_t
}

uint16_t assembler::Parser::parseSourceId() {
    // parse source id
    const auto sourceIdToken = this->peek();
    if (sourceIdToken.type != SVMATokenType::NUMBER) {
        this->throwUnexpectedTokenError(sourceIdToken);
    }

    // enforce sourceId can be represented as uint16_t
    if (!fitsUint16(sourceIdToken.value)) {
        throw AssemblerError(
            *this->filepath,
            sourceIdToken.line,
            sourceIdToken.column,
            "invalid source id '" +
                sourceIdToken.value +
                "'"
        );
    }
    this->next();
    return static_cast<uint16_t>(std::stoul(sourceIdToken.value));
}

void assembler::Parser::next() {
    this->tokenIdx++;
}

assembler::SVMAToken assembler::Parser::peek() {
    return this->tokenStream[this->tokenIdx];
}

assembler::SVMAToken assembler::Parser::peekNext() {
    return this->tokenStream[this->tokenIdx + 1];
}

void assembler::Parser::checkAndHandleValueIsValidAsType(const std::string& type, const std::string& value, const uint32_t lineNumber, const uint16_t columnNumber) const {
    try {
        if (value.empty()) {
            this->throwInvalidValueForTypeError(value, type, lineNumber, columnNumber);
        }

        std::string valueToCheck;
        if ( value[0] == '#') {
            valueToCheck = value.substr(1);
        } else {
            valueToCheck = value;
        }

        if (type == "i32") {
            const int64_t parsed = std::stoll(valueToCheck);

            if (parsed < std::numeric_limits<int32_t>::lowest() ||
                parsed > std::numeric_limits<int32_t>::max() ||
                !isInteger(valueToCheck)
            ) {
                this->throwInvalidValueForTypeError(value, type, lineNumber, columnNumber);
            }

        } else if (type == "ui32") {
            const uint64_t parsed = std::stoull(valueToCheck);

            if (parsed > std::numeric_limits<uint32_t>::max() ||
                !isInteger(valueToCheck)
            ) {
                this->throwInvalidValueForTypeError(value, type, lineNumber, columnNumber);
            }


        } else if (type == "i64") {
            if (!isInteger(valueToCheck)) this->throwInvalidValueForTypeError(value, type, lineNumber, columnNumber);
            std::stoll(valueToCheck);

        } else if (type == "ui64") {
            if (isSigned(valueToCheck) || !isInteger(valueToCheck)) {
                this->throwInvalidValueForTypeError(value, type, lineNumber, columnNumber);
            }

            std::stoull(valueToCheck);

        } else if (type == "f32") {
            const float parsed = std::stof(valueToCheck);

            if (!std::isfinite(parsed)) {
                this->throwInvalidValueForTypeError(value, type, lineNumber, columnNumber);
            }

        } else if (type == "f64") {
            const double parsed = std::stod(valueToCheck);

            if (!std::isfinite(parsed)) {
                this->throwInvalidValueForTypeError(value, type, lineNumber, columnNumber);
            }

        } else if (type == "ptr") {
            if (value[0] == '$') {
                return;
            }

            const uint64_t parsed = std::stoull(valueToCheck);

            if (parsed > std::numeric_limits<uint32_t>::max() ||
                !isInteger(valueToCheck)
            ) {
                this->throwInvalidValueForTypeError(value, type, lineNumber, columnNumber);
            }

        } else if (type == "slot") { // for loadL / storeL / addrL
            if (valueToCheck == "0" || !isInteger(valueToCheck)) {
                this->throwInvalidValueForTypeError(value, type, lineNumber, columnNumber);
            }

            const int64_t parsed = std::stoll(valueToCheck);

            if (parsed > std::numeric_limits<uint32_t>::max() ||
                parsed < -static_cast<int64_t>(std::numeric_limits<uint32_t>::max())
            ) {
                this->throwInvalidValueForTypeError(value, type, lineNumber, columnNumber);
            }

        } else if (type == "stack") {
            if (!isInteger(valueToCheck)) {
                this->throwInvalidValueForTypeError(value, type, lineNumber, columnNumber);
            }
        }

    } catch (const std::invalid_argument&) {
        this->throwInvalidValueForTypeError(value, type, lineNumber, columnNumber);
    } catch (const std::out_of_range&) {
        this->throwInvalidValueForTypeError(value, type, lineNumber, columnNumber);
    }
}

void assembler::Parser::checkAndHandleValueIsValidAsDataType(const std::string& type, const std::string& value, const uint32_t lineNumber, const uint16_t columnNumber) const {
    if (type == "str") {
        if (value[0] != '"' || value[value.size() - 1] != '"') {
            this->throwInvalidValueForTypeError(value, type, lineNumber, columnNumber);
        }
    } else {
        this->throwInvalidValueForTypeError(value, type, lineNumber, columnNumber);
    }
}

bool assembler::Parser::isInteger(const std::string &value) {
    try {
        std::size_t position;
        std::stoull(value, &position);

        return position == value.size();
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&) {
        return false;
    }
}

bool assembler::Parser::isSigned(const std::string &value) {
    if (!value.empty() && value[0] == '-') {
        return true;
    }
    return false;
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

assembler::OperandType assembler::Parser::mapTokenTypeToOperandType(const SVMATokenType tokenType) {
    switch (tokenType) {
        case SVMATokenType::IMMEDIATE: return OperandType::IMMEDIATE;
        case SVMATokenType::TYPE: return OperandType::TYPE;
        case SVMATokenType::DATA_TYPE: return OperandType::DATA_TYPE;
        case SVMATokenType::LABEL_REF: return OperandType::LABEL_REF;
        case SVMATokenType::NATIVE_REF: return OperandType::NATIVE_REF;
        case SVMATokenType::ERROR_REF: return OperandType::ERROR_REF;
    }
}

void assembler::Parser::throwUnexpectedTokenError(const SVMAToken& token) const {
    throw AssemblerError(
        *this->filepath,
        token.line,
        token.column,
        "unexpected token '" +
            token.value +
            "'"
    );
}

void assembler::Parser::throwInvalidValueForTypeError(const std::string& value, const std::string& type, const uint32_t lineNumber, const uint16_t columnNumber) const {
    throw AssemblerError(
        *this->filepath,
        lineNumber,
        columnNumber,
        "'" +
            value +
            "' is invalid for type '" +
            type +
            "'"
    );
}

void assembler::Parser::throwMethodMetadataOutOfRange(const std::string &value, const uint32_t lineNumber, const uint16_t columnNumber, const bool isArg) const {
    throw AssemblerError(
            *this->filepath,
            lineNumber,
            columnNumber,
            "'" +
                value +
                "' is out of range for number of method " +
                (isArg ? "arguments" : "locals")
        );
}

std::string assembler::Parser::tokenTypeToString(const SVMATokenType tokenType) {
    std::string s;
    switch (tokenType) {
        case SVMATokenType::DIRECTIVE: s = "DIRECTIVE"; break;
        case SVMATokenType::INSTRUCTION: s = "INSTRUCTION"; break;
        case SVMATokenType::TYPE: s = "TYPE"; break;
        case SVMATokenType::DATA_TYPE: s = "DATA_TYPE"; break;
        case SVMATokenType::NUMBER: s = "NUMBER"; break;
        case SVMATokenType::IMMEDIATE: s = "IMMEDIATE"; break;
        case SVMATokenType::HEX: s = "HEX"; break;
        case SVMATokenType::STRING: s = "STRING"; break;
        case SVMATokenType::LABEL_REF: s = "LABEL_REF"; break;
        case SVMATokenType::LABEL_DEF: s = "LABEL_DEF"; break;
        case SVMATokenType::METHOD_DEF: s = "METHOD_DEF"; break;
        case SVMATokenType::METHOD_METADATA_FIELD : s = "METHOD_METADATA_FIELD"; break;
        case SVMATokenType::NATIVE_REF: s = "NATIVE_REF"; break;
        case SVMATokenType::ERROR_REF: s = "ERROR_REF"; break;
    }
    return s;
}
