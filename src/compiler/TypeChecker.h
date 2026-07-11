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

        ast::Type getTypeOfExpr(const ast::Expr& expr) const;

        static std::string typeToString(const ast::Type& type);

        SymbolTable* symbolTable;
        const std::filesystem::path *path;
    };
}


#endif //SVM_TYPECHECKER_H