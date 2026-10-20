#include <gtest/gtest.h>
#include "../OptimiserTestUtils.h"

using namespace optimiserTest;

TEST(EXPR_BINARY, PLUS_INT_INT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(3)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = 1 + 2;"
    );
}

TEST(EXPR_BINARY, PLUS_INT_FLOAT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(3.5f)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float x = 1 + 2.5f;"
    );
}

TEST(EXPR_BINARY, PLUS_INT_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(66)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = 1 + 'A';"
    );
}

TEST(EXPR_BINARY, PLUS_FLOAT_FLOAT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(4.0f)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float x = 1.5f + 2.5f;"
    );
}

TEST(EXPR_BINARY, PLUS_FLOAT_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(66.5f)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float x = 1.5f + 'A';"
    );
}

TEST(EXPR_BINARY, PLUS_CHAR_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(130)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = 'A' + 'A';"
    );
}

TEST(EXPR_BINARY, MINUS_INT_INT) {
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
        "int x = 2 - 1;"
    );
}

TEST(EXPR_BINARY, MINUS_INT_FLOAT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(-1.5f)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float x = 1 - 2.5f;"
    );
}

TEST(EXPR_BINARY, MINUS_INT_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(-64)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = 1 - 'A';"
    );
}

TEST(EXPR_BINARY, MINUS_FLOAT_FLOAT) {
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
        "float x = 2.5f - 1.5f;"
    );
}

TEST(EXPR_BINARY, MINUS_FLOAT_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(3.5f)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float x = 68.5f - 'A';"
    );
}

TEST(EXPR_BINARY, MINUS_CHAR_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(-1)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = 'A' - 'B';"
    );
}

TEST(EXPR_BINARY, MULTIPLY_INT_INT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(20)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = 10 * 2;"
    );
}

TEST(EXPR_BINARY, MULTIPLY_INT_FLOAT) {
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
        "float x = 1 * 2.5f;"
    );
}

TEST(EXPR_BINARY, MULTIPLY_INT_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(130)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = 2 * 'A';"
    );
}

TEST(EXPR_BINARY, MUNLTIPLY_FLOAT_FLOAT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(3.75f)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float x = 1.5f * 2.5f;"
    );
}

TEST(EXPR_BINARY, MULTIPLY_FLOAT_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(32.5f)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float x = 0.5f * 'A';"
    );
}

TEST(EXPR_BINARY, MULTIPLY_CHAR_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(1089)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = '!' * '!';"
    );
}

TEST(EXPR_BINARY, DIVIDE_INT_INT) {
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
        "float x = 10 / 2;"
    );
}

TEST(EXPR_BINARY, DIVIDE_INT_FLOAT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(2.0f)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float x = 5 / 2.5f;"
    );
}

TEST(EXPR_BINARY, DIVIDE_INT_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(2.0f)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float x = 130 / 'A';"
    );
}

TEST(EXPR_BINARY, DIVIDE_FLOAT_FLOAT) {
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
        "float x = 3.75f / 1.5f;"
    );
}

TEST(EXPR_BINARY, DIVIDE_FLOAT_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(2.0f)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float x = 130.0f / 'A';"
    );
}

TEST(EXPR_BINARY, DIVIDE_CHAR_CHAR) {
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
        "float x = '!' / '!';"
    );
}

TEST(EXPR_BINARY, INTEGER_DIVIDE_INT_INT) {
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
        "int x = 4 // 2;"
    );
}

TEST(EXPR_BINARY, INTEGER_DIVIDE_INT_FLOAT) {
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
        "int x = 4 // 2.5f;"
    );
}

TEST(EXPR_BINARY, INTEGER_DIVIDE_INT_CHAR) {
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
        "int x = 130 // 'A';"
    );
}

TEST(EXPR_BINARY, INTEGER_DIVIDE_FLOAT_FLOAT) {
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
        "int x = 5.5f // 5.2f;"
    );
}

TEST(EXPR_BINARY, INTEGER_DIVIDE_FLOAT_CHAR) {
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
        "int x = 135.0f // 'A';"
    );
}

TEST(EXPR_BINARY, INTEGER_DIVIDE_CHAR_CHAR) {
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
        "int x = '!' // '!';"
    );
}

