#include "ConstantFolder.h"

#include <cmath>

#include "../../include/Error.h"

using namespace compiler;

ConstantFolder::ConstantFolder(const std::filesystem::path& filePath) : path(filePath) {};

void ConstantFolder::optimise(const ast::Program& program) const {
    for (auto& stm : program.statements) {
        this->foldStm(*stm);
    }
    for (auto& functionDecl : program.functionDecls) {
        this->foldStmBlock(*functionDecl->body);
    }
}

void ConstantFolder::foldStm(ast::Stm& stm) const {
    if (const auto* stmVarDecl = dynamic_cast<ast::StmVarDecl*>(&stm)) {
        if (stmVarDecl->optionalInitialiser != nullptr) {
            if (this->foldExpr(stmVarDecl->optionalInitialiser))
                this->convertLiteralToType(stmVarDecl->typeInfo->type.typeId, stmVarDecl->optionalInitialiser, this->extractConstantValue(*stmVarDecl->optionalInitialiser));
        }
    }
    else if (const auto* stmAssignment = dynamic_cast<ast::StmAssignment*>(&stm)) {
        if (this->foldExpr(stmAssignment->expr)) {
            this->convertLiteralToType(stmAssignment->symbol->type.typeId, stmAssignment->expr, this->extractConstantValue(*stmAssignment->expr));
        }
        for (const auto& index : stmAssignment->indices) {
            this->foldExpr(index->index);
        }
    }
    else if (const auto* stmBlock = dynamic_cast<ast::StmBlock*>(&stm)) {
        this->foldStmBlock(*stmBlock);
    }
    else if (const auto* stmIf = dynamic_cast<ast::StmIf*>(&stm)) {
        this->foldExpr(stmIf->condition);
        this->foldStmBlock(*stmIf->ifBlock);
        if (stmIf->elseStm != nullptr) this->foldStm(*stmIf->elseStm);
    }
    else if (const auto* stmWhile = dynamic_cast<ast::StmWhile*>(&stm)) {
        this->foldExpr(stmWhile->condition);
        this->foldStmBlock(*stmWhile->body);
    }
    else if (const auto* stmFor = dynamic_cast<ast::StmFor*>(&stm)) {
        if (std::holds_alternative<std::unique_ptr<ast::ForRange>>(stmFor->iterable)) {
            auto& forIterable = std::get<std::unique_ptr<ast::ForRange>>(stmFor->iterable);
            this->foldExpr(forIterable->start);
            this->convertLiteralToType(INT_TYPE_ID, forIterable->start, this->extractConstantValue(*forIterable->start));
            this->foldExpr(forIterable->end);
            this->convertLiteralToType(INT_TYPE_ID, forIterable->end, this->extractConstantValue(*forIterable->end));
            if (forIterable->step != nullptr) {
                this->foldExpr(forIterable->step);
                this->convertLiteralToType(INT_TYPE_ID, forIterable->step, this->extractConstantValue(*forIterable->step));
            }
        }
        this->foldStmBlock(*stmFor->body);
    }
    else if (const auto* stmReturn = dynamic_cast<ast::StmReturn*>(&stm)) {
        if (stmReturn->returnExpr != nullptr) {
            if (this->foldExpr(stmReturn->returnExpr))
                this->convertLiteralToType(stmReturn->functionSymbol->returnType.typeId, stmReturn->returnExpr, this->extractConstantValue(*stmReturn->returnExpr));
        }
    }
    else if (const auto* stmExpr = dynamic_cast<ast::StmExpr*>(&stm)) {
        this->foldExpr(stmExpr->expr);
    }
}

void ConstantFolder::foldStmBlock(const ast::StmBlock& stmBlock) const {
    for (auto& stm : stmBlock.statements) {
        this->foldStm(*stm);
    }
}

