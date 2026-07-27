#ifndef SVM_TYPECHECKER_H
#define SVM_TYPECHECKER_H
#include <filesystem>

#include "SymbolTable.h"
#include "AST.h"


namespace compiler {

    struct SemanticAnalysisResult {
        bool alwaysReturns = false;
    };

    class SemanticAnalyser {
    public:
        SemanticAnalyser(SymbolTable* symbolTable, const std::filesystem::path* filePath);

        void processProgram(const ast::Program& program);

    private:
        void declareGlobalStmVarDecl(Scope* scope, const ast::StmVarDecl& varDecl);
        void processFunctionDecl(const ast::FunctionDecl& functionDecl);
        std::vector<Type> processParameterList(const std::vector<std::unique_ptr<ast::Parameter>>& parameterList);

        SemanticAnalysisResult processStm(Scope* scope, const ast::Stm& stm);
        SemanticAnalysisResult processBlock(const ast::Block& block, const ScopeKind scopeKind);
        SemanticAnalysisResult processFunctionBody(const ast::FunctionDecl& functionDecl, FunctionSymbol* functionSymbol);
        SemanticAnalysisResult processStmVarDecl(Scope* scope, const ast::StmVarDecl& varDecl);
        SemanticAnalysisResult processAssignment(Scope* scope, const ast::StmAssignment& assignment);
        SemanticAnalysisResult processIfStatement(Scope* scope, const ast::IfStm& ifStm);
        SemanticAnalysisResult processWhileStatement(Scope* scope, const ast::WhileStm& whileStm);
        SemanticAnalysisResult processContinueStatement(Scope* scope, const ast::ContinueStm& continueStm);
        SemanticAnalysisResult processBreakStatement(Scope* scope, const ast::BreakStm& breakStm);
        SemanticAnalysisResult processFunctionCallStatement(Scope* scope, const ast::FunctionCallStm& functionCallStm);
        SemanticAnalysisResult processReturnStatement(Scope* scope, const ast::ReturnStm& returnStm);

        Type checkExprType(Scope* scope, const ast::Expr& expr);
        SemanticAnalysisResult processFunctionCall(FunctionSymbol* functionSymbol, const ast::FunctionCall& functionCall, const std::vector<Type>& argumentTypes) const;

        Symbol* checkSymbolIsDefined(Scope* scope, const std::string& identifier, const size_t line, const size_t column) const;

        static std::string typeToString(const Type& type);
        static std::string binaryOperatorToString(const BinaryOperator& binaryOperator);
        static std::string unaryOperatorToString(const UnaryOperator& unaryOperator);
        void checkType(const std::vector<Type>& expectedTypes, const Type& actualType, const size_t line, const size_t column) const;

        SymbolTable* symbolTable;
        const std::filesystem::path *path;
    };
}


#endif //SVM_TYPECHECKER_H