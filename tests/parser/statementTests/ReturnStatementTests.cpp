#include <gtest/gtest.h>
#include "../ParserTestUtils.h"

using namespace parserTest;

TEST(STM_RETURN, RETURN_WITHOUT_EXPRESSION) {
    const auto testCode = R"(
        return;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmReturn>(
            nullptr
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_RETURN, RETURN_WITH_EXPRESSION) {
    const auto testCode = R"(
        return 5;
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmReturn>(
            std::make_unique<ExpectedExprIntegerLiteral>(5)
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_BLOCK, INVALID_MISSING_SEMICOLON) {
    const auto testCode = R"(
        return
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}