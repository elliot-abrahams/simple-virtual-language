#include <gtest/gtest.h>
#include "../ParserTestUtils.h"

using namespace parserTest;

TEST(EXPR_POSTFIX, IDENTIFIER) {
    const auto testCode = R"(
        int x = y;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIdentifier>(
                std::make_unique<ExpectedIdentifier>("y")
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_POSTFIX, IDENTIFIER_WITH_ONE_INDEX) {
    const auto testCode = R"(
        int x = y[1];
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<PostfixOperator> expectedPostfixOperators;

    expectedPostfixOperators.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(1)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedExprIdentifier>(
                    std::make_unique<ExpectedIdentifier>("y")
                ),
                expectedPostfixOperators,
                std::nullopt
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_POSTFIX, IDENTIFIER_WITH_TWO_INDICES) {
    const auto testCode = R"(
        int x = y[1][5];
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<PostfixOperator> expectedPostfixOperators;

    expectedPostfixOperators.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(1)
        )
    );
    expectedPostfixOperators.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(5)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedExprIdentifier>(
                    std::make_unique<ExpectedIdentifier>("y")
                ),
                expectedPostfixOperators,
                std::nullopt
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_POSTFIX, FUNCTION_CALL_EXPR_WITH_ONE_INDEX) {
    const auto testCode = R"(
        int x = foo()[1];
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<PostfixOperator> expectedPostfixOperators;
    std::vector<std::unique_ptr<ExpectedExpr>> expectedArguments;

    expectedPostfixOperators.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(1)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedFunctionCallExpr>(
                    "foo",
                    std::move(expectedArguments)
                ),
                expectedPostfixOperators,
                std::nullopt
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_POSTFIX, NEW_EXPRESSION_WITH_ONE_INDEX) {
    const auto testCode = R"(
        int x = new int[3]{}[1];
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<PostfixOperator> expectedPostfixOperators;
    std::vector<std::unique_ptr<ExpectedIndex>> arrayDimension;
    std::vector<ArrayInitialiserElement> arrayInitialiserElements;
    std::unique_ptr<ExpectedArrayInitialiser> expectedArrayInitialiser = std::make_unique<ExpectedArrayInitialiser>(std::move(arrayInitialiserElements));

    expectedPostfixOperators.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(1)
        )
    );

    arrayDimension.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(3)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedExprNew>(
                    std::make_unique<Type>(compiler::INT_TYPE_ID, 1),
                    std::move(arrayDimension),
                    std::move(expectedArrayInitialiser)
                ),
                expectedPostfixOperators,
                std::nullopt
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_POSTFIX, PAREN_WITH_ONE_INDEX) {
    const auto testCode = R"(
        int x = (1)[1];
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<PostfixOperator> expectedPostfixOperators;

    expectedPostfixOperators.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(1)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedIntegerLiteral>(1),
                expectedPostfixOperators,
                std::nullopt
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_POSTFIX, INCREMENT) {
    const auto testCode = R"(
        int y = x++;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "y",
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedExprIdentifier>(
                    std::make_unique<ExpectedIdentifier>("x")
                ),
                noExpectedPostfixOperators,
                compiler::UnaryOperator::INCREMENT
            )
        )
    );

    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_POSTFIX, DECREMENT) {
    const auto testCode = R"(
        int y = x--;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "y",
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedExprIdentifier>(
                    std::make_unique<ExpectedIdentifier>("x")
                ),
                noExpectedPostfixOperators,
                compiler::UnaryOperator::DECREMENT
            )
        )
    );

    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_POSTFIX, IDENTIFIER_WITH_ONE_INDEX_INCREMENT) {
    const auto testCode = R"(
        int x = y[1]++;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<PostfixOperator> postfixOperators;

    postfixOperators.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(1)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedExprIdentifier>(
                    std::make_unique<ExpectedIdentifier>("y")
                ),
                postfixOperators,
                compiler::UnaryOperator::INCREMENT
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_POSTFIX, IDENTIFIER_WITH_FIELD_ACCESS) {
    const auto testCode = R"(
        int x = y.field;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<PostfixOperator> postfixOperators;

    postfixOperators.push_back(
        std::make_unique<ExpectedFieldAccess>(
            "field"
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedExprIdentifier>(
                    std::make_unique<ExpectedIdentifier>("y")
                ),
                postfixOperators,
                std::nullopt
            )
        )
    );

    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_POSTFIX, IDENTIFIER_WITH_MULTIPLE_INDICES_AND_FIELD_ACCESS_AND_INCREMENT) {
    const auto testCode = R"(
        int x = y[1].field[2].other_field++;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<PostfixOperator> postfixOperators;

    postfixOperators.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(1)
        )
    );

    postfixOperators.push_back(
        std::make_unique<ExpectedFieldAccess>(
            "field"
        )
    );

    postfixOperators.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(2)
        )
    );

    postfixOperators.push_back(
        std::make_unique<ExpectedFieldAccess>(
            "other_field"
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedExprIdentifier>(
                    std::make_unique<ExpectedIdentifier>("y")
                ),
                postfixOperators,
                compiler::UnaryOperator::INCREMENT
            )
        )
    );

    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_POSTFIX, INVALID_MISSING_IDENTIFIER) {
    ASSERT_THROW(PARSE("int x = ;"), SyntaxError);
}

TEST(EXPR_POSTFIX, INVALID_MISSING_IDENTIFIER_WITH_INDEX) {
    ASSERT_THROW(PARSE("int x = [1];"), SyntaxError);
}

TEST(EXPR_POSTFIX, INVALID_MISSING_LSQBR) {
    ASSERT_THROW(PARSE("int x = y 1];"), SyntaxError);
}

TEST(EXPR_POSTFIX, INVALID_MISSING_EXPR) {
    ASSERT_THROW(PARSE("int x = y [];"), SyntaxError);
}

TEST(EXPR_POSTFIX, INVALID_MISSING_RSQBR) {
    ASSERT_THROW(PARSE("int x = y [1;"), SyntaxError);
}

TEST(EXPR_POSTFIX, INVALID_MISSING_SECOND_LSQBR) {
    ASSERT_THROW(PARSE("int x = y [1] 2];"), SyntaxError);
}

TEST(EXPR_POSTFIX, INVALID_MISSING_SECOND_EXPR) {
    ASSERT_THROW(PARSE("int x = y [1] [];"), SyntaxError);
}

TEST(EXPR_POSTFIX, INVALID_MISSING_SECOND_RSQBR) {
    ASSERT_THROW(PARSE("int x = y [1] [2;"), SyntaxError);
}

TEST(EXPR_POSTFIX, INVALID_ORDER_OF_POSTFIX_OPERATORS_ONE) {
    ASSERT_THROW(PARSE("int x = y++.field"), SyntaxError);
}

TEST(EXPR_POSTFIX, INVALID_ORDER_OF_POSTFIX_OPERATORS_TWO) {
    ASSERT_THROW(PARSE("int x = y++[1]"), SyntaxError);
}
