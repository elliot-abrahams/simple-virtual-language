#include <gtest/gtest.h>
#include "../ParserTestUtils.h"

TEST(EXPR_CAST, FLOAT_TO_INT) {
    const auto testCode = R"(
        int x = (int)5.5f;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            std::make_unique<parserTest::ExpectedCastExpr>(
                compiler::Type::INT,
                std::make_unique<parserTest::ExpectedFloatLiteral>(5.5f)
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_CAST, INT_TO_FLOAT) {
    const auto testCode = R"(
        int x = (float)5;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            std::make_unique<parserTest::ExpectedCastExpr>(
                compiler::Type::FLOAT,
                std::make_unique<parserTest::ExpectedIntegerLiteral>(5)
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_CAST, BOOL_TO_BOOL) {
    const auto testCode = R"(
        int x = (bool)true;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            std::make_unique<parserTest::ExpectedCastExpr>(
                compiler::Type::BOOL,
                std::make_unique<parserTest::ExpectedBoolLiteral>(true)
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_CAST, INVALID_MISSING_LBR) {
    const auto testCode = R"(
        int x = int)5.5f;
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(EXPR_CAST, INVALID_MISSING_TYPE) {
    const auto testCode = R"(
        int x = ()5.5f;
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(EXPR_CAST, INVALID_MISSING_RBR) {
    const auto testCode = R"(
        int x = (int 5.5f;
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(EXPR_CAST, INVALID_MISSING_EXPR) {
    const auto testCode = R"(
        int x = (int);
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}