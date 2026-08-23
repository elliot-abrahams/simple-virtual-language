#include "Assembler.h"

#include <cstring>
#include <variant>
#include <iostream>

assembler::Assembler::Assembler() {}

std::optional<std::vector<uint8_t>> assembler::Assembler::assemble(const std::string& filePath) {
    // STEP 1 -> lex source file
    Lexer lexer;
    auto tokenStream = lexer.lex(filePath);

    if (!tokenStream.has_value()) {
        return std::nullopt;
    }
    return this->assembleFromTokens(tokenStream.value());
}

std::optional<std::vector<uint8_t>> assembler::Assembler::assembleString(const std::string& fileContent) {
    // STEP 1 -> lex source file
    Lexer lexer;
    auto tokenStream = lexer.lexString(fileContent);

    if (!tokenStream.has_value()) {
        return std::nullopt;
    }
    return this->assembleFromTokens(tokenStream.value());
}

std::optional<std::vector<uint8_t>> assembler::Assembler::assembleFromTokens(const std::vector<AssemblerDefs::SVMAToken>& tokenStream) {
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

bool assembler::Assembler::constructLabelTable() {
    this->section = AssemblerDefs::Section::CODE;

    constexpr uint8_t bytecodeHeaderLength = 20;
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
            codeSectionLength += 5; // 1 for number of args, 4 for number of locals

        // process INSTRUCTION
        } else if (std::holds_alternative<AssemblerDefs::Instruction>(statement)) {
            this->processInstruction(unhandledLabelRefs, codeSectionLength, std::get<AssemblerDefs::Instruction>(statement));

        // process DATA
        } else if (std::holds_alternative<AssemblerDefs::Data>(statement)) {
            this->processData(unhandledLabelRefs, codeSectionLength + dataSectionLength + 1, dataSectionLength, std::get<AssemblerDefs::Data>(statement));

        } else {
            // section (directive) Token
            this->section = std::get<AssemblerDefs::Section>(statement);

            if (this->section == AssemblerDefs::Section::DEBUG) {
                break;
            }
        }
    }

    if (!unhandledLabelRefs.empty()) {
        std::cerr << "Undefined label(s):" << std::endl;
        for (auto& label : unhandledLabelRefs) {
            std::cerr << "Line " << label.second << " -> " << label.first.substr(0, label.first.size() - 4) << " (" << label.first.substr(label.first.size() - 4, label.first.size()) << ")" << std::endl;
        }
        return false;
    }

    this->codeEndLocation = bytecodeHeaderLength + codeSectionLength - 1;
    this->dataEndLocation = bytecodeHeaderLength + codeSectionLength + dataSectionLength - 1;
    return true;
}