bool ConstantFolder::foldExpr(std::unique_ptr<ast::Expr>& expr) const {
    if (dynamic_cast<ast::ExprIntegerLiteral*>(expr.get())) return true;
    if (dynamic_cast<ast::ExprFloatLiteral*>(expr.get())) return true;
    if (dynamic_cast<ast::ExprBoolLiteral*>(expr.get())) return true;
    if (dynamic_cast<ast::ExprCharLiteral*>(expr.get())) return true;

    if (const auto* exprUnary = dynamic_cast<ast::ExprUnary*>(expr.get())) {
        // only fold exprUnary if inner expr is a constant value
        if (!this->foldExpr(exprUnary->expr)) return false;
        expr = std::move(this->foldExprUnary(*exprUnary));
        return true;
    }
    if (const auto* exprBinary = dynamic_cast<ast::ExprBinary*>(expr.get())) {
        auto foldedExpr = this->foldExprBinary(*exprBinary);
        if (!foldedExpr) return false;
        expr = std::move(foldedExpr);
        return true;
    }
    if (const auto& exprTernary = dynamic_cast<ast::ExprTernary*>(expr.get())) {
        // only fold exprTernary if condition expr, and the expression that the condition refers to are contant values
        if (!this->foldExpr(exprTernary->condition)) return false;

        if (std::get<bool>(this->extractConstantValue(*exprTernary->condition))) {
            // fold true expression
            expr = std::move(exprTernary->trueExpr);
        } else {
            // fold false expression
            expr = std::move(exprTernary->falseExpr);
        }
        return this->foldExpr(expr);
    }
    if (const auto& exprPostfix = dynamic_cast<ast::ExprPostfix*>(expr.get())) {
        for (const auto& postfixOperator : exprPostfix->postfixOperators) {
            if (std::holds_alternative<std::unique_ptr<ast::Index>>(postfixOperator)) {
                this->foldExpr(std::get<std::unique_ptr<ast::Index>>(postfixOperator)->index);
            }
        }
        return false;
    }
    if (const auto& exprCast = dynamic_cast<ast::ExprCast*>(expr.get())) {
        // only fold exprCast if inner expr is a constant value
        if (!this->foldExpr(exprCast->expr)) return false;

        expr = std::move(this->foldExprCast(*exprCast));
        return true;
    }
    if (const auto& exprFunctionCall = dynamic_cast<ast::ExprFunctionCall*>(expr.get())) {
        for (size_t index = 0; index < exprFunctionCall->arguments.size(); ++index) {
            if (this->foldExpr(exprFunctionCall->arguments[index]))
                this->convertLiteralToType(exprFunctionCall->functionSymbol->parameterTypes[index].typeId, exprFunctionCall->arguments[index], this->extractConstantValue(*exprFunctionCall->arguments[index]));
        }
        return false;
    }
    if (const auto& exprNew = dynamic_cast<ast::ExprNew*>(expr.get())) {
        for (auto& dimensions : exprNew->arrayDimensions) {
            this->foldExpr(dimensions->index);
        }
        if (exprNew->optionalInitialiser != nullptr) {
            for (auto& element : exprNew->optionalInitialiser->elements) {
                this->foldArrayInitialiserElement(exprNew->typeInfo->type.typeId, element);
            }
        }
    }
    return false;
}

void ConstantFolder::foldArrayInitialiserElement(const TypeId convertElementToType, ast::ArrayInitialiserElement& arrayInitialiserElement) const {
    if (std::holds_alternative<std::unique_ptr<ast::ArrayInitialiser>>(arrayInitialiserElement)) {
        for (auto& element : std::get<std::unique_ptr<ast::ArrayInitialiser>>(arrayInitialiserElement)->elements) {
            this->foldArrayInitialiserElement(convertElementToType, element);
        }
    }
    else {
        auto& element = std::get<std::unique_ptr<ast::Expr>>(arrayInitialiserElement);
        if (this->foldExpr(element)) {
            this->convertLiteralToType(convertElementToType, element, this->extractConstantValue(*element));
        }
    }
}

