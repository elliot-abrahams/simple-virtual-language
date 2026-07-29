#include <gtest/gtest.h>
#include "../parserTestUtils.h"

TEST(STM_CONTINUE, CONTINUE) {
    const auto testCode = R"(
        continue;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedContinueStm>()
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_IF, INVALID_MISSING_SEMICOLON) {
    const auto testCode = R"(
        continue
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}