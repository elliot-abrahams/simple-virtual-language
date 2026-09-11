#include <gtest/gtest.h>
#include "ParserTestUtils.h"

using namespace parserTest;

TEST(FUNCTION_DECL, VOID_FUNCTION_DECL) {
    const auto testCode = R"(
        void foo() {}
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedParameter>> expectedParameters;
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;

    expectedFunctionDecls.push_back(
        std::make_unique<ExpectedFunctionDecl>(
            std::make_unique<Type>(compiler::Type::VOID_RETURN_TYPE, 0),
            "foo",
            expectedParameters,
            std::make_unique<ExpectedBlock>(expectedStatements)
        )
    );

    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(FUNCTION_DECL, INT_FUNCTION_DECL) {
    const auto testCode = R"(
        int foo() {}
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedParameter>> expectedParameters;
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;

    expectedFunctionDecls.push_back(
        std::make_unique<ExpectedFunctionDecl>(
                std::make_unique<Type>(compiler::Type::INT, 0),
                "foo",
            expectedParameters,
            std::make_unique<ExpectedBlock>(expectedStatements)
        )
    );

    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(FUNCTION_DECL, FLOAT_FUNCTION_DECL) {
    const auto testCode = R"(
        float foo() {}
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedParameter>> expectedParameters;
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;

    expectedFunctionDecls.push_back(
        std::make_unique<ExpectedFunctionDecl>(
                std::make_unique<Type>(compiler::Type::FLOAT, 0),
                "foo",
            expectedParameters,
            std::make_unique<ExpectedBlock>(expectedStatements)
        )
    );

    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(FUNCTION_DECL, BOOL_FUNCTION_DECL) {
    const auto testCode = R"(
        bool foo() {}
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedParameter>> expectedParameters;
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;

    expectedFunctionDecls.push_back(
        std::make_unique<ExpectedFunctionDecl>(
                std::make_unique<Type>(compiler::Type::BOOL, 0),
                "foo",
            expectedParameters,
            std::make_unique<ExpectedBlock>(expectedStatements)
        )
    );

    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(FUNCTION_DECL, FUNCTION_DECL_ONE_PARAMETER) {
    const auto testCode = R"(
        bool foo(int a) {}
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedParameter>> expectedParameters;
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;

    expectedParameters.push_back(
        std::make_unique<ExpectedParameter>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "a"
        )
    );

    expectedFunctionDecls.push_back(
        std::make_unique<ExpectedFunctionDecl>(
            std::make_unique<Type>(compiler::Type::BOOL, 0),
            "foo",
            expectedParameters,
            std::make_unique<ExpectedBlock>(expectedStatements)
        )
    );

    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(FUNCTION_DECL, FUNCTION_DECL_MULTIPLE_PARAMETERS) {
    const auto testCode = R"(
        bool foo(int a, float f, bool b) {}
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedParameter>> expectedParameters;
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;

    expectedParameters.push_back(
        std::make_unique<ExpectedParameter>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "a"
        )
    );
    expectedParameters.push_back(
        std::make_unique<ExpectedParameter>(
            std::make_unique<Type>(compiler::Type::FLOAT, 0),
            "f"
        )
    );
    expectedParameters.push_back(
        std::make_unique<ExpectedParameter>(
            std::make_unique<Type>(compiler::Type::BOOL, 0),
            "b"
        )
    );

    expectedFunctionDecls.push_back(
        std::make_unique<ExpectedFunctionDecl>(
            std::make_unique<Type>(compiler::Type::BOOL, 0),
            "foo",
            expectedParameters,
            std::make_unique<ExpectedBlock>(expectedStatements)
        )
    );

    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(FUNCTION_DECL, FUNCTION_DECL_WITH_BODY) {
    const auto testCode = R"(
        void foo() {
            int x;
        }
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedParameter>> expectedParameters;
    std::vector<std::unique_ptr<ExpectedStm>> expectedFunctionStatements;
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;

    expectedFunctionStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 0),
            "x",
            nullptr
        )
    );

    expectedFunctionDecls.push_back(
        std::make_unique<ExpectedFunctionDecl>(
            std::make_unique<Type>(compiler::Type::VOID_RETURN_TYPE, 0),
            "foo",
            expectedParameters,
            std::make_unique<ExpectedBlock>(expectedFunctionStatements)
        )
    );

    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(FUNCTION_DECL, INVALID_MISSING_IDENTIFIER) {
    const auto testCode = R"(
        int () {}
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(FUNCTION_DECL, INVALID_MISSING_LBR) {
    const auto testCode = R"(
        int foo) {}
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(FUNCTION_DECL, INVALID_MISSING_RBR) {
    const auto testCode = R"(
        int foo( {}
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(FUNCTION_DECL, INVALID_MISSING_LCBR) {
    const auto testCode = R"(
        int foo() }
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(FUNCTION_DECL, INVALID_MISSING_RCBR) {
    const auto testCode = R"(
        int foo() {
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(FUNCTION_DECL, INVALID_MISSING_PARAMETER_IDENTIFIER) {
    const auto testCode = R"(
        int foo(int) {}
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(FUNCTION_DECL, INVALID_MISSING_PARAMETER_TYPE) {
    const auto testCode = R"(
        int foo(a) {}
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(FUNCTION_DECL, INVALID_MISSING_PARAMETER_COMMA) {
    const auto testCode = R"(
        int foo(int a int b) {}
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(FUNCTION_DECL, INVALID_MISSING_PARAMETER_AFTER_COMMA) {
    const auto testCode = R"(
        int foo(int a,) {}
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}

TEST(FUNCTION_DECL, INVALID_MISSING_PARAMETER_BEFORE_COMMA) {
    const auto testCode = R"(
        int foo(,int b) {}
    )";
    ASSERT_THROW(PARSE(testCode), SyntaxError);
}