bool assembler::Assembler::processLabelDef(std::map<std::string, uint32_t>& unhandledLabelRefs, const uint32_t location, const Label& label, const int& lineNumber) {
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

void assembler::Assembler::processInstruction(std::map<std::string, uint32_t>& unhandledLabelRefs, uint32_t& codeSectionLength, const AssemblerDefs::Instruction& instruction) {
    std::string type;
    codeSectionLength += 1; // 1 for opcode
    // loop through each operand
    for (auto& operand : instruction.operands) {

        switch (operand.type) {
            case AssemblerDefs::OperandType::TYPE: {
                codeSectionLength++;
                type = operand.value;
                break;
            }

            case AssemblerDefs::OperandType::LABEL_REF: {
                // process label ref depending on required label type for this instruction
                this->processLabelRef(unhandledLabelRefs, Label{operand.value, getOperandLabelType(instruction.opcode)}, instruction.lineNumber);
                codeSectionLength += 4;
                break;
            }

            case AssemblerDefs::OperandType::STRING: {
                codeSectionLength += 4;
                break;
            }

            case AssemblerDefs::OperandType::IMMEDIATE: {
                if (instruction.opcode == "loadL" || instruction.opcode == "storeL") {
                    codeSectionLength += 4;
                } else {
                    codeSectionLength += this->calculateBytesFromType(type);
                }
                break;
            }

            case AssemblerDefs::OperandType::NATIVE_REF: {
                codeSectionLength += 1;
                break;
            }
        }
    }
}

void assembler::Assembler::processData(std::map<std::string, uint32_t> &unhandledLabelRefs, const uint32_t location, uint32_t& dataSectionLength, const AssemblerDefs::Data& data) {
    // process label Def
    this->processLabelDef(unhandledLabelRefs, location, Label{data.name.substr(0, data.name.size() - 1), LabelType::DATA}, data.lineNumber);

    // check if data type is ptr
    if (data.type == "ptr") {
        this->processLabelRef(unhandledLabelRefs, Label{data.value, LabelType::DATA}, data.lineNumber);
    }
    // add length of data to dataSectionLength
    dataSectionLength += this->calculateBytesOfData(data);
}

void assembler::Assembler::processLabelRef(std::map<std::string, uint32_t> &unhandledLabelRefs, const Label& label, const int &lineNumber) {
    if (this->labelTable.find(label.getKey()) == this->labelTable.end()) {
        // label is not in labelTable
        unhandledLabelRefs.insert({label.getKey(), lineNumber});
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

uint8_t assembler::Assembler::calculateBytesOfData(const AssemblerDefs::Data& data) const {
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

std::optional<std::vector<uint8_t>> assembler::Assembler::generateBytecode() {
    std::vector<uint8_t> bytecode;
    this->section = AssemblerDefs::Section::CODE;

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
            this->section = std::get<AssemblerDefs::Section>(statement);

        // convert DEBUG_SOURCE
        } else if (std::holds_alternative<AssemblerDefs::DebugSource>(statement)) {
            this->pushBackVector(bytecode, this->convertDebugSource(std::get<AssemblerDefs::DebugSource>(statement)));

        // convert DEBUG_FUNCTION
        } else if (std::holds_alternative<AssemblerDefs::DebugFunction>(statement)) {
            this->pushBackVector(bytecode, this->convertDebugFunction(std::get<AssemblerDefs::DebugFunction>(statement)));

        // convert DEBUG_LINE
        } else if (std::holds_alternative<AssemblerDefs::DebugLine>(statement)) {
            this->pushBackVector(bytecode, this->convertDebugLine(std::get<AssemblerDefs::DebugLine>(statement)));
        }
    }

    // add header value (debug source end location)
    for (int i = 0; i < 4; i++) {
        bytecode.at(8 + i) = ((this->debugSourceLength + this->dataEndLocation) >> (i * 8)) & 0xFF;
    }

    // add header value (debug function end location)
    for (int i = 0; i < 4; i++) {
        bytecode.at(12 + i) = ((this->debugFunctionLength + this->debugSourceLength + this->dataEndLocation) >> (i * 8)) & 0xFF;
    }

    // add header value (bytecode end location)
    for (int i = 0; i < 4; i++) {
        bytecode.at(16 + i) = ((this->debugLineTableLength + this->debugFunctionLength + this->debugSourceLength + this->dataEndLocation) >> (i * 8)) & 0xFF;
    }

    return bytecode;
}

std::optional<std::vector<uint8_t>> assembler::Assembler::convertInstructionToBytes(const AssemblerDefs::Instruction &instruction) const {
    std::vector<uint8_t> bytecode;
    bytecode.push_back(AssemblerDefs::opcode.at(instruction.opcode)); // encode opcode
    std::string dataType;

    // loop through each operand
    for (auto& operand : instruction.operands) {

        switch (operand.type) {

            case AssemblerDefs::OperandType::TYPE: {
                bytecode.push_back(this->convertTypeToByte(operand.value));
                dataType = operand.value;
                break;
            }

            case AssemblerDefs::OperandType::LABEL_REF: {
                this->pushBackVector(bytecode, this->convertLabelRefToBytes(Label{operand.value, getOperandLabelType(instruction.opcode)}));
                break;
            }

            case AssemblerDefs::OperandType::STRING:
            case AssemblerDefs::OperandType::IMMEDIATE: {
                std::string typeToCheckAgainstImmediate = "";
                if (instruction.opcode == "loadL") {
                    typeToCheckAgainstImmediate = "i32"; // immediate of loadL is type i32
                } else {
                    typeToCheckAgainstImmediate = dataType;
                }
                auto data = this->convertDataToBytes(typeToCheckAgainstImmediate, operand.value, instruction.lineNumber);
                if (!data.has_value()) {
                    return std::nullopt;
                }
                this->pushBackVector(bytecode, data.value());
                break;
            }

            case AssemblerDefs::OperandType::NATIVE_REF: {
                // push native function id onto bytecode (as 1 byte)
                bytecode.push_back(AssemblerDefs::nativeRef.at(instruction.operands[0].value));
                break;
            }
        }
    }
    return bytecode;
}

std::vector<uint8_t> assembler::Assembler::convertMethodDefToBytes(const AssemblerDefs::MethodDef& methodDef) const {
    std::vector<uint8_t> bytecode;
    bytecode.push_back(methodDef.numberOfArguments);
    for (int i = 0; i < 4; i++) {
        bytecode.push_back((methodDef.numberOfLocals >> (i * 8)) & 0xFF);
    }
    return bytecode;
}

std::optional<std::vector<uint8_t>> assembler::Assembler::convertDataStatementToBytes(const AssemblerDefs::Data& data) const {
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

std::optional<std::vector<uint8_t>> assembler::Assembler::convertDataToBytes(const std::string& dataType, const std::string& data, const int& lineNumber) const {
    std::vector<uint8_t> bytecode;

    std::string dataToConvert = "";

    if (data[0] == '#') {
        // remove # from immediate
        dataToConvert = data.substr(1);
    } else {
        dataToConvert = data;
    }

    try {
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
    } catch (const std::out_of_range &e) {
        std::cerr << "Error found at Line " << lineNumber << std::endl;
        std::cerr << "Value is out of range for type " + dataType << std::endl;
        return std::nullopt;
    }

    return bytecode;
}

std::vector<uint8_t> assembler::Assembler::convertLabelRefToBytes(const Label& label) const {
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

std::vector<uint8_t> assembler::Assembler::convertDebugSource(const AssemblerDefs::DebugSource& debugSource) {
    std::vector<uint8_t> bytecode;
    // source Id
    for (int i = 0; i < 2; i++) {
        bytecode.push_back((debugSource.sourceId >> (i * 8)) & 0xFF);
    }
    this->debugSourceLength += 2;

    // path
    this->pushBackVector(bytecode, this->convertStringToBytes(debugSource.path));
    this->debugSourceLength += (4 + debugSource.path.size() - 2); // (-2) to disregard the space for quotation marks

    return bytecode;
}

std::vector<uint8_t> assembler::Assembler::convertDebugFunction(const AssemblerDefs::DebugFunction &debugFunction) {
    std::vector<uint8_t> bytecode;

    // start address
    for (int i = 0; i < 4; i++) {
        bytecode.push_back((debugFunction.startAddress >> (i * 8)) & 0xFF);
    }
    // end address
    for (int i = 0; i < 4; i++) {
        bytecode.push_back((debugFunction.endAddress >> (i * 8)) & 0xFF);
    }
    // source id
    for (int i = 0; i < 2; i++) {
        bytecode.push_back(debugFunction.sourceId >> (i * 8) & 0xFF);
    }
    // function name
    this->pushBackVector(bytecode, this->convertStringToBytes(debugFunction.name));

    this->debugFunctionLength += (14 + debugFunction.name.size() - 2); // (-2) to disregard the space for quotation marks

    return bytecode;
}

std::vector<uint8_t> assembler::Assembler::convertDebugLine(const AssemblerDefs::DebugLine& debugLine) {
    std::vector<uint8_t> bytecode;
    // start address
    for (int i = 0; i < 4; i++) {
        bytecode.push_back((debugLine.startAddress >> (i * 8)) & 0xFF);
    }
    // end address
    for (int i = 0; i < 4; i++) {
        bytecode.push_back((debugLine.endAddress >> (i * 8)) & 0xFF);
    }
    // source Id
    for (int i = 0; i < 2; i++) {
        bytecode.push_back((debugLine.sourceId >> (i * 8)) & 0xFF);
    }
    // line
    for (int i = 0; i < 4; i++) {
        bytecode.push_back((debugLine.line >> (i * 8)) & 0xFF);
    }
    // column
    for (int i = 0; i < 2; i++) {
        bytecode.push_back((debugLine.column >> (i * 8)) & 0xFF);
    }
    this->debugLineTableLength += 16;

    return bytecode;
}

std::vector<uint8_t> assembler::Assembler::pushBackVector(std::vector<uint8_t>& a, const std::vector<uint8_t>& b) const {
    a.insert(a.end(), b.begin(), b.end());
    return a;
}
