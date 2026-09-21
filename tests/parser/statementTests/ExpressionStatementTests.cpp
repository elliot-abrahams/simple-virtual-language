#include <gtest/gtest.h>
#include "../ParserTestUtils.h"

using namespace parserTest;

TEST(EXPR_STM, BINARY_EXPR) {
    const auto testCode = R"(
        1 + 2;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
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

TEST(EXPR_STM, UNARY_EXPR_VARIABLE_PLUS_SIGN) {
    const auto testCode = R"(
        +x;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
            std::make_unique<ExpectedUnaryExpr>(
                compiler::UnaryOperator::PLUS,
                std::make_unique<ExpectedExprIdentifier>(
                    std::make_unique<ExpectedIdentifier>("x")
                )
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_STM, UNARY_EXPR_VARIABLE_MINUS_SIGN) {
    const auto testCode = R"(
        -x;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
            std::make_unique<ExpectedUnaryExpr>(
                compiler::UnaryOperator::MINUS,
                std::make_unique<ExpectedExprIdentifier>(
                    std::make_unique<ExpectedIdentifier>("x")
                )
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_STM, UNARY_EXPR_VARIABLE_LOGICAL_NOT) {
    const auto testCode = R"(
        !x;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
            std::make_unique<ExpectedUnaryExpr>(
                compiler::UnaryOperator::LOGICAL_NOT,
                std::make_unique<ExpectedExprIdentifier>(
                    std::make_unique<ExpectedIdentifier>("x")
                )
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_STM, UNARY_EXPR_VARIABLE_INCREMENT) {
    const auto testCode = R"(
        ++x;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
            std::make_unique<ExpectedUnaryExpr>(
                compiler::UnaryOperator::INCREMENT,
                std::make_unique<ExpectedExprIdentifier>(
                    std::make_unique<ExpectedIdentifier>("x")
                )
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_STM, UNARY_EXPR_VARIABLE_DECREMENT) {
    const auto testCode = R"(
        --x;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
            std::make_unique<ExpectedUnaryExpr>(
                compiler::UnaryOperator::DECREMENT,
                std::make_unique<ExpectedExprIdentifier>(
                    std::make_unique<ExpectedIdentifier>("x")
                )
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_STM, POSTFIX_EXPR_VARIABLE_INCREMENT) {
    const auto testCode = R"(
        x++;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedExprIdentifier>(
                    std::make_unique<ExpectedIdentifier>("x")
                ),
                noExpectedIndices,
                compiler::UnaryOperator::INCREMENT
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_STM, POSTFIX_EXPR_VARIABLE_DECREMENT) {
    const auto testCode = R"(
        x--;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedExprIdentifier>(
                    std::make_unique<ExpectedIdentifier>("x")
                ),
                noExpectedIndices,
                compiler::UnaryOperator::DECREMENT
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_STM, POSTFIX_EXPR_VARIABLE_INCREMENT_INDEX) {
    const auto testCode = R"(
        x[1]++;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedIndex>> indices;

    indices.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(1)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedExprIdentifier>(
                    std::make_unique<ExpectedIdentifier>("x")
                ),
                indices,
                compiler::UnaryOperator::INCREMENT
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_STM, POSTFIX_EXPR_VARIABLE_DECREMENT_INDEX) {
    const auto testCode = R"(
        x[1]--;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedIndex>> indices;

    indices.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(1)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedExprIdentifier>(
                    std::make_unique<ExpectedIdentifier>("x")
                ),
                indices,
                compiler::UnaryOperator::DECREMENT
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_STM, POSTFIX_EXPR_FUNCTION_CALL_INCREMENT_INDEX) {
    const auto testCode = R"(
        foo()[1]++;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedIndex>> indices;
    std::vector<std::unique_ptr<ExpectedExpr>> expectedArguments;

    indices.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(1)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedFunctionCallExpr>(
                    "foo",
                    std::move(expectedArguments)
                ),
                indices,
                compiler::UnaryOperator::INCREMENT
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_STM, POSTFIX_EXPR_FUNCTION_CALL_DECREMENT_INDEX) {
    const auto testCode = R"(
        foo()[1]--;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedIndex>> indices;
    std::vector<std::unique_ptr<ExpectedExpr>> expectedArguments;

    indices.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(1)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedFunctionCallExpr>(
                    "foo",
                    std::move(expectedArguments)
                ),
                indices,
                compiler::UnaryOperator::DECREMENT
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_STM, POSTFIX_EXPR_FUNCTION_CALL_INCREMENT_INDEX_WITH_INDEXED_ARGUMENT) {
    const auto testCode = R"(
        foo(x[0][0]++)[1][1]++;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedIndex>> indices;
    std::vector<std::unique_ptr<ExpectedIndex>> indices2;
    std::vector<std::unique_ptr<ExpectedExpr>> expectedArguments;

    indices2.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(0)
        )
    );
    indices2.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(0)
        )
    );

    expectedArguments.push_back(
        std::make_unique<ExpectedExprPostfix>(
            std::make_unique<ExpectedExprIdentifier>(
                std::make_unique<ExpectedIdentifier>("x")
            ),
            indices2,
            compiler::UnaryOperator::INCREMENT
        )
    );

    indices.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(1)
        )
    );
    indices.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(1)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedFunctionCallExpr>(
                    "foo",
                    std::move(expectedArguments)
                ),
                indices,
                compiler::UnaryOperator::INCREMENT
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_STM, POSTFIX_EXPR_FUNCTION_CALL_DECREMENT_INDEX_WITH_INDEXED_ARGUMENT) {
    const auto testCode = R"(
        foo(x[0][0]--)[1][1]--;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedIndex>> indices;
    std::vector<std::unique_ptr<ExpectedIndex>> indices2;
    std::vector<std::unique_ptr<ExpectedExpr>> expectedArguments;

    indices2.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(0)
        )
    );
    indices2.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(0)
        )
    );

    expectedArguments.push_back(
        std::make_unique<ExpectedExprPostfix>(
            std::make_unique<ExpectedExprIdentifier>(
                std::make_unique<ExpectedIdentifier>("x")
            ),
            indices2,
            compiler::UnaryOperator::DECREMENT
        )
    );

    indices.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(1)
        )
    );
    indices.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(1)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedFunctionCallExpr>(
                    "foo",
                    std::move(expectedArguments)
                ),
                indices,
                compiler::UnaryOperator::DECREMENT
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_STM, PRIMARY_EXPR_FUNCTION_CALL) {
    const auto testCode = R"(
        foo();
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedExpr>> expectedArguments;

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
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

TEST(EXPR_STM, PRIMARY_EXPR_PAREN_FUNCTION_CALL) {
    const auto testCode = R"(
        (foo());
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedExpr>> expectedArguments;

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
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

TEST(EXPR_STM, PRIMARY_EXPR_FUNCTION_CALL_WITH_ONE_ARGUMENT) {
    const auto testCode = R"(
        foo(5);
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedExpr>> expectedArguments;

    expectedArguments.push_back(
        std::make_unique<ExpectedIntegerLiteral>(5)
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
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

TEST(EXPR_STM, PRIMARY_EXPR_PAREN) {
    const auto testCode = R"(
        (x);
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
            std::make_unique<ExpectedExprIdentifier>(
                std::make_unique<ExpectedIdentifier>("x")
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_STM, PRIMARY_EXPR_INT_LIT) {
    const auto testCode = R"(
        5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
            std::make_unique<ExpectedIntegerLiteral>(5)
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_STM, PRIMARY_EXPR_FLOAT_LIT) {
    const auto testCode = R"(
        5.5f;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
            std::make_unique<ExpectedFloatLiteral>(5.5f)
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_STM, PRIMARY_EXPR_BOOL_LIT) {
    const auto testCode = R"(
        true;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
            std::make_unique<ExpectedBoolLiteral>(true)
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_STM, PRIMARY_EXPR_NEW) {
    const auto testCode = R"(
        new int[1];
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedIndex>> arrayDimensions;

    arrayDimensions.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(1)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
            std::make_unique<ExpectedExprNew>(
                std::make_unique<Type>(compiler::Type::INT, 1),
                std::move(arrayDimensions),
                nullptr
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_STM, CAST_EXPR) {
    const auto testCode = R"(
        (int) 5.5f;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedExpressionStatement>(
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