#ifndef SIMPLE_VM_PARSER_H
#define SIMPLE_VM_PARSER_H

#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "AssemblerDefs.h"

namespace assembler {
    class Parser {

    public:
        Parser();

        std::optional<std::vector<AssemblerDefs::Statement>> parse(const std::vector<AssemblerDefs::SVMAToken>& tokenStream);

    private:
        std::optional<AssemblerDefs::Statement> parseToken();

        std::optional<AssemblerDefs::Statement> parseInstruction();
        std::optional<AssemblerDefs::Statement> parseLabelDef();
        std::optional<AssemblerDefs::Statement> parseData();
        std::optional<AssemblerDefs::Statement> parseMethodDef();
        std::optional<AssemblerDefs::Operand> parseType();
        std::optional<AssemblerDefs::Operand> parseImmediate();
        std::optional<AssemblerDefs::Operand> parseLabelRef();
        std::optional<AssemblerDefs::Operand> parseNativeRef();
        std::optional<AssemblerDefs::Operand> parseOperand(AssemblerDefs::SVMATokenType tokenType);
        std::optional<AssemblerDefs::Statement> parseDirective();

        std::optional<AssemblerDefs::DebugSource> parseDebugSource();
        std::optional<AssemblerDefs::DebugFunction> parseDebugFunction();
        std::optional<AssemblerDefs::DebugLine> parseDebugLine();

        void next();
        AssemblerDefs::SVMAToken peek();
        AssemblerDefs::SVMAToken peekNext();

        static bool checkAndHandleValueIsValidAsType(const std::string type, const std::string& value, const int& lineNumber);
        static bool checkAndHandleValueIsValidAsDataType(const std::string type, const std::string& value, const int& lineNumber);

        static bool isNumberInteger(const std::string& value);
        static bool isNumberDecimal(const std::string& value);
        static bool isNumberSigned(const std::string& value);

        static bool fitsUint16(const std::string& s);
        static bool fitsUint32(const std::string& s);

        AssemblerDefs::OperandType mapTokenTypeToOperandType(AssemblerDefs::SVMATokenType tokenType);

        static void printError(const std::string& msg, const int& lineNumber);
        static void handleUnexpectedTokenError(const std::vector<AssemblerDefs::SVMATokenType> &expectingTypes, const AssemblerDefs::SVMAToken& actualType, const int& lineNumber);
        static void handleIncorrectInstructionOperand(const std::string& instructionMnemonic, const AssemblerDefs::SVMATokenType expectedType, const int& lineNumber);
        static std::string tokenTypeToString(AssemblerDefs::SVMATokenType tokenType);

        std::vector<AssemblerDefs::SVMAToken> tokenStream;
        int tokenIdx;
        AssemblerDefs::Section section;
    };
}


#endif //SIMPLE_VM_PARSER_H