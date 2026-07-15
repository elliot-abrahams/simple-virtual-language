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
        void compileBinaryExpr(const ast::ExprBinaryOperator& expr);
        void compileUnaryExpr(const ast::ExprUnaryOperator& expr);

        void compileExprIdentifier(const ast::ExprIdentifier& identifier);
        void compileVarAccess(const ast::VarAccess& varAccess);

        void compileExprIntegerLiteral(const ast::ExprIntegerLiteral& literal);
        void compileExprFloatLiteral(const ast::ExprFloatLiteral& floatLiteral);
        void compileExprBoolLiteral(const ast::ExprBoolLiteral& boolLiteral);

        void compileGlobalVariables();

        static std::string typeToString(const ast::Type& type);

        std::string generateLabel(const std::string& label);

        void emit(const std::string& code);
        void emitStartOfDataRegion();
        void emitLabelDef(const std::string& label);

        std::vector<std::string> generatedCode;
        SymbolTable* symbolTable;
        uint32_t labelCounter;
    };
}


#endif //SVM_CODEGENERATOR_H