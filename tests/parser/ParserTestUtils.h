#ifndef SVM_PARSERTESTUTILS_H
#define SVM_PARSERTESTUTILS_H

#include "../../src/compiler/Compiler.h"
#include "../../src/include/Error.h"

namespace parserTest {
    struct ExpectedBlock;

    struct ExpectedStm {
        virtual ~ExpectedStm() = default;
    };

    struct ExpectedExpr {
        virtual ~ExpectedExpr() = default;
    };

    struct ExpectedIntegerLiteral final : ExpectedExpr {
        const int value;

        ExpectedIntegerLiteral(const int value) : value(value) {}
    };

    struct ExpectedFloatLiteral final : ExpectedExpr {
        const float value;

        ExpectedFloatLiteral(const float value) : value(value) {}
    };

    struct ExpectedBoolLiteral final : ExpectedExpr {
        const bool value;

        ExpectedBoolLiteral(const bool value) : value(value) {}
    };

    struct ExpectedIdentifier final {
        const std::string name;

        ExpectedIdentifier(const std::string& name) : name(name) {}
    };

    struct ExpectedExprIdentifier final : ExpectedExpr {
        const std::string name;

        ExpectedExprIdentifier(const std::string& name) : name(name) {}
    };

    struct ExpectedVarAccess final {
        const std::string name;

        ExpectedVarAccess(const std::string& name) : name(name) {}
    };

    struct ExpectedFunctionCallExpr final : ExpectedExpr {
        const std::string functionName;
        const std::vector<std::unique_ptr<ExpectedExpr>> expectedArguments;

        ExpectedFunctionCallExpr(const std::string& functionName,
            std::vector<std::unique_ptr<ExpectedExpr>> expectedArguments) :
        functionName(functionName),
        expectedArguments(std::move(expectedArguments)) {}
    };

    struct ExpectedUnaryExpr final : ExpectedExpr {
        const compiler::UnaryOperator unaryOperator;
        const std::unique_ptr<ExpectedExpr> expectedExpr;

        ExpectedUnaryExpr(const compiler::UnaryOperator unaryOperator,
            std::unique_ptr<ExpectedExpr> expectedExpr) :
        unaryOperator(unaryOperator),
        expectedExpr(std::move(expectedExpr)) {}
    };

    struct ExpectedBinaryExpr final : ExpectedExpr {
        const compiler::BinaryOperator binaryOperator;
        const std::unique_ptr<ExpectedExpr> expectedLeftExpr;
        const std::unique_ptr<ExpectedExpr> expectedRightExpr;

        ExpectedBinaryExpr(const compiler::BinaryOperator binaryOperator,
            std::unique_ptr<ExpectedExpr> expectedLeftExpr,
            std::unique_ptr<ExpectedExpr> expectedRightExpr) :
        binaryOperator(binaryOperator),
        expectedLeftExpr(std::move(expectedLeftExpr)),
        expectedRightExpr(std::move(expectedRightExpr)) {}
    };

    struct ExpectedReturnStm final : ExpectedStm {
        const std::unique_ptr<ExpectedExpr> expectedReturnExpr;

        ExpectedReturnStm(std::unique_ptr<ExpectedExpr> expectedReturnExpr) :
        expectedReturnExpr(std::move(expectedReturnExpr)) {}
    };

    struct ExpectedFunctionCallStm final : ExpectedStm {
        const std::unique_ptr<ExpectedFunctionCallExpr> expectedFunctionCallExpr;

        ExpectedFunctionCallStm(std::unique_ptr<ExpectedFunctionCallExpr> expectedFunctionCallExpr) :
        expectedFunctionCallExpr(std::move(expectedFunctionCallExpr)) {}
    };

    struct ExpectedBreakStm final : ExpectedStm {};

    struct ExpectedContinueStm final : ExpectedStm {};

    struct ExpectedWhileStm final : ExpectedStm {
        const std::unique_ptr<ExpectedExpr> expectedCondition;
        const std::unique_ptr<ExpectedBlock> expectedBody;

