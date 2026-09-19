#include "Parser.h"

#include <cmath>

#include "AST.h"
#include "../include/Error.h"


compiler::Parser::Parser(Tokeniser *tokeniser, const std::filesystem::path* path) :
    tokeniser(tokeniser), path(path) {}

/*
 *  program             = { statement | function_decl } ;
 */
std::unique_ptr<ast::Program> compiler::Parser::parseProgram() const {
    std::vector<std::unique_ptr<ast::Stm>> statements;
    std::vector<std::unique_ptr<ast::FunctionDecl>> functionDecls;

    while (this->tokeniser->tok().kind != TokenKind::END_OF_FILE) {
        const Token token = this->tokeniser->tok();
        switch (token.kind) {
            case TokenKind::INT_TYPE:
            case TokenKind::FLOAT_TYPE:
            case TokenKind::BOOL_TYPE: {
                auto type = this->parseType();
                // to determine whether to parse varDecl or functionDecl
                if (this->tokeniser->lookAhead(1).kind == TokenKind::LBR) {
                    functionDecls.push_back(this->parseFunctionDecl(std::move(type)));
                } else {
                    statements.push_back(this->parseVarDecl(std::move(type)));
                }
                break;
            }
            case TokenKind::VOID_TYPE:
                functionDecls.push_back(this->parseFunctionDecl(this->parseReturnType()));
                break;

            case TokenKind::LCBR:
            case TokenKind::IDENTIFIER:
            case TokenKind::IF:
            case TokenKind::WHILE:
            case TokenKind::CONTINUE:
            case TokenKind::BREAK:
            case TokenKind::RETURN:
                statements.push_back(this->parseStm());
                break;

            default:
                this->handleUnexpectedToken(token);
        }
    }

    return std::make_unique<ast::Program>(
        std::move(statements),
        std::move(functionDecls)
    );
}

/*
 *  function_decl               = return_type, IDENTIFIER, LBR, [ parameter_list ], RBR, block ;
 */
std::unique_ptr<ast::FunctionDecl> compiler::Parser::parseFunctionDecl(std::unique_ptr<ast::TypeInfo> returnType) const {
    std::unique_ptr<ast::Identifier> identifier = this->parseIdentifier();
    this->tokeniser->eat(TokenKind::LBR);

    std::vector<std::unique_ptr<ast::Parameter>> parameterList;
    if (this->tokeniser->tok().kind != TokenKind::RBR) {
        parameterList = this->parseParameterList();
    }

    this->tokeniser->eat(TokenKind::RBR);
    std::unique_ptr<ast::Block> block = this->parseBlock();

    return std::make_unique<ast::FunctionDecl>(
        returnType->line,
        returnType->column,
        std::move(returnType),
        std::move(identifier),
        std::move(parameterList),
        std::move(block)
    );
}

/*
 *  parameter_list              = parameter, { COMMA, parameter } ;
 */
std::vector<std::unique_ptr<ast::Parameter>> compiler::Parser::parseParameterList() const {
    std::vector<std::unique_ptr<ast::Parameter>> parameterList;

    parameterList.push_back(this->parseParameter());

    while (this->tokeniser->tok().kind == TokenKind::COMMA) {
        this->tokeniser->next();
        parameterList.push_back(this->parseParameter());
    }
    return parameterList;
}

/*
 *  parameter                   = type, IDENTIFIER ;
 */
std::unique_ptr<ast::Parameter> compiler::Parser::parseParameter() const {
    std::unique_ptr<ast::TypeInfo> type = this->parseType();
    std::unique_ptr<ast::Identifier> identifier = this->parseIdentifier();

    return std::make_unique<ast::Parameter>(
        type->line,
        type->column,
        std::move(type),
        std::move(identifier)
    );
}

/*
 *  statement           = block
 *                      | var_decl
 *                      | assignment
 *                      | if_statement
 *                      | while_statement
 *                      | continue_statement
 *                      | break_statement
 *                      | function_call_statement
 *                      | return_statement ;
 */
