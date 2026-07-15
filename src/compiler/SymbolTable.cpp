#include "SymbolTable.h"

compiler::SymbolTable::SymbolTable() {}

void compiler::SymbolTable::declareGlobalVariable(const std::string& identifier, const ast::Type& type, const bool isInitialised) {
    this->globals.insert(std::make_pair(identifier, Symbol{type, isInitialised}));
}

std::unordered_map<std::string, compiler::Symbol>& compiler::SymbolTable::getGlobalVariables() {
    return this->globals;
}

std::optional<compiler::Symbol*> compiler::SymbolTable::getGlobalVariable(const std::string& identifier) {
    auto it = this->globals.find(identifier);
    if (it != this->globals.end()) {
        return &it->second;
    }
    return std::nullopt;
}
