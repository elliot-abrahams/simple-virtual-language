#ifndef SVM_SYMBOLTABLE_H
#define SVM_SYMBOLTABLE_H
#include <cstdint>
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

    enum class ScopeKind {
        GLOBAL,
        BLOCK
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
            auto it = this->symbols.find(identifier);
            if (it != this->symbols.end()) {
                return &it->second;
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

        bool isGlobalScope() const {
            return this->kind == ScopeKind::GLOBAL;
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
    };

    class SymbolTable {
    public:
        SymbolTable();

        std::unordered_map<std::string, Symbol>& getGlobalVariables() const;

        Scope* enterScope();
        void leaveScope();

        void assignSlotsToLocalSymbols() const;

        Scope* globalScope;

    private:

        std::vector<Scope> scopes;
        std::stack<Scope*> scopeStack;
    };
}


#endif //SVM_SYMBOLTABLE_H