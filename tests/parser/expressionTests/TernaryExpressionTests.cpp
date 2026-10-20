#include <gtest/gtest.h>
#include "../ParserTestUtils.h"

using namespace parserTest;

TEST(TERNARY_EXPR, VALID) {
    const auto program = PARSE(R"(
        int x = true ? 1 : 2;
    )");
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
    std::make_unique<ExpectedStmVarDecl>(
        std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprTernary>(
                std::make_unique<ExpectedExprBoolLiteral>(true),
                std::make_unique<ExpectedExprIntegerLiteral>(1),
                std::make_unique<ExpectedExprIntegerLiteral>(2)
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(TERNARY_EXPR, ASSOCIATIVE) {
    const auto program = PARSE(R"(
        int x = true ? true ? 1 : 2 : 3;
    )");
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
    std::make_unique<ExpectedStmVarDecl>(
        std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprTernary>(
                std::make_unique<ExpectedExprBoolLiteral>(true),
                std::make_unique<ExpectedExprTernary>(
                    std::make_unique<ExpectedExprBoolLiteral>(true),
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