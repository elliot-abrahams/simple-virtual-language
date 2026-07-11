#ifndef SVM_CODEGENERATOR_H
#define SVM_CODEGENERATOR_H
#include "AST.h"
#include "SymbolTable.h"


namespace compiler {
    class CodeGenerator {
    public:
        CodeGenerator(SymbolTable* symbolTable);

        std::vector<std::string> generateCode(const ast::Program& program);

    private:
        void compileProgram(const ast::Program& program);
        void compileStm(const ast::Stm& stm);
        void compileStmVarDecl(const ast::StmVarDecl& varDecl);
        void compileStmAssignment(const ast::StmAssignment& assignment);

        void compileExpr(const ast::Expr& expr);
        void compileExprIdentifier(const ast::ExprIdentifier& identifier);
        void compileVarAccess(const ast::VarAccess& varAccess);

        void compileExprIntegerLiteral(const ast::ExprIntegerLiteral& literal);

        void compileGlobalVariables();

        static std::string typeToString(const ast::Type& type);

        void emit(const std::string& code);

        std::vector<std::string> generatedCode;
        SymbolTable* symbolTable;
    };
}


#endif //SVM_CODEGENERATOR_H