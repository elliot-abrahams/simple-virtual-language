#ifndef SVM_SYMBOLTABLE_H
#define SVM_SYMBOLTABLE_H
#include <cstdint>
#include <memory>
#include <string>
#include <optional>
#include <stack>
#include <unordered_map>
#include <vector>

#include "LanguageTypes.h"


namespace compiler {

    struct Scope;

    struct Symbol {
        Scope* scope;
        const Type type;
        int localSlot;
        mutable bool isInitialised;

        bool isGlobal() const;
    };

    struct FunctionSymbol {
        std::string label;
        const Type returnType;
        const std::vector<Type> parameterTypes;
    };;

    enum class ScopeKind {
        GLOBAL,
        BLOCK,
        FUNCTION
    };

    struct Scope {
        Scope* parent;
        std::vector<Scope*> children;

        ScopeKind kind;
        std::unordered_map<std::string, Symbol> symbols;

        void declareSymbol(const std::string& identifier, const Type& type, const int localSlot, const bool isInitialised) {
            this->symbols.insert(std::make_pair(identifier, Symbol{this, type, localSlot, isInitialised}));
        }

        std::optional<Symbol*> lookup(const std::string& identifier) {
            // search in this scope's symbols
            auto it = this->symbols.find(identifier);
            if (it != this->symbols.end()) {
                // symbol found in current scope
                return &it->second;
            }

            if (this->isFunctionScope()) {
                // don't look up symbols outside the current function scope, except in the global scope
                Scope* scopeToCheck = this->parent;
                // search for global scope
                while (!scopeToCheck->isGlobalScope()) {
                    scopeToCheck = scopeToCheck->parent;
                }
                // lookup symbol in global scope
                return scopeToCheck->lookup(identifier);
            }
            if (this->isGlobalScope()) {
                return std::nullopt;
            }
            // symbol not found in current scope
            return this->parent->lookup(identifier);
        }

        uint32_t calculateNumberOfLocalSlots(const uint32_t currentSlots = 0) const
        {
            const uint32_t maxLocalSlots = currentSlots + symbols.size();

            uint32_t slots = maxLocalSlots;

            for (auto& child : children)
            {
                slots = std::max(
                    slots,
                    child->calculateNumberOfLocalSlots(maxLocalSlots)
                );
            }

            return slots;
        }

        void assignSlotsToLocalSymbols(const int startingSlot) {
            int currentSlot = startingSlot;
            for (auto& symbol : this->symbols) {
                symbol.second.localSlot = currentSlot;
                currentSlot--;
            }

            for (const auto& child : this->children) {
                child->assignSlotsToLocalSymbols(currentSlot);
            }
        }

        bool isGlobalScope() const {
            return this->kind == ScopeKind::GLOBAL;
        }

        bool isFunctionScope() const {
            return this->kind == ScopeKind::FUNCTION;
        }
    };

    class SymbolTable {
    public:
        SymbolTable();

        bool declareFunction(const std::string& functionIdentifier, const Type& returnType, const std::vector<Type>& parameterTypes);

        FunctionSymbol* getFunctionSymbol(const std::string& functionIdentifier, const std::vector<Type>& parameterTypes);
        const FunctionSymbol* getCurrentFunctionSymbol() const;

        std::unordered_map<std::string, Symbol>& getGlobalVariables() const;

        Scope* enterScope();
        Scope* enterFunctionScope(const std::string& functionIdentifier, FunctionSymbol* functionSymbol);
        void leaveScope();

        void assignSlotsToLocalSymbols() const;

        Scope* globalScope;

    private:

        std::vector<Scope> scopes;
        std::stack<Scope*> scopeStack;

        std::unordered_map<std::string, std::vector<FunctionSymbol>> functions;
        std::stack<FunctionSymbol*> functionSymbolStack;

        Scope* getCurrentFunctionScope();
    };
}


#endif //SVM_SYMBOLTABLE_H