#include <gtest/gtest.h>
#include "ParserTestUtils.h"

TEST(FUNCTION_DECL, VOID_FUNCTION_DECL) {
    const auto testCode = R"(
        void foo() {}
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedParameter>> expectedParameters;
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;

    expectedFunctionDecls.push_back(
        std::make_unique<parserTest::ExpectedFunctionDecl>(
            compiler::Type::VOID_RETURN_TYPE,
            "foo",
            expectedParameters,
            std::make_unique<parserTest::ExpectedBlock>(expectedStatements)
        )
    );

    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(FUNCTION_DECL, INT_FUNCTION_DECL) {
    const auto testCode = R"(
        int foo() {}
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedParameter>> expectedParameters;
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;

    expectedFunctionDecls.push_back(
        std::make_unique<parserTest::ExpectedFunctionDecl>(
            compiler::Type::INT,
            "foo",
            expectedParameters,
            std::make_unique<parserTest::ExpectedBlock>(expectedStatements)
        )
    );

    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(FUNCTION_DECL, FLOAT_FUNCTION_DECL) {
    const auto testCode = R"(
        float foo() {}
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedParameter>> expectedParameters;
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;

    expectedFunctionDecls.push_back(
        std::make_unique<parserTest::ExpectedFunctionDecl>(
            compiler::Type::FLOAT,
            "foo",
            expectedParameters,
            std::make_unique<parserTest::ExpectedBlock>(expectedStatements)
        )
    );

    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(FUNCTION_DECL, BOOL_FUNCTION_DECL) {
    const auto testCode = R"(
        bool foo() {}
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedParameter>> expectedParameters;
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;

    expectedFunctionDecls.push_back(
        std::make_unique<parserTest::ExpectedFunctionDecl>(
            compiler::Type::BOOL,
            "foo",
            expectedParameters,
            std::make_unique<parserTest::ExpectedBlock>(expectedStatements)
        )
    );

    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(FUNCTION_DECL, FUNCTION_DECL_ONE_PARAMETER) {
    const auto testCode = R"(
        bool foo(int a) {}
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedParameter>> expectedParameters;
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;

    expectedParameters.push_back(
        std::make_unique<parserTest::ExpectedParameter>(
            compiler::Type::INT,
            "a"
        )
    );

    expectedFunctionDecls.push_back(
        std::make_unique<parserTest::ExpectedFunctionDecl>(
            compiler::Type::BOOL,
            "foo",
            expectedParameters,
            std::make_unique<parserTest::ExpectedBlock>(expectedStatements)
        )
    );

    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(FUNCTION_DECL, FUNCTION_DECL_MULTIPLE_PARAMETERS) {
    const auto testCode = R"(
        bool foo(int a, float f, bool b) {}
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedParameter>> expectedParameters;
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;

    expectedParameters.push_back(
        std::make_unique<parserTest::ExpectedParameter>(
            compiler::Type::INT,
            "a"
        )
    );
    expectedParameters.push_back(
        std::make_unique<parserTest::ExpectedParameter>(
            compiler::Type::FLOAT,
            "f"
        )
    );
    expectedParameters.push_back(
        std::make_unique<parserTest::ExpectedParameter>(
            compiler::Type::BOOL,
            "b"
        )
    );

    expectedFunctionDecls.push_back(
        std::make_unique<parserTest::ExpectedFunctionDecl>(
            compiler::Type::BOOL,
            "foo",
            expectedParameters,
            std::make_unique<parserTest::ExpectedBlock>(expectedStatements)
        )
    );

    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(FUNCTION_DECL, FUNCTION_DECL_WITH_BODY) {
    const auto testCode = R"(
        void foo() {
            int x;
        }
    )";
    const auto program = parserTest::PARSE(testCode);
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedParameter>> expectedParameters;
    std::vector<std::unique_ptr<parserTest::ExpectedStm>> expectedFunctionStatements;
    std::vector<std::unique_ptr<parserTest::ExpectedFunctionDecl>> expectedFunctionDecls;

    expectedFunctionStatements.push_back(
        std::make_unique<parserTest::ExpectedVarDecl>(
            compiler::Type::INT,
            "x",
            nullptr
        )
    );

    expectedFunctionDecls.push_back(
        std::make_unique<parserTest::ExpectedFunctionDecl>(
            compiler::Type::VOID_RETURN_TYPE,
            "foo",
            expectedParameters,
            std::make_unique<parserTest::ExpectedBlock>(expectedFunctionStatements)
        )
    );

    const auto expectedProgram = std::make_unique<parserTest::ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    parserTest::ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(FUNCTION_DECL, INVALID_MISSING_IDENTIFIER) {
    const auto testCode = R"(
        int () {}
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(FUNCTION_DECL, INVALID_MISSING_LBR) {
    const auto testCode = R"(
        int foo) {}
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(FUNCTION_DECL, INVALID_MISSING_RBR) {
    const auto testCode = R"(
        int foo( {}
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(FUNCTION_DECL, INVALID_MISSING_LCBR) {
    const auto testCode = R"(
        int foo() }
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(FUNCTION_DECL, INVALID_MISSING_RCBR) {
    const auto testCode = R"(
        int foo() {
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(FUNCTION_DECL, INVALID_MISSING_PARAMETER_IDENTIFIER) {
    const auto testCode = R"(
        int foo(int) {}
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(FUNCTION_DECL, INVALID_MISSING_PARAMETER_TYPE) {
    const auto testCode = R"(
        int foo(a) {}
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(FUNCTION_DECL, INVALID_MISSING_PARAMETER_COMMA) {
    const auto testCode = R"(
        int foo(int a int b) {}
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(FUNCTION_DECL, INVALID_MISSING_PARAMETER_AFTER_COMMA) {
    const auto testCode = R"(
        int foo(int a,) {}
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}

TEST(FUNCTION_DECL, INVALID_MISSING_PARAMETER_BEFORE_COMMA) {
    const auto testCode = R"(
        int foo(,int b) {}
    )";
    ASSERT_THROW(parserTest::PARSE(testCode), SyntaxError);
}
