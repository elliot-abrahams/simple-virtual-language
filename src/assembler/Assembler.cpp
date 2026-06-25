#include "Assembler.h"

#include <cstring>
#include <variant>
#include <iostream>
#include <cmath>

Assembler::Assembler() {}

std::optional<std::vector<uint8_t> > Assembler::assemble(const std::string& filePath) {
    // STEP 1 -> lex source file
    Lexer lexer;
    auto tokenStream = lexer.lex(filePath);

    if (!tokenStream.has_value()) {
        return std::nullopt;
    }
    return this->assembleFromTokens(tokenStream.value());
}

std::optional<std::vector<uint8_t> > Assembler::assembleString(const std::string& fileContent) {
    // STEP 1 -> lex source file
    Lexer lexer;
    auto tokenStream = lexer.lexString(fileContent);

    if (!tokenStream.has_value()) {
        return std::nullopt;
    }
    return this->assembleFromTokens(tokenStream.value());
}

std::optional<std::vector<uint8_t>> Assembler::assembleFromTokens(const std::vector<AssemblerDefs::SVMAToken>& tokenStream) {
    // STEP 2 -> parse the stream of tokens
    Parser parser;
    const auto parsedStatements = parser.parse(tokenStream);
    if (!parsedStatements.has_value()) {
        return std::nullopt;
    }
    this->statements = parsedStatements.value();

    // STEP 3 -> construct the label table
    const bool valid = this->constructLabelTable();
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

    std::map<std::string, uint32_t> unhandledLabelRefs;

    // loop through list of statements
    for (auto& statement : this->statements) {

        // for each statement:
        // increment code / data lengths by their byte length
        // add to label table and track if any label exists that aren't defined

        // process LABEL
        if (std::holds_alternative<AssemblerDefs::Label>(statement)) {
            bool isValid;
            auto label = std::get<AssemblerDefs::Label>(statement);
            isValid = this->processLabelDef(unhandledLabelRefs, codeSectionLength, Label{label.name.substr(0, label.name.size() - 1), LabelType::CODE}, label.lineNumber);
            if (!isValid) {
                return false;
            }

        // process METHOD_DEF
        } else if (std::holds_alternative<AssemblerDefs::MethodDef>(statement)) {
            auto methodDef = std::get<AssemblerDefs::MethodDef>(statement);
            bool isValid = this->processLabelDef(unhandledLabelRefs, codeSectionLength, Label{methodDef.name.substr(0, methodDef.name.size() - 1), LabelType::METHOD}, methodDef.lineNumber);
            if (!isValid) {
                return false;
            }
            codeSectionLength += 6; // 4 for label, 1 for number of args, 4 for number of locals

        // process INSTRUCTION
        } else if (std::holds_alternative<AssemblerDefs::Instruction>(statement)) {
            this->processInstruction(unhandledLabelRefs, codeSectionLength, std::get<AssemblerDefs::Instruction>(statement));

        // process DATA
        } else if (std::holds_alternative<AssemblerDefs::Data>(statement)) {
            this->processData(unhandledLabelRefs, codeSectionLength + dataSectionLength + 1, dataSectionLength, std::get<AssemblerDefs::Data>(statement));

        } else {
            // data section Token
            this->section = AssemblerDefs::Section::DATA;
        }
    }

    if (!unhandledLabelRefs.empty()) {
        std::cerr << "Undefined label(s):" << std::endl;
        for (auto& label : unhandledLabelRefs) {
            std::cerr << "Line " << label.second << " -> " << label.first.substr(0, label.first.size() - 4) << " (" << label.first.substr(label.first.size() - 4, label.first.size()) << ")" << std::endl;
        }
        return false;
    }

    this->dataStartLocation = bytecodeHeaderLength + codeSectionLength - 1;
    this->bytecodeLength = bytecodeHeaderLength + codeSectionLength + dataSectionLength - 1;
    return true;
}

bool Assembler::processLabelDef(std::map<std::string, uint32_t>& unhandledLabelRefs, const uint32_t location, const Label& label, const int& lineNumber) {
    // check if def already exists
    if (labelTable.find(label.getKey()) != labelTable.end()) {
        std::cerr << "Error found at Line " << lineNumber << std::endl;
        std::cerr << "Duplicate label " << label.name << std::endl;
        return false;
    }

    unhandledLabelRefs.erase(label.getKey());
    this->labelTable.insert({label.getKey(), location});
    return true;
}

void Assembler::processInstruction(std::map<std::string, uint32_t>& unhandledLabelRefs, uint32_t& codeSectionLength, const AssemblerDefs::Instruction& instruction) {
    std::string type;
    codeSectionLength += 1; // 1 for opcode
    // loop through each operand
    for (auto& operand : instruction.operands) {
        if (operand.type == AssemblerDefs::OperandType::TYPE || operand.type == AssemblerDefs::OperandType::DATA_TYPE) {
            codeSectionLength++;
            type = operand.value;
        } else if (operand.type == AssemblerDefs::OperandType::LABEL_REF) {
            // process label ref depending on required label type for this instruction
            this->processLabelRef(unhandledLabelRefs, Label{operand.value, getOperandLabelType(instruction.opcode)}, instruction.lineNumber);
            codeSectionLength += 4;
        } else if (operand.type == AssemblerDefs::OperandType::STRING) {
            codeSectionLength += 4;
        } else if (operand.type == AssemblerDefs::OperandType::IMMEDIATE) {
            codeSectionLength += this->calculateBytesFromType(type);
        }
    }
}

