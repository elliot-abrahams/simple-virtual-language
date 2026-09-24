#ifndef SIMPLE_VM_PARSER_H
#define SIMPLE_VM_PARSER_H

#include <filesystem>
#include <string>
#include <variant>
#include <vector>

#include "AssemblerDefs.h"

namespace assembler {
    class Parser {

    public:
        Parser();

        std::vector<Statement> parse(const std::filesystem::path* filepath, const std::vector<SVMAToken>& tokenStream);

    private:
        Statement parseToken();

        Statement parseInstruction();
        Statement parseLabelDef();
        Statement parseData();
        Statement parseMethodDef();
        Operand parseOperand(SVMATokenType tokenType);
        Statement parseDirective();

        SourceMetadata parseSourceMetadata();
        FunctionMetadata parseFunctionMetadata();
        LineTableMetadata parseLineTableMetadata();

        uint32_t parseAddress();
        uint16_t parseSourceId();

        void next();
        SVMAToken peek();
        SVMAToken peekNext();

        void checkAndHandleValueIsValidAsType(const std::string& type, const std::string& value, const uint32_t lineNumber, const uint16_t columnNumber) const;
        void checkAndHandleValueIsValidAsDataType(const std::string& type, const std::string& value, const uint32_t lineNumber, const uint16_t columnNumber) const;

        static bool isInteger(const std::string& value);
        static bool isSigned(const std::string& value);

        static bool fitsUint16(const std::string& s);
        static bool fitsUint32(const std::string& s);

        OperandType mapTokenTypeToOperandType(SVMATokenType tokenType);

        void throwUnexpectedTokenError(const SVMAToken& token) const;
        void throwInvalidValueForTypeError(const std::string& value, const std::string& type, const uint32_t lineNumber, const uint16_t columnNumber) const;
        void throwMethodMetadataOutOfRange(const std::string& value, const uint32_t lineNumber, const uint16_t columnNumber, const bool isArg) const;

        static std::string tokenTypeToString(SVMATokenType tokenType);

        std::vector<SVMAToken> tokenStream;
        int tokenIdx;
        Section section;
        const std::filesystem::path* filepath;
    };
}


#endif //SIMPLE_VM_PARSER_H