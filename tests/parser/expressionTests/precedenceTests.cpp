#include <gtest/gtest.h>
#include "../parserTestUtils.h"


TEST(EXPR_PRECEDENCE, UNARY_BEFORE_MULTIPLICATIVE) {
    const auto testCode = R"(
        int x = -1 * 2;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            std::make_unique<parserTest::ExpectedBinaryExpr>(
                compiler::BinaryOperator::MULTIPLY,
                std::make_unique<parserTest::ExpectedUnaryExpr>(
                    compiler::UnaryOperator::MINUS,
                    std::make_unique<parserTest::ExpectedIntegerLiteral>(1)
                ),
                std::make_unique<parserTest::ExpectedIntegerLiteral>(2)
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_PRECEDENCE, PARETHESES_BEFORE_MULTIPLICATIVE) {
    const auto testCode = R"(
        int x = 1 * (2 + 3);
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            std::make_unique<parserTest::ExpectedBinaryExpr>(
                compiler::BinaryOperator::MULTIPLY,
                std::make_unique<parserTest::ExpectedIntegerLiteral>(1),
                std::make_unique<parserTest::ExpectedBinaryExpr>(
                    compiler::BinaryOperator::PLUS,
                    std::make_unique<parserTest::ExpectedIntegerLiteral>(2),
                    std::make_unique<parserTest::ExpectedIntegerLiteral>(3)
                )
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_PRECEDENCE, MULTIPLICATIVE_BEFORE_ADDITIVE_1) {
    const auto testCode = R"(
        int x = 1 + 2 * 3;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            std::make_unique<parserTest::ExpectedBinaryExpr>(
                compiler::BinaryOperator::PLUS,
                std::make_unique<parserTest::ExpectedIntegerLiteral>(1),
                std::make_unique<parserTest::ExpectedBinaryExpr>(
                    compiler::BinaryOperator::MULTIPLY,
                    std::make_unique<parserTest::ExpectedIntegerLiteral>(2),
                    std::make_unique<parserTest::ExpectedIntegerLiteral>(3)
                )
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_PRECEDENCE, MULTIPLICATIVE_BEFORE_ADDITIVE_2) {
    const auto testCode = R"(
        int x = 1 * 2 + 3;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            std::make_unique<parserTest::ExpectedBinaryExpr>(
                compiler::BinaryOperator::PLUS,
                std::make_unique<parserTest::ExpectedBinaryExpr>(
                    compiler::BinaryOperator::MULTIPLY,
                    std::make_unique<parserTest::ExpectedIntegerLiteral>(1),
                    std::make_unique<parserTest::ExpectedIntegerLiteral>(2)
                ),
                std::make_unique<parserTest::ExpectedIntegerLiteral>(3)
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_PRECEDENCE, ADDITIVE_BEFORE_COMPARISON) {
    const auto testCode = R"(
        int x = 1 < 2 + 3;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            std::make_unique<parserTest::ExpectedBinaryExpr>(
                compiler::BinaryOperator::LESS_THAN,
                std::make_unique<parserTest::ExpectedIntegerLiteral>(1),
                std::make_unique<parserTest::ExpectedBinaryExpr>(
                    compiler::BinaryOperator::PLUS,
                    std::make_unique<parserTest::ExpectedIntegerLiteral>(2),
                    std::make_unique<parserTest::ExpectedIntegerLiteral>(3)
                )
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_PRECEDENCE, COMPARISON_BEFORE_EQUALITY) {
    const auto testCode = R"(
        int x = 1 == 2 < 3;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            std::make_unique<parserTest::ExpectedBinaryExpr>(
                compiler::BinaryOperator::EQUAL_EQUAL,
                std::make_unique<parserTest::ExpectedIntegerLiteral>(1),
                std::make_unique<parserTest::ExpectedBinaryExpr>(
                    compiler::BinaryOperator::LESS_THAN,
                    std::make_unique<parserTest::ExpectedIntegerLiteral>(2),
                    std::make_unique<parserTest::ExpectedIntegerLiteral>(3)
                )
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_PRECEDENCE, EQUALITY_BEFORE_LOGICAL_AND) {
    const auto testCode = R"(
        int x = 1 && 2 == 3;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            std::make_unique<parserTest::ExpectedBinaryExpr>(
                compiler::BinaryOperator::LOGICAL_AND,
                std::make_unique<parserTest::ExpectedIntegerLiteral>(1),
                std::make_unique<parserTest::ExpectedBinaryExpr>(
                    compiler::BinaryOperator::EQUAL_EQUAL,
                    std::make_unique<parserTest::ExpectedIntegerLiteral>(2),
                    std::make_unique<parserTest::ExpectedIntegerLiteral>(3)
                )
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_PRECEDENCE, LOGICAL_AND_BEFORE_LOGICAL_OR) {
    const auto testCode = R"(
        int x = 1 || 2 && 3;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            std::make_unique<parserTest::ExpectedBinaryExpr>(
                compiler::BinaryOperator::LOGICAL_OR,
                std::make_unique<parserTest::ExpectedIntegerLiteral>(1),
                std::make_unique<parserTest::ExpectedBinaryExpr>(
                    compiler::BinaryOperator::LOGICAL_AND,
                    std::make_unique<parserTest::ExpectedIntegerLiteral>(2),
                    std::make_unique<parserTest::ExpectedIntegerLiteral>(3)
                )
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}