#ifndef SVM_CODEGENERATOR_H
#define SVM_CODEGENERATOR_H
#include <unordered_set>

#include "Assembly.h"
#include "../AST.h"
#include "../SymbolTable.h"


namespace compiler {
    class AssemblyGenerator {
    public:
        AssemblyGenerator(SymbolTable* symbolTable);

        std::vector<AssemblyItem> compileProgram(const ast::Program& program);

        std::unordered_set<BuiltinFunctionId> getRequiredBuiltinFunctions();
        std::unordered_set<BuiltinDataId> getRequiredBuiltinData();

    private:
        void compileUserDefinedFunction(const ast::FunctionDecl& functionDecl);
        void compileFunctionDeclaration(const std::string& functionIdentifier, const ast::Block& body, const uint8_t numberOfArguments, const uint32_t numberOfLocals, const bool includeDefualtReturn, const uint32_t line, const uint16_t column, const uint32_t functionBodyEndLine, const uint16_t functionBodyEndColumn);
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
        void compileBinaryOperator(const ast::BinaryOperatorInfo& binaryOperatorinfo);
        void compileUnaryExpr(Scope* scope, const ast::ExprUnaryOperator& expr);
        void compileCastExpr(Scope* scope, const ast::ExprCast& castExpr);

        void compileFunctionCall(Scope* scope, const ast::FunctionCall& functionCall);
        void compileExprIdentifier(Scope* scope, const ast::ExprIdentifier& identifier);

        void compileExprIntegerLiteral(const ast::ExprIntegerLiteral& literal);
        void compileExprFloatLiteral(const ast::ExprFloatLiteral& floatLiteral);
        void compileExprBoolLiteral(const ast::ExprBoolLiteral& boolLiteral);

        void compileTypeConversionIfRequired(const Type currentType, const Type newType, const uint32_t line, const uint16_t column);

        void compileBuiltinFunctions();
        void compileGlobalVariables();

        static std::string typeToString(const Type& type);

        std::string generateLabel(const std::string& label);
        static std::string generateScopeFunctionIdentifier(const uint32_t scopeFunctionNumber);

        void emit(const AssemblyItem& assemblyItem);
        static AssemblyType toAssemblyType(const Type& type);
        static Number getDefaultNumber(const Type& type);

        std::vector<AssemblyItem> assembly;
        SymbolTable* symbolTable;
        uint32_t labelCounter;
        uint32_t scopeFunctionCounter;

        std::vector<const ast::Block*> pendingScopeFunctions;
        std::unordered_set<BuiltinFunctionId> requiredBuiltinFunctions;
        std::unordered_set<BuiltinDataId> requiredBuiltinData;
    };
}


#endif //SVM_CODEGENERATOR_H