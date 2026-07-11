#include "Parser.h"

#include "../include/Error.h"

compiler::Parser::Parser(Tokeniser *tokeniser, const std::filesystem::path* path) :
    tokeniser(tokeniser), path(path) {}

/*
 *  program             = { statement } ;
 */
std::unique_ptr<ast::Program> compiler::Parser::parseProgram() const {
    std::vector<std::unique_ptr<ast::Stm>> statements;

    while (this->tokeniser->tok().kind != TokenKind::END_OF_FILE) {
        statements.push_back(this->parseStm());
    }

    return std::make_unique<ast::Program>(std::move(statements));
}

/*
 *  statement           = var_decl
 *                      | assignment ;
 */
std::unique_ptr<ast::Stm> compiler::Parser::parseStm() const {
    const Token stm = tokeniser->tok();
    if (stm.kind == TokenKind::INTEGER_TYPE) {
        return this->parseVarDecl();
    }
    if (stm.kind == TokenKind::IDENTIFIER) {
        return this->parseAssignment();
    }
    handleUnexpectedToken(stm);
}

/*
 *  var_decl            = type, IDENTIFIER, [ EQUAL, expression ], SEMI ;
 */
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

/*
 *  assignment          = var_access, assignment_operator, expression, SEMI ;
 */
std::unique_ptr<ast::StmAssignment> compiler::Parser::parseAssignment() const {
    std::unique_ptr<ast::VarAccess> varAccess = this->parseVarAccess();
    ast::AssignmentOperator assignmentOperator = this->parseAssignmentOperator();
    std::unique_ptr<ast::Expr> expression = this->parseExpr();
    this->tokeniser->eat(TokenKind::SEMI);

    return std::make_unique<ast::StmAssignment>(
        varAccess->line,
        varAccess->column,
        std::move(varAccess),
        assignmentOperator,
        std::move(expression)
    );
}

/*
 *  expression          = primary ;
 */
std::unique_ptr<ast::Expr> compiler::Parser::parseExpr() const {
    return this->parsePrimary();
}

/*
 *  IDENTIFIER
 */
std::unique_ptr<ast::ExprIdentifier> compiler::Parser::parseExprIdentifier() const {
    const Token exprIdentifier = this->tokeniser->tok();
    this->tokeniser->next();

    return std::make_unique<ast::ExprIdentifier>(
        exprIdentifier.line,
        exprIdentifier.column,
        std::string(exprIdentifier.image)
    );
}

/*
 *  primary             = literal
 *                      | var_access ;
 */
std::unique_ptr<ast::Expr> compiler::Parser::parsePrimary() const {
    const Token primary = this->tokeniser->tok();
    if (primary.kind == TokenKind::INTEGER_LITERAL) {
        return this->parseLiteral();
    }
    if (primary.kind == TokenKind::IDENTIFIER) {
        return this->parseExprIdentifier();
    }
}

/*
 * IDENTIFIER
 */
std::unique_ptr<ast::VarAccess> compiler::Parser::parseVarAccess() const {
    const ast::Identifier identifier = this->parseIdentifier();
    return std::make_unique<ast::VarAccess>(
        identifier.line,
        identifier.column,
        identifier
    );
}

/*
 *  IDENTIFIER
 */
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
    this->handleUnexpectedToken(identifier);
}

/*
 *  type                = INTEGER_TYPE ;
 */
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
    this->handleUnexpectedToken(type);
}

/*
 *  literal             = INTEGER_LITERAL ;
 */
std::unique_ptr<ast::Expr> compiler::Parser::parseLiteral() const {
    const Token literal = this->tokeniser->tok();
    if (literal.kind == TokenKind::INTEGER_LITERAL) {
        this->tokeniser->next();
        return std::make_unique<ast::ExprIntegerLiteral>(
            literal.line,
            literal.column,
            std::stoi(literal.image)
        );
    }
    this->handleUnexpectedToken(literal);
}

/*
 * assignment_operator = EQUAL ;
 */
ast::AssignmentOperator compiler::Parser::parseAssignmentOperator() const {
    const Token assignmentOperator = this->tokeniser->tok();
    if (assignmentOperator.kind == TokenKind::EQUAL) {
        this->tokeniser->next();
        return ast::AssignmentOperator::EQUAL;
    }
    this->handleUnexpectedToken(assignmentOperator);
}

void compiler::Parser::handleUnexpectedToken(const Token& token) const {
    throw SyntaxError(
        this->path->string(),
        token.line,
        token.column,
        "unexpected token '" + token.image + "'"
    );
}
