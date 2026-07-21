#ifndef SVM_TYPECHECKER_H
#define SVM_TYPECHECKER_H
#include <filesystem>

#include "SymbolTable.h"
#include "AST.h"


namespace compiler {
    class SemanticAnalyser {
    public:
        SemanticAnalyser(SymbolTable* symbolTable, const std::filesystem::path* filePath);

        void processProgram(const ast::Program& program);

    private:
        void processStm(Scope* scope, const ast::Stm& stm);
        void processBlock(const ast::Block& block);
        void processStmVarDecl(Scope* scope, const ast::StmVarDecl& varDecl);
        void processAssignment(Scope* scope, const ast::StmAssignment& assignment);
        void processIfStatement(Scope* scope, const ast::IfStm& ifStm);

        Type checkExprType(Scope* scope, const ast::Expr& expr);

        Symbol* checkSymbolIsDefined(Scope* scope, const std::string& identifier, const size_t line, const size_t column);

        static std::string typeToString(const Type& type);
        static std::string binaryOperatorToString(const BinaryOperator& binaryOperator);

        SymbolTable* symbolTable;
        const std::filesystem::path *path;
    };
}


#endif //SVM_TYPECHECKER_H