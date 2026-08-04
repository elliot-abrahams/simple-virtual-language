#include <gtest/gtest.h>
#include "../ParserTestUtils.h"

TEST(STM_BREAK, BREAK) {
    const auto testCode = R"(
        break;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedBreakStm>()
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_BREAK, INVALID_MISSING_SEMICOLON) {
    const auto testCode = R"(
        break
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}