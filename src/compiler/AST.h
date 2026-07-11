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

    struct TypeInfo final : ASTNode {
        const Type type;

        TypeInfo(const size_t line, const size_t column, const Type type) :
            ASTNode(line, column), type(type) {}
    };

    struct Identifier final : ASTNode {
        const std::string name;

        Identifier(const size_t line, const size_t column, std::string name) :
            ASTNode(line, column),
            name(std::move(name)) {}
    };

    struct VarAccess final : ASTNode {
        const Identifier identifier;

        VarAccess(const size_t line, const size_t column, const Identifier &identifier) :
            ASTNode(line, column),
            identifier(std::move(identifier)) {}
    };

    struct ExprIdentifier final : Expr {
        const std::string name;

        ExprIdentifier(const size_t line, const size_t column, std::string name) :
            Expr(line, column),
            name(std::move(name)) {}
    };

    struct ExprIntegerLiteral final : Expr {
        const int value;

        ExprIntegerLiteral(const size_t line, const size_t column, const int value) :
            Expr(line, column),
            value(value) {}
    };

    struct StmVarDecl final : Stm {
        const TypeInfo typeInfo;
        const Identifier identifier;
        const std::unique_ptr<Expr> optionalInitializer;

        StmVarDecl(const size_t line,
                    const size_t column,
                    TypeInfo typeInfo,
                    Identifier identifier,
                    std::unique_ptr<Expr> optionalInitializer) :
            Stm(line, column),
            typeInfo(std::move(typeInfo)),
            identifier(std::move(identifier)),
            optionalInitializer(std::move(optionalInitializer)) {}
    };

    struct StmAssignment final : Stm {
        const std::unique_ptr<VarAccess> varAccess;
        const AssignmentOperator assignmentOperator;
        const std::unique_ptr<Expr> expression;

        StmAssignment(const size_t line,
                    const size_t column,
                    std::unique_ptr<VarAccess> varAccess,
                    const AssignmentOperator assignmentOperator,
                    std::unique_ptr<Expr> expression) :
            Stm(line, column),
            varAccess(std::move(varAccess)),
            assignmentOperator(assignmentOperator),
            expression(std::move(expression)) {}
    };

    struct Program {
        std::vector<std::unique_ptr<Stm>> statements;

        explicit Program(std::vector<std::unique_ptr<Stm>> statements) :
            statements(std::move(statements)) {}
    };

}

#endif //SVM_AST_H