#include "Assembler.h"

#include <cmath>
#include <cstring>
#include <variant>
#include <iostream>

#include "../include/Error.h"

assembler::Assembler::Assembler() {}

std::vector<uint8_t> assembler::Assembler::assemble(const std::filesystem::path* filePath) {
    this->filepath = filePath;

    // STEP 1 -> lex source file
    Lexer lexer;
    return this->assembleFromTokens(lexer.lex(filePath));
}

std::vector<uint8_t> assembler::Assembler::assembleString(const std::string& fileContent) {
    this->filepath = new std::filesystem::path("testing");

    // STEP 1 -> lex source file
    Lexer lexer;
    return this->assembleFromTokens(lexer.lexString(this->filepath, fileContent));
}

std::vector<uint8_t> assembler::Assembler::assembleFromTokens(const std::vector<SVMAToken>& tokenStream) {
    // STEP 2 -> parse the stream of tokens
    Parser parser;
    this->statements = parser.parse(filepath, tokenStream);

    // STEP 3 -> construct the label table
    this->constructLabelTable();

    // STEP 4 -> generate bytecode
    return this->generateBytecode();
}

void assembler::Assembler::constructLabelTable() {
    this->section = Section::CODE;

    constexpr uint8_t bytecodeHeaderLength = 20;
    uint32_t codeSectionLength = 0;
    uint32_t dataSectionLength = 0;

    std::map<std::string, LabelReference> unhandledLabelRefs;

    // loop through list of statements
    for (auto& statement : this->statements) {

        // for each statement:
        // increment code / data lengths by their byte length
        // add to label table and track if any label exists that aren't defined

        // process LABEL
        if (std::holds_alternative<Label>(statement)) {
            bool isValid;
            auto label = std::get<Label>(statement);
            this->processLabelDef(unhandledLabelRefs, codeSectionLength, LabelReference{label.name.substr(0, label.name.size() - 1), LabelType::CODE, label.lineNumber, label.column});

        // process METHOD_DEF
        } else if (std::holds_alternative<MethodDef>(statement)) {
            auto methodDef = std::get<MethodDef>(statement);
            this->processLabelDef(unhandledLabelRefs, codeSectionLength, LabelReference{methodDef.label.name.substr(0, methodDef.label.name.size() - 1), LabelType::METHOD, methodDef.label.lineNumber, methodDef.label.column});
            codeSectionLength += 5; // 1 for number of args, 4 for number of locals

        // process INSTRUCTION
        } else if (std::holds_alternative<Instruction>(statement)) {
            this->processInstruction(unhandledLabelRefs, codeSectionLength, std::get<Instruction>(statement));

        // process DATA
        } else if (std::holds_alternative<Data>(statement)) {
            this->processData(unhandledLabelRefs, codeSectionLength + dataSectionLength + 1, dataSectionLength, std::get<Data>(statement));

        } else {
            // section (directive) Token
            this->section = std::get<Section>(statement);

            if (this->section == Section::METADATA) {
                break;
            }
        }
    }

    if (!unhandledLabelRefs.empty()) {
        throw AssemblerError(
            *this->filepath,
            unhandledLabelRefs.begin()->second.line,
            unhandledLabelRefs.begin()->second.column,
            unhandledLabelRefs.begin()->second.getTypeAsString() +
                " label '" +
                unhandledLabelRefs.begin()->second.name
                + "' is undefined"
        );
    }

    this->codeEndLocation = bytecodeHeaderLength + codeSectionLength - 1;
    this->dataEndLocation = bytecodeHeaderLength + codeSectionLength + dataSectionLength - 1;
}

void assembler::Assembler::processLabelDef(std::map<std::string, LabelReference>& unhandledLabelRefs, const uint32_t location, const LabelReference& label) {
    // check if def already exists
    if (labelTable.find(label.getKey()) != labelTable.end()) {
        throw AssemblerError(
            *this->filepath,
            label.line,
            label.column,
            label.getTypeAsString() +
                " label '" +
                label.name +
                "' is already defined"
        );
    }
    unhandledLabelRefs.erase(label.getKey());
    this->labelTable.insert({label.getKey(), location});
}

