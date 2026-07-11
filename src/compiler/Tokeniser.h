#ifndef SVM_TOKENISER_H
#define SVM_TOKENISER_H
#include <filesystem>
#include <string_view>
#include "Token.h"


namespace compiler {
    class Tokeniser {

    public:
        Tokeniser(const std::string_view source, const std::filesystem::path* path);

        Token tok();
        std::string eat(const TokenKind& kind);
        void next();

    private:
        Token readToken();
        void advance();
        void setHead(const Token& token);

        Token headToken;

        std::string_view source;
        const std::filesystem::path* path;

        std::size_t start = 0;
        std::size_t current = 0;

        std::size_t line = 1;
        std::size_t column = 1;
    };
}


#endif //SVM_TOKENISER_H