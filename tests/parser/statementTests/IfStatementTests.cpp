#include <gtest/gtest.h>
#include "../ParserTestUtils.h"

using namespace parserTest;

TEST(STM_IF, IF) {
    const auto testCode = R"(
        if (true) {}
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedStm>> ifBlockExpectedStatements;
    expectedStatements.push_back(
        std::make_unique<ExpectedIfStm>(
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedBoolLiteral>(
                    true
                ),
                noExpectedIndices
            ),
            std::make_unique<ExpectedBlock>(
                ifBlockExpectedStatements
            ),
            nullptr
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_IF, IF_BINARY_EXPR) {
    const auto testCode = R"(
        if (x > 5) {}
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedStm>> ifBlockExpectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedIfStm>(
            std::make_unique<ExpectedBinaryExpr>(
                compiler::BinaryOperator::GREATER_THAN,
                std::make_unique<ExpectedExprPostfix>(
                    std::make_unique<ExpectedExprIdentifier>(
                        std::make_unique<ExpectedIdentifier>("x")
                    ),
                    noExpectedIndices
                ),
                std::make_unique<ExpectedExprPostfix>(
                    std::make_unique<ExpectedIntegerLiteral>(5),
                    noExpectedIndices
                )
            ),
            std::make_unique<ExpectedBlock>(
                ifBlockExpectedStatements
            ),
            nullptr
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_IF, IF_BODY) {
    const auto testCode = R"(
        if (true) {
            int x;
        }
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedStm>> ifBlockExpectedStatements;
    ifBlockExpectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
                std::make_unique<Type>(compiler::Type::INT, 0),
                "x",
            nullptr
        )
    );
    expectedStatements.push_back(
        std::make_unique<ExpectedIfStm>(
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedBoolLiteral>(true),
                noExpectedIndices
            ),
            std::make_unique<ExpectedBlock>(
                ifBlockExpectedStatements
            ),
            nullptr
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_IF, ELSE) {
    const auto testCode = R"(
        if (x > 5) {
        } else {}
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedStm>> emptyListOfStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedIfStm>(
            std::make_unique<ExpectedBinaryExpr>(
                compiler::BinaryOperator::GREATER_THAN,
                std::make_unique<ExpectedExprPostfix>(
                    std::make_unique<ExpectedExprIdentifier>(
                        std::make_unique<ExpectedIdentifier>("x")
                    ),
                    noExpectedIndices
                ),
                std::make_unique<ExpectedExprPostfix>(
                    std::make_unique<ExpectedIntegerLiteral>(5),
                    noExpectedIndices
                )
            ),
            std::make_unique<ExpectedBlock>(
                emptyListOfStatements
            ),
            std::make_unique<ExpectedBlock>(
                emptyListOfStatements
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_IF, ELSE_BODY) {
    const auto testCode = R"(
        if (x > 5) {
        } else {
            int x;
        }
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedStm>> ifBlockExpectedStatements;
    std::vector<std::unique_ptr<ExpectedStm>> elseBlockExpectedStatements;

    elseBlockExpectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
                std::make_unique<Type>(compiler::Type::INT, 0),
                "x",
            nullptr
        )
    );
    expectedStatements.push_back(
        std::make_unique<ExpectedIfStm>(
            std::make_unique<ExpectedBinaryExpr>(
                compiler::BinaryOperator::GREATER_THAN,
                std::make_unique<ExpectedExprPostfix>(
                    std::make_unique<ExpectedExprIdentifier>(
                        std::make_unique<ExpectedIdentifier>("x")
                    ),
                    noExpectedIndices
                ),
                std::make_unique<ExpectedExprPostfix>(
                    std::make_unique<ExpectedIntegerLiteral>(5),
                    noExpectedIndices
                )
            ),
            std::make_unique<ExpectedBlock>(
                ifBlockExpectedStatements
            ),
            std::make_unique<ExpectedBlock>(
                elseBlockExpectedStatements
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_IF, ELSE_IF) {
    const auto testCode = R"(
        if (false) {
        } else if (b) {}
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedStm>> ifBlockExpectedStatements;
    std::vector<std::unique_ptr<ExpectedStm>> elseBlockExpectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedIfStm>(
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedBoolLiteral>(false),
                noExpectedIndices
            ),
            std::make_unique<ExpectedBlock>(
                ifBlockExpectedStatements
            ),
            std::make_unique<ExpectedIfStm>(
                std::make_unique<ExpectedExprPostfix>(
                    std::make_unique<ExpectedExprIdentifier>(
                        std::make_unique<ExpectedIdentifier>("b")
                    ),
                    noExpectedIndices
                ),
                std::make_unique<ExpectedBlock>(
                    elseBlockExpectedStatements
                ),
                nullptr
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_IF, ELSE_IF_BODY) {
    const auto testCode = R"(
        if (false) {

        } else if (b) {
            int x;
        }
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedStm>> ifBlockExpectedStatements;
    std::vector<std::unique_ptr<ExpectedStm>> elseIfBlockExpectedStatements;

    elseIfBlockExpectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
                std::make_unique<Type>(compiler::Type::INT, 0),
                "x",
            nullptr
        )
    );
    expectedStatements.push_back(
        std::make_unique<ExpectedIfStm>(
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedBoolLiteral>(false),
                noExpectedIndices
            ),
            std::make_unique<ExpectedBlock>(
                ifBlockExpectedStatements
            ),
            std::make_unique<ExpectedIfStm>(
                std::make_unique<ExpectedExprPostfix>(
                    std::make_unique<ExpectedExprIdentifier>(
                        std::make_unique<ExpectedIdentifier>("b")
                    ),
                    noExpectedIndices
                ),
                std::make_unique<ExpectedBlock>(
                    elseIfBlockExpectedStatements
                ),
                nullptr
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_IF, INVALID_MISSING_CONDITION_AND_BLOCK) {
    const auto testCode = R"(
        if
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_IF, INVALID_MISSING_LBR) {
    const auto testCode = R"(
        if true) {}
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_IF, INVALID_MISSING_RBR) {
    const auto testCode = R"(
        if (true {}
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_IF, INVALID_MISSING_CONDITION) {
    const auto testCode = R"(
        if () {}
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_IF, INVALID_MISSING_LCBR) {
    const auto testCode = R"(
        if (true) }
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_IF, INVALID_MISSING_RCBR) {
    const auto testCode = R"(
        if (true) {
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_IF, INVALID_ELSE_MISSING_LCBR) {
    const auto testCode = R"(
        if (true) {
        } else }
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_IF, INVALID_ELSE_MISSING_RCBR) {
    const auto testCode = R"(
        if (true) {
        } else {
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_IF, INVALID_IF_STM) {
    const auto testCode = R"(
        if (true) x = 5;
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_IF, INVALID_ELSE_STM) {
    const auto testCode = R"(
        if (true) {
        } else x = 5;
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}