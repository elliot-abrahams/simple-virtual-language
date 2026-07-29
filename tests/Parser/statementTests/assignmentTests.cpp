#include <gtest/gtest.h>
#include "../parserTestUtils.h"

TEST(STM_ASSIGN, INT_LIT) {
    const auto testCode = R"(
        x = 5;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedAssignment>(
            std::make_unique<parserTest::ExpectedVarAccess>(
                "x"
            ),
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<parserTest::ExpectedIntegerLiteral>(
                5
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, FLOAT_LIT) {
    const auto testCode = R"(
        x = 5.5f;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedAssignment>(
            std::make_unique<parserTest::ExpectedVarAccess>(
                "x"
            ),
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<parserTest::ExpectedFloatLiteral>(
                5.5f
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, BOOL_LIT) {
    const auto testCode = R"(
        x = false;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedAssignment>(
            std::make_unique<parserTest::ExpectedVarAccess>(
                "x"
            ),
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<parserTest::ExpectedBoolLiteral>(
                false
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, IDENTIFIER_EXPR) {
    const auto testCode = R"(
        x = y;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedAssignment>(
            std::make_unique<parserTest::ExpectedVarAccess>(
                "x"
            ),
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<parserTest::ExpectedExprIdentifier>(
                "y"
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, UNARY_EXPR) {
    const auto testCode = R"(
        x = -5;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedAssignment>(
            std::make_unique<parserTest::ExpectedVarAccess>(
                "x"
            ),
            compiler::AssignmentOperator::EQUAL,
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

TEST(STM_ASSIGN, BINARY_EXPR) {
    const auto testCode = R"(
        x = 1 + 2;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedAssignment>(
            std::make_unique<parserTest::ExpectedVarAccess>(
                "x"
            ),
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<parserTest::ExpectedBinaryExpr>(
                compiler::BinaryOperator::PLUS,
                std::make_unique<parserTest::ExpectedIntegerLiteral>(
                    1
                ),
                std::make_unique<parserTest::ExpectedIntegerLiteral>(
                    2
                )
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, FUNCTION_CALL_EXPR) {
    const auto testCode = R"(
        x = foo();
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedExpr>> expectedArguments;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedAssignment>(
            std::make_unique<parserTest::ExpectedVarAccess>(
                "x"
            ),
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<parserTest::ExpectedFunctionCallExpr>(
                "foo",
                std::move(expectedArguments)
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, PAREN) {
    const auto testCode = R"(
        x = (5);
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedExpr>> expectedArguments;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedAssignment>(
            std::make_unique<parserTest::ExpectedVarAccess>(
                "x"
            ),
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<parserTest::ExpectedIntegerLiteral>(
                5
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, INVALID_MISSING_IDENTIFIER) {
    const auto testCode = R"(
        = 5;
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_ASSIGN, INVALID_MISSING_ASSIGNMENT_OPERATOR) {
    const auto testCode = R"(
        x 5;
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_ASSIGN, INVALID_MISSING_EXPR) {
    const auto testCode = R"(
        x = ;
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_ASSIGN, INVALID_MISSING_SEMICOLON) {
    const auto testCode = R"(
        x = 5
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_ASSIGN, INVALID_LITERAL_AS_VAR_ACCESS) {
    const auto testCode = R"(
        5 = x;
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}