std::unique_ptr<ast::Expr> ConstantFolder::foldExprUnary(const ast::ExprUnary& exprUnary) const {
    if (exprUnary.unaryOperatorInfo->unaryOperator == UnaryOperator::PLUS) {
        return std::move(exprUnary.expr);
    }

    const auto constantValue = extractConstantValue(*exprUnary.expr);

    if (std::holds_alternative<int>(constantValue)) {
        const int value = std::get<int>(constantValue);
        switch (exprUnary.unaryOperatorInfo->unaryOperator) {
            case UnaryOperator::MINUS: {
                auto constExpr = std::make_unique<ast::ExprIntegerLiteral>(
                    exprUnary.line,
                    exprUnary.column,
                    -value
                );
                constExpr->resultingType = Type{INT_TYPE_ID, 0};
                return constExpr;
            }
            case UnaryOperator::BITWISE_NOT: {
                auto constExpr = std::make_unique<ast::ExprIntegerLiteral>(
                    exprUnary.line,
                    exprUnary.column,
                    ~value
                );
                constExpr->resultingType = Type{INT_TYPE_ID, 0};
                return constExpr;
            }
        }
        throw std::runtime_error("invalid const fold");
    }
    if (std::holds_alternative<float>(constantValue)) {
        const float value = std::get<float>(constantValue);
        auto constExpr = std::make_unique<ast::ExprFloatLiteral>(
            exprUnary.line,
            exprUnary.column,
            -value
        );
        constExpr->resultingType = Type{FLOAT_TYPE_ID, 0};
        return constExpr;
    }
    if (std::holds_alternative<bool>(constantValue)) {
        const bool value = std::get<bool>(constantValue);
        auto constExpr = std::make_unique<ast::ExprBoolLiteral>(
            exprUnary.line,
            exprUnary.column,
            !value
        );
        constExpr->resultingType = Type{BOOL_TYPE_ID, 0};
        return constExpr;
    }
    if (std::holds_alternative<uint32_t>(constantValue)) {
        const uint32_t value = std::get<uint32_t>(constantValue);
        switch (exprUnary.unaryOperatorInfo->unaryOperator) {

            case UnaryOperator::MINUS: {
                auto constExpr = std::make_unique<ast::ExprIntegerLiteral>(
                    exprUnary.line,
                    exprUnary.column,
                    -static_cast<int>(value)
                );
                constExpr->resultingType = Type{INT_TYPE_ID, 0};
                return constExpr;
            }
            case UnaryOperator::BITWISE_NOT: {
                auto constExpr = std::make_unique<ast::ExprIntegerLiteral>(
                    exprUnary.line,
                    exprUnary.column,
                    ~static_cast<int>(value)
                );
                constExpr->resultingType = Type{INT_TYPE_ID, 0};
                return constExpr;
            }
        }
        throw std::runtime_error("invalid const fold");
    }
}

