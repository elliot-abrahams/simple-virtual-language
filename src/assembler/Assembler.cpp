#include "Assembler.h"

#include <cstring>
#include <variant>
#include <iostream>
#include <cmath>

Assembler::Assembler() {}

std::optional<std::vector<uint8_t> > Assembler::assemble(const std::string &filePath) {
    // STEP 1 -> lex source file
    Lexer lexer;
    auto tokenStream = lexer.lex(filePath);

    if (!tokenStream.has_value()) {
        return std::nullopt;
    }

    // STEP 2 -> parse the stream of tokens
    Parser parser;
    auto parsedStatements = parser.parse(tokenStream.value());
    if (!parsedStatements.has_value()) {
        return std::nullopt;
    }
    this->statements = parsedStatements.value();

    // STEP 3 -> construct the label table
    bool valid = this->constructLabelTable();
    if (!valid) {
        return std::nullopt;
    }

    // STEP 4 -> generate bytecode
    return this->generateBytecode();
}

bool Assembler::constructLabelTable() {
    this->section = AssemblerDefs::Section::CODE;

    uint8_t bytecodeHeaderLength = 8;
    uint32_t codeSectionLength = 0;
    uint32_t dataSectionLength = 0;

    std::map<std::string, int> unhandledLabels;

    // loop through list of statements
    for (auto& statement : this->statements) {

        // for each statement:
        // increment code / data lengths by their byte length
        // add to label table and track if any label exists that aren't defined

        // process LABEL
        if (std::holds_alternative<AssemblerDefs::Label>(statement)) {
            bool isValid;
            auto label = std::get<AssemblerDefs::Label>(statement);
            if (this->section == AssemblerDefs::Section::CODE) {
                isValid = this->processLabelDef(unhandledLabels, bytecodeHeaderLength + codeSectionLength, label.name, label.lineNumber);
            } else {
                isValid = this->processLabelDef(unhandledLabels, bytecodeHeaderLength + codeSectionLength + dataSectionLength, label.name, label.lineNumber);
            }

            if (!isValid) {
                return false;
            }

        // process METHOD_DEF
        } else if (std::holds_alternative<AssemblerDefs::MethodDef>(statement)) {
            auto methodDef = std::get<AssemblerDefs::MethodDef>(statement);
            bool isValid = this->processLabelDef(unhandledLabels, bytecodeHeaderLength + codeSectionLength, methodDef.name, methodDef.lineNumber);
            if (!isValid) {
                return false;
            }
            codeSectionLength += 6; // 4 for label, 1 for number of args, 4 for number of locals

        // process INSTRUCTION
        } else if (std::holds_alternative<AssemblerDefs::Instruction>(statement)) {
            this->processInstruction(unhandledLabels, codeSectionLength, std::get<AssemblerDefs::Instruction>(statement));

        // process DATA
        } else if (std::holds_alternative<AssemblerDefs::Data>(statement)) {
            this->processData(unhandledLabels, dataSectionLength, std::get<AssemblerDefs::Data>(statement));

        } else {
            // data section Token
            this->section = AssemblerDefs::Section::DATA;
        }
    }

    if (!unhandledLabels.empty()) {
        std::cerr << "Undefined label(s):" << std::endl;
        for (auto& label : unhandledLabels) {
            std::cerr << "Line " << label.second << " -> " << label.first << std::endl;
        }
        return false;
    }

    this->dataStartLocation = bytecodeHeaderLength + codeSectionLength - 1;
    this->bytecodeLength = bytecodeHeaderLength + codeSectionLength + dataSectionLength - 1;
    return true;
}

bool Assembler::processLabelDef(std::map<std::string, int>& unhandledRefs, const uint32_t location, const std::string& label, const int& lineNumber) {
    // check if def already exists
    if (labelTable.find(label.substr(0, label.size() - 1)) != labelTable.end()) {
        std::cerr << "Error found at Line " << lineNumber << std::endl;
        std::cerr << "Duplicate label " << label << std::endl;
        return false;
    }

    unhandledRefs.erase(label.substr(0, label.size() - 1));
    this->labelTable.insert({label.substr(0, label.size() - 1), location});
    return true;
}

