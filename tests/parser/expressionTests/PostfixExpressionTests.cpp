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
            std::make_unique<Type>(compiler::Type::INT, 0),
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
    std::vector<std::unique_ptr<ExpectedIndex>> indices;

    indices.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(1)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedExprIdentifier>(
                    std::make_unique<ExpectedIdentifier>("y")
                ),
                indices,
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
    std::vector<std::unique_ptr<ExpectedIndex>> indices;

    indices.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(1)
        )
    );
    indices.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(5)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedExprIdentifier>(
                    std::make_unique<ExpectedIdentifier>("y")
                ),
                indices,
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
    std::vector<std::unique_ptr<ExpectedIndex>> indices;
    std::vector<std::unique_ptr<ExpectedExpr>> expectedArguments;

    indices.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(1)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedFunctionCallExpr>(
                    "foo",
                    std::move(expectedArguments)
                ),
                indices,
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
    std::vector<std::unique_ptr<ExpectedIndex>> indices;
    std::vector<std::unique_ptr<ExpectedIndex>> arrayDimension;
    std::vector<ArrayInitialiserElement> arrayInitialiserElements;
    std::unique_ptr<ExpectedArrayInitialiser> expectedArrayInitialiser = std::make_unique<ExpectedArrayInitialiser>(std::move(arrayInitialiserElements));

    indices.push_back(
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
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedExprNew>(
                    std::make_unique<Type>(compiler::Type::INT, 1),
                    std::move(arrayDimension),
                    std::move(expectedArrayInitialiser)
                ),
                indices,
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
    std::vector<std::unique_ptr<ExpectedIndex>> indices;

    indices.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedIntegerLiteral>(1)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedIntegerLiteral>(1),
                indices,
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
            std::make_unique<Type>(compiler::Type::INT, 0),
            "y",
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

TEST(EXPR_POSTFIX, DECREMENT) {
    const auto testCode = R"(
        int y = x--;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "y",
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

TEST(EXPR_POSTFIX, IDENTIFIER_WITH_ONE_INDEX_INCREMENT) {
    const auto testCode = R"(
        int x = y[1]++;
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
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedExprIdentifier>(
                    std::make_unique<ExpectedIdentifier>("y")
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
