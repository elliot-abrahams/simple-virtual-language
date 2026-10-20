#include <gtest/gtest.h>
#include "../OptimiserTestUtils.h"

using namespace optimiserTest;

TEST(EXPR_TERNARY, INT_INT_TRUE) {
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
        "int x = true ? 1 : 2;"
    );
}

TEST(EXPR_TERNARY, INT_INT_FALSE) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(2)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = false ? 1 : 2;"
    );
}

TEST(EXPR_TERNARY, INT_FLOAT_TRUE) {
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
        "float x = true ? 1 : 2.5f;"
    );
}

TEST(EXPR_TERNARY, INT_FLOAT_FALSE) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(2.5f)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float x = false ? 1 : 2.5f;"
    );
}

TEST(EXPR_TERNARY, INT_CHAR_TRUE) {
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
        "int x = true ? 1 : 'A';"
    );
}

TEST(EXPR_TERNARY, INT_CHAR_FALSE) {
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
        "int x = false ? 1 : 'A';"
    );
}

TEST(EXPR_TERNARY, FLOAT_FLOAT_TRUE) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(1.5f)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float x = true ? 1.5f : 2.5f;"
    );
}

TEST(EXPR_TERNARY, FLOAT_FLOAT_FALSE) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(2.5f)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float x = false ? 1.5f : 2.5f;"
    );
}

TEST(EXPR_TERNARY, FLOAT_CHAR_TRUE) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(1.5f)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float x = true ? 1.5f : 'A';"
    );
}

TEST(EXPR_TERNARY, FLOAT_CHAR_FALSE) {
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
        "float x = false ? 1.5f : 'A';"
    );
}

TEST(EXPR_TERNARY, BOOL_BOOL_TRUE) {
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
        "bool x = true ? true : false;"
    );
}

TEST(EXPR_TERNARY, BOOL_BOOL_FALSE) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::BOOL_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprBoolLiteral>(false)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "bool x = false ? true : false;"
    );
}

TEST(EXPR_TERNARY, CHAR_CHAR_TRUE) {
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
        "char x = true ? 'A' : 'B';"
    );
}

TEST(EXPR_TERNARY, CHAR_CHAR_FALSE) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::CHAR_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprCharLiteral>('B')
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "char x = false ? 'A' : 'B';"
    );
}