#ifndef SVM_TOKENISER_H
#define SVM_TOKENISER_H
#include <deque>
#include <filesystem>
#include <string_view>

#include "Token.h"


namespace compiler {
    class Tokeniser {

    public:
        Tokeniser(const std::string_view source, const std::filesystem::path* path);

        Token tok();
        Token lookAhead(size_t n);
        std::string eat(const TokenKind& kind);
        void next();

    private:
        Token readToken();
        void advance();

        std::deque<Token> tokenBuffer;

        std::string_view source;
        const std::filesystem::path* path;

        std::size_t start = 0;
        std::size_t current = 0;

        std::size_t line = 1;
        std::size_t column = 1;
    };
}


#endif //SVM_TOKENISER_H