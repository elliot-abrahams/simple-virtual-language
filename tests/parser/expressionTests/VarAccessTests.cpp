#include <gtest/gtest.h>
#include "../ParserTestUtils.h"

using namespace parserTest;

TEST(EXPR_VAR_ACCESS, IDENTIFIER) {
    const auto testCode = R"(
        int x = y;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedIndex>> indices;

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            std::make_unique<ExpectedExprVarAccess>(
                "y",
                std::move(indices)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_VAR_ACCESS, IDENTIFIER_WITH_ONE_INDEX) {
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
            std::make_unique<ExpectedExprVarAccess>(
                "y",
                std::move(indices)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_VAR_ACCESS, IDENTIFIER_WITH_TWO_INDICES) {
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
            std::make_unique<ExpectedExprVarAccess>(
                "y",
                std::move(indices)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_VAR_ACCESS, INVALID_MISSING_IDENTIFIER) {
    ASSERT_THROW(PARSE("int x = ;"), SyntaxError);
}

TEST(EXPR_VAR_ACCESS, INVALID_MISSING_IDENTIFIER_WITH_INDEX) {
    ASSERT_THROW(PARSE("int x = [1];"), SyntaxError);
}

TEST(EXPR_VAR_ACCESS, INVALID_MISSING_LSQBR) {
    ASSERT_THROW(PARSE("int x = y 1];"), SyntaxError);
}

TEST(EXPR_VAR_ACCESS, INVALID_MISSING_EXPR) {
    ASSERT_THROW(PARSE("int x = y [];"), SyntaxError);
}

TEST(EXPR_VAR_ACCESS, INVALID_MISSING_RSQBR) {
    ASSERT_THROW(PARSE("int x = y [1;"), SyntaxError);
}

TEST(EXPR_VAR_ACCESS, INVALID_MISSING_SECOND_LSQBR) {
    ASSERT_THROW(PARSE("int x = y [1] 2];"), SyntaxError);
}

TEST(EXPR_VAR_ACCESS, INVALID_MISSING_SECOND_EXPR) {
    ASSERT_THROW(PARSE("int x = y [1] [];"), SyntaxError);
}

TEST(EXPR_VAR_ACCESS, INVALID_MISSING_SECOND_RSQBR) {
    ASSERT_THROW(PARSE("int x = y [1] [2;"), SyntaxError);
}
