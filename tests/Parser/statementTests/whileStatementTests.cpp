#include <gtest/gtest.h>
#include "../parserTestUtils.h"

TEST(STM_WHILE, WHILE) {
    const auto testCode = R"(
        while (true) {}
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedWhileBlock;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedWhileStm>(
            std::make_unique<parserTest::ExpectedBoolLiteral>(true),
            std::make_unique<parserTest::ExpectedBlock>(
                expectedWhileBlock
            )
        )
    );

    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_WHILE, WHILE_BINARY_EXPR) {
    const auto testCode = R"(
        while (x > 5) {}
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedWhileBlock;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedWhileStm>(
            std::make_unique<parserTest::ExpectedBinaryExpr>(
                compiler::BinaryOperator::GREATER_THAN,
                std::make_unique<parserTest::ExpectedExprIdentifier>("x"),
                std::make_unique<parserTest::ExpectedIntegerLiteral>(5)
            ),
            std::make_unique<parserTest::ExpectedBlock>(
                expectedWhileBlock
            )
        )
    );

    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_WHILE, WHILE_BODY) {
    const auto testCode = R"(
        while (true) {
            int x;
        }
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedWhileBody;
    expectedWhileBody.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            nullptr
        )
    );
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedWhileStm>(
            std::make_unique<parserTest::ExpectedBoolLiteral>(true),
            std::make_unique<parserTest::ExpectedBlock>(
                expectedWhileBody
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_WHILE, INVALID_MISSING_LBR) {
    const auto testCode = R"(
        while true) {}
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_WHILE, INVALID_MISSING_RBR) {
    const auto testCode = R"(
        while (true {}
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_WHILE, INVALID_MISSING_CONDITION) {
    const auto testCode = R"(
        while () {}
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_WHILE, INVALID_MISSING_LCBR) {
    const auto testCode = R"(
        while (true) }
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_WHILE, INVALID_MISSING_RCBR) {
    const auto testCode = R"(
        while (true) {
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}