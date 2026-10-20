#include <gtest/gtest.h>
#include "../ParserTestUtils.h"

using namespace parserTest;

TEST(STM_WHILE, WHILE) {
    const auto testCode = R"(
        while (true) {}
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedStmWhile>(
            std::make_unique<ExpectedExprBoolLiteral>(true),
            std::make_unique<ExpectedStmBlock>(
                noExpectedStms
            )
        )
    );

    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_WHILE, WHILE_BINARY_EXPR) {
    const auto testCode = R"(
        while (x > 5) {}
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmWhile>(
            std::make_unique<ExpectedExprBinary>(
                compiler::BinaryOperator::GREATER_THAN,
                std::make_unique<ExpectedExprIdentifier>(
                    std::make_unique<ExpectedIdentifier>("x")
                ),
                std::make_unique<ExpectedExprIntegerLiteral>(5)
            ),
            std::make_unique<ExpectedStmBlock>(
                noExpectedStms
            )
        )
    );

    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_WHILE, WHILE_BODY) {
    const auto testCode = R"(
        while (true) {
            int x;
        }
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedStm>> expectedWhileBody;
    expectedWhileBody.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            nullptr
        )
    );
    expectedStatements.push_back(
        std::make_unique<ExpectedStmWhile>(
            std::make_unique<ExpectedExprBoolLiteral>(true),
            std::make_unique<ExpectedStmBlock>(
                expectedWhileBody
            )
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_WHILE, INVALID_MISSING_LBR) {
    const auto testCode = R"(
        while true) {}
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_WHILE, INVALID_MISSING_RBR) {
    const auto testCode = R"(
        while (true {}
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_WHILE, INVALID_MISSING_CONDITION) {
    const auto testCode = R"(
        while () {}
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_WHILE, INVALID_MISSING_LCBR) {
    const auto testCode = R"(
        while (true) }
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_WHILE, INVALID_MISSING_RCBR) {
    const auto testCode = R"(
        while (true) {
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}