#ifndef SVM_TYPECHECKER_H
#define SVM_TYPECHECKER_H
#include <filesystem>

#include "SymbolTable.h"


namespace compiler {
    class TypeChecker {
    public:
        TypeChecker(SymbolTable* symbolTable, const std::filesystem::path* filePath);

        void checkProgram(const ast::Program& program) const;

    private:
        void checkStm(const ast::Stm& stm) const;
        void checkStmVarDecl(const ast::StmVarDecl& varDecl) const;

        ast::Type getTypeOfExpr(const ast::Expr& expr) const;

        SymbolTable* symbolTable;
        const std::filesystem::path *path;
    };
}


#endif //SVM_TYPECHECKER_H