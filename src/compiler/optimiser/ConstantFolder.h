#ifndef SV_CONSTANTFOLDER_H
#define SV_CONSTANTFOLDER_H


#include <filesystem>

#include "../AST.h"

namespace compiler {

    using ConstantValue = std::variant<
        int,
        float,
        bool,
        uint32_t
    >;

    class ConstantFolder {
    public:
        ConstantFolder(const std::filesystem::path& filePath);

        void optimise(const ast::Program& program) const;

    private:
        void foldStm(ast::Stm& stm) const;
        void foldStmBlock(const ast::StmBlock& stmBlock) const;
        bool foldExpr(std::unique_ptr<ast::Expr>& expr) const;
        void foldArrayInitialiserElement(const TypeId convertElementToType, ast::ArrayInitialiserElement& arrayInitialiserElement) const;

        std::unique_ptr<ast::Expr> foldExprUnary(const ast::ExprUnary& exprUnary) const;
        std::unique_ptr<ast::Expr> foldExprBinary(const ast::ExprBinary& exprBinary) const;
        std::unique_ptr<ast::Expr> foldExprCast(const ast::ExprCast& exprCast) const;

        ConstantValue extractConstantValue(const ast::Expr& expr) const;
        ConstantValue convertConstantValueToType(const TypeId resultingTypeId, const ConstantValue constantValue) const;
        void convertLiteralToType(const TypeId resultingTypeId, std::unique_ptr<ast::Expr>& literal, const ConstantValue constantValue) const;

        template <typename TLeft, typename TRight>
        bool foldEqualityOrComparisonOperator(const TLeft& left, const TRight& right, const BinaryOperator binaryOperator) const;

        void throwDivisionByZeroError(const uint32_t line, const uint16_t column, const std::string& message) const;

        const std::filesystem::path& path;
    };

}


#endif //SV_CONSTANTFOLDER_H