std::unique_ptr<ast::Stm> compiler::Parser::parseStm() const {
    const Token stm = tokeniser->tok();
    switch (stm.kind) {
        case TokenKind::LCBR: return this->parseBlock();

        case TokenKind::INT_TYPE:
        case TokenKind::FLOAT_TYPE:
        case TokenKind::BOOL_TYPE:
            return this->parseVarDecl(this->parseType());

        case TokenKind::IDENTIFIER: {
            if (this->tokeniser->lookAhead(1).kind == TokenKind::LBR) {
                return this->parseFunctionCallStatement();
            }
            return this->parseAssignment();
        }
        case TokenKind::IF: return this->parseIfStatement();
        case TokenKind::WHILE: return this->parseWhileStatement();
        case TokenKind::CONTINUE: return this->parseContinueStatement();
        case TokenKind::BREAK: return this->parseBreakStatement();
        case TokenKind::RETURN: return this->parseReturnStatement();

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
    while (stm.kind != TokenKind::RCBR) {
        statements.push_back(this->parseStm());
        stm = tokeniser->tok();
    }

    const auto RCBR_Token = this->tokeniser->tok();
    this->tokeniser->eat(TokenKind::RCBR);
    return std::make_unique<ast::Block>(
        LCBR_Token.line,
        LCBR_Token.column,
        RCBR_Token.line,
        RCBR_Token.column,
        std::move(statements),
        nullptr
    );
}

/*
 *  var_decl            = type, IDENTIFIER, [ EQUAL, expression ], SEMI ;
 */
std::unique_ptr<ast::StmVarDecl> compiler::Parser::parseVarDecl(std::unique_ptr<ast::TypeInfo> type) const {
    std::unique_ptr<ast::Identifier> identifier = this->parseIdentifier();

    // varDecl DOES NOT have an initialiser
    if (this->tokeniser->tok().kind != TokenKind::EQUAL) {
        this->tokeniser->eat(TokenKind::SEMI);
        return std::make_unique<ast::StmVarDecl>(
            type->line,
            type->column,
            std::move(type),
            std::move(identifier),
            nullptr
        );
    }

    // varDecl DOES have an initialiser
    this->tokeniser->eat(TokenKind::EQUAL);
    std::unique_ptr<ast::Expr> optionalInitialiser = this->parseExpr();
    this->tokeniser->eat(TokenKind::SEMI);

    return std::make_unique<ast::StmVarDecl>(
        type->line,
        type->column,
        std::move(type),
        std::move(identifier),
        std::move(optionalInitialiser)
    );
}

/*
 *  assignment                  = IDENTIFIER, { index }, assignment_operator, expression, SEMI ;
 */
std::unique_ptr<ast::StmAssignment> compiler::Parser::parseAssignment() const {
    std::unique_ptr<ast::Identifier> identifier = this->parseIdentifier();

    std::vector<std::unique_ptr<ast::Index>> indices;
    while (this->tokeniser->tok().kind == TokenKind::LSQBR) {
        indices.push_back(this->parseIndex());
    }

    std::unique_ptr<ast::AssignmentOperatorInfo> assignmentOperatorInfo = this->parseAssignmentOperator();
    std::unique_ptr<ast::Expr> expression = this->parseExpr();
    this->tokeniser->eat(TokenKind::SEMI);

    return std::make_unique<ast::StmAssignment>(
        identifier->line,
        identifier->column,
        std::move(identifier),
        indices,
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
 *  continue_statement          = CONTINUE, SEMI ;
 */
std::unique_ptr<ast::ContinueStm> compiler::Parser::parseContinueStatement() const {
    const Token token = this->tokeniser->tok();
    this->tokeniser->next();
    this->tokeniser->eat(TokenKind::SEMI);

    return std::make_unique<ast::ContinueStm>(
        token.line,
        token.column
    );
}

/*
 *  break_statement             = BREAK, SEMI ;
 */
std::unique_ptr<ast::BreakStm> compiler::Parser::parseBreakStatement() const {
    const Token token = this->tokeniser->tok();
    this->tokeniser->next();
    this->tokeniser->eat(TokenKind::SEMI);

    return std::make_unique<ast::BreakStm>(
        token.line,
        token.column
    );
}

/*
 *  function_call_statement    = function_call, SEMI ;
 */
std::unique_ptr<ast::FunctionCallStm> compiler::Parser::parseFunctionCallStatement() const {
    std::unique_ptr<ast::FunctionCall> functionCall = this->parseFunctionCall();
    this->tokeniser->eat(TokenKind::SEMI);

    return std::make_unique<ast::FunctionCallStm>(
        functionCall->line,
        functionCall->column,
        std::move(functionCall)
    );
}

/*
 *  return_statement            = RETURN, [ expression ], SEMI ;
 */
std::unique_ptr<ast::ReturnStm> compiler::Parser::parseReturnStatement() const {
    const Token token = this->tokeniser->tok();
    this->tokeniser->next();
    std::unique_ptr<ast::Expr> returnExpression = nullptr;
    if (this->tokeniser->tok().kind != TokenKind::SEMI) {
        returnExpression = this->parseExpr();
    }
    this->tokeniser->eat(TokenKind::SEMI);

    return std::make_unique<ast::ReturnStm>(
        token.line,
        token.column,
        std::move(returnExpression)
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
 *  multiplicative_expression   = unary_expression, { ( MULTIPLY | DIVIDE | INTEGER_DIVIDE | MODULO ), unary_expression } ;
 */
std::unique_ptr<ast::Expr> compiler::Parser::parseMultiplicativeExpression() const {
    auto left = this->parseUnaryExpression();
    const auto line = left->line;
    const auto column = left->column;

    Token token = this->tokeniser->tok();

    while (token.kind == TokenKind::MULTIPLY || token.kind == TokenKind::DIVIDE || token.kind == TokenKind::INTEGER_DIVIDE || token.kind == TokenKind::MODULO) {
        // parse binary operator
        BinaryOperator binaryOperator;
        switch (token.kind) {
            case TokenKind::MULTIPLY:
                binaryOperator = BinaryOperator::MULTIPLY;
                break;

            case TokenKind::DIVIDE:
                binaryOperator = BinaryOperator::DIVIDE;
                break;

            case TokenKind::INTEGER_DIVIDE:
                binaryOperator = BinaryOperator::INTEGER_DIVIDE;
                break;

            case TokenKind::MODULO:
                binaryOperator = BinaryOperator::MODULO;
                break;

            default:
                this->handleUnexpectedToken(this->tokeniser->tok());
        }

        auto binaryOperatorInfo = std::make_unique<ast::BinaryOperatorInfo>(
            token.line,
            token.column,
            binaryOperator
        );


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
 *                              | LBR, type, RBR, unary_expression
 *                              | postfix_expression ;
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

    if (token.kind == TokenKind::LBR &&
        isTypeToken(this->tokeniser->lookAhead(1).kind)
    ) {
        // parse cast expression

        this->tokeniser->eat(TokenKind::LBR);
        auto typeInfo = this->parseType();
        this->tokeniser->eat(TokenKind::RBR);
        auto expression = this->parseUnaryExpression();

        return std::make_unique<ast::ExprCast>(
            token.line,
            token.column,
            std::move(typeInfo),
            std::move(expression)
        );
    }
    return this->parseExprPostfix();
}

/*
 *  postfix_expression          = primary_expression, { index }, [ INCREMENT | DECREMENT ] ;
 */
std::unique_ptr<ast::Expr> compiler::Parser::parseExprPostfix() const {
    auto expression = this->parsePrimaryExpression();

    std::vector<std::unique_ptr<ast::Index>> indices;
    while (this->tokeniser->tok().kind == TokenKind::LSQBR) {
        indices.push_back(this->parseIndex());
    }

    auto incDecOp = this->parseIncrementDecrementOperator();

    if (indices.empty() && incDecOp == nullptr) {
        return expression;
    }

    return std::make_unique<ast::ExprPostfix>(
        expression->line,
        expression->column,
        std::move(expression),
        indices,
        std::move(incDecOp)
    );
}

/*
 *  primary_expression          = function_call
 *                              | IDENTIFIER
 *                              | new_expression
 *                              | literal
 *                              | LBR, expression, RBR ;
 */
std::unique_ptr<ast::Expr> compiler::Parser::parsePrimaryExpression() const {
    const Token token = this->tokeniser->tok();

    switch (token.kind) {
        case TokenKind::INT_LITERAL:
        case TokenKind::FLOAT_LITERAL:
        case TokenKind::BOOL_LITERAL:
            return this->parseLiteral();

        case TokenKind::IDENTIFIER: {
            if (this->tokeniser->lookAhead(1).kind == TokenKind::LBR) {
                return this->parseFunctionCall();
            }
            return this->parseExprIdentifier();
        }

        case TokenKind::NEW:
            return this->parseExprNew();

        case TokenKind::LBR: {
            this->tokeniser->next();
            auto expr = this->parseExpr();
            this->tokeniser->eat(TokenKind::RBR);
            return expr;
        }

        default:
            this->handleUnexpectedToken(token);
    }
}

/*
 *  function_call               = IDENTIFIER, LBR, [ argument_list ], RBR ;
 */
std::unique_ptr<ast::FunctionCall> compiler::Parser::parseFunctionCall() const {
    std::unique_ptr<ast::Identifier> identifier = this->parseIdentifier();
    this->tokeniser->eat(TokenKind::LBR);

    std::vector<std::unique_ptr<ast::Expr>> argumentList;
    if (this->tokeniser->tok().kind != TokenKind::RBR) {
        argumentList = this->parseArgumentList();
    }

    this->tokeniser->eat(TokenKind::RBR);

    return std::make_unique<ast::FunctionCall>(
        identifier->line,
        identifier->column,
        std::move(identifier),
        std::move(argumentList)
    );
}

/*
 *  argument_list               = expression, { COMMA, expression } ;
 */
std::vector<std::unique_ptr<ast::Expr>> compiler::Parser::parseArgumentList() const {
    std::vector<std::unique_ptr<ast::Expr>> argumentList;

    argumentList.push_back(this->parseExpr());

    while(this->tokeniser->tok().kind == TokenKind::COMMA) {
        this->tokeniser->next();
        argumentList.push_back(this->parseExpr());
    }
    return argumentList;
}

/*
 *  IDENTIFIER ;
 */
std::unique_ptr<ast::ExprIdentifier> compiler::Parser::parseExprIdentifier() const {
    auto identifier = this->parseIdentifier();

    return std::make_unique<ast::ExprIdentifier>(
        identifier->line,
        identifier->column,
        std::move(identifier)
    );
}

/*
 *  new_expression              = NEW, type, index { index } , [ array_initialiser ] ;
 */
std::unique_ptr<ast::ExprNew> compiler::Parser::parseExprNew() const {
    const auto tokenNEW = this->tokeniser->tok();
    this->tokeniser->eat(TokenKind::NEW);

    auto typeInfo = this->parsePrimitiveType(0);

    std::vector<std::unique_ptr<ast::Index>> arrayDimensions;
    arrayDimensions.push_back(this->parseIndex());

    while (this->tokeniser->tok().kind == TokenKind::LSQBR) {
        arrayDimensions.push_back(this->parseIndex());
    }

    typeInfo->dimension = arrayDimensions.size();

    std::unique_ptr<ast::ArrayInitialiser> optionalInitialiser;
    if (this->tokeniser->tok().kind == TokenKind::LCBR) {
        optionalInitialiser = this->parseArrayInitialiser();
    }

    return std::make_unique<ast::ExprNew>(
        tokenNEW.line,
        tokenNEW.column,
        std::move(typeInfo),
        arrayDimensions,
        std::move(optionalInitialiser)
    );
}

/*
 *  index                = LSQBR, expression, RSQBR ;
 */
std::unique_ptr<ast::Index> compiler::Parser::parseIndex() const {
    const auto tokenLSQBR = this->tokeniser->tok();
    this->tokeniser->eat(TokenKind::LSQBR);
    auto expr = this->parseExpr();
    this->tokeniser->eat(TokenKind::RSQBR);

    return std::make_unique<ast::Index>(
        tokenLSQBR.line,
        tokenLSQBR.column,
        std::move(expr)
    );
}

/*
 *  array_initialiser           = LCBR, [ array_initialiser_element, { COMMA, array_initialiser_element } ], RCBR ;
 */
std::unique_ptr<ast::ArrayInitialiser> compiler::Parser::parseArrayInitialiser() const {
    const auto tokenLCBR = this->tokeniser->tok();
    this->tokeniser->eat(TokenKind::LCBR);

    std::vector<ast::ArrayInitialiserElement> initialiserElements;

    while(this->tokeniser->tok().kind != TokenKind::RCBR) {
        initialiserElements.push_back(this->parseArrayInitialiserElement());
        if (this->tokeniser->tok().kind != TokenKind::COMMA) {
            break;
        }
        this->tokeniser->eat(TokenKind::COMMA);
    }
    this->tokeniser->eat(TokenKind::RCBR);

    return std::make_unique<ast::ArrayInitialiser>(
        tokenLCBR.line,
        tokenLCBR.column,
        std::move(initialiserElements)
    );

}

/*
 *  array_initialiser_element   = expression
 *                              | array_initialiser ;
 */
ast::ArrayInitialiserElement compiler::Parser::parseArrayInitialiserElement() const {
    if (this->tokeniser->tok().kind == TokenKind::LCBR) {
        return this->parseArrayInitialiser();
    }
    return this->parseExpr();
}

/*
 *  IDENTIFIER
 */
std::unique_ptr<ast::Identifier> compiler::Parser::parseIdentifier() const {
    const Token identifier = this->tokeniser->tok();
    if (identifier.kind == TokenKind::IDENTIFIER) {
        this->tokeniser->next();
        return std::make_unique<ast::Identifier>(
            identifier.line,
            identifier.column,
            identifier.image
        );
    }
    this->handleUnexpectedToken(identifier);
}

/*
 *  return_type                 = type | VOID ;
 */
std::unique_ptr<ast::TypeInfo> compiler::Parser::parseReturnType() const {
    const Token returnType = this->tokeniser->tok();
    if (returnType.kind == TokenKind::VOID_TYPE) {
        this->tokeniser->next();
        return std::make_unique<ast::TypeInfo> (
            returnType.line,
            returnType.column,
            Type::VOID_RETURN_TYPE,
            0
        );
    }
    return this->parseType();
}

/*
 *  type                        = primitive_type, { LSQBR, RSQBR } ;
 */
std::unique_ptr<ast::TypeInfo> compiler::Parser::parseType() const {
    auto primitiveType = this->parsePrimitiveType(0);

    unsigned int typeDimension = 0;

    while (this->tokeniser->tok().kind == TokenKind::LSQBR) {
        typeDimension++;
        this->tokeniser->next();
        this->tokeniser->eat(TokenKind::RSQBR);
    }
    primitiveType->dimension = typeDimension;
    return primitiveType;
}

/*
 *  primitive_type              = INT_TYPE
 *                              | FLOAT_TYPE
 *                              | BOOL_TYPE ;
 */
std::unique_ptr<ast::TypeInfo> compiler::Parser::parsePrimitiveType(const unsigned int dimension) const {
    const Token type = this->tokeniser->tok();
    this->tokeniser->next();
    switch (type.kind) {
        case TokenKind::INT_TYPE :
            return std::make_unique<ast::TypeInfo>(
                type.line,
                type.column,
                Type::INT,
                dimension
            );
        case TokenKind::FLOAT_TYPE :
            return std::make_unique<ast::TypeInfo>(
                type.line,
                type.column,
                Type::FLOAT,
                dimension
            );
        case TokenKind::BOOL_TYPE :
            return std::make_unique<ast::TypeInfo> (
                type.line,
                type.column,
                Type::BOOL,
                dimension
            );
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
        case TokenKind::INT_LITERAL : {
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

/*
 * [ INCREMENT | DECREMENT ]
 */
std::unique_ptr<ast::IncrementDecrementOperatorInfo> compiler::Parser::parseIncrementDecrementOperator() const {
    const auto token = this->tokeniser->tok();
    if (token.kind != TokenKind::INCREMENT && token.kind != TokenKind::DECREMENT) {
        return nullptr;
    }

    this->tokeniser->next();

    return std::make_unique<ast::IncrementDecrementOperatorInfo>(
        token.line,
        token.column,
        token.kind == TokenKind::INCREMENT ? IncrementDecrementOperator::INCREMENT : IncrementDecrementOperator::DECREMENT
    );
}

bool compiler::Parser::isTypeToken(const TokenKind& kind) {
    switch (kind) {
        case TokenKind::INT_TYPE:
        case TokenKind::FLOAT_TYPE:
        case TokenKind::BOOL_TYPE:
            return true;

        default:
            return false;
    }
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
