#ifndef SIMPLE_VM_LEXER_H
#define SIMPLE_VM_LEXER_H

#pragma once
#include <filesystem>

#include "AssemblerDefs.h"
#include <string>
#include <vector>


namespace assembler {
    class Lexer {

    public:
        Lexer();

        std::vector<SVMAToken> lex(const std::filesystem::path* filePath);
        std::vector<SVMAToken> lexString(const std::filesystem::path* testpath, const std::string& fileContent);


    private:
        std::vector<SVMAToken> buildTokenStream();

        SVMAToken lexToken();
        SVMAToken lexLabel();
        SVMAToken lexNumber();
        SVMAToken lexHex();
        SVMAToken lexImmediate();
        SVMAToken lexDirective();
        SVMAToken lexString();
        SVMAToken lexKeyWord();

        void next();
        char peek() const;
        char peekNext();
        std::string readUntilWhitespace();
        std::string readString();
        void skipWhitespace();
        void skipComment();

        static bool isValidLabel(const std::string& s);
        static bool isValidHex(const std::string& s);
        static bool isValidString(const std::string& s);

        void throwInvalidLabel(const std::string& label, const uint16_t columnNumber) const;

        std::vector<char> inputBuffer;
        int charIdx;
        uint32_t lineNumber;
        uint16_t columnNumber;
        bool reachedEndOfFile;

        const std::filesystem::path* filePath;
    };
}


#endif //SIMPLE_VM_LEXER_H