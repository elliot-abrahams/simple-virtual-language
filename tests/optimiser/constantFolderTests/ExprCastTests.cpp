#include <gtest/gtest.h>
#include "../OptimiserTestUtils.h"

using namespace optimiserTest;

TEST(EXPR_CAST, INT_TO_INT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(1)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = (int) 1;"
    );
}

TEST(EXPR_CAST, INT_TO_FLOAT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(5.0f)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float x = (float) 5;"
    );
}

TEST(EXPR_CAST, INT_TO_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::CHAR_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprCharLiteral>('A')
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "char x = (char) 65;"
    );
}

TEST(EXPR_CAST, FLOAT_TO_INT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(5)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = (int) 5.5f;"
    );
}

TEST(EXPR_CAST, FLOAT_TO_FLOAT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(5.5f)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float x = (float) 5.5f;"
    );
}

TEST(EXPR_CAST, FLOAT_TO_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::CHAR_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprCharLiteral>('A')
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "char x = (char) 65.5f;"
    );
}

TEST(EXPR_CAST, BOOL_TO_BOOL) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::BOOL_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprBoolLiteral>(true)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "bool x = (bool) true;"
    );
}

TEST(EXPR_CAST, CHAR_TO_INT) {
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
        "int x = (char) 'A';"
    );
}

TEST(EXPR_CAST, CHAR_TO_FLOAT) {
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
        "float x = (char) 'A';"
    );
}

TEST(EXPR_CAST, CHAR_TO_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::CHAR_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprCharLiteral>('A')
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "char x = (char) 'A';"
    );
}