#include <gtest/gtest.h>
#include "../ParserTestUtils.h"

using namespace parserTest;

TEST(IF, DECL_ITERABLE) {
    const auto program = PARSE(R"(
        for (int i : iterable) {}
    )");
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedStm>> expectedForBody;
    expectedStatements.push_back(
        std::make_unique<ExpectedForStm>(
            std::make_unique<ExpectedVarDecl>(
                std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
                "i",
                nullptr
            ),
            std::make_unique<ExpectedExprIdentifier>(
                std::make_unique<ExpectedIdentifier>("iterable")
            ),
            std::make_unique<ExpectedBlock>(expectedForBody)
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(IF, DECL_ITERABLE_BODY) {
    const auto program = PARSE(R"(
        for (int i : iterable) {
            int x;
        }
    )");
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedStm>> expectedForBody;
    expectedForBody.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
            "x",
            nullptr
        )
    );
    expectedStatements.push_back(
        std::make_unique<ExpectedForStm>(
            std::make_unique<ExpectedVarDecl>(
                std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
                "i",
                nullptr
            ),
            std::make_unique<ExpectedExprIdentifier>(
                std::make_unique<ExpectedIdentifier>("iterable")
            ),
            std::make_unique<ExpectedBlock>(expectedForBody)
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(IF, DECL_RANGE_NO_STEP) {
    const auto program = PARSE(R"(
        for (int i: 0..10) {}
    )");
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedStm>> expectedForBody;
    expectedStatements.push_back(
        std::make_unique<ExpectedForStm>(
            std::make_unique<ExpectedVarDecl>(
                std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
                "i",
                nullptr
            ),
            std::make_unique<ExpectedForRange>(
                std::make_unique<ExpectedIntegerLiteral>(0),
                std::make_unique<ExpectedIntegerLiteral>(10),
                nullptr
            ),
            std::make_unique<ExpectedBlock>(expectedForBody)
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(IF, DECL_RANGE_STEP) {
    const auto program = PARSE(R"(
        for (int i: 0..10 : 2) {}
    )");
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedStm>> expectedForBody;
    expectedStatements.push_back(
        std::make_unique<ExpectedForStm>(
            std::make_unique<ExpectedVarDecl>(
                std::make_unique<Type>(compiler::INT_TYPE_ID, 0),
                "i",
                nullptr
            ),
            std::make_unique<ExpectedForRange>(
                std::make_unique<ExpectedIntegerLiteral>(0),
                std::make_unique<ExpectedIntegerLiteral>(10),
                std::make_unique<ExpectedIntegerLiteral>(2)
            ),
            std::make_unique<ExpectedBlock>(expectedForBody)
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(IF, NO_DECL_ITERABLE) {
    const auto program = PARSE(R"(
        for (i : iterable) {}
    )");
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedStm>> expectedForBody;
    expectedStatements.push_back(
        std::make_unique<ExpectedForStm>(
            std::make_unique<ExpectedExprIdentifier>(
                std::make_unique<ExpectedIdentifier>("i")
            ),
            std::make_unique<ExpectedExprIdentifier>(
                std::make_unique<ExpectedIdentifier>("iterable")
            ),
            std::make_unique<ExpectedBlock>(expectedForBody)
        )
    );
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(noExpectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(IF, INVALID_MISSING_LBR) {
    ASSERT_THROW(PARSE(
            R"(
                for i : i) {}
            )"
        ),
        SyntaxError
    );
}

TEST(IF, INVALID_MISSING_VARIABLE) {
    ASSERT_THROW(PARSE(
            R"(
                for ( : i) {}
            )"
        ),
        SyntaxError
    );
}

TEST(IF, INVALID_MISSING_FIRST_COLON) {
    ASSERT_THROW(PARSE(
            R"(
                for (i  i) {}
            )"
        ),
        SyntaxError
    );
}

TEST(IF, INVALID_MISSING_ITERABLE) {
    ASSERT_THROW(PARSE(
            R"(
                for (i : ) {}
            )"
        ),
        SyntaxError
    );
}

TEST(IF, INVALID_MISSING_RANGE_START) {
    ASSERT_THROW(PARSE(
            R"(
                for (i :  .. 10) {}
            )"
        ),
        SyntaxError
    );
}

TEST(IF, INVALID_MISSING_DOT_DOT) {
    ASSERT_THROW(PARSE(
            R"(
                for (i : 0 10) {}
            )"
        ),
        SyntaxError
    );
}

TEST(IF, INVALID_MISSING_RANGE_END) {
    ASSERT_THROW(PARSE(
            R"(
                for (i : 0 .. ) {}
            )"
        ),
        SyntaxError
    );
}

TEST(IF, INVALID_MISSING_SECOND_COLON) {
    ASSERT_THROW(PARSE(
            R"(
                for (i : 0 .. 10  2) {}
            )"
        ),
        SyntaxError
    );
}

TEST(IF, INVALID_MISSING_RANGE_STEP) {
    ASSERT_THROW(PARSE(
            R"(
                for (i : 0 .. 10 :) {}
            )"
        ),
        SyntaxError
    );
}

TEST(IF, INVALID_MISSING_RBR) {
    ASSERT_THROW(PARSE(
            R"(
                for (i : i {}
            )"
        ),
        SyntaxError
    );
}

TEST(IF, INVALID_MISSING_LCBR) {
    ASSERT_THROW(PARSE(
            R"(
                for (i : i) }
            )"
        ),
        SyntaxError
    );
}

TEST(IF, INVALID_MISSING_RCBR) {
    ASSERT_THROW(PARSE(
            R"(
                for (i : i) {
            )"
        ),
        SyntaxError
    );
}