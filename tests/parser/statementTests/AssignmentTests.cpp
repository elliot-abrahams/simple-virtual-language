#include <gtest/gtest.h>
#include "../ParserTestUtils.h"

using namespace parserTest;

TEST(STM_ASSIGN, INT_LIT) {
    const auto testCode = R"(
        x = 5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<ExpectedIntegerLiteral>(
                5
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, FLOAT_LIT) {
    const auto testCode = R"(
        x = 5.5f;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<ExpectedFloatLiteral>(
                5.5f
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, BOOL_LIT) {
    const auto testCode = R"(
        x = false;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<ExpectedBoolLiteral>(
                false
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, IDENTIFIER_EXPR) {
    const auto testCode = R"(
        x = y;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<ExpectedExprIdentifier>(
                std::make_unique<ExpectedIdentifier>("y")
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, UNARY_EXPR) {
    const auto testCode = R"(
        x = -5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<ExpectedUnaryExpr>(
                compiler::UnaryOperator::MINUS,
                std::make_unique<ExpectedIntegerLiteral>(
                    5
                )
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, BINARY_EXPR) {
    const auto testCode = R"(
        x = 1 + 2;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::EQUAL,
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

TEST(STM_ASSIGN, FUNCTION_CALL_EXPR) {
    const auto testCode = R"(
        x = foo();
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedExpr>> expectedArguments;
    expectedStatements.push_back(
        std::make_unique<ExpectedAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<ExpectedFunctionCallExpr>(
                "foo",
                std::move(expectedArguments)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, PAREN) {
    const auto testCode = R"(
        x = (5);
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedExpr>> expectedArguments;
    expectedStatements.push_back(
        std::make_unique<ExpectedAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<ExpectedIntegerLiteral>(5)
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, INVALID_MISSING_IDENTIFIER) {
    const auto testCode = R"(
        = 5;
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_ASSIGN, INVALID_MISSING_ASSIGNMENT_OPERATOR) {
    const auto testCode = R"(
        x 5;
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_ASSIGN, INVALID_MISSING_EXPR) {
    const auto testCode = R"(
        x = ;
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_ASSIGN, INVALID_MISSING_SEMICOLON) {
    const auto testCode = R"(
        x = 5
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_ASSIGN, INVALID_LITERAL_AS_VAR_ACCESS) {
    const auto testCode = R"(
        5 = x;
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}