#include <gtest/gtest.h>
#include "../ParserTestUtils.h"

using namespace parserTest;

TEST(STM_BLOCK, EMPTY) {
    const auto testCode = R"(
        {}
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatementsInBlock;
    expectedStatements.push_back(
        std::make_unique<ExpectedBlock>(
            expectedStatementsInBlock
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_BLOCK, VAR_DECL) {
    const auto testCode = R"(
        {
            int x;
        }
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatementsInBlock;
    expectedStatementsInBlock.push_back(
        std::make_unique<ExpectedVarDecl>(
                std::make_unique<Type>(compiler::Type::INT, 0),
                "x",
            nullptr
        )
    );
    expectedStatements.push_back(
        std::make_unique<ExpectedBlock>(
            expectedStatementsInBlock
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_BLOCK, NESTED_VAR_DECL) {
    const auto testCode = R"(
        {
            {
                int x;
            }
        }
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatementsInBlock1;
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatementsInBlock2;
    expectedStatementsInBlock2.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            nullptr
        )
    );
    expectedStatementsInBlock1.push_back(
        std::make_unique<ExpectedBlock>(
            expectedStatementsInBlock2
        )
    );
    expectedStatements.push_back(
        std::make_unique<ExpectedBlock>(
            expectedStatementsInBlock1
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_BLOCK, INVALID_MISSING_RCBR) {
    const auto testCode = R"(
        {
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_BLOCK, INVALID_MISSING_LCBR) {
    const auto testCode = R"(
        }
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}