#ifndef SVM_TOKENSIERTESTUTILS_H
#define SVM_TOKENSIERTESTUTILS_H

#include "../../src/compiler/Token.h"
#include "../../src/compiler/Tokeniser.h"
#include "../../src/include/Error.h"
#include "gtest/gtest.h"


namespace tokeniserTest {

    inline void ASSERT_TOKEN_EQ(
        const std::string& source,
        const TokenKind expectedTokenKind,
        const std::string& expecetedTokenImage
    ) {
        const auto path = new std::filesystem::path("Testing");
        auto tokeniser = new compiler::Tokeniser(source, path);

        const Token token = tokeniser->tok();

        ASSERT_EQ(expectedTokenKind, token.kind);
        ASSERT_EQ(expecetedTokenImage, token.image);
    }

    inline void ASSERT_TOKEN_SEQUENCE_EQ(
        const std::string& source,
        const std::vector<TokenKind>& expectedTokenKinds,
        const std::vector<std::string>& expectedTokenImages
    ) {
        const auto path = new std::filesystem::path("Testing");
        const auto tokeniser = new compiler::Tokeniser(source, path);

        int index = 0;
        Token token = tokeniser->tok();
        while (token.kind != TokenKind::END_OF_FILE) {
            ASSERT_EQ(expectedTokenKinds[index], token.kind);
            ASSERT_EQ(expectedTokenImages[index], token.image);

            tokeniser->next();
            token = tokeniser->tok();
            index++;
        }
    }

    inline void ASSERT_FIRST_TOKEN_IS_NOT_IDENTIFIER(
        const std::string& source
    ) {
        const auto path = new std::filesystem::path("Testing");
        auto tokeniser = new compiler::Tokeniser(source, path);
        const Token token = tokeniser->tok();
        ASSERT_NE(token.kind, TokenKind::IDENTIFIER);
    }

    inline void ASSERT_TOKEN_THROWS_LEXICAL_ERROR(
        const std::string& source
    ) {
        const auto path = new std::filesystem::path("Testing");
        ASSERT_THROW(auto tokeniser = new compiler::Tokeniser(source, path), LexicalError);
    }

}


#endif //SVM_TOKENSIERTESTUTILS_H