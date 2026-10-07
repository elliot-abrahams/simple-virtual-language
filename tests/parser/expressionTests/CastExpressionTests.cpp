#include <gtest/gtest.h>
#include "../ParserTestUtils.h"

using namespace parserTest;

TEST(EXPR_CAST, FLOAT_TO_INT) {
    const auto testCode = R"(
        int x = (int)5.5f;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedCastExpr>(
                std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
                std::make_unique<ExpectedFloatLiteral>(5.5f)
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_CAST, INT_TO_FLOAT) {
    const auto testCode = R"(
        int x = (float)5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedCastExpr>(
                std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
                std::make_unique<ExpectedIntegerLiteral>(5)
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_CAST, BOOL_TO_CHAR) {
    const auto testCode = R"(
        int x = (char)true;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedCastExpr>(
                std::make_unique<Type>(compiler::CHAR_TYPE_ID, 0),
                std::make_unique<ExpectedBoolLiteral>(true)
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_CAST, CHAR_TO_BOOL) {
    const auto testCode = R"(
        int x = (bool)'a';
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedCastExpr>(
                std::make_unique<Type>(compiler::BOOL_TYPE_ID, 0),
                std::make_unique<ExpectedCharLiteral>('a')
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_CAST, INVALID_MISSING_LBR) {
    const auto testCode = R"(
        int x = int)5.5f;
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(EXPR_CAST, INVALID_MISSING_TYPE) {
    const auto testCode = R"(
        int x = ()5.5f;
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(EXPR_CAST, INVALID_MISSING_RBR) {
    const auto testCode = R"(
        int x = (int 5.5f;
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(EXPR_CAST, INVALID_MISSING_EXPR) {
    const auto testCode = R"(
        int x = (int);
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}