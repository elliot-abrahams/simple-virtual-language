#ifndef SVM_PARSER_H
#define SVM_PARSER_H
#include <filesystem>

#include "Tokeniser.h"
#include "AST.h"


namespace compiler {

    class Parser {
    public:
        Parser(Tokeniser* tokeniser, const std::filesystem::path* path);

        std::unique_ptr<ast::Program> parseProgram() const;

    private:
        std::unique_ptr<ast::FunctionDecl> parseFunctionDecl() const;
        std::vector<std::unique_ptr<ast::Parameter>> parseParameterList() const;
        std::unique_ptr<ast::Parameter> parseParameter() const;

        std::unique_ptr<ast::Stm> parseStm() const;
        std::unique_ptr<ast::Block> parseBlock() const;
        std::unique_ptr<ast::StmVarDecl> parseVarDecl() const;
        std::unique_ptr<ast::StmAssignment> parseAssignment() const;
        std::unique_ptr<ast::IfStm> parseIfStatement() const;
        std::unique_ptr<ast::WhileStm> parseWhileStatement() const;
        std::unique_ptr<ast::ContinueStm> parseContinueStatement() const;
        std::unique_ptr<ast::BreakStm> parseBreakStatement() const;
        std::unique_ptr<ast::FunctionCallStm> parseFunctionCallStatement() const;
        std::unique_ptr<ast::ReturnStm> parseReturnStatement() const;

        std::unique_ptr<ast::Expr> parseExpr() const;
        std::unique_ptr<ast::Expr> parseLogicalOrExpression() const;
        std::unique_ptr<ast::Expr> parseLogicalAndExpression() const;
        std::unique_ptr<ast::Expr> parseEqualityExpression() const;
        std::unique_ptr<ast::Expr> parseComparisonExpression() const;
        std::unique_ptr<ast::Expr> parseAdditiveExpression() const;
        std::unique_ptr<ast::Expr> parseMultiplicativeExpression() const;
        std::unique_ptr<ast::Expr> parseUnaryExpression() const;
        std::unique_ptr<ast::Expr> parsePrimaryExpression() const;
        std::unique_ptr<ast::FunctionCall> parseFunctionCall() const;
        std::vector<std::unique_ptr<ast::Expr>> parseArgumentList() const;
        std::unique_ptr<ast::VarAccess> parseVarAccess() const;
        std::unique_ptr<ast::ExprIdentifier> parseExprIdentifier() const;

        std::unique_ptr<ast::Identifier> parseIdentifier() const;
        std::unique_ptr<ast::TypeInfo> parseType() const;
        std::unique_ptr<ast::Expr> parseLiteral() const;
        std::unique_ptr<ast::AssignmentOperatorInfo> parseAssignmentOperator() const;

        void handleUnexpectedToken(const Token& token) const;
        void handleLiteralOutOfRangeError(const Token& token, const std::string& type) const;

        Tokeniser* tokeniser;
        const std::filesystem::path* path;
    };
}


#endif //SVM_PARSER_H