#include "Parser.h"

#include "../include/Error.h"

compiler::Parser::Parser(Tokeniser *tokeniser, const std::filesystem::path* path) :
    tokeniser(tokeniser), path(path) {}

std::unique_ptr<ast::Program> compiler::Parser::parseProgram() const {
    std::vector<std::unique_ptr<ast::Stm>> statements;

    while (this->tokeniser->tok().kind != TokenKind::END_OF_FILE) {
        statements.push_back(parseStm());
    }

    return std::make_unique<ast::Program>(std::move(statements));
}

std::unique_ptr<ast::Stm> compiler::Parser::parseStm() const {
    return parseVarDecl();
}

std::unique_ptr<ast::StmVarDecl> compiler::Parser::parseVarDecl() const {
    ast::TypeInfo type = this->parseType();
    ast::Identifier identifier = this->parseIdentifier();

    // varDecl DOES NOT have an initialiser
    if (this->tokeniser->tok().kind != TokenKind::EQUAL) {
        this->tokeniser->eat(TokenKind::SEMI);
        return std::make_unique<ast::StmVarDecl>(
            type.line,
            type.column,
            type,
            identifier,
            nullptr
        );
    }

    // varDecl DOES have an initialiser
    this->tokeniser->eat(TokenKind::EQUAL);
    std::unique_ptr<ast::Expr> optionalInitialiser = this->parseExpr();
    this->tokeniser->eat(TokenKind::SEMI);

    return std::make_unique<ast::StmVarDecl>(
        type.line,
        type.column,
        type,
        identifier,
        std::move(optionalInitialiser)
    );
}

ast::Identifier compiler::Parser::parseIdentifier() const {
    const Token identifier = this->tokeniser->tok();
    if (identifier.kind == TokenKind::IDENTIFIER) {
        this->tokeniser->next();
        return ast::Identifier{
            identifier.line,
            identifier.column,
            identifier.image
        };
    }
    throw SyntaxError(this->path->string(), identifier.line, identifier.column, "expected an identifier");
}

ast::TypeInfo compiler::Parser::parseType() const {
    const Token type = this->tokeniser->tok();
    if (type.kind == TokenKind::INTEGER_TYPE) {
        this->tokeniser->next();
        return ast::TypeInfo{
            type.line,
            type.column,
            ast::Type::INT
        };
    }
    throw SyntaxError(this->path->string(), type.line, type.column, "expected type");
}

std::unique_ptr<ast::Expr> compiler::Parser::parseExpr() const {
    const Token expr = this->tokeniser->tok();
    if (expr.kind == TokenKind::INTEGER_LITERAL) {
        this->tokeniser->next();
        return std::make_unique<ast::ExprIntegerLiteral>(
            expr.line,
            expr.column,
            std::stoi(expr.image)
        );
    }
    if (expr.kind == TokenKind::IDENTIFIER) {
        this->tokeniser->next();
        return std::make_unique<ast::ExprIdentifier>(
            expr.line,
            expr.column,
            std::string(expr.image)
        );
    }
    throw SyntaxError(this->path->string(), expr.line, expr.column, "expected an integer literal");
}
