#ifndef SVM_TYPECHECKER_H
#define SVM_TYPECHECKER_H
#include <filesystem>

#include "SymbolTable.h"
#include "AST.h"


namespace compiler {

    struct SemanticAnalysisResult {
        bool alwaysReturns = false;
    };

    enum class ExprCategory {
        VALUE,
        ASSIGNABLE
    };

    struct SemanticExprResult {
        SemanticType type;
        ExprCategory category;
    };

    class SemanticAnalyser {
    public:
        SemanticAnalyser(SymbolTable* symbolTable, const std::filesystem::path* filePath);

        void processProgram(const ast::Program& program);

    private:
        void processFunctionDecl(const ast::FunctionDecl& functionDecl);
        std::vector<SemanticType> processParameterList(const std::vector<std::unique_ptr<ast::Parameter>>& parameterList);

        SemanticAnalysisResult processStm(Scope* scope, const ast::Stm& stm);
        SemanticAnalysisResult processBlock(const ast::Block& block, const ScopeKind scopeKind);
        SemanticAnalysisResult processFunctionBody(const ast::FunctionDecl& functionDecl, FunctionSymbol* functionSymbol);
        SemanticAnalysisResult processStmVarDecl(Scope* scope, const ast::StmVarDecl& varDecl);
        SemanticAnalysisResult processAssignment(Scope* scope, const ast::StmAssignment& assignment);
        SemanticAnalysisResult processIfStatement(Scope* scope, const ast::IfStm& ifStm);
        SemanticAnalysisResult processWhileStatement(Scope* scope, const ast::WhileStm& whileStm);
        SemanticAnalysisResult processContinueStatement(Scope* scope, const ast::ContinueStm& continueStm);
        SemanticAnalysisResult processBreakStatement(Scope* scope, const ast::BreakStm& breakStm);
        SemanticAnalysisResult processReturnStatement(Scope* scope, const ast::ReturnStm& returnStm);
        SemanticAnalysisResult processExpressionStatement(Scope* scope, const ast::ExpressionStatement& expressionStm);

        void processIndex(Scope* scope, const ast::Index& index);
        unsigned int resolveAccessArrayDepth(const SemanticType& arrayType, const std::vector<std::unique_ptr<ast::Index>>& indices) const;

        SemanticExprResult checkExprType(Scope* scope, const ast::Expr& expr);
        SemanticExprResult processFunctionCall(Scope* scope, const ast::FunctionCall& functionCall);
        FunctionSymbol* resolveFunctionCall(std::vector<FunctionSymbol>* functionSymbols, const ast::FunctionCall& functionCall, const std::vector<SemanticType>& argumentTypes) const;

        static bool canImplicitlyConvert(const SemanticType& from, const SemanticType& to);
        Symbol* checkSymbolIsDefined(Scope* scope, const std::string& identifier, const size_t line, const size_t column) const;

        static std::string typeToString(const SemanticType& type);
        static std::string binaryOperatorToString(const BinaryOperator& binaryOperator);
        static std::string unaryOperatorToString(const UnaryOperator& unaryOperator);

        void throwTypeErrorFromBinaryOperator(const ast::ExprBinaryOperator& binaryOperator, const SemanticType& leftType, const SemanticType& rightType) const;
        void throwInvalidExpressionTypeAsStatement(const ast::Expr& expr);
        void checkType(const std::vector<SemanticType>& expectedTypes, const SemanticType& actualType, const size_t line, const size_t column) const;
        static std::string functionSignatureToString(const std::string& functionIdentifier, const std::vector<SemanticType>& parameterTypes);

        SymbolTable* symbolTable;
        const std::filesystem::path *path;
    };
}


#endif //SVM_TYPECHECKER_H