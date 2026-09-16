#include <gtest/gtest.h>
#include "../ParserTestUtils.h"

using namespace parserTest;

TEST(STM_FUNCTION_CALL, FUNCTION_CALL) {
    const auto testCode = R"(
        foo();
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedExpr>> expectedArguments;

    expectedStatements.push_back(
        std::make_unique<ExpectedFunctionCallStm>(
            std::make_unique<ExpectedFunctionCallExpr>(
                "foo",
                std::move(expectedArguments)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_FUNCTION_CALL, FUNCTION_CALL_WITH_ONE_ARGUMENT) {
    const auto testCode = R"(
        foo(5);
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedExpr>> expectedArguments;

    expectedArguments.push_back(
        std::make_unique<ExpectedExprPostfix>(
            std::make_unique<ExpectedIntegerLiteral>(5),
            noExpectedIndices
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedFunctionCallStm>(
            std::make_unique<ExpectedFunctionCallExpr>(
                "foo",
                std::move(expectedArguments)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_FUNCTION_CALL, NESTED_FUNCTION_CALL) {
    const auto testCode = R"(
        foo(bar());
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedExpr>> expectedArgumentsFoo;
    std::vector<std::unique_ptr<ExpectedExpr>> expectedArgumentsBar;

    expectedArgumentsFoo.push_back(
        std::make_unique<ExpectedExprPostfix>(
            std::make_unique<ExpectedFunctionCallExpr>(
                "bar",
                std::move(expectedArgumentsBar)
            ),
            noExpectedIndices
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedFunctionCallStm>(
            std::make_unique<ExpectedFunctionCallExpr>(
                "foo",
                std::move(expectedArgumentsFoo)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_FUNCTION_CALL, FUNCTION_CALL_WITH_TWO_ARGUMENTS) {
    const auto testCode = R"(
        foo(5, true);
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedExpr>> expectedArguments;

    expectedArguments.push_back(
        std::make_unique<ExpectedExprPostfix>(
            std::make_unique<ExpectedIntegerLiteral>(5),
            noExpectedIndices
        )
    );
    expectedArguments.push_back(
        std::make_unique<ExpectedExprPostfix>(
            std::make_unique<ExpectedBoolLiteral>(true),
            noExpectedIndices
        )
    );
    expectedStatements.push_back(
        std::make_unique<ExpectedFunctionCallStm>(
            std::make_unique<ExpectedFunctionCallExpr>(
                "foo",
                std::move(expectedArguments)
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_FUNCTION_CALL, INVALID_MISSING_LBR) {
    const auto testCode = R"(
        foo);
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_FUNCTION_CALL, INVALID_MISSING_RBR) {
    const auto testCode = R"(
        foo(;
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_FUNCTION_CALL, INVALID_MISSING_SEMICOLON) {
    const auto testCode = R"(
        foo()
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_FUNCTION_CALL, INVALID_MISSING_COMMA) {
    const auto testCode = R"(
        foo(5 5)
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_FUNCTION_CALL, INVALID_MISSING_EXPR_AFTER_COMMA) {
    const auto testCode = R"(
        foo(5,)
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(STM_FUNCTION_CALL, INVALID_MISSING_EXPR_BEFORE_COMMA) {
    const auto testCode = R"(
        foo(,5)
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}