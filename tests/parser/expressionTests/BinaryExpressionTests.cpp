#include <gtest/gtest.h>
#include "../ParserTestUtils.h"

using namespace parserTest;

TEST(EXPR_BINARY, ADD) {
    const auto testCode = R"(
        int x = 1 + 2;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedBinaryExpr>(
                compiler::BinaryOperator::PLUS,
                std::make_unique<ExpectedIntegerLiteral>(1),
                std::make_unique<ExpectedIntegerLiteral>(2)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_BINARY, SUBTRACT) {
    const auto testCode = R"(
        int x = 1 - 2;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedBinaryExpr>(
                compiler::BinaryOperator::MINUS,
                std::make_unique<ExpectedIntegerLiteral>(1),
                std::make_unique<ExpectedIntegerLiteral>(2)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_BINARY, MULTIPLY) {
    const auto testCode = R"(
        int x = 1 * 2;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedBinaryExpr>(
                compiler::BinaryOperator::MULTIPLY,
                std::make_unique<ExpectedIntegerLiteral>(1),
                std::make_unique<ExpectedIntegerLiteral>(2)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_BINARY, DIVIDE) {
    const auto testCode = R"(
        int x = 1 / 2;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedBinaryExpr>(
                compiler::BinaryOperator::DIVIDE,
                std::make_unique<ExpectedIntegerLiteral>(1),
                std::make_unique<ExpectedIntegerLiteral>(2)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_BINARY, INTEGER_DIVIDE) {
    const auto testCode = R"(
        int x = 5 // 6;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedBinaryExpr>(
                compiler::BinaryOperator::INTEGER_DIVIDE,
                std::make_unique<ExpectedIntegerLiteral>(5),
                std::make_unique<ExpectedIntegerLiteral>(6)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_BINARY, MODULO) {
    const auto testCode = R"(
        int x = 1 % 2;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedBinaryExpr>(
                compiler::BinaryOperator::MODULO,
                std::make_unique<ExpectedIntegerLiteral>(1),
                std::make_unique<ExpectedIntegerLiteral>(2)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_BINARY, LOGICAL_OR) {
    const auto testCode = R"(
        int x = 1 || 2;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedBinaryExpr>(
                compiler::BinaryOperator::LOGICAL_OR,
                std::make_unique<ExpectedIntegerLiteral>(1),
                std::make_unique<ExpectedIntegerLiteral>(2)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_BINARY, LOGICAL_AND) {
    const auto testCode = R"(
        int x = 1 && 2;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedBinaryExpr>(
                compiler::BinaryOperator::LOGICAL_AND,
                std::make_unique<ExpectedIntegerLiteral>(1),
                std::make_unique<ExpectedIntegerLiteral>(2)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_BINARY, EQUAL) {
    const auto testCode = R"(
        int x = 1 == 2;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedBinaryExpr>(
                compiler::BinaryOperator::EQUAL_EQUAL,
                std::make_unique<ExpectedIntegerLiteral>(1),
                std::make_unique<ExpectedIntegerLiteral>(2)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_BINARY, NOT_EQUAL) {
    const auto testCode = R"(
        int x = 1 != 2;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedBinaryExpr>(
                compiler::BinaryOperator::NOT_EQUAL,
                std::make_unique<ExpectedIntegerLiteral>(1),
                std::make_unique<ExpectedIntegerLiteral>(2)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_BINARY, LESS_THAN) {
    const auto testCode = R"(
        int x = 1 < 2;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedBinaryExpr>(
                compiler::BinaryOperator::LESS_THAN,
                std::make_unique<ExpectedIntegerLiteral>(1),
                std::make_unique<ExpectedIntegerLiteral>(2)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL) {
    const auto testCode = R"(
        int x = 1 <= 2;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedBinaryExpr>(
                compiler::BinaryOperator::LESS_THAN_OR_EQUAL,
                std::make_unique<ExpectedIntegerLiteral>(1),
                std::make_unique<ExpectedIntegerLiteral>(2)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_BINARY, GREATER_THAN) {
    const auto testCode = R"(
        int x = 1 > 2;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedBinaryExpr>(
                compiler::BinaryOperator::GREATER_THAN,
                std::make_unique<ExpectedIntegerLiteral>(1),
                std::make_unique<ExpectedIntegerLiteral>(2)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL) {
    const auto testCode = R"(
        int x = 1 >= 2;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedBinaryExpr>(
                compiler::BinaryOperator::GREATER_THAN_OR_EQUAL,
                std::make_unique<ExpectedIntegerLiteral>(1),
                std::make_unique<ExpectedIntegerLiteral>(2)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_BINARY, ASSOCIATIVE_ADD) {
    const auto testCode = R"(
        int x = 1 + 2 + 3;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedBinaryExpr>(
                compiler::BinaryOperator::PLUS,
                std::make_unique<ExpectedBinaryExpr>(
                    compiler::BinaryOperator::PLUS,
                    std::make_unique<ExpectedIntegerLiteral>(1),
                    std::make_unique<ExpectedIntegerLiteral>(2)
                ),
                std::make_unique<ExpectedIntegerLiteral>(3)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_BINARY, ASSOCIATIVE_LOGICAL_AND) {
    const auto testCode = R"(
        int x = 1 && 2 && 3;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedBinaryExpr>(
                compiler::BinaryOperator::LOGICAL_AND,
                std::make_unique<ExpectedBinaryExpr>(
                    compiler::BinaryOperator::LOGICAL_AND,
                    std::make_unique<ExpectedIntegerLiteral>(1),
                    std::make_unique<ExpectedIntegerLiteral>(2)
                ),
                std::make_unique<ExpectedIntegerLiteral>(3)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_BLOCK, INVALID_MISSING_BINARY_OPERATOR) {
    const auto testCode = R"(
        int x = 5 5;
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_BLOCK, INVALID_MISSING_RIGHT_EXPRESSION) {
    const auto testCode = R"(
        int x = 5 + ;
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}