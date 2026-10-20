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
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<ExpectedExprIntegerLiteral>(
                5
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, FLOAT_LIT) {
    const auto testCode = R"(
        x = 5.5f;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<ExpectedExprFloatLiteral>(
                5.5f
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, BOOL_LIT) {
    const auto testCode = R"(
        x = false;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<ExpectedExprBoolLiteral>(
                false
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, CHAR_LIT) {
    const auto testCode = R"(
        x = 'a';
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<ExpectedExprCharLiteral>(
                'a'
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, IDENTIFIER_EXPR) {
    const auto testCode = R"(
        x = y;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmAssignment>(
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
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, UNARY_EXPR) {
    const auto testCode = R"(
        x = -5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<ExpectedExprUnary>(
                compiler::UnaryOperator::MINUS,
                std::make_unique<ExpectedExprIntegerLiteral>(
                    5
                )
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, BINARY_EXPR) {
    const auto testCode = R"(
        x = 1 + 2;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<ExpectedExprBinary>(
                compiler::BinaryOperator::ADD,
                std::make_unique<ExpectedExprIntegerLiteral>(1),
                std::make_unique<ExpectedExprIntegerLiteral>(2)
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
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
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<ExpectedExprFunctionCall>(
                "foo",
                std::move(expectedArguments)
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
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
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<ExpectedExprIntegerLiteral>(5)
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, PLUS_EQUAL) {
    const auto testCode = R"(
        x += 5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::PLUS_EQUAL,
            std::make_unique<ExpectedExprIntegerLiteral>(
                5
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, MINUE_EQUAL) {
    const auto testCode = R"(
        x -= 5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::MINUS_EQUAL,
            std::make_unique<ExpectedExprIntegerLiteral>(
                5
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, MULTIPLY_EQUAL) {
    const auto testCode = R"(
        x *= 5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::MULTIPLY_EQUAL,
            std::make_unique<ExpectedExprIntegerLiteral>(
                5
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, DIVIDE_EQUAL) {
    const auto testCode = R"(
        x /= 5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::DIVIDE_EQUAL,
            std::make_unique<ExpectedExprIntegerLiteral>(
                5
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, INTEGER_DIVIDE_EQUAL) {
    const auto testCode = R"(
        x //= 5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::INTEGER_DIVIDE_EQUAL,
            std::make_unique<ExpectedExprIntegerLiteral>(
                5
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, MODULO_EQUAL) {
    const auto testCode = R"(
        x %= 5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::MODULO_EQUAL,
            std::make_unique<ExpectedExprIntegerLiteral>(
                5
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, BITWISE_OR_EQUAL) {
    const auto testCode = R"(
        x |= 5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::BITWISE_OR_EQUAL,
            std::make_unique<ExpectedExprIntegerLiteral>(
                5
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, BITWISE_XOR_EQUAL) {
    const auto testCode = R"(
        x ^= 5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::BITWISE_XOR_EQUAL,
            std::make_unique<ExpectedExprIntegerLiteral>(
                5
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, BITWISE_AND_EQUAL) {
    const auto testCode = R"(
        x &= 5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::BITWISE_AND_EQUAL,
            std::make_unique<ExpectedExprIntegerLiteral>(
                5
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, LEFT_SHIFT_EQUAL) {
    const auto testCode = R"(
        x <<= 5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::LEFT_SHIFT_EQUAL,
            std::make_unique<ExpectedExprIntegerLiteral>(
                5
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, ARITHMETIC_RIGHT_SHIFT_EQUAL) {
    const auto testCode = R"(
        x >>= 5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::ARITHMETIC_RIGHT_SHIFT_EQUAL,
            std::make_unique<ExpectedExprIntegerLiteral>(
                5
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, LOGICAL_RIGHT_SHIFT_EQUAL) {
    const auto testCode = R"(
        x >>>= 5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::LOGICAL_RIGHT_SHIFT_EQUAL,
            std::make_unique<ExpectedExprIntegerLiteral>(
                5
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, LOGICAL_OR_EQUAL) {
    const auto testCode = R"(
        x ||= 5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::LOGICAL_OR_EQUAL,
            std::make_unique<ExpectedExprIntegerLiteral>(
                5
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_ASSIGN, LOGICAL_AND_EQUAL) {
    const auto testCode = R"(
        x &&= 5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>(
                "x"
            ),
            noExpectedIndices,
            compiler::AssignmentOperator::LOGICAL_AND_EQUAL,
            std::make_unique<ExpectedExprIntegerLiteral>(
                5
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
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