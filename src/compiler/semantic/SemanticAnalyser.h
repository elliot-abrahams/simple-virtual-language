#ifndef SVM_TYPECHECKER_H
#define SVM_TYPECHECKER_H
#include <filesystem>

#include "SymbolTable.h"
#include "TypeRegistry.h"
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
        SemanticAnalysisResult processStmBlock(const ast::StmBlock& block, const ScopeKind scopeKind);
        SemanticAnalysisResult processStmBlockWithScope(Scope* scope, const ast::StmBlock& block);
        SemanticAnalysisResult processFunctionBody(const ast::FunctionDecl& functionDecl, FunctionSymbol* functionSymbol);
        SemanticAnalysisResult processStmVarDecl(Scope* scope, const ast::StmVarDecl& varDecl, const bool markAsInitialised);
        SemanticAnalysisResult processStmAssignment(Scope* scope, const ast::StmAssignment& assignment);
        SemanticAnalysisResult processStmIf(Scope* scope, const ast::StmIf& ifStm);
        SemanticAnalysisResult processStmWhile(Scope* scope, const ast::StmWhile& whileStm);
        SemanticAnalysisResult processStmFor(Scope* scope, const ast::StmFor& forStm);
        SemanticAnalysisResult processStmContinue(Scope* scope, const ast::StmContinue& continueStm);
        SemanticAnalysisResult processStmBreak(Scope* scope, const ast::StmBreak& breakStm);
        SemanticAnalysisResult processStmReturn(Scope* scope, const ast::StmReturn& returnStm);
        SemanticAnalysisResult processStmExpr(Scope* scope, const ast::StmExpr& expressionStm);

        void processIndex(Scope* scope, const ast::Index& index);
        unsigned int resolveAccessArrayDepth(const Type& arrayType, const std::vector<std::unique_ptr<ast::Index>>& indices) const;

        ExpressionInfo processExpr(Scope* scope, const ast::Expr& expr);

        ExpressionInfo processExprTernary(Scope* scope, const ast::ExprTernary& exprTernary);
        ExpressionInfo processExprBinary(Scope* scope, const ast::ExprBinary& exprBinary);
        ExpressionInfo processExprUnary(Scope* scope, const ast::ExprUnary& exprUnary);
        ExpressionInfo processExprPostfix(Scope* scope, const ast::ExprPostfix& exprPostfix);
        ExpressionInfo processExprCast(Scope* scope, const ast::ExprCast& exprCast);
        ExpressionInfo processExprFunctionCall(Scope* scope, const ast::ExprFunctionCall& exprFunctionCall);
        ExpressionInfo processExprNew(Scope* scope, const ast::ExprNew& exprNew);
        ExpressionInfo processExprIdentifier(Scope* scope, const ast::ExprIdentifier& exprIdentifier, const bool markAsInitialised);
        ExpressionInfo processExprIntegerLiteral(const ast::ExprIntegerLiteral& exprIntegerLiteral);
        ExpressionInfo processExprFloatLiteral(const ast::ExprFloatLiteral& exprFloatLiteral);
        ExpressionInfo processExprBoolLiteral(const ast::ExprBoolLiteral& exprBoolLiteral);
        ExpressionInfo processExprCharLiteral(const ast::ExprCharLiteral& exprCharLiteral);

        FunctionSymbol* resolveFunctionCall(std::vector<FunctionSymbol>* functionSymbols, const ast::ExprFunctionCall& functionCall, const std::vector<Type>& argumentTypes) const;

        Type processTernaryOperator(const ast::ExprTernary& ternaryOperator);
        Type processBinaryOperator(const ast::BinaryOperatorInfo& binaryOperatorInfo, const Type& leftType, const Type& rightType);
        Type processAddition(const ast::BinaryOperatorInfo& binaryOperatorInfo, const Type& leftType, const Type& rightType);
        Type processSubtraction(const ast::BinaryOperatorInfo& binaryOperatorInfo, const Type& leftType, const Type& rightType);
        Type processMultiplication(const ast::BinaryOperatorInfo& binaryOperatorInfo, const Type& leftType, const Type& rightType);
        Type processDivision(const ast::BinaryOperatorInfo& binaryOperatorInfo, const Type& leftType, const Type& rightType);
        Type processIntegerDivision(const ast::BinaryOperatorInfo& binaryOperatorInfo, const Type& leftType, const Type& rightType);
        Type processModulo(const ast::BinaryOperatorInfo& binaryOperatorInfo, const Type& leftType, const Type& rightType);
        Type processBitwiseOp(const ast::BinaryOperatorInfo& binaryOperatorInfo, const Type& leftType, const Type& rightType);
        Type processLogical(const ast::BinaryOperatorInfo& binaryOperatorInfo, const Type& leftType, const Type& rightType);
        Type processEquality(const ast::BinaryOperatorInfo& binaryOperatorInfo, const Type& leftType, const Type& rightType);
        Type processComparison(const ast::BinaryOperatorInfo& binaryOperatorInfo, const Type& leftType, const Type& rightType);

        static bool canCastToType(const Type& from, const Type& to);
        static bool canImplicitlyConvert(const Type& from, const Type& to);
        Symbol* checkSymbolIsDefined(Scope* scope, const std::string& identifier, const size_t line, const size_t column) const;

        static std::string typeToString(const Type& type);
        static std::string binaryOperatorToString(const BinaryOperator& binaryOperator);
        static std::string unaryOperatorToString(const UnaryOperator& unaryOperator);
        static std::string assignmentOperatorToString(const AssignmentOperator& assignmentOperator);

        void throwTypeErrorFromTernaryOperator(const ast::ExprTernary& ternaryOperator);
        void throwTypeErrorFromUnaryOperator(const ast::ExprUnary& unaryOperator, const Type& type) const;
        void throwTypeErrorFromBinaryOperator(const ast::BinaryOperatorInfo& binaryOperatorInfo, const Type& leftType, const Type& rightType) const;
        void throwInvalidExpressionTypeAsStatement(const ast::Expr& expr) const;
        void throwTypeErrorFromForVariable(const Type& variableType, const uint32_t forVariableLine, const uint16_t forVariableColumn) const;
        void throwTypeErrorFromForRange(const ast::Expr& rangeExpr, const Type& rangeType, const Type& variableType) const;
        void checkConditionTypeIsBool(const Type& conditionType, const size_t line, const size_t column) const;

        static std::string functionSignatureToString(const std::string& functionIdentifier, const std::vector<Type>& parameterTypes);

        SymbolTable* symbolTable;
        TypeRegistry* typeRegistry;
        const std::filesystem::path *path;
    };
}


#endif //SVM_TYPECHECKER_H