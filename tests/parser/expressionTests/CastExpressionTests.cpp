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
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedCastExpr>(
                std::make_unique<Type>(compiler::Type::INT, 0),
                std::make_unique<ExpectedFloatLiteral>(5.5f)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
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
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedCastExpr>(
                std::make_unique<Type>(compiler::Type::FLOAT, 0),
                std::make_unique<ExpectedIntegerLiteral>(5)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_CAST, BOOL_TO_BOOL) {
    const auto testCode = R"(
        int x = (bool)true;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedCastExpr>(
                std::make_unique<Type>(compiler::Type::BOOL, 0),
                std::make_unique<ExpectedBoolLiteral>(true)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
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