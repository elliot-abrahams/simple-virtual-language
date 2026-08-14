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
        void compileWhileStatement(Scope* scope, const ast::WhileStm& whileStm);
        void compileContinueStatement(Scope* scope, const ast::ContinueStm& continueStm);
        void compileBreakStatement(Scope* scope, const ast::BreakStm& breakStm);
        void compileFunctionCallStatement(Scope* scope, const ast::FunctionCallStm& functionCallStm);
        void compileReturnStatement(Scope* scope, const ast::ReturnStm& returnStm);

        void compileExpr(Scope* scope, const ast::Expr& expr);
        void compileBinaryExpr(Scope* scope, const ast::ExprBinaryOperator& expr);
        void compileBinaryOperator(const BinaryOperator binaryOperator);
        void compileUnaryExpr(Scope* scope, const ast::ExprUnaryOperator& expr);

        void compileFunctionCall(Scope* scope, const ast::FunctionCall& functionCall);
        void compileExprIdentifier(Scope* scope, const ast::ExprIdentifier& identifier);

        void compileExprIntegerLiteral(const ast::ExprIntegerLiteral& literal);
        void compileExprFloatLiteral(const ast::ExprFloatLiteral& floatLiteral);
        void compileExprBoolLiteral(const ast::ExprBoolLiteral& boolLiteral);

        void compileBuiltinFunctions();
        void compileGlobalVariables();
        void compileBuiltinData();

        static std::string typeToString(const Type& type);

        std::string generateLabel(const std::string& label);
        static std::string generateLabelDefFromLabel(const std::string& label);
        static std::string generateScopeFunctionIdentifier(const uint32_t scopeFunctionNumber);

        void emitWithDoubleIndentConversion(const Type currentType, const Type newType);
        void emitWithDoubleIndent(const std::string& assembly);
        void emitWithSingleIndent(const std::string& assembly);
        void emit(const std::string& code);

        std::vector<std::string> generatedCode;
        SymbolTable* symbolTable;
        uint32_t labelCounter;
        uint32_t scopeFunctionCounter;

        std::vector<const ast::Block*> pendingScopeFunctions;
        std::unordered_set<BuiltinId> calledBuiltins;
        std::unordered_set<BuiltinDataId> calledBuiltinData;
    };
}


#endif //SVM_CODEGENERATOR_H