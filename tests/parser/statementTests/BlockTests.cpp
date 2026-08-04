#include <gtest/gtest.h>
#include "../ParserTestUtils.h"

TEST(STM_BLOCK, EMPTY) {
    const auto testCode = R"(
        {}
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatementsInBlock;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedBlock>(
            expectedStatementsInBlock
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_BLOCK, VAR_DECL) {
    const auto testCode = R"(
        {
            int x;
        }
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatementsInBlock;
    expectedStatementsInBlock.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            nullptr
        )
    );
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedBlock>(
            expectedStatementsInBlock
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_BLOCK, NESTED_VAR_DECL) {
    const auto testCode = R"(
        {
            {
                int x;
            }
        }
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatementsInBlock1;
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatementsInBlock2;
    expectedStatementsInBlock2.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            nullptr
        )
    );
    expectedStatementsInBlock1.push_back(
        std::make_unique<parserTest::ExpectedBlock>(
            expectedStatementsInBlock2
        )
    );
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedBlock>(
            expectedStatementsInBlock1
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_BLOCK, INVALID_MISSING_RCBR) {
    const auto testCode = R"(
        {
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_BLOCK, INVALID_MISSING_LCBR) {
    const auto testCode = R"(
        }
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}