std::unique_ptr<ast::Expr> ConstantFolder::foldExprBinary(const ast::ExprBinary& exprBinary) const {

    // short circuit evaluation for && and ||
    switch (exprBinary.binaryOperatorInfo->binaryOperator) {
        case BinaryOperator::LOGICAL_AND: {
            if (this->foldExpr(exprBinary.left) && !std::get<bool>(this->extractConstantValue(*exprBinary.left))) {
                // return false boolLiteral if left expr constant value is false
                auto constExpr = std::make_unique<ast::ExprBoolLiteral>(
                    exprBinary.line,
                    exprBinary.column,
                    false
                );
                constExpr->resultingType = Type{BOOL_TYPE_ID, 0};
                return constExpr;
            }
            // fold right expression
            if (!this->foldExpr(exprBinary.right)) {
                return nullptr;
            }
            const auto rightConstantValue = extractConstantValue(*exprBinary.right);
            auto constExpr = std::make_unique<ast::ExprBoolLiteral>(
                exprBinary.line,
                exprBinary.column,
                std::get<bool>(rightConstantValue)
            );
            constExpr->resultingType = Type{BOOL_TYPE_ID, 0};
            return constExpr;
        }
        case BinaryOperator::LOGICAL_OR: {
            if (this->foldExpr(exprBinary.left) && std::get<bool>(this->extractConstantValue(*exprBinary.left))) {
                // return true boolLiteral if left expr constant value is true
                auto constExpr = std::make_unique<ast::ExprBoolLiteral>(
                    exprBinary.line,
                    exprBinary.column,
                    true
                );
                constExpr->resultingType = Type{BOOL_TYPE_ID, 0};
                return constExpr;
            }
            // fold right expression
            if (!this->foldExpr(exprBinary.right)) {
                return nullptr;
            }
            const auto rightConstantValue = extractConstantValue(*exprBinary.right);
            auto constExpr = std::make_unique<ast::ExprBoolLiteral>(
                exprBinary.line,
                exprBinary.column,
                std::get<bool>(rightConstantValue)
            );
            constExpr->resultingType = Type{BOOL_TYPE_ID, 0};
            return constExpr;
        }
    }

    // only fold exprBinary if both left expr and right expr are constant values
    if (!this->foldExpr(exprBinary.left) || !this->foldExpr(exprBinary.right)) return nullptr;

    ConstantValue convertedLeftConstantValue;
    ConstantValue convertedRightConstantValue;
    switch (exprBinary.binaryOperatorInfo->binaryOperator) {
        case BinaryOperator::INTEGER_DIVIDE: {
            convertedLeftConstantValue = this->convertConstantValueToType(FLOAT_TYPE_ID, this->extractConstantValue(*exprBinary.left));
            convertedRightConstantValue = this->convertConstantValueToType(FLOAT_TYPE_ID, this->extractConstantValue(*exprBinary.right));
        }

        case BinaryOperator::EQUAL_EQUAL:
        case BinaryOperator::NOT_EQUAL:
        case BinaryOperator::LESS_THAN:
        case BinaryOperator::LESS_THAN_OR_EQUAL:
        case BinaryOperator::GREATER_THAN:
        case BinaryOperator::GREATER_THAN_OR_EQUAL: {
            break;
        }

        default: {
            convertedLeftConstantValue = this->convertConstantValueToType(exprBinary.resultingType.typeId, this->extractConstantValue(*exprBinary.left));
            convertedRightConstantValue = this->convertConstantValueToType(exprBinary.resultingType.typeId, this->extractConstantValue(*exprBinary.right));
        }
    }

    switch (exprBinary.binaryOperatorInfo->binaryOperator) {
        case BinaryOperator::ADD: {

            // resulting type is either float or int
            if (std::holds_alternative<float>(convertedLeftConstantValue)) {
                auto constExpr = std::make_unique<ast::ExprFloatLiteral>(
                    exprBinary.line,
                    exprBinary.column,
                    std::get<float>(convertedLeftConstantValue) + std::get<float>(convertedRightConstantValue)
                );
                constExpr->resultingType = Type{FLOAT_TYPE_ID, 0};
                return constExpr;
            }
            if (std::holds_alternative<int>(convertedLeftConstantValue)) {
                auto constExpr = std::make_unique<ast::ExprIntegerLiteral>(
                    exprBinary.line,
                    exprBinary.column,
                    std::get<int>(convertedLeftConstantValue) + std::get<int>(convertedRightConstantValue)
                );
                constExpr->resultingType = Type{INT_TYPE_ID, 0};
                return constExpr;
            }
        }
        case BinaryOperator::SUBTRACT: {
            // resulting type is either float or int
            if (std::holds_alternative<float>(convertedLeftConstantValue)) {
                auto constExpr = std::make_unique<ast::ExprFloatLiteral>(
                    exprBinary.line,
                    exprBinary.column,
                    std::get<float>(convertedLeftConstantValue) - std::get<float>(convertedRightConstantValue)
                );
                constExpr->resultingType = Type{FLOAT_TYPE_ID, 0};
                return constExpr;
            }
            if (std::holds_alternative<int>(convertedLeftConstantValue)) {
                auto constExpr = std::make_unique<ast::ExprIntegerLiteral>(
                    exprBinary.line,
                    exprBinary.column,
                    std::get<int>(convertedLeftConstantValue) - std::get<int>(convertedRightConstantValue)
                );
                constExpr->resultingType = Type{INT_TYPE_ID, 0};
                return constExpr;
            }
        }
        case BinaryOperator::MULTIPLY: {
            // resulting type is either float or int
            if (std::holds_alternative<float>(convertedLeftConstantValue)) {
                auto constExpr = std::make_unique<ast::ExprFloatLiteral>(
                    exprBinary.line,
                    exprBinary.column,
                    std::get<float>(convertedLeftConstantValue) * std::get<float>(convertedRightConstantValue)
                );
                constExpr->resultingType = Type{FLOAT_TYPE_ID, 0};
                return constExpr;
            }
            if (std::holds_alternative<int>(convertedLeftConstantValue)) {
                auto constExpr = std::make_unique<ast::ExprIntegerLiteral>(
                    exprBinary.line,
                    exprBinary.column,
                    std::get<int>(convertedLeftConstantValue) * std::get<int>(convertedRightConstantValue)
                );
                constExpr->resultingType = Type{INT_TYPE_ID, 0};
                return constExpr;
            }
        }
        case BinaryOperator::DIVIDE: {
            // resulting type is always float
            if (std::get<float>(convertedRightConstantValue) == 0) {
                this->throwDivisionByZeroError(exprBinary.binaryOperatorInfo->line, exprBinary.binaryOperatorInfo->column, "division by zero");
            }
            auto constExpr = std::make_unique<ast::ExprFloatLiteral>(
                exprBinary.line,
                exprBinary.column,
                std::get<float>(convertedLeftConstantValue) / std::get<float>(convertedRightConstantValue)
            );
            constExpr->resultingType = Type{FLOAT_TYPE_ID, 0};
            return constExpr;
        }
        case BinaryOperator::INTEGER_DIVIDE: {
            // resulting type is always int
            if (std::get<float>(convertedRightConstantValue) == 0) {
                throwDivisionByZeroError(exprBinary.binaryOperatorInfo->line, exprBinary.binaryOperatorInfo->column, "division by zero");
            }
            auto constExpr = std::make_unique<ast::ExprIntegerLiteral>(
                exprBinary.line,
                exprBinary.column,
                static_cast<int>(std::get<float>(convertedLeftConstantValue) / std::get<float>(convertedRightConstantValue))
            );
            constExpr->resultingType = Type{INT_TYPE_ID, 0};
            return constExpr;
        }
        case BinaryOperator::MODULO: {
            // resulting type is either float or int
            if (std::holds_alternative<float>(convertedLeftConstantValue)) {
                if (std::get<float>(convertedRightConstantValue) == 0) {
                    this->throwDivisionByZeroError(exprBinary.binaryOperatorInfo->line, exprBinary.binaryOperatorInfo->column, "modulo by zero");
                }
                auto constExpr = std::make_unique<ast::ExprFloatLiteral>(
                    exprBinary.line,
                    exprBinary.column,
                    std::fmod(std::get<float>(convertedLeftConstantValue), std::get<float>(convertedRightConstantValue))
                );
                constExpr->resultingType = Type{FLOAT_TYPE_ID, 0};
                return constExpr;
            }
            if (std::holds_alternative<int>(convertedLeftConstantValue)) {
                if (std::get<int>(convertedRightConstantValue) == 0) {
                    this->throwDivisionByZeroError(exprBinary.binaryOperatorInfo->line, exprBinary.binaryOperatorInfo->column, "modulo by zero");
                }
                auto constExpr = std::make_unique<ast::ExprIntegerLiteral>(
                    exprBinary.line,
                    exprBinary.column,
                    std::get<int>(convertedLeftConstantValue) % std::get<int>(convertedRightConstantValue)
                );
                constExpr->resultingType = Type{INT_TYPE_ID, 0};
                return constExpr;
            }
        }
        case BinaryOperator::BITWISE_AND: {
            // resulting type is always int
            auto constExpr = std::make_unique<ast::ExprIntegerLiteral>(
                exprBinary.line,
                exprBinary.column,
                std::get<int>(convertedLeftConstantValue) & std::get<int>(convertedRightConstantValue)
            );
            constExpr->resultingType = Type{INT_TYPE_ID, 0};
            return constExpr;
        }
        case BinaryOperator::BITWISE_OR: {
            // resulting type is always int
            auto constExpr = std::make_unique<ast::ExprIntegerLiteral>(
                exprBinary.line,
                exprBinary.column,
                std::get<int>(convertedLeftConstantValue) | std::get<int>(convertedRightConstantValue)
            );
            constExpr->resultingType = Type{INT_TYPE_ID, 0};
            return constExpr;
        }
        case BinaryOperator::BITWISE_XOR: {
            // resulting type is always int
            auto constExpr = std::make_unique<ast::ExprIntegerLiteral>(
                exprBinary.line,
                exprBinary.column,
                std::get<int>(convertedLeftConstantValue) ^ std::get<int>(convertedRightConstantValue)
            );
            constExpr->resultingType = Type{INT_TYPE_ID, 0};
            return constExpr;
        }
        case BinaryOperator::LEFT_SHIFT: {
            // resulting type is always int
            auto constExpr = std::make_unique<ast::ExprIntegerLiteral>(
                exprBinary.line,
                exprBinary.column,
                std::get<int>(convertedLeftConstantValue) << std::get<int>(convertedRightConstantValue)
            );
            constExpr->resultingType = Type{INT_TYPE_ID, 0};
            return constExpr;
        }
        case BinaryOperator::ARITHMETIC_RIGHT_SHIFT: {
            // resulting type is always int
            auto constExpr = std::make_unique<ast::ExprIntegerLiteral>(
                exprBinary.line,
                exprBinary.column,
                std::get<int>(convertedLeftConstantValue) >> std::get<int>(convertedRightConstantValue)
            );
            constExpr->resultingType = Type{INT_TYPE_ID, 0};
            return constExpr;
        }
        case BinaryOperator::LOGICAL_RIGHT_SHIFT: {
            // resulting type is always int
            auto constExpr = std::make_unique<ast::ExprIntegerLiteral>(
                exprBinary.line,
                exprBinary.column,
                static_cast<unsigned int>(std::get<int>(convertedLeftConstantValue)) >> static_cast<unsigned int>(std::get<int>(convertedRightConstantValue))
            );
            constExpr->resultingType = Type{INT_TYPE_ID, 0};
            return constExpr;
        }
        case BinaryOperator::EQUAL_EQUAL:
        case BinaryOperator::NOT_EQUAL:
        case BinaryOperator::LESS_THAN:
        case BinaryOperator::LESS_THAN_OR_EQUAL:
        case BinaryOperator::GREATER_THAN:
        case BinaryOperator::GREATER_THAN_OR_EQUAL: {
            // resulting type is always bool
            auto constExpr = std::make_unique<ast::ExprBoolLiteral>(
                exprBinary.line,
                exprBinary.column,
                std::visit(
                    [this, &exprBinary](const auto& left, const auto& right) {
                        return this->foldEqualityOrComparisonOperator(left, right, exprBinary.binaryOperatorInfo->binaryOperator);
                    },
                    this->extractConstantValue(*exprBinary.left),
                    this->extractConstantValue(*exprBinary.right)
                )
            );
            constExpr->resultingType = Type{BOOL_TYPE_ID, 0};
            return constExpr;
        }
    }
    throw std::runtime_error("invalid const fold");
}

