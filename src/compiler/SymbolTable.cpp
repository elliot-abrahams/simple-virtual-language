#include "SymbolTable.h"

#include <functional>
#include <stdexcept>

#include "../include/Error.h"

compiler::SymbolTable::SymbolTable() {}

bool compiler::Symbol::isGlobal() const {
    return this->scope->isGlobalScope();
}

bool compiler::SymbolTable::declareFunction(const std::string& functionIdentifier, const Type& returnType, const std::vector<Type>& parameterTypes) {
    if (this->functions.find(functionIdentifier) == this->functions.end()) {
        // function with the same identifier has not been initialised
        this->functions.insert(std::make_pair(functionIdentifier, std::vector{FunctionSymbol{"", returnType, parameterTypes, BuiltinId::NONE}}));
        return true;
    }
    // function with the same identifier has already been initialised
    const std::vector<FunctionSymbol> functionsWithSameName = this->functions.at(functionIdentifier);

    bool validSignature = true;

    // for each function with the same identifier
    for (const auto& function : functionsWithSameName) {
        // check parameter types are not identical
        int identicalTypeCount = 0;

        for (int parameterIndex = 0; parameterIndex < parameterTypes.size(); parameterIndex++) {
            if (parameterTypes.at(parameterIndex) == function.parameterTypes.at(parameterIndex)) {
                identicalTypeCount++;
            }
        }
        if (identicalTypeCount == function.parameterTypes.size()) {
            // function signature is identical
            validSignature = false;
            break;
        }
    }

    if (validSignature) {
        this->functions.at(functionIdentifier).push_back(FunctionSymbol{"", returnType, parameterTypes, BuiltinId::NONE});
    }
    return validSignature;
}

void compiler::SymbolTable::declareBuiltinFunction(const BuiltinId builtinId, const std::string &functionIdentifier, const Type &returnType, const std::vector<Type> &parameterTypes) {
    const auto newFunctionSymbol = FunctionSymbol{"", returnType, parameterTypes, builtinId};
    if (this->functions.find(functionIdentifier) == this->functions.end()) {
        // function with the same identifier has not been initialised
        this->functions.insert(std::make_pair(functionIdentifier, std::vector{newFunctionSymbol}));
    } else {
        this->functions.at(functionIdentifier).push_back(newFunctionSymbol);
    }
}

compiler::FunctionSymbol* compiler::SymbolTable::getFunctionSymbol(const std::string &functionIdentifier, const std::vector<Type>& parameterTypes) {
    auto it = this->functions.find(functionIdentifier);
    if (it != this->functions.end()) {

        // loop through each function with the given identifier
        for (auto& function : it->second) {
            if (parameterTypes.size() != function.parameterTypes.size()) {
                continue;
            }
            // loop through each parameter of variable function
            bool identicalSignature = true;
            for (int parameterIndex = 0; parameterIndex < parameterTypes.size(); parameterIndex++) {
                if (parameterTypes[parameterIndex] != function.parameterTypes[parameterIndex]) {
                    identicalSignature = false;
                    break;
                }
            }
            if (identicalSignature) {
                // all parameter types of variable function match
                return &function;
            }
        }
    }
    return nullptr;
}

const compiler::FunctionSymbol* compiler::SymbolTable::getCurrentFunctionSymbol() const {
    if (this->functionSymbolStack.empty()) {
        return nullptr;
    }
    return this->functionSymbolStack.top();
}

std::unordered_map<std::string, compiler::Symbol>& compiler::SymbolTable::getGlobalVariables() const {
    return this->globalScope->symbols;
}

compiler::Scope* compiler::SymbolTable::enterScope() {
    ScopeKind scopeKind;
    Scope* parent;
    if (this->scopeStack.empty()) {
        scopeKind = ScopeKind::GLOBAL;
        parent = nullptr;
    } else {
        scopeKind = ScopeKind::BLOCK;
        parent = this->scopeStack.top();
    }
    const auto scope = new Scope{parent, {}, scopeKind, {}};

    if (parent == nullptr) {
        this->globalScope = scope;
    } else {
        // add new scope to parent scope's list of children
        this->scopeStack.top()->children.push_back(scope);
    }
    this->scopeStack.push(scope);
    this->scopes.push_back(*scope);
    return scope;
}

compiler::Scope* compiler::SymbolTable::enterFunctionScope(const std::string& functionIdentifier, FunctionSymbol* functionSymbol) {
    const auto newScope = new Scope{this->scopeStack.top(), {}, ScopeKind::FUNCTION, {}};
    this->scopeStack.top()->children.push_back(newScope); // add new scope to parent scope's list of children
    this->scopeStack.push(newScope);
    this->scopes.push_back(*newScope);
    this->functionSymbolStack.push(functionSymbol);
    return newScope;
}

void compiler::SymbolTable::leaveScope() {
    if (this->scopeStack.top()->isFunctionScope()) {
        this->functionSymbolStack.pop();
    }
    this->scopeStack.pop();
}

void compiler::SymbolTable::assignSlotsToLocalSymbols() const {
    for (const auto& scope : this->globalScope->children) {
        scope->assignSlotsToLocalSymbols(-1);
    }
}

compiler::Scope *compiler::SymbolTable::getCurrentFunctionScope() {
    Scope* currentScopeToCheck = this->scopeStack.top();
    while (currentScopeToCheck->parent != nullptr) {
        if (currentScopeToCheck->isFunctionScope()) {
            return currentScopeToCheck;
        }
        currentScopeToCheck = currentScopeToCheck->parent;
    }
    return nullptr;
}
