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
        void processProgram(const ast::Program& program);
        void processStm(const ast::Stm& stm);
        void processStmVarDecl(const ast::StmVarDecl& varDecl);

        void processExpr(const ast::Expr& expr);
        void processExprIntegerLiteral(const ast::ExprIntegerLiteral& literal);
        void processExprIdentifier(const ast::ExprIdentifier& identifier);

        void processGlobalVariables();

        static std::string typeToString(ast::Type type);

        void emit(const std::string& code);

        std::vector<std::string> generatedCode;
        SymbolTable* symbolTable;
    };
}


#endif //SVM_CODEGENERATOR_H