#include <gtest/gtest.h>
#include "../parserTestUtils.h"

TEST(STM_FUNCTION_CALL, FUNCTION_CALL) {
    const auto testCode = R"(
        foo();
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedExpr>> expectedArguments;

    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedFunctionCallStm>(
            std::make_unique<parserTest::ExpectedFunctionCallExpr>(
                "foo",
                std::move(expectedArguments)
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_FUNCTION_CALL, FUNCTION_CALL_WITH_ONE_ARGUMENT) {
    const auto testCode = R"(
        foo(5);
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedExpr>> expectedArguments;

    expectedArguments.push_back(
        std::make_unique<parserTest::ExpectedIntegerLiteral>(5)
    );

    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedFunctionCallStm>(
            std::make_unique<parserTest::ExpectedFunctionCallExpr>(
                "foo",
                std::move(expectedArguments)
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_FUNCTION_CALL, NESTED_FUNCTION_CALL) {
    const auto testCode = R"(
        foo(bar());
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedExpr>> expectedArgumentsFoo;
    std::vector<std::unique_ptr<parserTest::ExpectedExpr>> expectedArgumentsBar;

    expectedArgumentsFoo.push_back(
        std::make_unique<parserTest::ExpectedFunctionCallExpr>(
            "bar",
            std::move(expectedArgumentsBar)
        )
    );

    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedFunctionCallStm>(
            std::make_unique<parserTest::ExpectedFunctionCallExpr>(
                "foo",
                std::move(expectedArgumentsFoo)
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_FUNCTION_CALL, FUNCTION_CALL_WITH_TWO_ARGUMENTS) {
    const auto testCode = R"(
        foo(5, true);
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedExpr>> expectedArguments;

    expectedArguments.push_back(
        std::make_unique<parserTest::ExpectedIntegerLiteral>(5)
    );
    expectedArguments.push_back(
        std::make_unique<parserTest::ExpectedBoolLiteral>(true)
    );
    expectedStatements.push_back(
        std::make_unique<parserTest::ExpectedFunctionCallStm>(
            std::make_unique<parserTest::ExpectedFunctionCallExpr>(
                "foo",
                std::move(expectedArguments)
            )
        )
    );
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(STM_FUNCTION_CALL, INVALID_MISSING_LBR) {
    const auto testCode = R"(
        foo);
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_FUNCTION_CALL, INVALID_MISSING_RBR) {
    const auto testCode = R"(
        foo(;
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_FUNCTION_CALL, INVALID_MISSING_SEMICOLON) {
    const auto testCode = R"(
        foo()
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_FUNCTION_CALL, INVALID_MISSING_COMMA) {
    const auto testCode = R"(
        foo(5 5)
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_FUNCTION_CALL, INVALID_MISSING_EXPR_AFTER_COMMA) {
    const auto testCode = R"(
        foo(5,)
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(STM_FUNCTION_CALL, INVALID_MISSING_EXPR_BEFORE_COMMA) {
    const auto testCode = R"(
        foo(,5)
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}