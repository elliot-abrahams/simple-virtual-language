#ifndef SVM_CODEGENERATOR_H
#define SVM_CODEGENERATOR_H
#include <unordered_set>

#include "Assembly.h"
#include "../AST.h"
#include "../semantic/SymbolTable.h"


namespace compiler {

    enum class ExprResult {
        VALUE,   // produce value
        PTR,     // produce pointer to value
        DISCARD  // produce nothing
    };

    using ScopeFunctionBody = std::variant<
        const ast::StmBlock*,
        const ast::StmFor*
    >;

    class AssemblyGenerator {
    public:
        AssemblyGenerator(SymbolTable* symbolTable);

        std::vector<AssemblyItem> compileProgram(const ast::Program& program);

        std::unordered_set<BuiltinFunctionId> getRequiredBuiltinFunctions();
        std::unordered_set<BuiltinDataId> getRequiredBuiltinData();

    private:
        void compileUserDefinedFunction(const ast::FunctionDecl& functionDecl);
        void compileFunctionDeclaration(const MethodDefType methodType, const std::string& functionIdentifier, const ast::Stm& body, const uint8_t numberOfArguments, const uint32_t numberOfLocals, const bool includeDefaultReturn, const uint32_t line, const uint16_t column, const uint32_t functionBodyEndLine, const uint16_t functionBodyEndColumn);
        void compilePendingScopeFunctions();

        void compileArrayIndex(Scope* scope, const ast::Index& index, Type& typeAtDepth);

        void compileStm(Scope* scope, const ast::Stm& stm);
        void compileBlock(const ast::StmBlock& stmBlock);
        void compileStmVarDecl(Scope* scope, const ast::StmVarDecl& stmVarDecl);
        void compileStmAssignment(Scope* scope, const ast::StmAssignment& stmAssignment);
        void compileStmIf(Scope* scope, const ast::StmIf& stmIf);
        void compileStmWhile(Scope* scope, const ast::StmWhile& stmWhile);
        void compileStmFor(Scope* scope, const ast::StmFor& stmFor);
        void compileStmContinue(const Scope* scope, const ast::StmContinue& stmContinue);
        void compileStmBreak(const Scope* scope, const ast::StmBreak& stmBreak);
        void compileStmReturn(Scope* scope, const ast::StmReturn& stmReturn);
        void compileStmExpr(Scope* scope, const ast::StmExpr& stmExpr);

        void compileExpr(Scope* scope, const ast::Expr& expr, const ExprResult exprResult);
        void compileExprBinary(Scope* scope, const ast::ExprBinary& exprBinary);
        void compileExprBinaryWithLeftExprOnStack(Scope* scope, const ast::BinaryOperatorInfo& binaryOperatorInfo, const Type& exprResultingType, const uint32_t exprLine, const uint16_t exprColumn, const Type& leftType, const uint32_t leftLine, const uint16_t leftColumn, const ast::Expr& right);
        void compileBinaryOperator(const ast::BinaryOperatorInfo& binaryOperatorInfo);
        void compileExprUnary(Scope* scope, const ast::ExprUnary& exprUnary, const ExprResult exprResult);
        void compileExprPostfix(Scope* scope, const ast::ExprPostfix& exprPostfix, const ExprResult exprResult);
        void compileExprTernary(Scope* scope, const ast::ExprTernary& exprTernary, const ExprResult exprResult);
        void compileExprCast(Scope* scope, const ast::ExprCast& exprCast);
        void compileExprNew(Scope* scope, const ast::ExprNew& exprNew);
        void compileArrayAlloc(Scope* scope, const ast::ExprNew& exprNew, unsigned int depth, Type& typeAtDepth);
        void compileArrayInitialiser(Scope* scope, const ast::ArrayInitialiser& arrayInitialiser, const SourceLocation& optionalInitialiserSource, const Type& typeOfDeepestElement);
        void compileExprFunctionCall(Scope* scope, const ast::ExprFunctionCall& exprFunctionCall);
        void compileExprIdentifier(Scope* scope, const ast::ExprIdentifier& exprIdentifier, const ExprResult exprResult);
        void compileExprIntegerLiteral(const ast::ExprIntegerLiteral& exprIntegerLiteral);
        void compileExprFloatLiteral(const ast::ExprFloatLiteral& exprFloatLiteral);
        void compileExprBoolLiteral(const ast::ExprBoolLiteral& exprBoolLiteral);
        void compilerExprCharLiteral(const ast::ExprCharLiteral& exprCharLiteral);

        void compileTypeConversionIfRequired(const Type& currentType, const Type& newType, const SourceLocation& sourceLocation);
        void compileTypeConversionIfRequired(const AssemblyType currentType, const AssemblyType newType, const SourceLocation& sourceLocation);

        void compileBuiltinFunctions();
        void compileGlobalVariables();

        std::string generateLabel(const std::string& label);
        static std::string generateScopeFunctionIdentifier(const uint32_t scopeFunctionNumber);
        void registerScopeFunction(const ScopeFunctionBody& scopeFunctionBody, const uint32_t blockEndLine, const uint16_t blockEndColumn);

        void emit(const AssemblyItem& assemblyItem);

        static AssemblyType toAssemblyType(const Type& type);
        Number getNumber(const Type& type, const uint64_t value);
        Number getNumber(const AssemblyType, const uint64_t value);

        std::vector<AssemblyItem> assembly;
        SymbolTable* symbolTable;
        uint32_t labelCounter;
        uint32_t scopeFunctionCounter;
        bool isCompilingScopeFunctions = false;

        std::vector<ScopeFunctionBody> pendingScopeFunctions;
        std::unordered_set<BuiltinFunctionId> requiredBuiltinFunctions;
        std::unordered_set<BuiltinDataId> requiredBuiltinData;
    };
}


#endif //SVM_CODEGENERATOR_H