std::unique_ptr<ast::Expr> ConstantFolder::foldExprCast(const ast::ExprCast& exprCast) const {
    this->convertLiteralToType(exprCast.resultingType.typeId, exprCast.expr, extractConstantValue(*exprCast.expr));
    return std::move(exprCast.expr);
}

ConstantValue ConstantFolder::extractConstantValue(const ast::Expr& expr) const {
    if (const auto* exprIntegerLiteral = dynamic_cast<const ast::ExprIntegerLiteral*>(&expr)) {
        return exprIntegerLiteral->value;
    }
    if (const auto* exprFloatLiteral = dynamic_cast<const ast::ExprFloatLiteral*>(&expr)) {
        return exprFloatLiteral->value;
    }
    if (const auto* exprBoolLiteral = dynamic_cast<const ast::ExprBoolLiteral*>(&expr)) {
        return exprBoolLiteral->value;
    }
    if (const auto* exprCharLiteral = dynamic_cast<const ast::ExprCharLiteral*>(&expr)) {
        return exprCharLiteral->value;
    }
    throw std::runtime_error("invalid const fold");
}

ConstantValue ConstantFolder::convertConstantValueToType(const TypeId resultingTypeId, const ConstantValue constantValue) const {
    if (std::holds_alternative<int>(constantValue)) {
        switch (resultingTypeId) {
            case INT_TYPE_ID: return constantValue;
            case FLOAT_TYPE_ID: return static_cast<float>(std::get<int>(constantValue));
            case CHAR_TYPE_ID: return static_cast<uint32_t>(std::get<int>(constantValue));
        }
    }
    if (std::holds_alternative<float>(constantValue)) {
        switch (resultingTypeId) {
            case INT_TYPE_ID: return static_cast<int>(std::get<float>(constantValue));
            case FLOAT_TYPE_ID: return constantValue;
            case CHAR_TYPE_ID: return static_cast<uint32_t>(std::get<float>(constantValue));
        }
    }
    if (std::holds_alternative<bool>(constantValue)) {
        return constantValue;
    }
    if (std::holds_alternative<uint32_t>(constantValue)) {
        switch (resultingTypeId) {
            case INT_TYPE_ID: return static_cast<int>(std::get<uint32_t>(constantValue));
            case FLOAT_TYPE_ID: return static_cast<float>(std::get<uint32_t>(constantValue));
            case CHAR_TYPE_ID: return constantValue;
        }
    }
}