void assembler::Assembler::processInstruction(std::map<std::string, LabelReference>& unhandledLabelRefs, uint32_t& codeSectionLength, const Instruction& instruction) {
    std::string type;
    codeSectionLength += 1; // 1 for opcode
    // loop through each operand
    for (auto& operand : instruction.operands) {

        switch (operand.type) {
            case OperandType::TYPE: {
                codeSectionLength++;
                type = operand.value;
                break;
            }

            case OperandType::LABEL_REF: {
                // process label ref depending on required label type for this instruction
                this->processLabelRef(unhandledLabelRefs, LabelReference{operand.value, getOperandLabelType(instruction.opcode), operand.line, operand.column});
                codeSectionLength += 4;
                break;
            }

            case OperandType::STRING: {
                codeSectionLength += 4;
                break;
            }

            case OperandType::IMMEDIATE: {
                if (
                    instruction.opcode == "dup" ||
                    instruction.opcode == "rotD" ||
                    instruction.opcode == "rotU") {
                    codeSectionLength += 2;
                } else if (
                    instruction.opcode == "loadL" ||
                    instruction.opcode == "storeL"
                ) {
                    codeSectionLength += 4;
                } else {
                    codeSectionLength += this->calculateBytesFromType(type);
                }
                break;
            }

            case OperandType::NATIVE_REF:
            case OperandType::ERROR_REF: {
                codeSectionLength += 1;
                break;
            }
        }
    }
}

void assembler::Assembler::processData(std::map<std::string, LabelReference> &unhandledLabelRefs, const uint32_t location, uint32_t& dataSectionLength, const Data& data) {
    // process label Def
    this->processLabelDef(unhandledLabelRefs, location, LabelReference{data.label.name.substr(0, data.label.name.size() - 1), LabelType::DATA, data.label.lineNumber, data.label.column});

    // check if data type is ptr
    if (data.type == "ptr" && data.value[0] == '$') {
        this->processLabelRef(unhandledLabelRefs, LabelReference{data.value, LabelType::DATA, data.label.lineNumber, data.label.column});
    }
    // add length of data to dataSectionLength
    dataSectionLength += this->calculateBytesOfData(data);
}

void assembler::Assembler::processLabelRef(std::map<std::string, LabelReference> &unhandledLabelRefs, const LabelReference& label) {
    if (this->labelTable.find(label.getKey()) == this->labelTable.end()) {
        // label is not in labelTable
        unhandledLabelRefs.insert({label.getKey(), label});
    }
}

assembler::LabelType assembler::Assembler::getOperandLabelType(const std::string &instructionMnemonic) {
    if (
        instructionMnemonic == "push" ||
        instructionMnemonic == "loadG" ||
        instructionMnemonic == "storeG"
    ) {
        return LabelType::DATA;
    }
    if (
        instructionMnemonic == "jmp" ||
        instructionMnemonic == "jez" ||
        instructionMnemonic == "jnz"
    ) {
        return LabelType::CODE;
    }
    return LabelType::METHOD;
}

uint8_t assembler::Assembler::calculateBytesOfData(const Data& data) const {
    uint8_t length = 1; // 1 for data type
    if (data.type == "str") {
        length += 4; // 4 bytes for length of string

        const std::string rawString = data.value.substr(1, data.value.size() - 2); // remove quptation marks

        length += static_cast<uint32_t>(rawString.size()); // number of bytes of encoded string (UTF-8)

    } else {
        length += this->calculateBytesFromType(data.type);
    }
    return length;
}

uint8_t assembler::Assembler::calculateBytesFromType(const std::string& type) const {
    if (type == "i64" || type == "ui64" || type == "f64") {
        return 8;
    }
    return 4;
}

