#ifndef SVM_AST_H
#define SVM_AST_H
#include <memory>
#include <vector>
#include <string>

#include "SymbolTable.h"
#include "LanguageTypes.h"

namespace compiler {
    struct Scope;
}

namespace ast {
    struct Block;

    struct ASTNode {
        const size_t line;
        const size_t column;

        ASTNode(const size_t line, const size_t column) :
            line(line), column(column) {}

        virtual ~ASTNode() = default;
    };

    struct Stm : ASTNode {
        using ASTNode::ASTNode;
        virtual ~Stm() = default;
    };

    struct Expr : ASTNode {
        mutable compiler::Type resultingType;
        using ASTNode::ASTNode;
        virtual ~Expr() = default;
    };

    struct ExprIntegerLiteral final : Expr {
        const int value;

        ExprIntegerLiteral(const size_t line, const size_t column, const int value) :
            Expr(line, column),
            value(value) {}
    };

    struct ExprFloatLiteral final : Expr {
        const float value;

        ExprFloatLiteral(const size_t line, const size_t column, const float value) :
            Expr(line, column),
            value(value) {}
    };

    struct ExprBoolLiteral final : Expr {
        const bool value;

        ExprBoolLiteral(const size_t line, const size_t column, const bool value) :
            Expr(line, column),
            value(value) {}
    };

    struct TypeInfo final : ASTNode {
        const compiler::Type type;

        TypeInfo(const size_t line, const size_t column, const compiler::Type type) :
            ASTNode(line, column), type(type) {}
    };

    struct AssignmentOperatorInfo final : ASTNode {
        const compiler::AssignmentOperator assignmentOperator;

        AssignmentOperatorInfo(const size_t line, const size_t column, const compiler::AssignmentOperator assignmentOperator) :
            ASTNode(line, column), assignmentOperator(assignmentOperator) {}
    };

    struct BinaryOperatorInfo final : ASTNode {
        const compiler::BinaryOperator binaryOperator;

        BinaryOperatorInfo(const size_t line, const size_t column, const compiler::BinaryOperator binaryOperator) :
            ASTNode(line, column), binaryOperator(binaryOperator) {}
    };

    struct UnaryOperatorInfo final : ASTNode {
        const compiler::UnaryOperator unaryOperator;

        UnaryOperatorInfo(const size_t line, const size_t column, const compiler::UnaryOperator unaryOperator) :
            ASTNode(line, column), unaryOperator(unaryOperator) {}
    };

    struct Identifier final : ASTNode {
        const std::string name;

        Identifier(const size_t line, const size_t column, std::string name) :
            ASTNode(line, column),
            name(std::move(name)) {}
    };

    struct ExprIdentifier final : Expr {
        const std::string name;

        ExprIdentifier(const size_t line, const size_t column, std::string name) :
            Expr(line, column),
            name(std::move(name)) {}
    };

    struct VarAccess final : ASTNode {
        const std::unique_ptr<Identifier> identifier;

        VarAccess(const size_t line, const size_t column, std::unique_ptr<Identifier> identifier) :
            ASTNode(line, column),
            identifier(std::move(identifier)) {}
    };

    struct FunctionCall final : Expr {
        const std::unique_ptr<Identifier> identifier;
        const std::vector<std::unique_ptr<Expr>> arguments;
        mutable compiler::FunctionSymbol* functionSymbol = nullptr;

        FunctionCall(const size_t line,
                    const size_t column,
                    std::unique_ptr<Identifier> identifier,
                    std::vector<std::unique_ptr<Expr>> arguments) :
            Expr(line, column),
            identifier(std::move(identifier)),
            arguments(std::move(arguments)) {}
    };

    struct ExprUnaryOperator final : Expr {
        const std::unique_ptr<UnaryOperatorInfo> unaryOperatorInfo;
        const std::unique_ptr<Expr> expr;

        ExprUnaryOperator(const size_t line,
                    const size_t column,
                    std::unique_ptr<UnaryOperatorInfo> unaryOperatorInfo,
                    std::unique_ptr<Expr> expr) :
            Expr(line, column),
            unaryOperatorInfo(std::move(unaryOperatorInfo)),
            expr(std::move(expr)) {}
    };

    struct ExprBinaryOperator final : Expr {
        const std::unique_ptr<Expr> left;
        const std::unique_ptr<BinaryOperatorInfo> binaryOperatorInfo;
        const std::unique_ptr<Expr> right;

        ExprBinaryOperator(const size_t line,
                    const size_t column,
                    std::unique_ptr<Expr> left,
                    std::unique_ptr<BinaryOperatorInfo> binaryOperatorInfo,
                    std::unique_ptr<Expr> right) :
            Expr(line, column),
            left(std::move(left)),
            binaryOperatorInfo(std::move(binaryOperatorInfo)),
            right(std::move(right)) {}
    };

    struct ReturnStm final : Stm {
        const std::unique_ptr<Expr> returnExpression;