void ConstantFolder::convertLiteralToType(const TypeId resultingTypeId, std::unique_ptr<ast::Expr>& literal, const ConstantValue constantValue) const {
    if (literal->resultingType.typeId == resultingTypeId) return;

    const auto convertedConstantValue = this->convertConstantValueToType(resultingTypeId, constantValue);

    if (std::holds_alternative<int>(convertedConstantValue)) {
        auto constExpr = std::make_unique<ast::ExprIntegerLiteral>(
            literal->line,
            literal->column,
            std::get<int>(convertedConstantValue)
        );
        constExpr->resultingType = Type{INT_TYPE_ID, 0};
        literal = std::move(constExpr);
    }
    else if (std::holds_alternative<float>(convertedConstantValue)) {
        auto constExpr = std::make_unique<ast::ExprFloatLiteral>(
            literal->line,
            literal->column,
            std::get<float>(convertedConstantValue)
        );
        constExpr->resultingType = Type{FLOAT_TYPE_ID, 0};
        literal = std::move(constExpr);
    }
    else if (std::holds_alternative<bool>(convertedConstantValue)) {
        auto constExpr = std::make_unique<ast::ExprBoolLiteral>(
            literal->line,
            literal->column,
            std::get<bool>(convertedConstantValue)
        );
        constExpr->resultingType = Type{BOOL_TYPE_ID, 0};
        literal = std::move(constExpr);
    }
    else if (std::holds_alternative<uint32_t>(convertedConstantValue)) {
        auto constExpr = std::make_unique<ast::ExprCharLiteral>(
            literal->line,
            literal->column,
            std::get<uint32_t>(convertedConstantValue)
        );
        constExpr->resultingType = Type{CHAR_TYPE_ID, 0};
        literal = std::move(constExpr);
    }
}

template <typename TLeft, typename TRight>
bool ConstantFolder::foldEqualityOrComparisonOperator(const TLeft& left, const TRight& right, const BinaryOperator binaryOperator) const {
    switch (binaryOperator) {
        case BinaryOperator::EQUAL_EQUAL: return left == right;
        case BinaryOperator::NOT_EQUAL: return left != right;
        case BinaryOperator::LESS_THAN: return left < right;
        case BinaryOperator::LESS_THAN_OR_EQUAL: return left <= right;
        case BinaryOperator::GREATER_THAN: return left > right;
        case BinaryOperator::GREATER_THAN_OR_EQUAL: return left >= right;
        default:
            throw std::runtime_error("invalid const fold");
    }
}

void ConstantFolder::throwDivisionByZeroError(const uint32_t line, const uint16_t column, const std::string& message) const {
    throw SemanticError(
        this->path,
        line,
        column,
        SemanticErrorType::DIVISION_BY_ZERO,
        message
    );
}