std::vector<uint8_t> assembler::Assembler::generateBytecode() {
    std::vector<uint8_t> bytecode;
    this->section = Section::CODE;

    // generate header
    for (int i = 0; i < 4; i++) {
        bytecode.push_back((this->codeEndLocation >> (i * 8)) & 0xFF); // header (code end location))
    }

    for (int i = 0; i < 4; i++) {
        bytecode.push_back((this->dataEndLocation >> (i * 8)) & 0xFF); // header (data end location)
    }

    for (int i = 0; i < 12; i++) { // add placeholder values for header's third, fourth and fifth field
        bytecode.push_back((0 >> (i * 8)) & 0xFF);
    }


    // loop through each statement
    for (auto& statement : this->statements) {
        if (std::holds_alternative<Label>(statement)) {
            continue;
        }
        // convert METHOD_DEF
        if (std::holds_alternative<MethodDef>(statement)) {
            this->pushBackVector(bytecode, this->convertMethodDefToBytes(std::get<MethodDef>(statement)));

        // convert INSTRUCTION
        } else if (std::holds_alternative<Instruction>(statement)) {
            auto data = this->convertInstructionToBytes(std::get<Instruction>(statement));
            pushBackVector(bytecode, data);

        // convert DATA
        } else if (std::holds_alternative<Data>(statement)) {
            auto data = this->convertDataStatementToBytes(std::get<Data>(statement));
            this->pushBackVector(bytecode, data);

        } else if (std::holds_alternative<Section>(statement)) {
            this->section = std::get<Section>(statement);

        // convert SOURCE_METADATA
        } else if (std::holds_alternative<SourceMetadata>(statement)) {
            this->pushBackVector(bytecode, this->convertSourceMetadata(std::get<SourceMetadata>(statement)));

        // convert FUNCTION_METADATA
        } else if (std::holds_alternative<FunctionMetadata>(statement)) {
            this->pushBackVector(bytecode, this->convertFunctionMetadata(std::get<FunctionMetadata>(statement)));

        // convert LINE_TABLE_METADATA
        } else if (std::holds_alternative<LineTableMetadata>(statement)) {
            this->pushBackVector(bytecode, this->convertLineTableMetadata(std::get<LineTableMetadata>(statement)));
        }
    }

    // add header value (source metadata end location)
    for (int i = 0; i < 4; i++) {
        bytecode.at(8 + i) = ((this->sourceMetadataLength + this->dataEndLocation) >> (i * 8)) & 0xFF;
    }

    // add header value (function metadata end location)
    for (int i = 0; i < 4; i++) {
        bytecode.at(12 + i) = ((this->functionMetadataLength + this->sourceMetadataLength + this->dataEndLocation) >> (i * 8)) & 0xFF;
    }

    // add header value (bytecode end location)
    for (int i = 0; i < 4; i++) {
        bytecode.at(16 + i) = ((this->lineTableMetadataLength + this->functionMetadataLength + this->sourceMetadataLength + this->dataEndLocation) >> (i * 8)) & 0xFF;
    }

    return bytecode;
}

std::vector<uint8_t> assembler::Assembler::convertInstructionToBytes(const Instruction &instruction) const {
    std::vector<uint8_t> bytecode;
    bytecode.push_back(opcode.at(instruction.opcode)); // encode opcode
    std::string dataType;

    // loop through each operand
    for (auto& operand : instruction.operands) {

        switch (operand.type) {

            case OperandType::TYPE: {
                bytecode.push_back(convertTypeToByte(operand.value));
                dataType = operand.value;
                break;
            }

            case OperandType::LABEL_REF: {
                this->pushBackVector(bytecode, this->convertLabelRefToBytes(LabelReference{operand.value, getOperandLabelType(instruction.opcode)}));
                break;
            }

            //case OperandType::STRING:
            case OperandType::IMMEDIATE: {

                if (instruction.opcode == "rotD" || instruction.opcode == "rotU") {
                    const auto value = std::stoll(operand.value.substr(1, operand.value.size()));
                    // ensure immediate is between 3 and 1024 (inclusive)
                    if (value > 1024 || value < 3) {
                        throw AssemblerError(
                            *this->filepath,
                            operand.line,
                            operand.column,
                            "invalid immediate '" +
                                operand.value +
                                "' for instruction '" +
                                instruction.opcode +
                                "'"
                        );
                    }
                    const auto raw = static_cast<uint16_t>(value);
                    for (int i = 0; i < 2; i++) {
                        bytecode.push_back((raw >> (i * 8)) & 0xFF);
                    }

                } else if (instruction.opcode == "dup") {
                    const auto value = std::stoll(operand.value.substr(1, operand.value.size()));
                    // ensure immediate is between 0 and 1023 (inclusive)
                    if (value > 1023 || value < 0) {
                        throw AssemblerError(
                            *this->filepath,
                            operand.line,
                            operand.column,
                            "invalid immediate '" +
                                operand.value +
                                "' for instruction '" +
                                instruction.opcode +
                                "'"
                        );
                    }
                    const auto raw = static_cast<uint16_t>(value);
                    for (int i = 0; i < 2; i++) {
                        bytecode.push_back((raw >> (i * 8)) & 0xFF);
                    }

                } else {
                    std::string typeToCheckAgainstImmediate;
                    if (instruction.opcode == "loadL") {
                        typeToCheckAgainstImmediate = "i32"; // immediate of loadL is type i32
                    } else {
                        typeToCheckAgainstImmediate = dataType;
                    }
                    auto data = this->convertDataToBytes(typeToCheckAgainstImmediate, operand.value, instruction.line, instruction.column);
                    this->pushBackVector(bytecode, data);
                }
                break;
            }

            case OperandType::NATIVE_REF: {
                // push native function id onto bytecode (as 1 byte)
                bytecode.push_back(nativeRef.at(instruction.operands[0].value));
                break;
            }

            case OperandType::ERROR_REF: {
                // push error ref id onto bytecode (as 1 byte)
                bytecode.push_back(errorRef.at(instruction.operands[0].value));
                break;
            }
        }
    }
    return bytecode;
}