        ReturnStm(const size_t line,
                const size_t column,
                std::unique_ptr<Expr> returnExpression) :
            Stm(line, column),
            returnExpression(std::move(returnExpression)) {}
    };

    struct FunctionCallStm final : Stm {
        const std::unique_ptr<FunctionCall> functionCall;

        FunctionCallStm(const size_t line,
                    const size_t column,
                    std::unique_ptr<FunctionCall> functionCall) :
            Stm(line, column),
            functionCall(std::move(functionCall)) {}
    };

    struct BreakStm final : Stm {
        BreakStm(const size_t line,
                const size_t column) :
            Stm(line, column) {}
    };

    struct ContinueStm final : Stm {
        ContinueStm(const size_t line,
                    const size_t column) :
            Stm(line, column) {}
    };

    struct WhileStm final : Stm {
        const std::unique_ptr<Expr> condition;
        const std::unique_ptr<Block> block;

        WhileStm(const size_t line,
                const size_t column,
                std::unique_ptr<Expr> condition,
                std::unique_ptr<Block> block) :
            Stm(line, column),
            condition(std::move(condition)),
            block(std::move(block)) {}
    };

    struct IfStm final : Stm {
        const std::unique_ptr<Expr> condition;
        const std::unique_ptr<Block> ifBlock;
        const std::unique_ptr<Stm> elseStm;

        IfStm(const size_t line,
                const size_t column,
                std::unique_ptr<Expr> condition,
                std::unique_ptr<Block> ifBlock,
                std::unique_ptr<Stm> elseStm) :
            Stm(line, column),
            condition(std::move(condition)),
            ifBlock(std::move(ifBlock)),
            elseStm(std::move(elseStm)) {}
    };

    struct StmAssignment final : Stm {
        const std::unique_ptr<VarAccess> varAccess;
        const std::unique_ptr<AssignmentOperatorInfo> assignmentOperatorInfo;
        const std::unique_ptr<Expr> expression;

        StmAssignment(const size_t line,
                    const size_t column,
                    std::unique_ptr<VarAccess> varAccess,
                    std::unique_ptr<AssignmentOperatorInfo> assignmentOperatorInfo,
                    std::unique_ptr<Expr> expression) :
            Stm(line, column),
            varAccess(std::move(varAccess)),
            assignmentOperatorInfo(std::move(assignmentOperatorInfo)),
            expression(std::move(expression)) {}
    };

    struct StmVarDecl final : Stm {
        const std::unique_ptr<TypeInfo> typeInfo;
        const std::unique_ptr<Identifier> identifier;
        const std::unique_ptr<Expr> optionalInitialiser;

        StmVarDecl(const size_t line,
                    const size_t column,
                    std::unique_ptr<TypeInfo> typeInfo,
                    std::unique_ptr<Identifier> identifier,
                    std::unique_ptr<Expr> optionalInitialiser) :
            Stm(line, column),
            typeInfo(std::move(typeInfo)),
            identifier(std::move(identifier)),
            optionalInitialiser(std::move(optionalInitialiser)) {}
    };

    struct Block : Stm {
        const std::vector<std::unique_ptr<Stm>> statements;
        mutable compiler::Scope* scope;

        Block(const size_t line,
                    const size_t column,
                    std::vector<std::unique_ptr<Stm>> statements,
                    compiler::Scope* scope) :
            Stm(line, column),
            statements(std::move(statements)),
            scope(scope) {}
    };

    struct Parameter final : ASTNode {
        const std::unique_ptr<TypeInfo> typeInfo;
        const std::unique_ptr<Identifier> identifier;

        Parameter(const size_t line,
                const size_t column,
                std::unique_ptr<TypeInfo> typeInfo,
                std::unique_ptr<Identifier> identifier) :
            ASTNode(line, column),
            typeInfo(std::move(typeInfo)),
            identifier(std::move(identifier)) {}
    };

    struct FunctionDecl final : ASTNode {
        const std::unique_ptr<TypeInfo> returnTypeInfo;
        const std::unique_ptr<Identifier> identifier;
        const std::vector<std::unique_ptr<Parameter>> parameters;
        const std::unique_ptr<Block> body;

        FunctionDecl(const size_t line,
                    const size_t column,
                    std::unique_ptr<TypeInfo> returnTypeInfo,
                    std::unique_ptr<Identifier> identifier,
                    std::vector<std::unique_ptr<Parameter>> parameters,
                    std::unique_ptr<Block> block) :
            ASTNode(line, column),
            returnTypeInfo(std::move(returnTypeInfo)),
            identifier(std::move(identifier)),
            parameters(std::move(parameters)),
            body(std::move(block)) {}
    };

    struct Program {
        const std::vector<std::unique_ptr<Stm>> statements;
        const std::vector<std::unique_ptr<FunctionDecl>> functionDecls;

        explicit Program(std::vector<std::unique_ptr<Stm>> statements,
                        std::vector<std::unique_ptr<FunctionDecl>> functionDecls) :
            statements(std::move(statements)),
            functionDecls(std::move(functionDecls)){}
    };

}

#endif //SVM_AST_H