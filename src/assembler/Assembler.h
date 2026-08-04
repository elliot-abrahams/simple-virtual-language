#ifndef SIMPLE_VM_ASSEMBLER_H
#define SIMPLE_VM_ASSEMBLER_H

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

#pragma once
#include "../include/AssemblerDefs.h"
#include "Lexer.h"
#include "Parser.h"


namespace assembler {
    enum class LabelType {
        CODE,
        METHOD,
        DATA
    };

    struct Label {
        std::string name;
        LabelType type;

        std::string getKey() const {
            switch (type) {
                case LabelType::CODE: return std::string(this->name + "CODE");
                case LabelType::METHOD: return std::string(this->name + "MTHD");
                case LabelType::DATA: return std::string(this->name + "DATA");
            }
            return "";
        }
    };

    class Assembler {

    public:
        Assembler();

        std::optional<std::vector<uint8_t>> assemble(const std::string& filePath);
        std::optional<std::vector<uint8_t>> assembleString(const std::string& fileContent);

    private:
        std::optional<std::vector<uint8_t>> assembleFromTokens(const std::vector<AssemblerDefs::SVMAToken>& tokens);

        bool constructLabelTable();

        bool processLabelDef(std::map<std::string, uint32_t>& unhandledLabelRefs, uint32_t location, const Label& label, const int& lineNumber);
        void processInstruction(std::map<std::string, uint32_t>& unhandledLabelRefs, uint32_t& codeSectionLength, const AssemblerDefs::Instruction& instruction);
        void processData(std::map<std::string, uint32_t>& unhandledLabelRefs, const uint32_t location, uint32_t& dataSectionLength, const AssemblerDefs::Data& data);
        void processLabelRef(std::map<std::string, uint32_t>& unhandledLabelRefs, const Label& label, const int& lineNumber);

        static LabelType getOperandLabelType(const std::string& instructionMnemonic);

        uint8_t calculateBytesOfData(const AssemblerDefs::Data& data) const;
        uint8_t calculateBytesFromType(const std::string& type) const;

        std::optional<std::vector<uint8_t>> generateBytecode();

        std::optional<std::vector<uint8_t>> convertInstructionToBytes(const AssemblerDefs::Instruction& instruction) const;
        std::vector<uint8_t> convertMethodDefToBytes(const AssemblerDefs::MethodDef& methodDef) const;
        std::optional<std::vector<uint8_t>> convertDataStatementToBytes(const AssemblerDefs::Data& data) const;

        static uint8_t convertTypeToByte(const std::string& type);
        uint8_t convertDataTypeToByte(const std::string& dataType) const;
        std::optional<std::vector<uint8_t>> convertDataToBytes(const std::string& dataType, const std::string& data, const int& lineNumber) const;
        std::vector<uint8_t> convertLabelRefToBytes(const Label& label) const;
        std::vector<uint8_t> convertStringToBytes(const std::string& string) const;

        std::vector<uint8_t> pushBackVector(std::vector<uint8_t>& a, const std::vector<uint8_t>& b) const;

        AssemblerDefs::Section section;
        std::vector<AssemblerDefs::Statement> statements;
        std::map<std::string, uint32_t> labelTable;
        uint32_t dataStartLocation;
        uint32_t bytecodeLength;
    };
}


#endif //SIMPLE_VM_ASSEMBLER_H