        ExpectedWhileStm(std::unique_ptr<ExpectedExpr> expectedCondition,
            std::unique_ptr<ExpectedBlock> expectedBody) :
        expectedCondition(std::move(expectedCondition)),
        expectedBody(std::move(expectedBody)) {}
    };

    struct ExpectedIfStm final : ExpectedStm {
        const std::unique_ptr<ExpectedExpr> expectedCondition;
        const std::unique_ptr<ExpectedBlock> ifBlock;
        const std::unique_ptr<ExpectedStm> elseStm;

        ExpectedIfStm(std::unique_ptr<ExpectedExpr> conditionExpr,
            std::unique_ptr<ExpectedBlock> ifBlock,
            std::unique_ptr<ExpectedStm> elseStm) :
        expectedCondition(std::move(conditionExpr)),
        ifBlock(std::move(ifBlock)),
        elseStm(std::move(elseStm)) {}
    };

    struct ExpectedAssignment final : ExpectedStm {
        const std::unique_ptr<ExpectedVarAccess> expectedVarAccess;
        const compiler::AssignmentOperator assignmentOperator;
        const std::unique_ptr<ExpectedExpr> expectedExpr;

        ExpectedAssignment(std::unique_ptr<ExpectedVarAccess> expectedVarAccess,
            const compiler::AssignmentOperator assignmentOperator,
            std::unique_ptr<ExpectedExpr> expectedExpr) :
        expectedVarAccess(std::move(expectedVarAccess)),
        assignmentOperator(assignmentOperator),
        expectedExpr(std::move(expectedExpr)) {}
    };

    struct ExpectedVarDecl final : ExpectedStm {
        const compiler::Type expectedType;
        const std::string expectedName;
        const std::unique_ptr<ExpectedExpr> expectedOptionalInitialiser;

        ExpectedVarDecl(const compiler::Type expectedType,
            const std::string& expectedName,
            std::unique_ptr<ExpectedExpr> expectedOptionalInitialiser) :
        expectedType(expectedType),
        expectedName(expectedName),
        expectedOptionalInitialiser(std::move(expectedOptionalInitialiser)) {}
    };

    struct ExpectedBlock final : ExpectedStm {
        const std::vector<std::unique_ptr<ExpectedStm>> statements;

        ExpectedBlock(std::vector<std::unique_ptr<ExpectedStm>>& statements) :
        statements(std::move(statements)) {}
    };

    struct ExpectedParameter final {
        const compiler::Type expectedType;
        const std::string expectedParameterName;

        ExpectedParameter(const compiler::Type expectedType,
            const std::string& expectedParameterName) :
        expectedType(expectedType),
        expectedParameterName(expectedParameterName) {}
    };

    struct ExpectedFunctionDecl final {
        const compiler::Type expectedReturnType;
        const std::string expectedFunctionName;
        const std::vector<std::unique_ptr<ExpectedParameter>> expectedParameters;
        const std::unique_ptr<ExpectedBlock> expectedBody;

        ExpectedFunctionDecl(const compiler::Type expectedReturnType,
            const std::string& expectedFunctionName,
            std::vector<std::unique_ptr<ExpectedParameter>>& expectedParameters,
            std::unique_ptr<ExpectedBlock> expectedBody) :
        expectedReturnType(expectedReturnType),
        expectedFunctionName(expectedFunctionName),
        expectedParameters(std::move(expectedParameters)),
        expectedBody(std::move(expectedBody)) {}
    };

    struct ExpectedProgram final {
        const std::vector<std::unique_ptr<ExpectedStm>> expectedStatements;
        const std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls;

        ExpectedProgram(std::vector<std::unique_ptr<ExpectedStm>> expectedStatements,
            std::vector<std::unique_ptr<ExpectedFunctionDecl>> expectedFunctionDecls) :
        expectedStatements(std::move(expectedStatements)),
        expectedFunctionDecls(std::move(expectedFunctionDecls)) {}
    };

    inline std::unique_ptr<ast::Program> PARSE(
        const std::string& sourceCode
    ) {
        // parse source code
        return compiler::Compiler::testParsing(sourceCode);
    }

