#ifndef SVM_AST_H
#define SVM_AST_H
#include <memory>
#include <utility>
#include <vector>
#include <string>
#include <variant>

#include "SymbolTable.h"
#include "LanguageTypes.h"

namespace compiler {
    struct Scope;
}

namespace ast {
    struct Index;
    struct ArrayInitialiser;
    struct Block;

    struct ASTNode {
        const uint32_t line;
        const uint16_t column;

        ASTNode(const uint32_t line, const uint16_t column) :
            line(line), column(column) {}

        virtual ~ASTNode() = default;
    };

    struct Stm : ASTNode {
        using ASTNode::ASTNode;
        virtual ~Stm() = default;
    };

    struct Expr : ASTNode {
        mutable compiler::SemanticType resultingType;
        using ASTNode::ASTNode;
        virtual ~Expr() = default;
    };

    struct ExprIntegerLiteral final : Expr {
        const int value;

        ExprIntegerLiteral(const uint32_t line, const uint16_t column, const int value) :
            Expr(line, column),
            value(value) {}
    };

    struct ExprFloatLiteral final : Expr {
        const float value;

        ExprFloatLiteral(const uint32_t line, const uint16_t column, const float value) :
            Expr(line, column),
            value(value) {}
    };

    struct ExprBoolLiteral final : Expr {
        const bool value;

        ExprBoolLiteral(const uint32_t line, const uint16_t column, const bool value) :
            Expr(line, column),
            value(value) {}
    };

    struct TypeInfo final : ASTNode {
        compiler::SemanticType type;

        TypeInfo(const uint32_t line, const uint16_t column, const compiler::SemanticType type) :
            ASTNode(line, column), type(type) {}
    };

    struct AssignmentOperatorInfo final : ASTNode {
        const compiler::AssignmentOperator assignmentOperator;

        AssignmentOperatorInfo(const uint32_t line, const uint16_t column, const compiler::AssignmentOperator assignmentOperator) :
            ASTNode(line, column), assignmentOperator(assignmentOperator) {}
    };

    struct BinaryOperatorInfo final : ASTNode {
        const compiler::BinaryOperator binaryOperator;

        BinaryOperatorInfo(const uint32_t line, const uint16_t column, const compiler::BinaryOperator binaryOperator) :
            ASTNode(line, column), binaryOperator(binaryOperator) {}
    };

    struct UnaryOperatorInfo final : ASTNode {
        const compiler::UnaryOperator unaryOperator;

        UnaryOperatorInfo(const uint32_t line, const uint16_t column, const compiler::UnaryOperator unaryOperator) :
            ASTNode(line, column), unaryOperator(unaryOperator) {}
    };

    struct Identifier final : ASTNode {
        const std::string name;

        Identifier(const uint32_t line, const uint16_t column, std::string name) :
            ASTNode(line, column),
            name(std::move(name)) {}
    };

    struct ExprIdentifier final : Expr {
        const std::unique_ptr<Identifier> identifier;

        ExprIdentifier(const uint32_t line, const uint16_t column, std::unique_ptr<Identifier> identifier) :
            Expr(line, column),
            identifier(std::move(identifier)) {}
    };

    using ArrayInitialiserElement = std::variant<
            std::unique_ptr<Expr>,
            std::unique_ptr<ArrayInitialiser>
    >;

    struct ArrayInitialiser final : ASTNode {
        const std::vector<ArrayInitialiserElement> elements;

        ArrayInitialiser(const uint32_t line, const uint16_t column, std::vector<ArrayInitialiserElement> elements) :
            ASTNode(line, column), elements(std::move(elements)) {}
    };

    struct Index final : ASTNode {
        const std::unique_ptr<Expr> index;

        Index(const uint32_t line, const uint16_t column, std::unique_ptr<Expr> index) :
            ASTNode(line, column), index(std::move(index)) {}
    };

    struct ExprNew final : Expr {
        const std::unique_ptr<TypeInfo> typeInfo;
        const std::vector<std::unique_ptr<Index>> arrayDimensions;
        const std::unique_ptr<ArrayInitialiser> optionalInitialiser;

        ExprNew(const uint32_t line,
                const uint16_t column,
                std::unique_ptr<TypeInfo> typeInfo,
                std::vector<std::unique_ptr<Index>>& arrayDimensions,
                std::unique_ptr<ArrayInitialiser> optionalInitialiser) :
            Expr(line, column),
            typeInfo(std::move(typeInfo)),
            arrayDimensions(std::move(arrayDimensions)),
            optionalInitialiser(std::move(optionalInitialiser)) {}
    };

    struct FunctionCall final : Expr {
        const std::unique_ptr<Identifier> identifier;
        const std::vector<std::unique_ptr<Expr>> arguments;
        mutable compiler::FunctionSymbol* functionSymbol = nullptr;

        FunctionCall(const uint32_t line,
                    const uint16_t column,
                    std::unique_ptr<Identifier> identifier,
                    std::vector<std::unique_ptr<Expr>> arguments) :
            Expr(line, column),
            identifier(std::move(identifier)),
            arguments(std::move(arguments)) {}

        std::vector<compiler::SemanticType> getArgumentTypes() const {
            std::vector<compiler::SemanticType> types;
            types.reserve(arguments.size());

            for (const auto& argument : this->arguments) {
                types.push_back(argument->resultingType );
            }

            return types;
        }
    };

    struct ExprCast final : Expr {
        const std::unique_ptr<TypeInfo> typeInfo;
        const std::unique_ptr<Expr> expr;

        ExprCast(const uint32_t line,
                const uint16_t column,
                std::unique_ptr<TypeInfo> typeInfo,
                std::unique_ptr<Expr> expr) :
            Expr(line, column),
            typeInfo(std::move(typeInfo)),
        expr(std::move(expr)) {}
    };