void Assembler::processData(std::map<std::string, uint32_t> &unhandledLabelRefs, const uint32_t location, uint32_t& dataSectionLength, const AssemblerDefs::Data& data) {
    // process label Def
    this->processLabelDef(unhandledLabelRefs, location, Label{data.name.substr(0, data.name.size() - 1), LabelType::DATA}, data.lineNumber);

    // check if data type is ptr
    if (data.type == "ptr") {
        this->processLabelRef(unhandledLabelRefs, Label{data.value, LabelType::DATA}, data.lineNumber);
    }
    // add length of data to dataSectionLength
    dataSectionLength += this->calculateBytesOfData(data);
}

void Assembler::processLabelRef(std::map<std::string, uint32_t> &unhandledLabelRefs, const Label& label, const int &lineNumber) {
    if (this->labelTable.find(label.getKey()) == this->labelTable.end()) {
        // label is not in labelTable
        unhandledLabelRefs.insert({label.getKey(), lineNumber});
    }
}

LabelType Assembler::getOperandLabelType(const std::string &instructionMnemonic) {
    if (
        instructionMnemonic == "push" ||
        instructionMnemonic == "loadG" ||
        instructionMnemonic == "storeG"
    ) {
        return LabelType::DATA;

    } else if (
        instructionMnemonic == "jmp" ||
        instructionMnemonic == "jez" ||
        instructionMnemonic == "jnz"
    ) {
        return LabelType::CODE;
    }
    return LabelType::METHOD;
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
    this->section = AssemblerDefs::Section::CODE;

    for (int i = 0; i < 4; i++) {
        bytecode.push_back((this->dataStartLocation >> (i * 8)) & 0xFF);
    }

    for (int i = 0; i < 4; i++) {
        bytecode.push_back((this->bytecodeLength >> (i * 8)) & 0xFF);
    }

    // loop through each statement
    for (auto& statement : this->statements) {
        if (std::holds_alternative<AssemblerDefs::Label>(statement)) {
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
        } else if (std::holds_alternative<AssemblerDefs::Section>(statement)) {
            this->section = AssemblerDefs::Section::DATA;
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
            this->pushBackVector(bytecode, this->convertLabelRefToBytes(Label{operand.value, getOperandLabelType(instruction.opcode)}));
        } else if (operand.type == AssemblerDefs::OperandType::STRING ||
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

uint8_t Assembler::convertTypeToByte(const std::string &type) {
    if (type == "i32") return 0x00;
    if (type == "ui32") return 0x01;
    if (type == "i64") return 0x02;
    if (type == "ui64") return 0x03;
    if (type == "f32") return 0x04;
    if (type == "f64") return 0x05;
    return 0x06;
}

uint8_t Assembler::convertDataTypeToByte(const std::string &dataType) const {
    if (dataType == "str") {
        return 0x07;
    }
    return this->convertTypeToByte(dataType);
}

std::optional<std::vector<uint8_t>> Assembler::convertDataToBytes(const std::string& dataType, const std::string& data, const int& lineNumber) const {
    std::vector<uint8_t> bytecode;

    std::string dataToConvert = "";

    if (data[0] == '#') {
        // remove # from immediate
        dataToConvert = data.substr(1);
    } else {
        dataToConvert = data;
    }

    if (dataType == "i32") {
        const int64_t parsed = std::stoll(dataToConvert);

        const int32_t value = static_cast<int32_t>(parsed);
        const uint32_t raw = static_cast<uint32_t>(value);

        for (int i = 0; i < 4; i++) {
            bytecode.push_back((raw >> (i * 8)) & 0xFF);
        }

    } else if (dataType == "ui32" || dataType == "") { // "" for loadL, storeL
        const uint32_t raw = static_cast<uint32_t>(std::stoull(dataToConvert));

        for (int i = 0; i < 4; i++) {
            bytecode.push_back((raw >> (i * 8)) & 0xFF);
        }

    } else if (dataType == "i64") {
        const int64_t value = std::stoll(dataToConvert);
        const uint64_t raw = static_cast<uint64_t>(value);

        for (int i = 0; i < 8; i++) {
            bytecode.push_back((raw >> (i * 8)) & 0xFF);
        }

    } else if (dataType == "ui64") {
        const uint64_t raw = std::stoull(dataToConvert);

        for (int i = 0; i < 8; i++) {
            bytecode.push_back((raw >> (i * 8)) & 0xFF);
        }

    } else if (dataType == "f32") {
        const float value = std::stof(dataToConvert);
        uint32_t raw;

        std::memcpy(&raw, &value, sizeof(float));

        for (int i = 0; i < 4; i++) {
            bytecode.push_back((raw >> (i * 8)) & 0xFF);
        }

    } else if (dataType == "f64") {
        const double value = std::stod(dataToConvert);
        uint64_t raw;

        std::memcpy(&raw, &value, sizeof(double));

        for (int i = 0; i < 8; i++) {
            bytecode.push_back((raw >> (i * 8)) & 0xFF);
        }

    } else if (dataType == "ptr") {
        this->pushBackVector(bytecode, this->convertLabelRefToBytes(Label{dataToConvert, LabelType::DATA}));
    } else if (dataType == "str") {
        this->pushBackVector(bytecode, this->convertStringToBytes(data));
    }
    return bytecode;
}

std::vector<uint8_t> Assembler::convertLabelRefToBytes(const Label& label) const {
    std::vector<uint8_t> bytecode;

    // resolve label ref to memory address
    for (int i = 0; i < 4; i++) {
        bytecode.push_back((this->labelTable.at(label.getKey()) >> (i * 8)) & 0xFF);
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