    inline void ASSERT_VAR_ACCESS_EQ(const ExpectedVarAccess& expectedVarAccess, const ast::VarAccess& actualVarAccess) {
        ASSERT_EQ(expectedVarAccess.name, actualVarAccess.identifier->name);
    }

    inline void ASSERT_EXPR_EQ(const ExpectedExpr& expectedExpr, const ast::Expr& actualExpr) {
        if (auto* expectedIntegerLiteral = dynamic_cast<const ExpectedIntegerLiteral*>(&expectedExpr)) {
            auto* actualIntegerLiteral = dynamic_cast<const ast::ExprIntegerLiteral*>(&actualExpr);
            ASSERT_NE(actualIntegerLiteral, nullptr);
            ASSERT_EQ(actualIntegerLiteral->value, expectedIntegerLiteral->value);

        } else if (auto* expectedFloatLiteral = dynamic_cast<const ExpectedFloatLiteral*>(&expectedExpr)) {
            auto* actualFloatLiteral = dynamic_cast<const ast::ExprFloatLiteral*>(&actualExpr);
            ASSERT_NE(actualFloatLiteral, nullptr);
            ASSERT_EQ(actualFloatLiteral->value, expectedFloatLiteral->value);

        } else if (auto* expectedBoolLiteral = dynamic_cast<const ExpectedBoolLiteral*>(&expectedExpr)) {
            auto* actualBoolLiteral = dynamic_cast<const ast::ExprBoolLiteral*>(&actualExpr);
            ASSERT_NE(actualBoolLiteral, nullptr);
            ASSERT_EQ(actualBoolLiteral->value, expectedBoolLiteral->value);

        } else if (auto* expectedIdentifier = dynamic_cast<const ExpectedExprIdentifier*>(&expectedExpr)) {
            auto* actualIdentifier = dynamic_cast<const ast::ExprIdentifier*>(&actualExpr);
            ASSERT_NE(actualIdentifier, nullptr);
            ASSERT_EQ(actualIdentifier->name, expectedIdentifier->name);

        } else if (auto* expectedBinaryExpr = dynamic_cast<const ExpectedBinaryExpr*>(&expectedExpr)) {
            auto* actualBinaryExpr = dynamic_cast<const ast::ExprBinaryOperator*>(&actualExpr);
            ASSERT_NE(actualBinaryExpr, nullptr);
            ASSERT_EQ(expectedBinaryExpr->binaryOperator, actualBinaryExpr->binaryOperatorInfo->binaryOperator);
            ASSERT_EXPR_EQ(*expectedBinaryExpr->expectedLeftExpr, *actualBinaryExpr->left);
            ASSERT_EXPR_EQ(*expectedBinaryExpr->expectedRightExpr, *actualBinaryExpr->right);

        } else if (auto* expectedUnaryExpr = dynamic_cast<const ExpectedUnaryExpr*>(&expectedExpr)) {
            auto* actualUnaryExpr = dynamic_cast<const ast::ExprUnaryOperator*>(&actualExpr);
            ASSERT_NE(actualUnaryExpr, nullptr);
            ASSERT_EQ(expectedUnaryExpr->unaryOperator, actualUnaryExpr->unaryOperatorInfo->unaryOperator);
            ASSERT_EXPR_EQ(*expectedUnaryExpr->expectedExpr, *actualUnaryExpr->expr);

        } else if (auto* expectedFunctionCallExpr = dynamic_cast<const ExpectedFunctionCallExpr*>(&expectedExpr)) {
            auto* actualFunctionCallExpr = dynamic_cast<const ast::FunctionCall*>(&actualExpr);
            ASSERT_NE(actualFunctionCallExpr, nullptr);
            ASSERT_EQ(expectedFunctionCallExpr->functionName, actualFunctionCallExpr->identifier->name);
            ASSERT_EQ(expectedFunctionCallExpr->expectedArguments.size(), actualFunctionCallExpr->arguments.size());
            for (size_t i = 0; i < expectedFunctionCallExpr->expectedArguments.size(); ++i) {
                ASSERT_EXPR_EQ(*expectedFunctionCallExpr->expectedArguments[i], *actualFunctionCallExpr->arguments[i]);
            }

        } else {
            FAIL();
        }
    }