void Assembler::processInstruction(std::map<std::string, int>& unhandledLabelRefs, uint32_t& codeSectionLength, const AssemblerDefs::Instruction& instruction) {
    std::string type;
    codeSectionLength += 1; // 1 for opcode
    // loop through each operand
    for (auto& operand : instruction.operands) {
        if (operand.type == AssemblerDefs::OperandType::TYPE || operand.type == AssemblerDefs::OperandType::DATA_TYPE) {
            codeSectionLength++;
            type = operand.value;
        } else if (operand.type == AssemblerDefs::OperandType::CHAR) {
            codeSectionLength++;
        } else if (operand.type == AssemblerDefs::OperandType::LABEL_REF) {
            this->processLabelRef(unhandledLabelRefs, operand.value, instruction.lineNumber);
            codeSectionLength += 4;
        } else if (operand.type == AssemblerDefs::OperandType::STRING) {
            codeSectionLength += 4;
        } else if (operand.type == AssemblerDefs::OperandType::IMMEDIATE) {
            codeSectionLength += this->calculateBytesFromType(type);
        }
    }
}

void Assembler::processData(std::map<std::string, int> &unhandledLabelRefs, uint32_t& dataSectionLength, const AssemblerDefs::Data& data) {
    // check if data type is ptr
    if (data.type == "ptr") {
        // add label to unhandledLabelRefs
        unhandledLabelRefs.insert({data.value, data.lineNumber});
    }
    // add length of data to dataSectionLength
    dataSectionLength += this->calculateBytesOfData(data);
}

void Assembler::processLabelRef(std::map<std::string, int> &unhandledRefs, const std::string& label, const int &lineNumber) {
    if (this->labelTable.find(label) == this->labelTable.end()) {
        // label is not in labelTable
        unhandledRefs.insert({label, lineNumber});
    }
}

uint8_t Assembler::calculateBytesOfData(const AssemblerDefs::Data& data) const {
    uint8_t length = 1; // 1 for data type
    if (data.type == "str") {
        length += 4; // 4 bytes for length of string
        length += data.value.size() - 2; // 1 byte per character
    } else {
        length += this->calculateBytesFromType(data.type);
    }
    return length;
}

uint8_t Assembler::calculateBytesFromType(const std::string& type) const {
    if (type == "i64" || type == "ui64" || type == "f64") {
        return 8;
    }
    if (type == "char") {
        return 1;
    }
    return 4;
}

std::optional<std::vector<uint8_t>> Assembler::generateBytecode() {
    std::vector<uint8_t> bytecode;

    for (int i = 0; i < 4; i++) {
        bytecode.push_back((this->dataStartLocation >> (i * 8)) & 0xFF);
    }

    for (int i = 0; i < 4; i++) {
        bytecode.push_back((this->bytecodeLength >> (i * 8)) & 0xFF);
    }

    // loop through each statement
    for (auto& statement : this->statements) {
        if (std::holds_alternative<AssemblerDefs::Label>(statement) || std::holds_alternative<AssemblerDefs::Section>(statement)) {
            continue;
        }
        // convert METHOD_DEF
        if (std::holds_alternative<AssemblerDefs::MethodDef>(statement)) {
            this->pushBackVector(bytecode, this->convertMethodDefToBytes(std::get<AssemblerDefs::MethodDef>(statement)));

        // convert INSTRUCTION
        } else if (std::holds_alternative<AssemblerDefs::Instruction>(statement)) {
            auto data = this->convertInstructionToBytes(std::get<AssemblerDefs::Instruction>(statement));
            if (!data.has_value()) {
                return std::nullopt;
            }
            this->pushBackVector(bytecode, data.value());

        // convert DATA
        } else if (std::holds_alternative<AssemblerDefs::Data>(statement)) {
            auto data = this->convertDataStatementToBytes(std::get<AssemblerDefs::Data>(statement));
            if (!data.has_value()) {
                return std::nullopt;
            }
            this->pushBackVector(bytecode, data.value());
        }
    }
    return bytecode;
}

