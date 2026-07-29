#include <gtest/gtest.h>
#include "../parserTestUtils.h"

TEST(STM_IF, IF) {
    const auto testCode = R"(
        if (true) {}
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> ifBlockExpectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedIfStm>(
            std::make_unique<parserTest::ExpectedBoolLiteral>(
                true
            ),
            std::make_unique<parserTest::ExpectedBlock>(
                ifBlockExpectedStatements
            ),
            nullptr
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_IF, IF_BINARY_EXPR) {
    const auto testCode = R"(
        if (x > 5) {}
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> ifBlockExpectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedIfStm>(
            std::make_unique<parserTest::ExpectedBinaryExpr>(
                compiler::BinaryOperator::GREATER_THAN,
                std::make_unique<parserTest::ExpectedExprIdentifier>("x"),
                std::make_unique<parserTest::ExpectedIntegerLiteral>(5)
            ),
            std::make_unique<parserTest::ExpectedBlock>(
                ifBlockExpectedStatements
            ),
            nullptr
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_IF, IF_BODY) {
    const auto testCode = R"(
        if (true) {
            int x;
        }
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> ifBlockExpectedStatements;
    ifBlockExpectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            nullptr
        )
    );
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedIfStm>(
            std::make_unique<parserTest::ExpectedBoolLiteral>(
                true
            ),
            std::make_unique<parserTest::ExpectedBlock>(
                ifBlockExpectedStatements
            ),
            nullptr
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_IF, ELSE) {
    const auto testCode = R"(
        if (x > 5) {
        } else {}
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> emptyListOfStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedIfStm>(
            std::make_unique<parserTest::ExpectedBinaryExpr>(
                compiler::BinaryOperator::GREATER_THAN,
                std::make_unique<parserTest::ExpectedExprIdentifier>("x"),
                std::make_unique<parserTest::ExpectedIntegerLiteral>(5)
            ),
            std::make_unique<parserTest::ExpectedBlock>(
                emptyListOfStatements
            ),
            std::make_unique<parserTest::ExpectedBlock>(
                emptyListOfStatements
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_IF, ELSE_BODY) {
    const auto testCode = R"(
        if (x > 5) {
        } else {
            int x;
        }
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> ifBlockExpectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> elseBlockExpectedStatements;
    elseBlockExpectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            nullptr
        )
    );
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedIfStm>(
            std::make_unique<parserTest::ExpectedBinaryExpr>(
                compiler::BinaryOperator::GREATER_THAN,
                std::make_unique<parserTest::ExpectedExprIdentifier>("x"),
                std::make_unique<parserTest::ExpectedIntegerLiteral>(5)
            ),
            std::make_unique<parserTest::ExpectedBlock>(
                ifBlockExpectedStatements
            ),
            std::make_unique<parserTest::ExpectedBlock>(
                elseBlockExpectedStatements
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_IF, ELSE_IF) {
    const auto testCode = R"(
        if (false) {
        } else if (b) {}
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> ifBlockExpectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> elseBlockExpectedStatements;
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedIfStm>(
            std::make_unique<parserTest::ExpectedBoolLiteral>(
                false
            ),
            std::make_unique<parserTest::ExpectedBlock>(
                ifBlockExpectedStatements
            ),
            std::make_unique<parserTest::ExpectedIfStm>(
                std::make_unique<parserTest::ExpectedExprIdentifier>("b"),
            std::make_unique<parserTest::ExpectedBlock>(
                elseBlockExpectedStatements
            ),
            nullptr
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_IF, ELSE_IF_BODY) {
    const auto testCode = R"(
        if (false) {

        } else if (b) {
            int x;
        }
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> ifBlockExpectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> elseIfBlockExpectedStatements;
    elseIfBlockExpectedStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            nullptr
        )
    );
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedIfStm>(
            std::make_unique<parserTest::ExpectedBoolLiteral>(
                false
            ),
            std::make_unique<parserTest::ExpectedBlock>(
                ifBlockExpectedStatements
            ),
            std::make_unique<parserTest::ExpectedIfStm>(
                std::make_unique<parserTest::ExpectedExprIdentifier>("b"),
                std::make_unique<parserTest::ExpectedBlock>(
                    elseIfBlockExpectedStatements
                ),
                nullptr
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_IF, INVALID_MISSING_CONDITION_AND_BLOCK) {
    const auto testCode = R"(
        if
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_IF, INVALID_MISSING_LBR) {
    const auto testCode = R"(
        if true) {}
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_IF, INVALID_MISSING_RBR) {
    const auto testCode = R"(
        if (true {}
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_IF, INVALID_MISSING_CONDITION) {
    const auto testCode = R"(
        if () {}
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_IF, INVALID_MISSING_LCBR) {
    const auto testCode = R"(
        if (true) }
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_IF, INVALID_MISSING_RCBR) {
    const auto testCode = R"(
        if (true) {
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_IF, INVALID_ELSE_MISSING_LCBR) {
    const auto testCode = R"(
        if (true) {
        } else }
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_IF, INVALID_ELSE_MISSING_RCBR) {
    const auto testCode = R"(
        if (true) {
        } else {
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_IF, INVALID_IF_STM) {
    const auto testCode = R"(
        if (true) x = 5;
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_IF, INVALID_ELSE_STM) {
    const auto testCode = R"(
        if (true) {
        } else x = 5;
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}