TEST(EXPR_BINARY, MODULO_INT_INT) {
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
        "int x = 10 % 3;"
    );
}

TEST(EXPR_BINARY, MODULO_INT_FLOAT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(0.5f)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float x = 5 % 2.25f;"
    );
}

TEST(EXPR_BINARY, MODULO_INT_CHAR) {
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
        "int x = 131 % 'A';"
    );
}

TEST(EXPR_BINARY, MODULO_FLOAT_FLOAT) {
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
        "float x = 5.5f % 1.5f;"
    );
}

TEST(EXPR_BINARY, MODULO_FLOAT_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(0.5f)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float x = 130.5f % 'A';"
    );
}

TEST(EXPR_BINARY, MODULO_CHAR_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(0)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = '!' % '!';"
    );
}

TEST(EXPR_BINARY, BITWISE_AND_INT_INT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(10)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = -1 & 10;"
    );
}

TEST(EXPR_BINARY, BITWISE_AND_INT_CHAR) {
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
        "int x = 65 & 'A';"
    );
}

TEST(EXPR_BINARY, BITWISE_AND_CHAR_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(64)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = 'A' & 'B';"
    );
}

TEST(EXPR_BINARY, BITWISE_OR_INT_INT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(14)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = 12 | 10;"
    );
}

TEST(EXPR_BINARY, BITWISE_OR_INT_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(97)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = 32 | 'A';"
    );
}

TEST(EXPR_BINARY, BITWISE_OR_CHAR_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(67)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = 'A' | 'B';"
    );
}

TEST(EXPR_BINARY, BITWISE_XOR_INT_INT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(6)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = 12 ^ 10;"
    );
}

TEST(EXPR_BINARY, BITWISE_XOR_INT_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(64)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = 1 ^ 'A';"
    );
}

TEST(EXPR_BINARY, BITWISE_XOR_CHAR_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(3)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = 'A' ^ 'B';"
    );
}

TEST(EXPR_BINARY, LEFT_SHIFT_INT_INT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(8)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = 1 << 3;"
    );
}

TEST(EXPR_BINARY, LEFT_SHIFT_NEGATIVE_INT_INT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(-32)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = -8 << 2;"
    );
}

TEST(EXPR_BINARY, LEFT_SHIFT_INT_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(130)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = 'A' << 1;"
    );
}

TEST(EXPR_BINARY, LEFT_SHIFT_CHAR_CHAR) {
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
        "int x = 'A' << '\\0';"
    );
}

TEST(EXPR_BINARY, ARITHMETIC_RIGHT_SHIFT_INT_INT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(4)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = 16 >> 2;"
    );
}

TEST(EXPR_BINARY, ARITHMETIC_RIGHT_SHIFT_NEGATIVE_INT_INT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(-4)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = -8 >> 1;"
    );
}

TEST(EXPR_BINARY, ARITHMETIC_RIGHT_SHIFT_INT_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(32)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = 'A' >> 1;"
    );
}

TEST(EXPR_BINARY, ARITHMETIC_RIGHT_SHIFT_CHAR_CHAR) {
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
        "int x = 'A' >> '\\0';"
    );
}

TEST(EXPR_BINARY, LOGICAL_RIGHT_SHIFT_INT_INT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(4)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = 16 >>> 2;"
    );
}

TEST(EXPR_BINARY, LOGICAL_RIGHT_SHIFT_NEGATIVE_INT_INT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(2147483584)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = -128 >>> 1;"
    );
}

TEST(EXPR_BINARY, LOGICAL_RIGHT_SHIFT_INT_CHAR) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprIntegerLiteral>(32)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "int x = 'A' >>> 1;"
    );
}

TEST(EXPR_BINARY, LOGICAL_RIGHT_SHIFT_CHAR_CHAR) {
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
        "int x = 'A' >>> '\\0';"
    );
}

TEST(EXPR_BINARY, LOGICAL_OR_FALSE_FALSE) {
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
        "bool x = false || false;"
    );
}

TEST(EXPR_BINARY, LOGICAL_OR_FALSE_TRUE) {
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
        "bool x = false || true;"
    );
}

TEST(EXPR_BINARY, LOGICAL_OR_TRUE_FALSE) {
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
        "bool x = true || false;"
    );
}

