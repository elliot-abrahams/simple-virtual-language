//
// Created by erael on 04/05/2026.
//

#ifndef SIMPLE_VM_PARSER_H
#define SIMPLE_VM_PARSER_H

#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "Types.h"


class Parser {

public:
    Parser();

    std::optional<std::vector<Types::Statement>> parse(std::vector<Types::SVMAToken>& tokenStream);

private:
    std::optional<Types::Statement> parseToken();

    std::optional<Types::Statement> parseInstruction();
    std::optional<Types::Statement> parseLabelDef();
    std::optional<Types::Statement> parseData();
    std::optional<Types::Statement> parseMethodDef();
    std::optional<Types::Statement> parseSectionStart();
    std::optional<Types::Operand> parseType();
    std::optional<Types::Operand> parseDataType();
    std::optional<Types::Operand> parseImmediate();
    std::optional<Types::Operand> parseLabelRef();
    std::optional<Types::Operand> parseOperand(Types::SVMATokenType tokenType);

    void next();
    Types::SVMAToken peek();
    Types::SVMAToken peekNext();

    static bool isNumberInteger(const Types::SVMAToken& token);
    static bool isNumberSigned(const Types::SVMAToken& token);

    Types::OperandType mapTokenTypeToOperandType(Types::SVMATokenType tokenType);

    void handleUnexpectedTokenError(const std::vector<Types::SVMATokenType> &expectingTypes);
    static std::string tokenTypeToString(Types::SVMATokenType tokenType);

    std::vector<Types::SVMAToken> tokenStream;
    int tokenIdx;
    Types::Section section;
};


#endif //SIMPLE_VM_PARSER_H