    inline void ASSERT_VAR_DECL_EQ(const ExpectedVarDecl& expectedVarDecl, const ast::StmVarDecl& actualVarDecl) {
        ASSERT_EQ(expectedVarDecl.expectedType, actualVarDecl.typeInfo->type);
        ASSERT_EQ(expectedVarDecl.expectedName, actualVarDecl.identifier->name);

        if (expectedVarDecl.expectedOptionalInitialiser == nullptr) {
            ASSERT_EQ(actualVarDecl.optionalInitialiser, nullptr);
        } else {
            ASSERT_EXPR_EQ(*expectedVarDecl.expectedOptionalInitialiser, *actualVarDecl.optionalInitialiser);
        }
    }

    inline void ASSERT_STATEMENT_EQ(const ExpectedStm& expectedStm, const ast::Stm& actualStm) {
        if (auto* expectedBlock = dynamic_cast<const ExpectedBlock*>(&expectedStm)) {
            auto* actualBlock = dynamic_cast<const ast::Block*>(&actualStm);
            ASSERT_NE(actualBlock, nullptr);
            ASSERT_EQ(expectedBlock->statements.size(), actualBlock->statements.size());
            for (size_t i = 0; i < expectedBlock->statements.size(); ++i) {
                ASSERT_STATEMENT_EQ(*expectedBlock->statements[i], *actualBlock->statements[i]);
            }

        } else if (auto* expectedVarDecl = dynamic_cast<const ExpectedVarDecl*>(&expectedStm)) {
            auto* actualVarDecl = dynamic_cast<const ast::StmVarDecl*>(&actualStm);
            ASSERT_NE(actualVarDecl, nullptr);
            ASSERT_VAR_DECL_EQ(*expectedVarDecl, *actualVarDecl);

        } else if (auto* expectedAssignment = dynamic_cast<const ExpectedAssignment*>(&expectedStm)) {
            auto* actualAssignment = dynamic_cast<const ast::StmAssignment*>(&actualStm);
            ASSERT_NE(actualAssignment, nullptr);
            ASSERT_VAR_ACCESS_EQ(*expectedAssignment->expectedVarAccess, *actualAssignment->varAccess);
            ASSERT_EQ(expectedAssignment->assignmentOperator, actualAssignment->assignmentOperatorInfo->assignmentOperator);
            ASSERT_EXPR_EQ(*expectedAssignment->expectedExpr, *actualAssignment->expression);

        } else if (auto* expectedIfStm = dynamic_cast<const ExpectedIfStm*>(&expectedStm)) {
            auto* actualIfStm = dynamic_cast<const ast::IfStm*>(&actualStm);
            ASSERT_NE(actualIfStm, nullptr);
            ASSERT_EXPR_EQ(*expectedIfStm->expectedCondition, *actualIfStm->condition);
            ASSERT_EQ(expectedIfStm->ifBlock->statements.size(), actualIfStm->ifBlock->statements.size());
            ASSERT_STATEMENT_EQ(*expectedIfStm->ifBlock, *actualIfStm->ifBlock);
            if (expectedIfStm->elseStm == nullptr) {
                ASSERT_EQ(actualIfStm->elseStm, nullptr);
            } else {
                ASSERT_STATEMENT_EQ(*expectedIfStm->elseStm, *actualIfStm->elseStm);
            }

        } else if (auto* expectedWhileStm = dynamic_cast<const ExpectedWhileStm*>(&expectedStm)) {
            auto* actualWhileStm = dynamic_cast<const ast::WhileStm*>(&actualStm);
            ASSERT_NE(actualWhileStm, nullptr);
            ASSERT_EXPR_EQ(*expectedWhileStm->expectedCondition, *actualWhileStm->condition);
            ASSERT_EQ(expectedWhileStm->expectedBody->statements.size(), actualWhileStm->block->statements.size());
            ASSERT_STATEMENT_EQ(*expectedWhileStm->expectedBody, *actualWhileStm->block);

        } else if (auto* expectedContinueStm = dynamic_cast<const ExpectedContinueStm*>(&expectedStm)) {
            auto* actualContinueStm = dynamic_cast<const ast::ContinueStm*>(&actualStm);
            ASSERT_NE(actualContinueStm, nullptr);

        } else if (auto* expectedBreakStm = dynamic_cast<const ExpectedBreakStm*>(&expectedStm)) {
            auto* actualBreakStm = dynamic_cast<const ast::BreakStm*>(&actualStm);
            ASSERT_NE(actualBreakStm, nullptr);

        } else if (auto* expectedFunctionCallStm = dynamic_cast<const ExpectedFunctionCallStm*>(&expectedStm)) {
            auto* actualFunctionCallStm = dynamic_cast<const ast::FunctionCallStm*>(&actualStm);
            ASSERT_NE(actualFunctionCallStm, nullptr);
            ASSERT_EXPR_EQ(*expectedFunctionCallStm->expectedFunctionCallExpr, *actualFunctionCallStm->functionCall);

        } else if (auto* expectedReturnStm = dynamic_cast<const ExpectedReturnStm*>(&expectedStm)) {
            auto* actualReturnStm = dynamic_cast<const ast::ReturnStm*>(&actualStm);
            ASSERT_NE(actualReturnStm, nullptr);
            if (expectedReturnStm->expectedReturnExpr == nullptr) {
                ASSERT_EQ(actualReturnStm->returnExpression, nullptr);
            } else {
                ASSERT_EXPR_EQ(*expectedReturnStm->expectedReturnExpr, *actualReturnStm->returnExpression);
            }


        } else {
            FAIL();
        }
    }

