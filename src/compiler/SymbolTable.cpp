#include "SymbolTable.h"

compiler::SymbolTable::SymbolTable() {}

bool compiler::Symbol::isGlobal() const {
    return this->scope->isGlobalScope();
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

void compiler::SymbolTable::leaveScope() {
    this->scopeStack.pop();
}

void compiler::SymbolTable::assignSlotsToLocalSymbols() const {
    for (const auto& scope : this->globalScope->children) {
        scope->assignSlotsToLocalSymbols(-1);
    }
}
