#include <gtest/gtest.h>
#include "../ParserTestUtils.h"

using namespace parserTest;

TEST(EXPR_NEW, ONE_INDEX_WITH_NO_INITIALISER) {
    const auto testCode = R"(
        int[] x = new int[4];
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedIndex>> arrayDimension;

    arrayDimension.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedIntegerLiteral>(4),
                noExpectedIndices
            )
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 1),
            "x",
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedExprNew>(
                    std::make_unique<Type>(compiler::Type::INT, 1),
                    std::move(arrayDimension),
                    nullptr
                ),
                noExpectedIndices
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_NEW, TWO_INDICES_WITH_NO_INITIALISER) {
    const auto testCode = R"(
        int[] x = new int[4][2];
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedIndex>> arrayDimension;

    arrayDimension.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedIntegerLiteral>(4),
                noExpectedIndices
            )
        )
    );
    arrayDimension.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedIntegerLiteral>(2),
                noExpectedIndices
            )
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 1),
            "x",
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedExprNew>(
                    std::make_unique<Type>(compiler::Type::INT, 2),
                    std::move(arrayDimension),
                    nullptr
                ),
                noExpectedIndices
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_NEW, ONE_INDEX_WITH_EMPTY_INITIALISER) {
    const auto testCode = R"(
        int[] x = new int[4]{};
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedIndex>> arrayDimension;
    std::vector<ArrayInitialiserElement> arrayInitialiserElements;
    std::unique_ptr<ExpectedArrayInitialiser> expectedArrayInitialisers = std::make_unique<ExpectedArrayInitialiser>(std::move(arrayInitialiserElements));

    arrayDimension.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedIntegerLiteral>(4),
                noExpectedIndices
            )
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 1),
            "x",
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedExprNew>(
                    std::make_unique<Type>(compiler::Type::INT, 1),
                    std::move(arrayDimension),
                    std::move(expectedArrayInitialisers)
                ),
                noExpectedIndices
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}


TEST(EXPR_NEW, ONE_INDEX_WITH_INITIALISER) {
    const auto testCode = R"(
        int[] x = new int[4] {1, 2};
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedIndex>> arrayDimension;
    std::vector<ArrayInitialiserElement> arrayInitialiserElements;

    arrayDimension.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedIntegerLiteral>(4),
                noExpectedIndices
            )
        )
    );

    arrayInitialiserElements.emplace_back(
        std::make_unique<ExpectedExprPostfix>(
            std::make_unique<ExpectedIntegerLiteral>(1),
            noExpectedIndices
        )
    );
    arrayInitialiserElements.emplace_back(
        std::make_unique<ExpectedExprPostfix>(
            std::make_unique<ExpectedIntegerLiteral>(2),
            noExpectedIndices
        )
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 1),
            "x",
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedExprNew>(
                    std::make_unique<Type>(compiler::Type::INT, 1),
                    std::move(arrayDimension),
                    std::make_unique<ExpectedArrayInitialiser>(
                        std::move(arrayInitialiserElements)
                    )
                ),
                noExpectedIndices
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_NEW, ONE_INDEX_WITH_NESTED_INITIALISER) {
    const auto testCode = R"(
        int[] x = new int[4] {{1, 2}, {3, 4}};
    )";
    const auto program = PARSE(testCode);
    std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
    std::vector<std::unique_ptr<ExpectedIndex>> arrayDimension;
    std::vector<ArrayInitialiserElement> arrayInitialiser;
    std::vector<ArrayInitialiserElement> nestedArrayInitialiser1;
    std::vector<ArrayInitialiserElement> nestedArrayInitialiser2;

    arrayDimension.push_back(
        std::make_unique<ExpectedIndex>(
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedIntegerLiteral>(4),
                noExpectedIndices
            )
        )
    );

    nestedArrayInitialiser1.push_back(
        std::make_unique<ExpectedExprPostfix>(
            std::make_unique<ExpectedIntegerLiteral>(1),
            noExpectedIndices
        )
    );
    nestedArrayInitialiser1.push_back(
    std::make_unique<ExpectedExprPostfix>(
        std::make_unique<ExpectedIntegerLiteral>(2),
        noExpectedIndices
    )    );

    nestedArrayInitialiser2.push_back(
    std::make_unique<ExpectedExprPostfix>(
        std::make_unique<ExpectedIntegerLiteral>(3),
        noExpectedIndices
    )    );
    nestedArrayInitialiser2.push_back(
    std::make_unique<ExpectedExprPostfix>(
        std::make_unique<ExpectedIntegerLiteral>(4),
        noExpectedIndices
    )    );

    arrayInitialiser.emplace_back(
        std::make_unique<ExpectedArrayInitialiser>(std::move(nestedArrayInitialiser1))
    );
    arrayInitialiser.emplace_back(
        std::make_unique<ExpectedArrayInitialiser>(std::move(nestedArrayInitialiser2))
    );

    expectedStatements.push_back(
        std::make_unique<ExpectedVarDecl>(
            std::make_unique<Type>(compiler::Type::INT, 1),
            "x",
            std::make_unique<ExpectedExprPostfix>(
                std::make_unique<ExpectedExprNew>(
                    std::make_unique<Type>(compiler::Type::INT, 1),
                    std::move(arrayDimension),
                    std::make_unique<ExpectedArrayInitialiser>(
                        std::move(arrayInitialiser)
                    )
                ),
                noExpectedIndices
            )
        )
    );
    std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;
    const auto expectedProgram = std::make_unique<ExpectedProgram>(std::move(expectedStatements), std::move(expectedFunctionDecls));
    ASSERT_PROGRAM_EQ(*expectedProgram, *program);
}

TEST(EXPR_NEW, INVALID_MISSING_TYPE) {
    ASSERT_THROW(PARSE("int[] x = new [4];"), SyntaxError);
}

TEST(EXPR_NEW, INVALID_MISSING_LSQBR) {
    ASSERT_THROW(PARSE("int[] x = new int 4];"), SyntaxError);
}

TEST(EXPR_NEW, INVALID_MISSING_EXPR) {
    ASSERT_THROW(PARSE("int[] x = new int[];"), SyntaxError);
}

TEST(EXPR_NEW, INVALID_MISSING_RSQBR) {
    ASSERT_THROW(PARSE("int[] x = new int [4;"), SyntaxError);
}