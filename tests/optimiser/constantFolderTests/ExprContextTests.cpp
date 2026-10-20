#include <gtest/gtest.h>
#include "../OptimiserTestUtils.h"

using namespace optimiserTest;

TEST(EXPR_CONTEXT, FUNCTION_DECL) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedParameter>> expectedParameters;
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    std::vector<std::unique_ptr<ExpectedStm>> expectedFunctionBodyStatements;

    expectedFunctionBodyStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(1.0f)
        )
    );

    expectedFunctionDecls.push_back(
        std::make_unique<ExpectedFunctionDecl>(
                std::make_unique<Type>(compiler::VOID_TYPE_ID, 0),
                "foo",
            expectedParameters,
            std::make_unique<ExpectedStmBlock>(expectedFunctionBodyStatements)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(expectedFunctionDecls)),
        R"(
            void foo() {
                float x = 1;
            }
        )"
    );
}

TEST(EXPR_CONTEXT, FUNCTION_ARGUMENT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedParameter>> expectedParameters;
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    std::vector<std::unique_ptr<ExpectedStm>> expectedFunctionBodyStatements;
    std::vector<std::unique_ptr<ExpectedExpr>> expectedArguments;

    expectedArguments.push_back(
        std::make_unique<ExpectedExprFloatLiteral>(1.0f)
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedStmExpr>(
            std::make_unique<ExpectedExprFunctionCall>(
                "foo",
                std::move(expectedArguments)
            )
        )
    );

    expectedParameters.push_back(
        std::make_unique<ExpectedParameter>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "a"
        )
    );

    expectedFunctionDecls.push_back(
        std::make_unique<ExpectedFunctionDecl>(
                std::make_unique<Type>(compiler::VOID_TYPE_ID, 0),
                "foo",
            expectedParameters,
            std::make_unique<ExpectedStmBlock>(expectedFunctionBodyStatements)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(expectedFunctionDecls)),
        R"(
            void foo(float a) {}
            foo(1);
        )"
    );
}

TEST(EXPR_CONTEXT, STM_RETURN) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedParameter>> expectedParameters;
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    std::vector<std::unique_ptr<ExpectedStm>> expectedFunctionBodyStatements;

    expectedFunctionBodyStatements.push_back(
        std::make_unique<ExpectedStmReturn>(
            std::make_unique<ExpectedExprFloatLiteral>(1.0f)
        )
    );

    expectedFunctionDecls.push_back(
        std::make_unique<ExpectedFunctionDecl>(
                std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
                "foo",
            expectedParameters,
            std::make_unique<ExpectedStmBlock>(expectedFunctionBodyStatements)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(expectedFunctionDecls)),
        R"(
            float foo() {
                return 1;
            }
        )"
    );
}

TEST(EXPR_CONTEXT, STM_BLOCK) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedStm>> expectedBlockStatements;

    expectedBlockStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            std::make_unique<ExpectedExprFloatLiteral>(1.0f)
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedStmBlock>(
            expectedBlockStatements
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        R"(
            {
                float x = 1;
            }
        )"
    );
}

TEST(EXPR_CONTEXT, STM_ASSIGNMENT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 0),
            "x",
            nullptr
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedStmAssignment>(
            std::make_unique<ExpectedIdentifier>("x"),
            noExpectedIndices,
            compiler::AssignmentOperator::EQUAL,
            std::make_unique<ExpectedExprFloatLiteral>(1.0f)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        R"(
            float x;
            x = 1;
        )"
    );
}

TEST(EXPR_CONTEXT, STM_IF) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmIf>(
            std::make_unique<ExpectedExprBoolLiteral>(true),
            std::make_unique<ExpectedStmBlock>(
                noExpectedStms
            ),
            nullptr
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "if (true || false) {}"
    );
}

TEST(EXPR_CONTEXT, STM_WHILE) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmWhile>(
            std::make_unique<ExpectedExprBoolLiteral>(true),
            std::make_unique<ExpectedStmBlock>(
                noExpectedStms
            )
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "while (true || false) {}"
    );
}

TEST(EXPR_CONTEXT, STM_FOR_RANGE) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;

    expectedStatements.push_back(
        std::make_unique<ExpectedStmFor>(
            std::make_unique<ExpectedStmVarDecl>(
                std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
                "i",
                nullptr
            ),
            std::make_unique<ExpectedForRange>(
                std::make_unique<ExpectedExprIntegerLiteral>(97),
                std::make_unique<ExpectedExprIntegerLiteral>(97),
                std::make_unique<ExpectedExprIntegerLiteral>(97)
            ),
            std::make_unique<ExpectedStmBlock>(noExpectedStms)
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "for (int i : 'a' .. 'a' : 'a') {}"
    );
}

TEST(EXPR_CONTEXT, EXPR_NEW_ARRAY_INITIALISER_ELEMENT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedIndex>> arrayDimension;
    std::vector<ArrayInitialiserElement> arrayInitialiserElements;

    arrayDimension.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedExprIntegerLiteral>(1)
        )
    );

    arrayInitialiserElements.emplace_back(
        std::make_unique<ExpectedExprFloatLiteral>(1.0f)
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 1),
            "arr",
            std::make_unique<ExpectedExprNew>(
                std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 1),
                std::move(arrayDimension),
                std::make_unique<ExpectedArrayInitialiser>(
                    std::move(arrayInitialiserElements)
                )
            )
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float[] arr = new float[1]{1};"
    );
}

TEST(EXPR_CONTEXT, EXPR_NEW_NESTED_ARRAY_INITIALISER_ELEMENT) {
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedIndex>> arrayDimension;
    std::vector<ArrayInitialiserElement> arrayInitialiser;
    std::vector<ArrayInitialiserElement> nestedArrayInitialiser;

    arrayDimension.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedExprIntegerLiteral>(1)
        )
    );
    arrayDimension.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedExprIntegerLiteral>(1)
        )
    );

    nestedArrayInitialiser.push_back(
        std::make_unique<ExpectedExprFloatLiteral>(1.0f)
    );

    arrayInitialiser.emplace_back(
        std::make_unique<ExpectedArrayInitialiser>(std::move(nestedArrayInitialiser))
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedStmVarDecl>(
            std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 2),
            "arr",
            std::make_unique<ExpectedExprNew>(
                std::make_unique<Type>(compiler::FLOAT_TYPE_ID, 2),
                std::move(arrayDimension),
                std::make_unique<ExpectedArrayInitialiser>(
                    std::move(arrayInitialiser)
                )
            )
        )
    );

    ASSERT_GENERATED_AST_AFTER_CONSTANT_FOLDING_EQ(
        ExpectedProgram(std::move(expectedStatements), std::move(noExpectedFunctionDecls)),
        "float[][] arr = new float[1][1] {{1}};"
    );
}