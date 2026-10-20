#include <gtest/gtest.h>
#include "../ParserTestUtils.h"

using namespace parserTest;

TEST(EXPR_PRECEDENCE, CAST_UNARY_BEFORE_MULTIPLICATIVE) {
    const auto testCode = R"(
        int x = (int) 1.8f * 2;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprBinary>(
                compiler::BinaryOperator::MULTIPLY,
                std::make_unique<ExpectedExprCast>(
                    std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
                    std::make_unique<ExpectedExprFloatLiteral>(1.8f)
                ),
                std::make_unique<ExpectedExprIntegerLiteral>(2)
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_PRECEDENCE, UNARY_BEFORE_MULTIPLICATIVE) {
    const auto testCode = R"(
        int x = -1 * 2;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprBinary>(
                compiler::BinaryOperator::MULTIPLY,
                std::make_unique<ExpectedExprUnary>(
                    compiler::UnaryOperator::MINUS,
                    std::make_unique<ExpectedExprIntegerLiteral>(1)
                ),
                std::make_unique<ExpectedExprIntegerLiteral>(2)
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_PRECEDENCE, PARETHESES_BEFORE_MULTIPLICATIVE) {
    const auto testCode = R"(
        int x = 1 * (2 + 3);
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprBinary>(
                compiler::BinaryOperator::MULTIPLY,
                std::make_unique<ExpectedExprIntegerLiteral>(1),
                std::make_unique<ExpectedExprBinary>(
                    compiler::BinaryOperator::ADD,
                    std::make_unique<ExpectedExprIntegerLiteral>(2),
                    std::make_unique<ExpectedExprIntegerLiteral>(3)
                )
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_PRECEDENCE, MULTIPLICATIVE_BEFORE_ADDITIVE_1) {
    const auto testCode = R"(
        int x = 1 + 2 * 3;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprBinary>(
                compiler::BinaryOperator::ADD,
                std::make_unique<ExpectedExprIntegerLiteral>(1),
                std::make_unique<ExpectedExprBinary>(
                    compiler::BinaryOperator::MULTIPLY,
                    std::make_unique<ExpectedExprIntegerLiteral>(2),
                    std::make_unique<ExpectedExprIntegerLiteral>(3)
                )
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_PRECEDENCE, MULTIPLICATIVE_BEFORE_ADDITIVE_2) {
    const auto testCode = R"(
        int x = 1 * 2 + 3;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprBinary>(
                compiler::BinaryOperator::ADD,
                std::make_unique<ExpectedExprBinary>(
                    compiler::BinaryOperator::MULTIPLY,
                    std::make_unique<ExpectedExprIntegerLiteral>(1),
                    std::make_unique<ExpectedExprIntegerLiteral>(2)
                ),
                std::make_unique<ExpectedExprIntegerLiteral>(3)
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_PRECEDENCE, ADDITIVE_BEFORE_SHIFT) {
    const auto testCode = R"(
        int x = 1 << 2 + 3;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprBinary>(
                compiler::BinaryOperator::LEFT_SHIFT,
                std::make_unique<ExpectedExprIntegerLiteral>(1),
                std::make_unique<ExpectedExprBinary>(
                    compiler::BinaryOperator::ADD,
                    std::make_unique<ExpectedExprIntegerLiteral>(2),
                    std::make_unique<ExpectedExprIntegerLiteral>(3)
                )
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_PRECEDENCE, SHIFT_BEFORE_COMPARISON) {
    const auto testCode = R"(
        int x = 1 < 2 << 3;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprBinary>(
                compiler::BinaryOperator::LESS_THAN,
                std::make_unique<ExpectedExprIntegerLiteral>(1),
                std::make_unique<ExpectedExprBinary>(
                    compiler::BinaryOperator::LEFT_SHIFT,
                    std::make_unique<ExpectedExprIntegerLiteral>(2),
                    std::make_unique<ExpectedExprIntegerLiteral>(3)
                )
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_PRECEDENCE, COMPARISON_BEFORE_EQUALITY) {
    const auto testCode = R"(
        int x = 1 == 2 < 3;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprBinary>(
                compiler::BinaryOperator::EQUAL_EQUAL,
                std::make_unique<ExpectedExprIntegerLiteral>(1),
                std::make_unique<ExpectedExprBinary>(
                    compiler::BinaryOperator::LESS_THAN,
                    std::make_unique<ExpectedExprIntegerLiteral>(2),
                    std::make_unique<ExpectedExprIntegerLiteral>(3)
                )
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_PRECEDENCE, EQUALITY_BEFORE_BITWISE_AND) {
    const auto testCode = R"(
        int x = 1 & 2 == 3;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprBinary>(
                compiler::BinaryOperator::BITWISE_AND,
                std::make_unique<ExpectedExprIntegerLiteral>(1),
                std::make_unique<ExpectedExprBinary>(
                    compiler::BinaryOperator::EQUAL_EQUAL,
                    std::make_unique<ExpectedExprIntegerLiteral>(2),
                    std::make_unique<ExpectedExprIntegerLiteral>(3)
                )
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_PRECEDENCE, BITWISE_AND_BEFORE_BITWISE_XOR) {
    const auto testCode = R"(
        int x = 1 ^ 2 & 3;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprBinary>(
                compiler::BinaryOperator::BITWISE_XOR,
                std::make_unique<ExpectedExprIntegerLiteral>(1),
                std::make_unique<ExpectedExprBinary>(
                    compiler::BinaryOperator::BITWISE_AND,
                    std::make_unique<ExpectedExprIntegerLiteral>(2),
                    std::make_unique<ExpectedExprIntegerLiteral>(3)
                )
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_PRECEDENCE, BITWISE_XOR_BEFORE_BITWISE_OR) {
    const auto testCode = R"(
        int x = 1 | 2 ^ 3;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprBinary>(
                compiler::BinaryOperator::BITWISE_OR,
                std::make_unique<ExpectedExprIntegerLiteral>(1),
                std::make_unique<ExpectedExprBinary>(
                    compiler::BinaryOperator::BITWISE_XOR,
                    std::make_unique<ExpectedExprIntegerLiteral>(2),
                    std::make_unique<ExpectedExprIntegerLiteral>(3)
                )
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_PRECEDENCE, BITWISE_OR_BEFORE_LOGICAL_AND) {
    const auto testCode = R"(
        int x = 1 && 2 | 3;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprBinary>(
                compiler::BinaryOperator::LOGICAL_AND,
                std::make_unique<ExpectedExprIntegerLiteral>(1),
                std::make_unique<ExpectedExprBinary>(
                    compiler::BinaryOperator::BITWISE_OR,
                    std::make_unique<ExpectedExprIntegerLiteral>(2),
                    std::make_unique<ExpectedExprIntegerLiteral>(3)
                )
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_PRECEDENCE, LOGICAL_AND_BEFORE_LOGICAL_OR) {
    const auto testCode = R"(
        int x = 1 || 2 && 3;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprBinary>(
                compiler::BinaryOperator::LOGICAL_OR,
                std::make_unique<ExpectedExprIntegerLiteral>(1),
                std::make_unique<ExpectedExprBinary>(
                    compiler::BinaryOperator::LOGICAL_AND,
                    std::make_unique<ExpectedExprIntegerLiteral>(2),
                    std::make_unique<ExpectedExprIntegerLiteral>(3)
                )
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_PRECEDENCE, LOGICAL_OR_BEFORE_TERNARY) {
    const auto testCode = R"(
        int x = true || false ? 1  : 2;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprTernary>(
                std::make_unique<ExpectedExprBinary>(
                    compiler::BinaryOperator::LOGICAL_OR,
                    std::make_unique<ExpectedExprBoolLiteral>(true),
                    std::make_unique<ExpectedExprBoolLiteral>(false)
                ),
                std::make_unique<ExpectedExprIntegerLiteral>(1),
                std::make_unique<ExpectedExprIntegerLiteral>(2)
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}