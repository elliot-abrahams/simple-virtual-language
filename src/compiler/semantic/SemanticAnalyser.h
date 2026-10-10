#ifndef SVM_TYPECHECKER_H
#define SVM_TYPECHECKER_H
#include <filesystem>

#include "SymbolTable.h"
#include "../AST.h"


namespace compiler {

    struct SemanticAnalysisResult {
        bool alwaysReturns = false;
    };

    struct ExpressionInfo {
        Type type;
        Assignability assignability;
    };

    class SemanticAnalyser {
    public:
        SemanticAnalyser(SymbolTable* symbolTable, TypeRegistry* typeRegistry, const std::filesystem::path* filePath);

        void processProgram(const ast::Program& program);

    private:
        void processFunctionDecl(const ast::FunctionDecl& functionDecl);

        SemanticAnalysisResult processStm(Scope* scope, const ast::Stm& stm);
        SemanticAnalysisResult processBlock(const ast::Block& block, const ScopeKind scopeKind);
        SemanticAnalysisResult processBlockWithScope(Scope* scope, const ast::Block& block);
        SemanticAnalysisResult processFunctionBody(const ast::FunctionDecl& functionDecl, FunctionSymbol* functionSymbol);
        SemanticAnalysisResult processStmVarDecl(Scope* scope, const ast::StmVarDecl& varDecl, const bool markAsInitialised);
        SemanticAnalysisResult processAssignment(Scope* scope, const ast::StmAssignment& assignment);
        SemanticAnalysisResult processIfStatement(Scope* scope, const ast::IfStm& ifStm);
        SemanticAnalysisResult processWhileStatement(Scope* scope, const ast::WhileStm& whileStm);
        SemanticAnalysisResult processForStatement(Scope* scope, const ast::ForStm& forStm);
        SemanticAnalysisResult processContinueStatement(Scope* scope, const ast::ContinueStm& continueStm);
        SemanticAnalysisResult processBreakStatement(Scope* scope, const ast::BreakStm& breakStm);
        SemanticAnalysisResult processReturnStatement(Scope* scope, const ast::ReturnStm& returnStm);
        SemanticAnalysisResult processExpressionStatement(Scope* scope, const ast::ExpressionStatement& expressionStm);

        void processIndex(Scope* scope, const ast::Index& index);
        unsigned int resolveAccessArrayDepth(const Type& arrayType, const std::vector<std::unique_ptr<ast::Index>>& indices) const;

        ExpressionInfo processExpr(Scope* scope, const ast::Expr& expr);

        ExpressionInfo processBinaryExpr(Scope* scope, const ast::ExprBinaryOperator& binaryOperatorExpr);
        ExpressionInfo processUnaryExpr(Scope* scope, const ast::ExprUnaryOperator& unaryOperatorExpr);
        ExpressionInfo processPostfixExpr(Scope* scope, const ast::ExprPostfix& postfixExpr);
        ExpressionInfo processCastExpr(Scope* scope, const ast::ExprCast& castExpr);
        ExpressionInfo processFunctionCall(Scope* scope, const ast::FunctionCall& functionCall);
        ExpressionInfo processNewExpr(Scope* scope, const ast::ExprNew& newExpr);
        ExpressionInfo processIdentifierExpr(Scope* scope, const ast::ExprIdentifier& identifierExpr, const bool markAsInitialised);
        ExpressionInfo processIntegerLiteral(const ast::ExprIntegerLiteral& integerLiteral);
        ExpressionInfo processFloatLiteral(const ast::ExprFloatLiteral& floatLiteral);
        ExpressionInfo processBoolLiteral(const ast::ExprBoolLiteral& boolLiteral);
        ExpressionInfo processCharLiteral(const ast::ExprCharLiteral& charLiteral);

        FunctionSymbol* resolveFunctionCall(std::vector<FunctionSymbol>* functionSymbols, const ast::FunctionCall& functionCall, const std::vector<Type>& argumentTypes) const;

        Type processAddition(const ast::ExprBinaryOperator& binaryOperator, const Type& leftType, const Type& rightType);
        Type processSubtraction(const ast::ExprBinaryOperator& binaryOperator, const Type& leftType, const Type& rightType);
        Type processMultiplication(const ast::ExprBinaryOperator& binaryOperator, const Type& leftType, const Type& rightType);
        Type processDivision(const ast::ExprBinaryOperator& binaryOperator, const Type& leftType, const Type& rightType);
        Type processIntegerDivision(const ast::ExprBinaryOperator& binaryOperator, const Type& leftType, const Type& rightType);
        Type processModulo(const ast::ExprBinaryOperator& binaryOperator, const Type& leftType, const Type& rightType);
        Type processBitwiseOp(const ast::ExprBinaryOperator& binaryOperator, const Type& leftType, const Type& rightType);
        Type processLogical(const ast::ExprBinaryOperator& binaryOperator, const Type& leftType, const Type& rightType);
        Type processEquality(const ast::ExprBinaryOperator& binaryOperator, const Type& leftType, const Type& rightType);
        Type processComparison(const ast::ExprBinaryOperator& binaryOperator, const Type& leftType, const Type& rightType);

        static bool canCastToType(const Type& from, const Type& to);
        static bool canImplicitlyConvert(const Type& from, const Type& to);
        Symbol* checkSymbolIsDefined(Scope* scope, const std::string& identifier, const size_t line, const size_t column) const;

        static std::string typeToString(const Type& type);
        static std::string binaryOperatorToString(const BinaryOperator& binaryOperator);
        static std::string unaryOperatorToString(const UnaryOperator& unaryOperator);

        void throwTypeErrorFromUnaryOperator(const ast::ExprUnaryOperator& unaryOperator, const Type& type) const;
        void throwTypeErrorFromBinaryOperator(const ast::ExprBinaryOperator& binaryOperator, const Type& leftType, const Type& rightType) const;
        void throwInvalidExpressionTypeAsStatement(const ast::Expr& expr) const;
        void throwTypeErrorFromForVariable(const Type& variableType, const uint32_t forVariableLine, const uint16_t forVariableColumn) const;
        void throwTypeErrorFromForRange(const ast::Expr& rangeExpr, const Type& rangeType, const Type& variableType) const;
        void checkConditionType(const Type& conditionType, const size_t line, const size_t column) const;

        //static std::vector<SemanticType> get
        static std::string functionSignatureToString(const std::string& functionIdentifier, const std::vector<Type>& parameterTypes);

        SymbolTable* symbolTable;
        TypeRegistry* typeRegistry;
        const std::filesystem::path *path;
    };
}


#endif //SVM_TYPECHECKER_H