std::optional<std::vector<uint8_t>> Assembler::convertInstructionToBytes(const AssemblerDefs::Instruction &instruction) const {
    std::vector<uint8_t> bytecode;
    bytecode.push_back(AssemblerDefs::opcode.at(instruction.opcode)); // encode opcode
    std::string dataType;

    // loop through each operand
    for (auto& operand : instruction.operands) {
        if (operand.type == AssemblerDefs::OperandType::TYPE) {
            bytecode.push_back(this->convertTypeToByte(operand.value));
            dataType = operand.value;
        } else if (operand.type == AssemblerDefs::OperandType::DATA_TYPE) {
            bytecode.push_back(this->convertDataTypeToByte(operand.value));
            dataType = operand.value;
        } else if (operand.type == AssemblerDefs::OperandType::LABEL_REF) {
            this->pushBackVector(bytecode, this->convertLabelRefToBytes(operand.value));
        } else if (operand.type == AssemblerDefs::OperandType::CHAR ||
            operand.type == AssemblerDefs::OperandType::STRING ||
            operand.type == AssemblerDefs::OperandType::IMMEDIATE) {
            auto data = this->convertDataToBytes(dataType, operand.value, instruction.lineNumber);
            if (!data.has_value()) {
                return std::nullopt;
            }
            this->pushBackVector(bytecode, data.value());
        }
    }
    return bytecode;
}

std::vector<uint8_t> Assembler::convertMethodDefToBytes(const AssemblerDefs::MethodDef& methodDef) const {
    std::vector<uint8_t> bytecode;
    bytecode.push_back(methodDef.numberOfArguments);
    for (int i = 0; i < 4; i++) {
        bytecode.push_back((methodDef.numberOfLocals >> (i * 8)) & 0xFF);
    }
    return bytecode;
}

std::optional<std::vector<uint8_t>> Assembler::convertDataStatementToBytes(const AssemblerDefs::Data& data) const {
    std::vector<uint8_t> bytecode;
    // encode data type to bytecode
    bytecode.push_back(this->convertDataTypeToByte(data.type));
    // encode data to bytecode
    auto byteList = this->convertDataToBytes(data.type, data.value, data.lineNumber);
    if (!byteList.has_value()) {
        return std::nullopt;
    }
    this->pushBackVector(bytecode, byteList.value());
    return bytecode;
}

uint8_t Assembler::convertTypeToByte(const std::string &type) const {
    if (type == "i32") return 0x00;
    if (type == "ui32") return 0x01;
    if (type == "i64") return 0x02;
    if (type == "ui64") return 0x03;
    if (type == "f32") return 0x04;
    if (type == "f64") return 0x05;
    if (type == "ptr") return 0x06;
    //if (type == "char")
    return 0x07;
}

uint8_t Assembler::convertDataTypeToByte(const std::string &dataType) const {
    if (dataType == "str") {
        return 0x08;
    }
    return this->convertTypeToByte(dataType);
}

