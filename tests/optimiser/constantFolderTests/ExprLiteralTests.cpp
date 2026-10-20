#include <gtest/gtest.h>
#include "../OptimiserTestUtils.h"

using namespace optimiserTest;

TEST(EXPR_LITERAL, IMPLICIT_INT_TO_FLOAT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(1.0f)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float x = 1;"
    );
}

TEST(EXPR_LITERAL, IMPLICIT_CHAR_TO_INT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(65)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = 'A';"
    );
}

TEST(EXPR_LITERAL, IMPLICIT_CHAR_TO_FLOAT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(65.0f)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float x = 'A';"
    );
}