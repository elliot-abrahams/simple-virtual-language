#include "SymbolTable.h"

#include <stdexcept>

compiler::SymbolTable::SymbolTable() {}

bool compiler::Symbol::isGlobal() const {
    return this->scope->isGlobalScope();
}

void compiler::SymbolTable::declareFunction(const std::string& functionIdentifier, const Type& returnType, const std::vector<Type>& parameterTypes) {
    this->functions.insert(std::make_pair(functionIdentifier, FunctionSymbol{returnType, parameterTypes}));
}

const compiler::FunctionSymbol *compiler::SymbolTable::getFunctionSymbol(const std::string &functionIdentifier) const {
    auto it = this->functions.find(functionIdentifier);
    if (it != this->functions.end()) {
        return &it->second;
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

compiler::Scope* compiler::SymbolTable::enterFunctionScope(const std::string& functionIdentifier) {
    const auto newScope = new Scope{this->scopeStack.top(), {}, ScopeKind::FUNCTION, {}};
    this->scopeStack.top()->children.push_back(newScope); // add new scope to parent scope's list of children
    this->scopeStack.push(newScope);
    this->scopes.push_back(*newScope);
    // add functionSymbol to stack
    this->functionSymbolStack.push(&this->functions.at(functionIdentifier));
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
