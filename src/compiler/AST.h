#ifndef SVM_AST_H
#define SVM_AST_H
#include <memory>
#include <vector>
#include <string>

namespace ast {

    enum class Type {
        INT
    };

    enum class AssignmentOperator {
        EQUAL
    };

    enum class BinaryOperator {
        PLUS,
        MINUS,
        MULTIPLY,
        DIVIDE,
        MODULO
    };

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
        using ASTNode::ASTNode;
        virtual ~Expr() = default;
    };

    struct ExprIntegerLiteral final : Expr {
        const int value;

        ExprIntegerLiteral(const size_t line, const size_t column, const int value) :
            Expr(line, column),
            value(value) {}
    };

    struct TypeInfo final : ASTNode {
        const Type type;

        TypeInfo(const size_t line, const size_t column, const Type type) :
            ASTNode(line, column), type(type) {}
    };

    struct AssignmentOperatorInfo final : ASTNode {
        const AssignmentOperator assignmentOperator;

        AssignmentOperatorInfo(const size_t line, const size_t column, const AssignmentOperator assignmentOperator) :
            ASTNode(line, column), assignmentOperator(assignmentOperator) {}
    };

    struct BinaryOperatorInfo final : ASTNode {
        const BinaryOperator binaryOperator;

        BinaryOperatorInfo(const size_t line, const size_t column, const BinaryOperator binaryOperator) :
            ASTNode(line, column), binaryOperator(binaryOperator) {}
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
        const Identifier identifier;

        VarAccess(const size_t line, const size_t column, const Identifier& identifier) :
            ASTNode(line, column),
            identifier(identifier) {}
    };

    struct ExprUnaryOperator final : Expr {
        const std::unique_ptr<BinaryOperatorInfo> binaryOperatorInfo;
        const std::unique_ptr<Expr> expr;

        ExprUnaryOperator(const size_t line,
                    const size_t column,
                    std::unique_ptr<BinaryOperatorInfo> binaryOperatorInfo,
                    std::unique_ptr<Expr> expr) :
            Expr(line, column),
            binaryOperatorInfo(std::move(binaryOperatorInfo)),
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
        const TypeInfo typeInfo;
        const Identifier identifier;
        const std::unique_ptr<Expr> optionalInitialiser;

        StmVarDecl(const size_t line,
                    const size_t column,
                    TypeInfo typeInfo,
                    Identifier identifier,
                    std::unique_ptr<Expr> optionalInitialiser) :
            Stm(line, column),
            typeInfo(std::move(typeInfo)),
            identifier(std::move(identifier)),
            optionalInitialiser(std::move(optionalInitialiser)) {}
    };

    struct Program {
        std::vector<std::unique_ptr<Stm>> statements;

        explicit Program(std::vector<std::unique_ptr<Stm>> statements) :
            statements(std::move(statements)) {}
    };

}

#endif //SVM_AST_H