TEST(EXPR_BINARY, LOGICAL_OR_TRUE_FALSE_SHORT_CIRCUIT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::BOOL_TYPE_ID, 0),
            "s",
            std::make_unique<ExpectedExprBoolLiteral>(false)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::BOOL_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprBoolLiteral>(true)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        R"(
            bool s = false;
            bool x = true || s;
        )"
    );
}

TEST(EXPR_BINARY, LOGICAL_OR_TRUE_TRUE) {
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
        "bool x = true || true;"
    );
}

TEST(EXPR_BINARY, LOGICAL_OR_TRUE_TRUE_SHORT_CIRCUIT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::BOOL_TYPE_ID, 0),
            "s",
            std::make_unique<ExpectedExprBoolLiteral>(true)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::BOOL_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprBoolLiteral>(true)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        R"(
            bool s = true;
            bool x = true || s;
        )"
    );
}

TEST(EXPR_BINARY, LOGICAL_AND_FALSE_FALSE) {
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
        "bool x = false && false;"
    );
}

TEST(EXPR_BINARY, LOGICAL_AND_FALSE_FALSE_SHORT_CIRCUIT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::BOOL_TYPE_ID, 0),
            "s",
            std::make_unique<ExpectedExprBoolLiteral>(false)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::BOOL_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprBoolLiteral>(false)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        R"(
            bool s = false;
            bool x = false && s;
        )"
    );
}

TEST(EXPR_BINARY, LOGICAL_AND_FALSE_TRUE) {
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
        "bool x = false && true;"
    );
}

TEST(EXPR_BINARY, LOGICAL_AND_FALSE_TRUE_SHORT_CIRCUIT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::BOOL_TYPE_ID, 0),
            "s",
            std::make_unique<ExpectedExprBoolLiteral>(true)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::BOOL_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprBoolLiteral>(false)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        R"(
            bool s = true;
            bool x = false && s;
        )"
    );
}

TEST(EXPR_BINARY, LOGICAL_AND_TRUE_FALSE) {
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
        "bool x = true && false;"
    );
}

TEST(EXPR_BINARY, LOGICAL_AND_TRUE_TRUE) {
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
        "bool x = true && true;"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_INT_INT_TRUE) {
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
        "bool x = 1 == 1;"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_INT_INT_FALSE) {
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
        "bool x = 1 == 2;"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_INT_FLOAT_TRUE) {
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
        "bool x = 1 == 1.0f;"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_INT_FLOAT_FALSE) {
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
        "bool x = 1 == 1.1f;"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_INT_CHAR_TRUE) {
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
        "bool x = 65 == 'A';"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_INT_CHAR_FALSE) {
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
        "bool x = 66 == 'A';"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_FLOAT_FLOAT_TRUE) {
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
        "bool x = 1.0f == 1.0f;"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_FLOAT_FLOAT_FALSE) {
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
        "bool x = 1.1f == 1.0f;"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_FLOAT_CHAR_TRUE) {
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
        "bool x = 65.0f == 'A';"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_FLOAT_CHAR_FALSE) {
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
        "bool x = 65.1f == 'A';"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_CHAR_CHAR_TRUE) {
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
        "bool x = 'A' == 'A';"
    );
}

TEST(EXPR_BINARY, EQUAL_EQUAL_CHAR_CHAR_FALSE) {
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
        "bool x = 'A' == 'B';"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_INT_INT_TRUE) {
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
        "bool x = 1 != 2;"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_INT_INT_FALSE) {
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
        "bool x = 1 != 1;"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_INT_FLOAT_TRUE) {
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
        "bool x = 1 != 1.1f;"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_INT_FLOAT_FALSE) {
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
        "bool x = 1 != 1.0f;"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_INT_CHAR_TRUE) {
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
        "bool x = 66 != 'A';"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_INT_CHAR_FALSE) {
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
        "bool x = 65 != 'A';"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_FLOAT_FLOAT_TRUE) {
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
        "bool x = 1.1f != 1.0f;"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_FLOAT_FLOAT_FALSE) {
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
        "bool x = 1.0f != 1.0f;"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_FLOAT_CHAR_TRUE) {
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
        "bool x = 65.1f != 'A';"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_FLOAT_CHAR_FALSE) {
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
        "bool x = 65.0f != 'A';"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_CHAR_CHAR_TRUE) {
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
        "bool x = 'A' != 'B';"
    );
}

