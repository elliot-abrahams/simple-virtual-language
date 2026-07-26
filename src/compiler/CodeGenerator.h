#ifndef SVM_CODEGENERATOR_H
#define SVM_CODEGENERATOR_H
#include <unordered_set>

#include "AST.h"
#include "SymbolTable.h"


namespace compiler {
    class CodeGenerator {
    public:
        CodeGenerator(SymbolTable* symbolTable);

        std::vector<std::string> generateCode(const ast::Program& program);

    private:
        void compileProgram(const ast::Program& program);

        void compileFunctionDeclaration(const std::string& functionIdentifier, const ast::Block& body, const uint8_t numberOfArguments, const uint32_t numberOfLocals, const bool includeDefualtReturn);
        void compilePendingScopeFunctions();

        void compileStm(Scope* scope, const ast::Stm& stm);
        void compileBlock(const ast::Block& block);
        void compileStmVarDecl(Scope* scope, const ast::StmVarDecl& varDecl);
        void compileStmAssignment(Scope* scope, const ast::StmAssignment& assignment);
        void compileIfStatement(Scope* scope, const ast::IfStm& ifStm);
        void compileWhileStatement(const ast::WhileStm& whileStm);
        void compileFunctionCallStatement(const ast::FunctionCallStm& functionCallStm);
        void compileReturnStatement(const ast::ReturnStm& returnStm);

        void compileExpr(const ast::Expr& expr);
        void compileBinaryExpr(const ast::ExprBinaryOperator& expr);
        void compileUnaryExpr(const ast::ExprUnaryOperator& expr);

        void compileFunctionCall(const ast::FunctionCall& functionCall);
        void compileExprIdentifier(const ast::ExprIdentifier& identifier);

        void compileExprIntegerLiteral(const ast::ExprIntegerLiteral& literal);
        void compileExprFloatLiteral(const ast::ExprFloatLiteral& floatLiteral);
        void compileExprBoolLiteral(const ast::ExprBoolLiteral& boolLiteral);

        void compileBuiltinFunctions();
        void compileGlobalVariables();

        static std::string typeToString(const Type& type);

        std::string generateLabel(const std::string& label);
        static std::string generateLabelDefFromLabel(const std::string& label);
        static std::string generateScopeFunctionIdentifier(const uint32_t scopeFunctionNumber);

        void emitWithIndent(const std::string& code);
        void emit(const std::string& code);

        std::vector<std::string> generatedCode;
        SymbolTable* symbolTable;
        uint32_t labelCounter;
        uint32_t scopeFunctionCounter;

        std::vector<const ast::Block*> pendingScopeFunctions;
        std::unordered_set<BuiltinId> calledBuiltins;
    };
}


#endif //SVM_CODEGENERATOR_H