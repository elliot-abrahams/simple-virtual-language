#include <gtest/gtest.h>
#include "../parserTestUtils.h"

TEST(STM_RETURN, RETURN_WITHOUT_EXPRESSION) {
    const auto testCode = R"(
        return;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedReturnStm>(
            nullptr
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_RETURN, RETURN_WITH_EXPRESSION) {
    const auto testCode = R"(
        return 5;
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedReturnStm>(
            std::make_unique<parserTest::ExpectedIntegerLiteral>(5)
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_BLOCK, INVALID_MISSING_SEMICOLON) {
    const auto testCode = R"(
        return
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}