TEST(EXPR_BINARY, NOT_EQUAL_CHAR_CHAR_FALSE) {
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
        "bool x = 'A' != 'A';"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_INT_TRUE) {
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
        "bool x = 5 < 10;"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_INT_FALSE_ONE) {
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
        "bool x = 10 < 5;"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_INT_FALSE_TWO) {
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
        "bool x = 5 < 5;"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_FLOAT_TRUE) {
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
        "bool x = 5 < 5.1f;"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_FLOAT_FALSE_ONE) {
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
        "bool x = 5 < 4.9f;"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_FLOAT_FALSE_TWO) {
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
        "bool x = 5 < 5.0f;"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_CHAR_TRUE) {
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
        "bool x = 64 < 'A';"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_CHAR_FALSE_ONE) {
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
        "bool x = 66 < 'A';"
    );
}

TEST(EXPR_BINARY, LESS_THAN_INT_CHAR_FALSE_TWO) {
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
        "bool x = 65 < 'A';"
    );
}

TEST(EXPR_BINARY, LESS_THAN_FLOAT_FLOAT_TRUE) {
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
        "bool x = 5.5f < 10.5f;"
    );
}

TEST(EXPR_BINARY, LESS_THAN_FLOAT_FLOAT_FALSE_ONE) {
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
        "bool x = 10.5f < 5.5f;"
    );
}

TEST(EXPR_BINARY, LESS_THAN_FLOAT_FLOAT_FALSE_TWO) {
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
        "bool x = 5.5f < 5.5f;"
    );
}

TEST(EXPR_BINARY, LESS_THAN_FLOAT_CHAR_TRUE) {
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
        "bool x = 64.0f < 'A';"
    );
}

TEST(EXPR_BINARY, LESS_THAN_FLOAT_CHAR_FALSE_ONE) {
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
        "bool x = 66.0f < 'A';"
    );
}

TEST(EXPR_BINARY, LESS_THAN_FLOAT_CHAR_FALSE_TWO) {
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
        "bool x = 65.0f < 'A';"
    );
}

TEST(EXPR_BINARY, LESS_THAN_CHAR_CHAR_TRUE) {
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
        "bool x = 'A' < 'B';"
    );
}

TEST(EXPR_BINARY, LESS_THAN_CHAR_CHAR_FALSE_ONE) {
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
        "bool x = 'B' < 'A';"
    );
}

TEST(EXPR_BINARY, LESS_THAN_CHAR_CHAR_FALSE_TWO) {
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
        "bool x = 'A' < 'A';"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_TO_INT_INT_TRUE_ONE) {
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
        "bool x = 5 <= 10;"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_TO_INT_INT_TRUE_TWO) {
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
        "bool x = 5 <= 5;"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_TO_INT_INT_FALSE) {
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
        "bool x = 10 <= 5;"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_TO_INT_FLOAT_TRUE_ONE) {
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
        "bool x = 5 <= 5.1f;"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_TO_INT_FLOAT_TRUE_TWO) {
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
        "bool x = 5 <= 5.0f;"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_TO_INT_FLOAT_FALSE) {
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
        "bool x = 5 <= 4.9f;"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_TO_INT_CHAR_TRUE_ONE) {
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
        "bool x = 64 <= 'A';"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_TO_INT_CHAR_TRUE_TWO) {
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
        "bool x = 65 <= 'A';"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_TO_INT_CHAR_FALSE) {
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
        "bool x = 66 <= 'A';"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_TO_FLOAT_FLOAT_TRUE_ONE) {
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
        "bool x = 5.5f <= 10.5f;"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_TO_FLOAT_FLOAT_TRUE_TWO) {
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
        "bool x = 5.5f <= 5.5f;"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_TO_FLOAT_FLOAT_FALSE) {
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
        "bool x = 10.5f <= 5.5f;"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_TO_FLOAT_CHAR_TRUE_ONE) {
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
        "bool x = 64.0f <= 'A';"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_TO_FLOAT_CHAR_TRUE_TWO) {
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
        "bool x = 65.0f <= 'A';"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_TO_FLOAT_CHAR_FALSE) {
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
        "bool x = 66.0f <= 'A';"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_TO_CHAR_CHAR_TRUE_ONE) {
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
        "bool x = 'A' <= 'B';"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_TO_CHAR_CHAR_TRUE_TWO) {
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
        "bool x = 'A' <= 'A';"
    );
}

