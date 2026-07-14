#include "Parser.h"

#include <cmath>
#include <math.h>

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
    std::unique_ptr<ast::AssignmentOperatorInfo> assignmentOperatorInfo = this->parseAssignmentOperator();
    std::unique_ptr<ast::Expr> expression = this->parseExpr();
    this->tokeniser->eat(TokenKind::SEMI);

    return std::make_unique<ast::StmAssignment>(
        varAccess->line,
        varAccess->column,
        std::move(varAccess),
        std::move(assignmentOperatorInfo),
        std::move(expression)
    );
}

/*
 *  expression          = additive_expression ;
 */
std::unique_ptr<ast::Expr> compiler::Parser::parseExpr() const {
    return this->parseAdditiveExpression();
}

/*
 *  additive_expression         = multiplicative_expression, { ( PLUS | MINUS ), multiplicative_expression } ;
 */
std::unique_ptr<ast::Expr> compiler::Parser::parseAdditiveExpression() const {
    auto left = this->parseMultiplicativeExpression();
    const auto line = left->line;
    const auto column = left->column;

    Token token = this->tokeniser->tok();

    while (token.kind == TokenKind::PLUS || token.kind == TokenKind::MINUS) {

        // parse arithmetic operator
        std::unique_ptr<ast::BinaryOperatorInfo> binaryOperatorInfo = nullptr;
        switch (token.kind) {
            case TokenKind::PLUS:
                binaryOperatorInfo = std::make_unique<ast::BinaryOperatorInfo>(
                    token.line,
                    token.column,
                    ast::BinaryOperator::PLUS
                );
                break;
            case TokenKind::MINUS:
                binaryOperatorInfo = std::make_unique<ast::BinaryOperatorInfo>(
                    token.line,
                    token.column,
                    ast::BinaryOperator::MINUS
                );
                break;
            default:
                this->handleUnexpectedToken(this->tokeniser->tok());
        }
        this->tokeniser->next();

        // parse right expression
        std::unique_ptr<ast::Expr> right = this->parseMultiplicativeExpression();

        left = std::make_unique<ast::ExprBinaryOperator>(
            line,
            column,
            std::move(left),
            std::move(binaryOperatorInfo),
            std::move(right)
        );
        token = this->tokeniser->tok();
    }
    return left;
}

/*
 *  multiplicative_expression   = unary_expression, { ( MULTIPLY | DIVIDE | MODULO ), unary_expression } ;
 */
std::unique_ptr<ast::Expr> compiler::Parser::parseMultiplicativeExpression() const {
    auto left = this->parseUnaryExpression();
    const auto line = left->line;
    const auto column = left->column;

    Token token = this->tokeniser->tok();

    while (token.kind == TokenKind::MULTIPLY || token.kind == TokenKind::DIVIDE || token.kind == TokenKind::MODULO) {

        // parse arithmetic operator
        std::unique_ptr<ast::BinaryOperatorInfo> binaryOperatorInfo = nullptr;
        switch (token.kind) {
            case TokenKind::MULTIPLY:
                binaryOperatorInfo = std::make_unique<ast::BinaryOperatorInfo>(
                    token.line,
                    token.column,
                    ast::BinaryOperator::MULTIPLY
                );
                break;
            case TokenKind::DIVIDE:
                binaryOperatorInfo = std::make_unique<ast::BinaryOperatorInfo>(
                    token.line,
                    token.column,
                    ast::BinaryOperator::DIVIDE
                );
                break;
            case TokenKind::MODULO:
                binaryOperatorInfo = std::make_unique<ast::BinaryOperatorInfo>(
                    token.line,
                    token.column,
                    ast::BinaryOperator::MODULO
                );
                break;
            default:
                this->handleUnexpectedToken(this->tokeniser->tok());
        }
        this->tokeniser->next();

        // parse right expression
        std::unique_ptr<ast::Expr> right = this->parseUnaryExpression();

        left = std::make_unique<ast::ExprBinaryOperator>(
            line,
            column,
            std::move(left),
            std::move(binaryOperatorInfo),
            std::move(right)
        );
        token = this->tokeniser->tok();
    }
    return left;
}

/*
 *  unary_expression            = ( PLUS | MINUS ), unary_expression
 *                              | primary_expression ;
 */
std::unique_ptr<ast::Expr> compiler::Parser::parseUnaryExpression() const {
    const auto token = this->tokeniser->tok();

    if (token.kind == TokenKind::PLUS || token.kind == TokenKind::MINUS) {
        std::unique_ptr<ast::BinaryOperatorInfo> binaryOperatorInfo = nullptr;

        if (token.kind == TokenKind::PLUS) {
            binaryOperatorInfo = std::make_unique<ast::BinaryOperatorInfo>(
                token.line,
                token.column,
                ast::BinaryOperator::PLUS
            );
        } else if (token.kind == TokenKind::MINUS) {
            binaryOperatorInfo = std::make_unique<ast::BinaryOperatorInfo>(
                token.line,
                token.column,
                ast::BinaryOperator::MINUS
            );
        }
        this->tokeniser->next();
        auto expr = this->parseUnaryExpression();

        return std::make_unique<ast::ExprUnaryOperator>(
            expr->line,
            expr->column,
            std::move(binaryOperatorInfo),
            std::move(expr)
        );
    } else {
        return this->parsePrimaryExpression();
    }
}

/*
 *  primary_expression          = literal
 *                              | var_access
 *                              | LBR, expression, RBR ;
 */
std::unique_ptr<ast::Expr> compiler::Parser::parsePrimaryExpression() const {
    const Token primaryExpression = this->tokeniser->tok();
    if (primaryExpression.kind == TokenKind::INTEGER_LITERAL) {
        return this->parseLiteral();
    }
    if (primaryExpression.kind == TokenKind::IDENTIFIER) {
        return this->parseExprIdentifier();
    }
    if (primaryExpression.kind == TokenKind::LBR) {
        this->tokeniser->next();
        auto expr = this->parseExpr();
        this->tokeniser->eat(TokenKind::RBR);
        return expr;
    }
    this->handleUnexpectedToken(primaryExpression);
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
        try {
            return std::make_unique<ast::ExprIntegerLiteral>(
                literal.line,
                literal.column,
                std::stoi(literal.image)
            );
        } catch (std::out_of_range& e) {
            throw SyntaxError(
                this->path->string(),
                literal.line,
                literal.column,
                literal.image + " is out of range for type 'int'."
            );
        }
    }
    this->handleUnexpectedToken(literal);
}

/*
 * assignment_operator = EQUAL ;
 */
std::unique_ptr<ast::AssignmentOperatorInfo> compiler::Parser::parseAssignmentOperator() const {
    const Token assignmentOperator = this->tokeniser->tok();
    if (assignmentOperator.kind == TokenKind::EQUAL) {
        this->tokeniser->next();
        return std::make_unique<ast::AssignmentOperatorInfo>(
            assignmentOperator.line,
            assignmentOperator.column,
            ast::AssignmentOperator::EQUAL
        );
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
