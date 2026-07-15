#ifndef SVM_SYMBOLTABLE_H
#define SVM_SYMBOLTABLE_H
#include <optional>
#include <unordered_map>

#include "AST.h"


namespace compiler {

    struct Symbol {
        const ast::Type type;
        mutable bool isInitialised;
    };

    class SymbolTable {
    public:
        SymbolTable();

        void declareGlobalVariable(const std::string& identifier, const ast::Type& type, const bool isInitialised);

        std::unordered_map<std::string, Symbol>& getGlobalVariables();
        std::optional<Symbol*> getGlobalVariable(const std::string& identifier);

    private:
        std::unordered_map<std::string, Symbol> globals;
    };
}


#endif //SVM_SYMBOLTABLE_H