std::vector<uint8_t> assembler::Assembler::convertMethodDefToBytes(const MethodDef& methodDef) {
    std::vector<uint8_t> bytecode;
    bytecode.push_back(methodDef.numberOfArguments);
    for (int i = 0; i < 4; i++) {
        bytecode.push_back((methodDef.numberOfLocals >> (i * 8)) & 0xFF);
    }
    return bytecode;
}

std::vector<uint8_t> assembler::Assembler::convertDataStatementToBytes(const Data& data) const {
    std::vector<uint8_t> bytecode;
    // encode data type to bytecode
    bytecode.push_back(this->convertDataTypeToByte(data.type));
    // encode data to bytecode
    auto byteList = this->convertDataToBytes(data.type, data.value, data.lineNumber, data.column);
    this->pushBackVector(bytecode, byteList);
    return bytecode;
}

uint8_t assembler::Assembler::convertTypeToByte(const std::string &type) {
    if (type == "i32") return 0x00;
    if (type == "ui32") return 0x01;
    if (type == "i64") return 0x02;
    if (type == "ui64") return 0x03;
    if (type == "f32") return 0x04;
    if (type == "f64") return 0x05;
    return 0x06;
}

uint8_t assembler::Assembler::convertDataTypeToByte(const std::string &dataType) const {
    if (dataType == "str") {
        return 0x07;
    }
    return this->convertTypeToByte(dataType);
}

std::vector<uint8_t> assembler::Assembler::convertDataToBytes(const std::string& dataType, const std::string& data, const uint32_t lineNumber, const uint16_t columnNumber) const {
    std::vector<uint8_t> bytecode;

    std::string dataToConvert = "";

    if (data[0] == '#') {
        // remove # from immediate
        dataToConvert = data.substr(1);
    } else {
        dataToConvert = data;
    }

    try {
        if (dataType == "i32" || dataType == "") { // "" for loadL, storeL
            const int64_t parsed = std::stoll(dataToConvert);

            const int32_t value = static_cast<int32_t>(parsed);
            const uint32_t raw = static_cast<uint32_t>(value);

            for (int i = 0; i < 4; i++) {
                bytecode.push_back((raw >> (i * 8)) & 0xFF);
            }

        } else if (dataType == "ui32") {
            const uint64_t parsed = std::stoull(dataToConvert);

            const uint32_t raw = static_cast<uint32_t>(parsed);

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

            if (!std::isfinite(value)) {
                throw std::out_of_range("");
            }

            uint32_t raw;
            std::memcpy(&raw, &value, sizeof(float));

            for (int i = 0; i < 4; i++) {
                bytecode.push_back((raw >> (i * 8)) & 0xFF);
            }

        } else if (dataType == "f64") {
            const double value = std::stod(dataToConvert);
            if (!std::isfinite(value)) {
                throw std::out_of_range("");
            }

            uint64_t raw;
            std::memcpy(&raw, &value, sizeof(double));

            for (int i = 0; i < 8; i++) {
                bytecode.push_back((raw >> (i * 8)) & 0xFF);
            }

        } else if (dataType == "ptr") {
            if (data[0] == '$') {
                this->pushBackVector(bytecode, this->convertLabelRefToBytes(LabelReference{dataToConvert, LabelType::DATA}));
            } else {
                const uint64_t raw = std::stoull(data);

                if (raw > UINT32_MAX) {
                    throw std::out_of_range("");
                }

                const uint32_t rawValToEncode = static_cast<uint32_t>(std::stoull(dataToConvert));

                for (int i = 0; i < 4; i++) {
                    bytecode.push_back((rawValToEncode >> (i * 8)) & 0xFF);
                }
            }
        } else if (dataType == "str") {
            this->pushBackVector(bytecode, this->convertStringToBytes(data));
        }
    } catch (const std::out_of_range &e) {
        throw AssemblerError(
            *this->filepath,
            lineNumber,
            columnNumber,
            "'" +
                data +
                "' is invalid for type '" +
                dataType +
                "'"
        );
    }
    return bytecode;
}

