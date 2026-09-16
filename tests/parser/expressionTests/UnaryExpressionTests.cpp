#include <gtest/gtest.h>
#include "../ParserTestUtils.h"

using namespace parserTest;

TEST(EXPR_UNARY, INT_PLUS) {
    const auto testCode = R"(
        int x = +5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
                std::make_unique<ExpectedUnaryExpr>(
                compiler::UnaryOperator::PLUS,
                std::make_unique<ExpectedExprPostfix>(
                    std::make_unique<ExpectedIntegerLiteral>(
                        5
                    ),
                    noExpectedIndices
                )
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_UNARY, VARIABLE_PLUS) {
    const auto testCode = R"(
        int x = +y;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedIndex>> expectedIndices;

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedUnaryExpr>(
                compiler::UnaryOperator::PLUS,
                std::make_unique<ExpectedExprPostfix>(
                    std::make_unique<ExpectedExprIdentifier>(
                        std::make_unique<ExpectedIdentifier>("y")
                    ),
                    noExpectedIndices
                )
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_UNARY, INT_MINUS) {
    const auto testCode = R"(
        int x = -5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedUnaryExpr>(
                compiler::UnaryOperator::MINUS,
                std::make_unique<ExpectedExprPostfix>(
                    std::make_unique<ExpectedIntegerLiteral>(5),
                    noExpectedIndices
                )
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_UNARY, INT_MINUS_PAREN) {
    const auto testCode = R"(
        int x = -(5);
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedUnaryExpr>(
                compiler::UnaryOperator::MINUS,
                std::make_unique<ExpectedExprPostfix>(
                    std::make_unique<ExpectedExprPostfix>(
                        std::make_unique<ExpectedIntegerLiteral>(5),
                        noExpectedIndices
                    ),
                    noExpectedIndices
                )
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_UNARY, VARIABLE_MINUS) {
    const auto testCode = R"(
        int x = -y;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedIndex>> expectedIndices;

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
             std::make_unique<ExpectedUnaryExpr>(
                 compiler::UnaryOperator::MINUS,
                 std::make_unique<ExpectedExprPostfix>(
                     std::make_unique<ExpectedExprIdentifier>(
                         std::make_unique<ExpectedIdentifier>("y")
                     ),
                     noExpectedIndices
                 )
             )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_UNARY, INT_LOGICAL_NOT) {
    const auto testCode = R"(
        int x = !5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedUnaryExpr>(
                compiler::UnaryOperator::LOGICAL_NOT,
                std::make_unique<ExpectedExprPostfix>(
                    std::make_unique<ExpectedIntegerLiteral>(5),
                    noExpectedIndices
                )
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_UNARY, VARIABLE_LOGICAL_NOT) {
    const auto testCode = R"(
        int x = !y;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedIndex>> expectedIndices;

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedUnaryExpr>(
                compiler::UnaryOperator::LOGICAL_NOT,
                std::make_unique<ExpectedExprPostfix>(
                    std::make_unique<ExpectedExprIdentifier>(
                        std::make_unique<ExpectedIdentifier>("y")
                    ),
                    noExpectedIndices
                )
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_UNARY, INT_NESTED_LOGICAL_NOT) {
    const auto testCode = R"(
        int x = !!5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedUnaryExpr>(
                compiler::UnaryOperator::LOGICAL_NOT,
                std::make_unique<ExpectedUnaryExpr>(
                    compiler::UnaryOperator::LOGICAL_NOT,
                    std::make_unique<ExpectedExprPostfix>(
                        std::make_unique<ExpectedIntegerLiteral>(5),
                        noExpectedIndices
                    )
                )
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_UNARY, INT_NESTED_SIGN) {
    const auto testCode = R"(
        int x = -+-5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedUnaryExpr>(
                compiler::UnaryOperator::MINUS,
                std::make_unique<ExpectedUnaryExpr>(
                    compiler::UnaryOperator::PLUS,
                    std::make_unique<ExpectedUnaryExpr>(
                        compiler::UnaryOperator::MINUS,
                        std::make_unique<ExpectedExprPostfix>(
                            std::make_unique<ExpectedIntegerLiteral>(5),
                            noExpectedIndices
                        )
                    )
                )
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_UNARY, INVALID_MISSING_EXPR) {
    const auto testCode = R"(
        int x = !;
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(EXPR_UNARY, INVALID_MISSING_LBR) {
    const auto testCode = R"(
        int x = (-5;
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(EXPR_UNARY, INVALID_MISSING_RBR) {
    const auto testCode = R"(
        int x = -5);
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}