std::optional<std::vector<uint8_t>> Assembler::convertDataToBytes(const std::string& dataType, const std::string& data, const int& lineNumber) const {
    std::vector<uint8_t> bytecode;
    try {
        if (dataType == "i32") {
            const int64_t parsed = std::stoll(data.substr(1));

            if (parsed > std::numeric_limits<int32_t>::max() || parsed < std::numeric_limits<int32_t>::min()) {
                this->handleValueOutOfRangeError(dataType, data, lineNumber);
                return std::nullopt;
            }

            const int32_t value = static_cast<int32_t>(parsed);
            const uint32_t raw = static_cast<uint32_t>(value);

            for (int i = 0; i < 4; i++) {
                bytecode.push_back((raw >> (i * 8)) & 0xFF);
            }

        } else if (dataType == "ui32") {
            if (data[1] == '-') {
                this->handleValueOutOfRangeError(dataType, data, lineNumber);
                return std::nullopt;
            }

            const uint64_t parsed = std::stoull(data);
            if (parsed > std::numeric_limits<uint32_t>::max()) {
                this->handleValueOutOfRangeError(dataType, data, lineNumber);
                return std::nullopt;
            }

            const uint32_t raw = static_cast<uint32_t>(parsed);

            for (int i = 0; i < 4; i++) {
                bytecode.push_back((raw >> (i * 8)) & 0xFF);
            }

        } else if (dataType == "i64") {
            const int64_t value = std::stoll(data.substr(1));
            const uint64_t raw = static_cast<uint64_t>(value);

            for (int i = 0; i < 8; i++) {
                bytecode.push_back((raw >> (i * 8)) & 0xFF);
            }

        } else if (dataType == "ui64") {
            if (data[1] == '-') {
                this->handleValueOutOfRangeError(dataType, data, lineNumber);
                return std::nullopt;
            }

            const uint64_t raw = std::stoull(data.substr(1));

            for (int i = 0; i < 8; i++) {
                bytecode.push_back((raw >> (i * 8)) & 0xFF);
            }

        } else if (dataType == "f32") {
            const float value = std::stof(data.substr(1));

            if (!std::isfinite(value)) {
                this->handleValueOutOfRangeError(dataType, data, lineNumber);
                return std::nullopt;
            }

            uint32_t raw;
            std::memcpy(&raw, &value, sizeof(float));

            for (int i = 0; i < 4; i++) {
                bytecode.push_back((raw >> (i * 8)) & 0xFF);
            }

        } else if (dataType == "f64") {
            const double value = std::stod(data.substr(1));

            if (!std::isfinite(value)) {
                this->handleValueOutOfRangeError(dataType, data, lineNumber);
                return std::nullopt;
            }

            uint64_t raw;
            std::memcpy(&raw, &value, sizeof(double));

            for (int i = 0; i < 8; i++) {
                bytecode.push_back((raw >> (i * 8)) & 0xFF);
            }

        } else if (dataType == "ptr") {
            this->pushBackVector(bytecode, this->convertLabelRefToBytes(data.substr(1)));
        } else if (dataType == "char") {
            bytecode.push_back(data[1]); // encode char to uint8_t
        } else if (dataType == "str") {
            this->pushBackVector(bytecode, this->convertStringToBytes(data));
        }
    } catch (const std::out_of_range&) {
        this->handleValueOutOfRangeError(dataType, data, lineNumber);
    }
    return bytecode;
}

std::vector<uint8_t> Assembler::convertLabelRefToBytes(const std::string& label) const {
    std::vector<uint8_t> bytecode;

    // resolve label ref to memory address
    for (int i = 0; i < 4; i++) {
        bytecode.push_back((this->labelTable.at(label) >> (i * 8)) & 0xFF);
    }
    return bytecode;
}

std::vector<uint8_t> Assembler::convertStringToBytes(const std::string &string) const {
    std::vector<uint8_t> bytecode;

    std::string str = string.substr(1, string.size() - 2);
    uint32_t size = str.size();

    // encode string length (4 bytes)
    for (int i = 0; i < 4; i++) {
        bytecode.push_back((size >> (i * 8)) & 0xFF);
    }

    // encode string (1 byte per char)
    for (auto it = str.begin(); it != str.end(); ++it) {
        bytecode.push_back(static_cast<uint8_t>(*it));
    }
    return bytecode;
}

std::vector<uint8_t> Assembler::pushBackVector(std::vector<uint8_t>& a, const std::vector<uint8_t>& b) const {
    a.insert(a.end(), b.begin(), b.end());
    return a;
}

void Assembler::handleValueOutOfRangeError(const std::string& dataType, const std::string& data, const int& lineNumber) const {
    std::cerr << "Error found at Line " << lineNumber << std::endl;
    std::cerr << data << " out of range for " << dataType << std::endl;
}