std::vector<uint8_t> assembler::Assembler::convertLabelRefToBytes(const LabelReference& label) const {
    std::vector<uint8_t> bytecode;

    // resolve label ref to memory address
    for (int i = 0; i < 4; i++) {
        bytecode.push_back((this->labelTable.at(label.getKey()) >> (i * 8)) & 0xFF);
    }
    return bytecode;
}

std::vector<uint8_t> assembler::Assembler::convertStringToBytes(const std::string &string) const {
    std::vector<uint8_t> bytecode;

    std::string str = string.substr(1, string.size() - 2);
    uint32_t numberOfBytes = str.size();

    // encode string length (4 bytes)
    for (int i = 0; i < 4; i++) {
        bytecode.push_back((numberOfBytes >> (i * 8)) & 0xFF);
    }

    // encode string as UTF-8
    bytecode.insert(bytecode.end(), str.begin(), str.end());

    return bytecode;
}

std::vector<uint8_t> assembler::Assembler::convertSourceMetadata(const SourceMetadata& sourceMetadata) {
    std::vector<uint8_t> bytecode;
    // source Id
    for (int i = 0; i < 2; i++) {
        bytecode.push_back((sourceMetadata.sourceId >> (i * 8)) & 0xFF);
    }
    this->sourceMetadataLength += 2;

    // path
    this->pushBackVector(bytecode, this->convertStringToBytes(sourceMetadata.path));
    this->sourceMetadataLength += (4 + sourceMetadata.path.size() - 2); // (-2) to disregard the space for quotation marks

    return bytecode;
}

std::vector<uint8_t> assembler::Assembler::convertFunctionMetadata(const FunctionMetadata& functionMetadata) {
    std::vector<uint8_t> bytecode;

    // start address
    for (int i = 0; i < 4; i++) {
        bytecode.push_back((functionMetadata.startAddress >> (i * 8)) & 0xFF);
    }
    // end address
    for (int i = 0; i < 4; i++) {
        bytecode.push_back((functionMetadata.endAddress >> (i * 8)) & 0xFF);
    }
    // source id
    for (int i = 0; i < 2; i++) {
        bytecode.push_back(functionMetadata.sourceId >> (i * 8) & 0xFF);
    }
    // function name
    this->pushBackVector(bytecode, this->convertStringToBytes(functionMetadata.name));

    this->functionMetadataLength += (14 + functionMetadata.name.size() - 2); // (-2) to disregard the space for quotation marks

    return bytecode;
}

std::vector<uint8_t> assembler::Assembler::convertLineTableMetadata(const LineTableMetadata& lineTableMetadata) {
    std::vector<uint8_t> bytecode;
    // start address
    for (int i = 0; i < 4; i++) {
        bytecode.push_back((lineTableMetadata.startAddress >> (i * 8)) & 0xFF);
    }
    // end address
    for (int i = 0; i < 4; i++) {
        bytecode.push_back((lineTableMetadata.endAddress >> (i * 8)) & 0xFF);
    }
    // source Id
    for (int i = 0; i < 2; i++) {
        bytecode.push_back((lineTableMetadata.sourceId >> (i * 8)) & 0xFF);
    }
    // line
    for (int i = 0; i < 4; i++) {
        bytecode.push_back((lineTableMetadata.line >> (i * 8)) & 0xFF);
    }
    // column
    for (int i = 0; i < 2; i++) {
        bytecode.push_back((lineTableMetadata.column >> (i * 8)) & 0xFF);
    }
    this->lineTableMetadataLength += 16;

    return bytecode;
}

std::vector<uint8_t> assembler::Assembler::pushBackVector(std::vector<uint8_t>& a, const std::vector<uint8_t>& b) const {
    a.insert(a.end(), b.begin(), b.end());
    return a;
}
