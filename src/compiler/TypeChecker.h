#ifndef SVM_TYPECHECKER_H
#define SVM_TYPECHECKER_H
#include <filesystem>

#include "SymbolTable.h"


namespace compiler {
    class TypeChecker {
    public:
        TypeChecker(SymbolTable* symbolTable, const std::filesystem::path* filePath);

        void processProgram(const ast::Program& program);

    private:
        void processStm(const ast::Stm& stm);
        void processStmVarDecl(const ast::StmVarDecl& varDecl) const;
        void processAssignment(const ast::StmAssignment& assignment);

        ast::Type checkExprType(const ast::Expr& expr) const;

        Symbol* checkSymbolIsDefined(const std::string& identifier, const size_t line, const size_t column) const;

        static std::string typeToString(const ast::Type& type);
        static std::string binaryOperatorToString(const ast::BinaryOperator& arithmeticOperator);

        SymbolTable* symbolTable;
        const std::filesystem::path *path;
    };
}


#endif //SVM_TYPECHECKER_H