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
        std::unique_ptr<ast::Stm> parseStm() const;
        std::unique_ptr<ast::StmVarDecl> parseVarDecl() const;
        std::unique_ptr<ast::StmAssignment> parseAssignment() const;

        std::unique_ptr<ast::Expr> parseExpr() const;
        std::unique_ptr<ast::ExprIdentifier> parseExprIdentifier() const;
        std::unique_ptr<ast::Expr> parsePrimary() const;
        std::unique_ptr<ast::VarAccess> parseVarAccess() const;

        ast::Identifier parseIdentifier() const;
        ast::TypeInfo parseType() const;
        std::unique_ptr<ast::Expr> parseLiteral() const;
        ast::AssignmentOperator parseAssignmentOperator() const;

        void handleUnexpectedToken(const Token& token) const;

        Tokeniser* tokeniser;
        const std::filesystem::path* path;
    };
}


#endif //SVM_PARSER_H