TEST(EXPR_BINARY, LESS_THAN_OR_EQUAL_TO_CHAR_CHAR_FALSE) {
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
        "bool x = 'B' <= 'A';"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_INT_TRUE) {
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
        "bool x = 10 > 5;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_INT_FALSE_ONE) {
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
        "bool x = 5 > 10;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_INT_FALSE_TWO) {
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
        "bool x = 5 > 5;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_FLOAT_TRUE) {
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
        "bool x = 5 > 4.9f;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_FLOAT_FALSE_ONE) {
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
        "bool x = 5 > 5.1f;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_FLOAT_FALSE_TWO) {
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
        "bool x = 5 > 5.0f;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_CHAR_TRUE) {
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
        "bool x = 'A' > 64;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_CHAR_FALSE_ONE) {
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
        "bool x = 'A' > 66;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_INT_CHAR_FALSE_TWO) {
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
        "bool x = 65 > 'A';"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_FLOAT_FLOAT_TRUE) {
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
        "bool x = 10.5f > 5.5f;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_FLOAT_FLOAT_FALSE_ONE) {
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
        "bool x = 5.5f > 10.5f;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_FLOAT_FLOAT_FALSE_TWO) {
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
        "bool x = 5.5f > 5.5f;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_FLOAT_CHAR_TRUE) {
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
        "bool x = 'A' > 64.0f;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_FLOAT_CHAR_FALSE_ONE) {
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
        "bool x = 'A' > 66.0f;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_FLOAT_CHAR_FALSE_TWO) {
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
        "bool x = 'A' > 65.0f;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_CHAR_CHAR_TRUE) {
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
        "bool x = 'B' > 'A';"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_CHAR_CHAR_FALSE_ONE) {
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
        "bool x = 'A' > 'B';"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_CHAR_CHAR_FALSE_TWO) {
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
        "bool x = 'A' > 'A';"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_TO_INT_INT_TRUE_ONE) {
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
        "bool x = 10 >= 5;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_TO_INT_INT_TRUE_TWO) {
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
        "bool x = 5 >= 5;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_TO_INT_INT_FALSE) {
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
        "bool x = 5 >= 10;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_TO_INT_FLOAT_TRUE_ONE) {
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
        "bool x = 5.1f >= 5;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_TO_INT_FLOAT_TRUE_TWO) {
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
        "bool x = 5 >= 5.0f;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_TO_INT_FLOAT_FALSE) {
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
        "bool x = 4.9f >= 5;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_TO_INT_CHAR_TRUE_ONE) {
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
        "bool x = 'A' >= 64;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_TO_INT_CHAR_TRUE_TWO) {
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
        "bool x = 65 >= 'A';"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_TO_INT_CHAR_FALSE) {
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
        "bool x = 'A' >= 66;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_TO_FLOAT_FLOAT_TRUE_ONE) {
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
        "bool x = 10.5f >= 5.5f;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_TO_FLOAT_FLOAT_TRUE_TWO) {
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
        "bool x = 5.5f >= 5.5f;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_TO_FLOAT_FLOAT_FALSE) {
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
        "bool x = 5.5f >= 10.5f;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_TO_FLOAT_CHAR_TRUE_ONE) {
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
        "bool x = 'A' >= 64.0f;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_TO_FLOAT_CHAR_TRUE_TWO) {
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
        "bool x = 65.0f >= 'A';"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_TO_FLOAT_CHAR_FALSE) {
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
        "bool x = 'A' >= 66.0f;"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_TO_CHAR_CHAR_TRUE_ONE) {
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
        "bool x = 'B' >= 'A';"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_TO__CHAR_CHAR_TRUE_TWO) {
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
        "bool x = 'A' >= 'A';"
    );
}

TEST(EXPR_BINARY, GREATER_THAN_OR_EQUAL_TO_CHAR_CHAR_FALSE) {
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
        "bool x = 'A' >= 'B';"
    );
}