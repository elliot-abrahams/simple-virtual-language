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
 *  statement           = block
 *                      | var_decl
 *                      | assignment
 *                      | if_statement
 *                      | while_statement ;
 */
std::unique_ptr<ast::Stm> compiler::Parser::parseStm() const {
    const Token stm = tokeniser->tok();
    switch (stm.kind) {
        case TokenKind::LCBR: return this->parseBlock();

        case TokenKind::INTEGER_TYPE:
        case TokenKind::FLOAT_TYPE:
        case TokenKind::BOOL_TYPE:
            return this->parseVarDecl();

        case TokenKind::IDENTIFIER: return this->parseAssignment();
        case TokenKind::IF: return this->parseIfStatement();
        case TokenKind::WHILE: return this->parseWhileStatement();

        default:
            this->handleUnexpectedToken(stm);
    }
}

/*
 *  block                       = LCBR, { statement }, RCBR ;
 */
std::unique_ptr<ast::Block> compiler::Parser::parseBlock() const {
    const auto LCBR_Token = this->tokeniser->tok();
    this->tokeniser->eat(TokenKind::LCBR);


    std::vector<std::unique_ptr<ast::Stm>> statements;

    Token stm = tokeniser->tok();
    while (stm.kind == TokenKind::LCBR ||
        stm.kind == TokenKind::INTEGER_TYPE ||
        stm.kind == TokenKind::FLOAT_TYPE ||
        stm.kind == TokenKind::BOOL_TYPE ||
        stm.kind == TokenKind::IDENTIFIER) {

        statements.push_back(this->parseStm());
        stm = tokeniser->tok();
    }

    this->tokeniser->eat(TokenKind::RCBR);
    return std::make_unique<ast::Block>(
        LCBR_Token.line,
        LCBR_Token.column,
        std::move(statements),
        nullptr
    );
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
 *  if_statement                = IF, LBR, expression, RBR, block, [ ELSE, ( block | if_statement ) ] ;
 */
std::unique_ptr<ast::IfStm> compiler::Parser::parseIfStatement() const {
    const auto token = this->tokeniser->tok();
    this->tokeniser->next();
    this->tokeniser->eat(TokenKind::LBR);
    std::unique_ptr<ast::Expr> condition = this->parseExpr();
    this->tokeniser->eat(TokenKind::RBR);
    std::unique_ptr<ast::Block> ifBlock = this->parseBlock();

    std::unique_ptr<ast::Stm> elseStm = nullptr;
    if (this->tokeniser->tok().kind == TokenKind::ELSE) {
        this->tokeniser->next();

        if (this->tokeniser->tok().kind == TokenKind::IF) {
            elseStm = this->parseIfStatement();
        } else {
            elseStm = this->parseBlock();
        }
    }
    return std::make_unique<ast::IfStm>(
        token.line,
        token.column,
        std::move(condition),
        std::move(ifBlock),
        std::move(elseStm)
    );
}

/*
 *  while_statement             = WHILE, LBR, expression, RBR, block ;
 */
std::unique_ptr<ast::WhileStm> compiler::Parser::parseWhileStatement() const {
    const auto token = this->tokeniser->tok();
    this->tokeniser->next();
    this->tokeniser->eat(TokenKind::LBR);
    std::unique_ptr<ast::Expr> condition = this->parseExpr();
    this->tokeniser->eat(TokenKind::RBR);
    std::unique_ptr<ast::Block> block = this->parseBlock();

    return std::make_unique<ast::WhileStm>(
        token.line,
        token.column,
        std::move(condition),
        std::move(block)
    );
}


/*
 *  expression          = logical_or_expression ;
 */
std::unique_ptr<ast::Expr> compiler::Parser::parseExpr() const {
    return this->parseLogicalOrExpression();
}

/*
 *  logical_or_expression       = logical_and_expression, { LOGICAL_OR, logical_and_expression } ;
 */
std::unique_ptr<ast::Expr> compiler::Parser::parseLogicalOrExpression() const {
    auto left = this->parseLogicalAndExpression();
    const auto line = left->line;
    const auto column = left->column;

    Token token = this->tokeniser->tok();

    while (token.kind == TokenKind::LOGICAL_OR) {

        // parse binary operator
        std::unique_ptr<ast::BinaryOperatorInfo>  binaryOperatorInfo = std::make_unique<ast::BinaryOperatorInfo>(
            token.line,
            token.column,
            BinaryOperator::LOGICAL_OR
        );
        this->tokeniser->next();

        // parse right expression
        std::unique_ptr<ast::Expr> right = this->parseLogicalAndExpression();

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
 *  logical_and_expression      = equality_expression, { LOGICAL_AND, equality_expression } ;
 */
std::unique_ptr<ast::Expr> compiler::Parser::parseLogicalAndExpression() const {
    auto left = this->parseEqualityExpression();
    const auto line = left->line;
    const auto column = left->column;

    Token token = this->tokeniser->tok();

    while (token.kind == TokenKind::LOGICAL_AND) {

        // parse binary operator
        std::unique_ptr<ast::BinaryOperatorInfo>  binaryOperatorInfo = std::make_unique<ast::BinaryOperatorInfo>(
            token.line,
            token.column,
            BinaryOperator::LOGICAL_AND
        );
        this->tokeniser->next();

        // parse right expression
        std::unique_ptr<ast::Expr> right = this->parseEqualityExpression();

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
 *  equality_expression         = comparison_expression, { ( EQUAL_EQUAL | NOT_EQUAL ), comparison_expression } ;
 */
std::unique_ptr<ast::Expr> compiler::Parser::parseEqualityExpression() const {
    auto left = this->parseComparisonExpression();
    const auto line = left->line;
    const auto column = left->column;

    Token token = this->tokeniser->tok();

    while (token.kind == TokenKind::EQUAL_EQUAL || token.kind == TokenKind::NOT_EQUAL) {

        // parse binary operator
        std::unique_ptr<ast::BinaryOperatorInfo> binaryOperatorInfo = nullptr;
        switch (token.kind) {
            case TokenKind::EQUAL_EQUAL:
                binaryOperatorInfo = std::make_unique<ast::BinaryOperatorInfo>(
                    token.line,
                    token.column,
                    BinaryOperator::EQUAL_EQUAL
                );
                break;
            case TokenKind::NOT_EQUAL:
                binaryOperatorInfo = std::make_unique<ast::BinaryOperatorInfo>(
                    token.line,
                    token.column,
                    BinaryOperator::NOT_EQUAL
                );
                break;
            default:
                this->handleUnexpectedToken(this->tokeniser->tok());
        }
        this->tokeniser->next();

        // parse right expression
        std::unique_ptr<ast::Expr> right = this->parseComparisonExpression();

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
 *  comparison_expression       = additive_expression, { ( LESS_THAN | LESS_THAN_OR_EQUAL | GREATER_THAN | GREATER_THAN_OR_EQUAL ), additive_expression } ;
 */
std::unique_ptr<ast::Expr> compiler::Parser::parseComparisonExpression() const {
    auto left = this->parseAdditiveExpression();
    const auto line = left->line;
    const auto column = left->column;

    Token token = this->tokeniser->tok();

    while (token.kind == TokenKind::LESS_THAN ||
        token.kind == TokenKind::LESS_THAN_OR_EQUAL ||
        token.kind == TokenKind::GREATER_THAN ||
        token.kind == TokenKind::GREATER_THAN_OR_EQUAL) {

        // parse binary operator
        std::unique_ptr<ast::BinaryOperatorInfo> binaryOperatorInfo = nullptr;
        switch (token.kind) {
            case TokenKind::LESS_THAN:
                binaryOperatorInfo = std::make_unique<ast::BinaryOperatorInfo>(
                    token.line,
                    token.column,
                    BinaryOperator::LESS_THAN
                );
                break;
            case TokenKind::LESS_THAN_OR_EQUAL:
                binaryOperatorInfo = std::make_unique<ast::BinaryOperatorInfo>(
                    token.line,
                    token.column,
                    BinaryOperator::LESS_THAN_OR_EQUAL
                );
                break;
            case TokenKind::GREATER_THAN:
                binaryOperatorInfo = std::make_unique<ast::BinaryOperatorInfo>(
                    token.line,
                    token.column,
                    BinaryOperator::GREATER_THAN
                );
                break;
            case TokenKind::GREATER_THAN_OR_EQUAL:
                binaryOperatorInfo = std::make_unique<ast::BinaryOperatorInfo>(
                    token.line,
                    token.column,
                    BinaryOperator::GREATER_THAN_OR_EQUAL
                );
                break;
            default:
                this->handleUnexpectedToken(this->tokeniser->tok());
        }
        this->tokeniser->next();

        // parse right expression
        std::unique_ptr<ast::Expr> right = this->parseAdditiveExpression();

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
 *  additive_expression         = multiplicative_expression, { ( PLUS | MINUS ), multiplicative_expression } ;
 */
std::unique_ptr<ast::Expr> compiler::Parser::parseAdditiveExpression() const {
    auto left = this->parseMultiplicativeExpression();
    const auto line = left->line;
    const auto column = left->column;

    Token token = this->tokeniser->tok();

    while (token.kind == TokenKind::PLUS || token.kind == TokenKind::MINUS) {

        // parse binary operator
        std::unique_ptr<ast::BinaryOperatorInfo> binaryOperatorInfo = nullptr;
        switch (token.kind) {
            case TokenKind::PLUS:
                binaryOperatorInfo = std::make_unique<ast::BinaryOperatorInfo>(
                    token.line,
                    token.column,
                    BinaryOperator::PLUS
                );
                break;
            case TokenKind::MINUS:
                binaryOperatorInfo = std::make_unique<ast::BinaryOperatorInfo>(
                    token.line,
                    token.column,
                    BinaryOperator::MINUS
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

        // parse binary operator
        std::unique_ptr<ast::BinaryOperatorInfo> binaryOperatorInfo = nullptr;
        switch (token.kind) {
            case TokenKind::MULTIPLY:
                binaryOperatorInfo = std::make_unique<ast::BinaryOperatorInfo>(
                    token.line,
                    token.column,
                    BinaryOperator::MULTIPLY
                );
                break;
            case TokenKind::DIVIDE:
                binaryOperatorInfo = std::make_unique<ast::BinaryOperatorInfo>(
                    token.line,
                    token.column,
                    BinaryOperator::DIVIDE
                );
                break;
            case TokenKind::MODULO:
                binaryOperatorInfo = std::make_unique<ast::BinaryOperatorInfo>(
                    token.line,
                    token.column,
                    BinaryOperator::MODULO
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
 *  unary_expression            = ( PLUS | MINUS | LOGICAL_NOT ), unary_expression
 *                              | primary_expression ;
 */
std::unique_ptr<ast::Expr> compiler::Parser::parseUnaryExpression() const {
    const auto token = this->tokeniser->tok();

    if (token.kind == TokenKind::PLUS || token.kind == TokenKind::MINUS || token.kind == TokenKind::LOGICAL_NOT) {

        // parse unary operator
        std::unique_ptr<ast::UnaryOperatorInfo> unaryOperatorInfo = nullptr;
        switch (token.kind) {
            case TokenKind::PLUS: {
                unaryOperatorInfo = std::make_unique<ast::UnaryOperatorInfo>(
                    token.line,
                    token.column,
                    UnaryOperator::PLUS
                );
                break;
            }
            case TokenKind::MINUS: {
                unaryOperatorInfo = std::make_unique<ast::UnaryOperatorInfo>(
                    token.line,
                    token.column,
                    UnaryOperator::MINUS
                );
                break;
            }
            case TokenKind::LOGICAL_NOT: {
                unaryOperatorInfo = std::make_unique<ast::UnaryOperatorInfo>(
                    token.line,
                    token.column,
                    UnaryOperator::LOGICAL_NOT
                );
                break;
            }
        }
        this->tokeniser->next();
        auto expr = this->parseUnaryExpression();

        return std::make_unique<ast::ExprUnaryOperator>(
            expr->line,
            expr->column,
            std::move(unaryOperatorInfo),
            std::move(expr)
        );
    }
    return this->parsePrimaryExpression();
}

/*
 *  primary_expression          = literal
 *                              | var_access
 *                              | LBR, expression, RBR ;
 */
std::unique_ptr<ast::Expr> compiler::Parser::parsePrimaryExpression() const {
    const Token primaryExpression = this->tokeniser->tok();
    if (primaryExpression.kind == TokenKind::INTEGER_LITERAL ||
        primaryExpression.kind == TokenKind::FLOAT_LITERAL ||
        primaryExpression.kind == TokenKind::BOOL_LITERAL) {
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
 *  type                = INTEGER_TYPE
 *                      | FLOAT_TYPE
 *                      | BOOL_TYPE ;
 */
ast::TypeInfo compiler::Parser::parseType() const {
    const Token type = this->tokeniser->tok();
    switch (type.kind) {
        case TokenKind::INTEGER_TYPE : {
            this->tokeniser->next();
            return ast::TypeInfo{
                type.line,
                type.column,
                Type::INT
            };
        }
        case TokenKind::FLOAT_TYPE : {
            this->tokeniser->next();
            return ast::TypeInfo{
                type.line,
                type.column,
                Type::FLOAT
            };
        }
        case TokenKind::BOOL_TYPE : {
            this->tokeniser->next();
            return ast::TypeInfo{
                type.line,
                type.column,
                Type::BOOL
            };
        }
        default:
            this->handleUnexpectedToken(type);
    }
}

/*
 *  literal             = INTEGER_LITERAL
 *                      | FLOAT_LITERAL
 *                      | BOOL_LITERAL;
 */
std::unique_ptr<ast::Expr> compiler::Parser::parseLiteral() const {
    const Token literal = this->tokeniser->tok();
    switch (literal.kind) {
        case TokenKind::INTEGER_LITERAL : {
            this->tokeniser->next();
            try {
                return std::make_unique<ast::ExprIntegerLiteral>(
                    literal.line,
                    literal.column,
                    std::stoi(literal.image)
                );
            } catch (std::out_of_range& e) {
                handleLiteralOutOfRangeError(literal, "int");
            }
        }
        case TokenKind::FLOAT_LITERAL : {
            this->tokeniser->next();
            try {
                return std::make_unique<ast::ExprFloatLiteral>(
                    literal.line,
                    literal.column,
                    std::stof(literal.image)
                );
            } catch (std::out_of_range& e) {
                this->handleLiteralOutOfRangeError(literal, "float");
            }
        }
        case TokenKind::BOOL_LITERAL : {
            this->tokeniser->next();
            return std::make_unique<ast::ExprBoolLiteral>(
                literal.line,
                literal.column,
                literal.image == "true"
            );
        }
        default:
            this->handleUnexpectedToken(literal);
    }
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
            AssignmentOperator::EQUAL
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

void compiler::Parser::handleLiteralOutOfRangeError(const Token& token, const std::string& type) const {
    throw SyntaxError(
        this->path->string(),
        token.line,
        token.column,
        token.image + " is out of range for type '" + type + "'"
    );
}
