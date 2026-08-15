#ifndef SVM_SYMBOLTABLE_H
#define SVM_SYMBOLTABLE_H
#include <string>
#include <optional>
#include <stack>
#include <unordered_map>
#include <vector>

#include "LanguageTypes.h"
#include "BuiltinFunctions.h"


namespace compiler {

    struct Scope;

    struct Symbol {
        Scope* scope;
        const Type type;
        int localSlot;
        mutable bool isInitialised;

        bool isGlobal() const;
        bool isArgument() const;
    };

    struct FunctionSymbol {
        std::string label;
        const Type returnType;
        const std::vector<Type> parameterTypes;

        const BuiltinId builtinId;
    };;

    struct LoopContext {
        std::string continueLabel;
        std::string breakLabel;
    };

    enum class ScopeKind {
        GLOBAL,
        BLOCK,
        FUNCTION,
        WHILE
    };

    struct Scope {
        Scope* parent;
        std::vector<Scope*> children;

        ScopeKind kind;
        std::unordered_map<std::string, Symbol> symbols;

        LoopContext* loopContext;

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

        const Scope* lookupWhileScope() const {
            auto scopeToCheck = this;
            while (!scopeToCheck->isGlobalScope() && !scopeToCheck->isFunctionScope()) {
                if (scopeToCheck->isWhileScope()) {
                    return scopeToCheck;
                }
                scopeToCheck = scopeToCheck->parent;
            }
            return nullptr;
        }

        uint32_t calculateNumberOfLocalSlots(const uint32_t currentSlots = 0) const {
            uint32_t maxLocalSlots = currentSlots;
            for (const auto& symbol : this->symbols) {
                if (!symbol.second.isArgument()) {
                    maxLocalSlots++;
                }
            }

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
                if (symbol.second.isArgument()) {
                    continue;
                }
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

        bool isWhileScope() const {
            return this->kind == ScopeKind::WHILE;
        }
    };

    class SymbolTable {
    public:
        SymbolTable();

        FunctionSymbol* declareFunction(const std::string& functionIdentifier, const std::string& functionLabel, const Type& returnType, const std::vector<Type>& parameterTypes);
        void declareBuiltinFunction(const BuiltinId builtinId, const std::string& functionIdentifier, const Type& returnType, const std::vector<Type>& parameterTypes);

        std::vector<FunctionSymbol>* getFunctionSymbols(const std::string& functionIdentifier, const std::vector<Type>& parameterTypes);
        const FunctionSymbol* getCurrentFunctionSymbol() const;

        std::unordered_map<std::string, Symbol>& getGlobalVariables() const;

        Scope* enterScope(const ScopeKind scopeKind);
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