#ifndef SIMPLE_VM_ASSEMBLER_H
#define SIMPLE_VM_ASSEMBLER_H

#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "AssemblerDefs.h"
#include "Lexer.h"
#include "Parser.h"


namespace assembler {
    enum class LabelType {
        CODE,
        METHOD,
        DATA
    };

    struct LabelReference {
        const std::string name;
        const LabelType type;

        const uint32_t line;
        const uint16_t column;

        std::string getKey() const {
            switch (type) {
                case LabelType::CODE: return std::string(this->name + "C");
                case LabelType::METHOD: return std::string(this->name + "M");
                case LabelType::DATA: return std::string(this->name + "D");
            }
            return "";
        }

        std::string getTypeAsString() const {
            switch (this->type) {
                case LabelType::CODE: return "code";
                case LabelType::METHOD: return "method";
                case LabelType::DATA: return "data";
            }
            return "";
        }
    };

    class Assembler {

    public:
        Assembler();

        std::vector<uint8_t> assemble(const std::filesystem::path* filePath);
        std::vector<uint8_t> assembleString(const std::string& fileContent);

    private:
        std::vector<uint8_t> assembleFromTokens(const std::vector<SVMAToken>& tokenStream);

        void constructLabelTable();

        void processLabelDef(std::map<std::string, LabelReference>& unhandledLabelRefs, uint32_t location, const LabelReference& label);
        void processInstruction(std::map<std::string, LabelReference>& unhandledLabelRefs, uint32_t& codeSectionLength, const Instruction& instruction);
        void processData(std::map<std::string, LabelReference>& unhandledLabelRefs, const uint32_t location, uint32_t& dataSectionLength, const Data& data);
        void processLabelRef(std::map<std::string, LabelReference>& unhandledLabelRefs, const LabelReference& label);

        static LabelType getOperandLabelType(const std::string& instructionMnemonic);

        uint8_t calculateBytesOfData(const Data& data) const;
        uint8_t calculateBytesFromType(const std::string& type) const;

        std::vector<uint8_t> generateBytecode();

        std::vector<uint8_t> convertInstructionToBytes(const Instruction& instruction) const;

        static std::vector<uint8_t> convertMethodDefToBytes(const MethodDef& methodDef);
        std::vector<uint8_t> convertDataStatementToBytes(const Data& data) const;

        static uint8_t convertTypeToByte(const std::string& type);
        uint8_t convertDataTypeToByte(const std::string& dataType) const;
        std::vector<uint8_t> convertDataToBytes(const std::string& dataType, const std::string& data, const uint32_t lineNumber, const uint16_t columnNumber) const;
        std::vector<uint8_t> convertLabelRefToBytes(const LabelReference& label) const;
        std::vector<uint8_t> convertStringToBytes(const std::string& string) const;

        std::vector<uint8_t> convertSourceMetadata(const SourceMetadata& sourceMetadata);
        std::vector<uint8_t> convertFunctionMetadata(const FunctionMetadata& functionMetadata);
        std::vector<uint8_t> convertLineTableMetadata(const LineTableMetadata& lineTableMetadata);

        std::vector<uint8_t> pushBackVector(std::vector<uint8_t>& a, const std::vector<uint8_t>& b) const;

        Section section;
        std::vector<Statement> statements;
        std::map<std::string, uint32_t> labelTable;
        uint32_t codeEndLocation = 0;
        uint32_t dataEndLocation = 0;
        uint32_t sourceMetadataLength = 0;
        uint32_t functionMetadataLength = 0;
        uint32_t lineTableMetadataLength = 0;

        const std::filesystem::path* filepath;
    };
}


#endif //SIMPLE_VM_ASSEMBLER_H