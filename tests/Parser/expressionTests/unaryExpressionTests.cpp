#include <gtest/gtest.h>
#include "../parserTestUtils.h"

TEST(EXPR_UNARY, INT_PLUS) {
    const auto testCode = R"(
        int x = +5;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            std::make_unique<parserTest::ExpectedUnaryExpr>(
                compiler::UnaryOperator::PLUS,
                std::make_unique<parserTest::ExpectedIntegerLiteral>(
                    5
                )
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_UNARY, VARIABLE_PLUS) {
    const auto testCode = R"(
        int x = +y;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            std::make_unique<parserTest::ExpectedUnaryExpr>(
                compiler::UnaryOperator::PLUS,
                std::make_unique<parserTest::ExpectedExprIdentifier>(
                    "y"
                )
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_UNARY, INT_MINUS) {
    const auto testCode = R"(
        int x = -5;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            std::make_unique<parserTest::ExpectedUnaryExpr>(
                compiler::UnaryOperator::MINUS,
                std::make_unique<parserTest::ExpectedIntegerLiteral>(
                    5
                )
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_UNARY, INT_MINUS_PAREN) {
    const auto testCode = R"(
        int x = -(5);
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            std::make_unique<parserTest::ExpectedUnaryExpr>(
                compiler::UnaryOperator::MINUS,
                std::make_unique<parserTest::ExpectedIntegerLiteral>(
                    5
                )
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_UNARY, VARIABLE_MINUS) {
    const auto testCode = R"(
        int x = -y;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            std::make_unique<parserTest::ExpectedUnaryExpr>(
                compiler::UnaryOperator::MINUS,
                std::make_unique<parserTest::ExpectedExprIdentifier>(
                    "y"
                )
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_UNARY, INT_LOGICAL_NOT) {
    const auto testCode = R"(
        int x = !5;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            std::make_unique<parserTest::ExpectedUnaryExpr>(
                compiler::UnaryOperator::LOGICAL_NOT,
                std::make_unique<parserTest::ExpectedIntegerLiteral>(
                    5
                )
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_UNARY, VARIABLE_LOGICAL_NOT) {
    const auto testCode = R"(
        int x = !y;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            std::make_unique<parserTest::ExpectedUnaryExpr>(
                compiler::UnaryOperator::LOGICAL_NOT,
                std::make_unique<parserTest::ExpectedExprIdentifier>(
                    "y"
                )
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_UNARY, INT_NESTED_LOGICAL_NOT) {
    const auto testCode = R"(
        int x = !!5;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            std::make_unique<parserTest::ExpectedUnaryExpr>(
                compiler::UnaryOperator::LOGICAL_NOT,
                std::make_unique<parserTest::ExpectedUnaryExpr>(
                    compiler::UnaryOperator::LOGICAL_NOT,
                    std::make_unique<parserTest::ExpectedIntegerLiteral>(
                        5
                    )
                )
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_UNARY, INT_NESTED_SIGN) {
    const auto testCode = R"(
        int x = -+-5;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            std::make_unique<parserTest::ExpectedUnaryExpr>(
                compiler::UnaryOperator::MINUS,
                std::make_unique<parserTest::ExpectedUnaryExpr>(
                    compiler::UnaryOperator::PLUS,
                    std::make_unique<parserTest::ExpectedUnaryExpr>(
                        compiler::UnaryOperator::MINUS,
                        std::make_unique<parserTest::ExpectedIntegerLiteral>(
                        5
                        )
                    )
                )
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_UNARY, INVALID_MISSING_EXPR) {
    const auto testCode = R"(
        int x = !;
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(EXPR_UNARY, INVALID_MISSING_LBR) {
    const auto testCode = R"(
        int x = (-5;
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(EXPR_UNARY, INVALID_MISSING_RBR) {
    const auto testCode = R"(
        int x = -5);
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}