    struct ExprPostfix final : Expr {
        const std::unique_ptr<Expr> expression;
        const std::vector<std::unique_ptr<Index>> indices;
        const std::unique_ptr<UnaryOperatorInfo> unaryOperatorInfo;

        ExprPostfix(const uint32_t line,
                    const uint16_t column,
                    std::unique_ptr<Expr> expression,
                    std::vector<std::unique_ptr<Index>>& indices,
                    std::unique_ptr<UnaryOperatorInfo> unaryOperatorInfo) :
            Expr(line, column),
            expression(std::move(expression)),
            indices(std::move(indices)),
            unaryOperatorInfo(std::move(unaryOperatorInfo)) {}
    };

    struct ExprUnaryOperator final : Expr {
        const std::unique_ptr<UnaryOperatorInfo> unaryOperatorInfo;
        const std::unique_ptr<Expr> expr;

        ExprUnaryOperator(const uint32_t line,
                    const uint16_t column,
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

        ExprBinaryOperator(const uint32_t line,
                    const uint16_t column,
                    std::unique_ptr<Expr> left,
                    std::unique_ptr<BinaryOperatorInfo> binaryOperatorInfo,
                    std::unique_ptr<Expr> right) :
            Expr(line, column),
            left(std::move(left)),
            binaryOperatorInfo(std::move(binaryOperatorInfo)),
            right(std::move(right)) {}
    };

    struct ExpressionStatement final : Stm {
        const std::unique_ptr<Expr> expression;

        ExpressionStatement(const uint32_t line,
                    const uint16_t column,
                    std::unique_ptr<Expr> expression) :
            Stm(line, column),
            expression(std::move(expression)) {}
    };

    struct ReturnStm final : Stm {
        const std::unique_ptr<Expr> returnExpression;
        mutable compiler::FunctionSymbol* functionSymbol = nullptr;

        ReturnStm(const uint32_t line,
                const uint16_t column,
                std::unique_ptr<Expr> returnExpression) :
            Stm(line, column),
            returnExpression(std::move(returnExpression)) {}
    };

    struct BreakStm final : Stm {
        BreakStm(const uint32_t line,
                const uint16_t column) :
            Stm(line, column) {}
    };

    struct ContinueStm final : Stm {
        ContinueStm(const uint32_t line,
                    const uint16_t column) :
            Stm(line, column) {}
    };

    struct WhileStm final : Stm {
        const std::unique_ptr<Expr> condition;
        const std::unique_ptr<Block> block;

        WhileStm(const uint32_t line,
                const uint16_t column,
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

        IfStm(const uint32_t line,
                const uint16_t column,
                std::unique_ptr<Expr> condition,
                std::unique_ptr<Block> ifBlock,
                std::unique_ptr<Stm> elseStm) :
            Stm(line, column),
            condition(std::move(condition)),
            ifBlock(std::move(ifBlock)),
            elseStm(std::move(elseStm)) {}
    };

    struct StmAssignment final : Stm {
        const std::unique_ptr<Identifier> identifier;
        const std::vector<std::unique_ptr<Index>> indices;
        const std::unique_ptr<AssignmentOperatorInfo> assignmentOperatorInfo;
        const std::unique_ptr<Expr> expression;

        StmAssignment(const uint32_t line,
                    const uint16_t column,
                    std::unique_ptr<Identifier> identifier,
                    std::vector<std::unique_ptr<Index>>& indices,
                    std::unique_ptr<AssignmentOperatorInfo> assignmentOperatorInfo,
                    std::unique_ptr<Expr> expression) :
            Stm(line, column),
            identifier(std::move(identifier)),
            indices(std::move(indices)),
            assignmentOperatorInfo(std::move(assignmentOperatorInfo)),
            expression(std::move(expression)) {}
    };

    struct StmVarDecl final : Stm {
        const std::unique_ptr<TypeInfo> typeInfo;
        const std::unique_ptr<Identifier> identifier;
        const std::unique_ptr<Expr> optionalInitialiser;

        StmVarDecl(const uint32_t line,
                    const uint16_t column,
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
        const uint32_t blockEndLine;
        const uint16_t blockEndColumn;

        Block(const uint32_t line,
                    const uint16_t column,
                    const uint32_t blockEndLine,
                    const uint16_t blockEndColumn,
                    std::vector<std::unique_ptr<Stm>> statements,
                    compiler::Scope* scope) :
            Stm(line, column),
            blockEndLine(blockEndLine),
            blockEndColumn(blockEndColumn),
            statements(std::move(statements)),
            scope(scope) {}
    };

    struct Parameter final : ASTNode {
        const std::unique_ptr<TypeInfo> typeInfo;
        const std::unique_ptr<Identifier> identifier;

        Parameter(const uint32_t line,
                const uint16_t column,
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
        mutable compiler::FunctionSymbol* functionSymbol = nullptr;

        FunctionDecl(const uint32_t line,
                    const uint16_t column,
                    std::unique_ptr<TypeInfo> returnTypeInfo,
                    std::unique_ptr<Identifier> identifier,
                    std::vector<std::unique_ptr<Parameter>> parameters,
                    std::unique_ptr<Block> block) :
            ASTNode(line, column),
            returnTypeInfo(std::move(returnTypeInfo)),
            identifier(std::move(identifier)),
            parameters(std::move(parameters)),
            body(std::move(block)) {}

        std::vector<compiler::SemanticType> getParameterTypes() const {
            std::vector<compiler::SemanticType> types;
            types.reserve(parameters.size());

            for (const auto& parameter : this->parameters) {
                types.push_back(parameter->typeInfo->type);
            }

            return types;
        }
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