    inline void ASSERT_PARAMETER_EQ(const ExpectedParameter& expectedParameter, const ast::Parameter& actualParameter) {
        ASSERT_EQ(expectedParameter.expectedType, actualParameter.typeInfo->type);
        ASSERT_EQ(expectedParameter.expectedParameterName, expectedParameter.expectedParameterName);
    }

    inline void ASSERT_FUNCTION_DECL_EQ(const ExpectedFunctionDecl& expectedFunctionDecl, const ast::FunctionDecl& actualFunctionDecl) {
        ASSERT_EQ(expectedFunctionDecl.expectedReturnType, actualFunctionDecl.returnTypeInfo->type);
        ASSERT_EQ(expectedFunctionDecl.expectedFunctionName, actualFunctionDecl.identifier->name);

        ASSERT_EQ(expectedFunctionDecl.expectedParameters.size(), actualFunctionDecl.parameters.size());
        for (size_t i = 0; i < expectedFunctionDecl.expectedParameters.size(); ++i) {
            ASSERT_PARAMETER_EQ(*expectedFunctionDecl.expectedParameters[i], *actualFunctionDecl.parameters[i]);
        }

        ASSERT_STATEMENT_EQ(*expectedFunctionDecl.expectedBody, *actualFunctionDecl.body);
    }

    inline void ASSERT_PROGRAM_EQ(const ExpectedProgram& expectedProgram, const ast::Program& actualProgram){
        ASSERT_EQ(expectedProgram.expectedStatements.size(), actualProgram.statements.size());
        ASSERT_EQ(expectedProgram.expectedFunctionDecls.size(), actualProgram.functionDecls.size());

        for (size_t i = 0; i < expectedProgram.expectedStatements.size(); i++) {
            ASSERT_STATEMENT_EQ(*expectedProgram.expectedStatements.at(i), *actualProgram.statements.at(i));
        }

        for (size_t i = 0; i < expectedProgram.expectedFunctionDecls.size(); i++) {
            ASSERT_FUNCTION_DECL_EQ(*expectedProgram.expectedFunctionDecls.at(i), *actualProgram.functionDecls.at(i));
        }
    };
};

#endif //SVM_PARSERTESTUTILS_H