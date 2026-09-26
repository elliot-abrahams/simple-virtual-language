#include "TypeRegistry.h"

#include <stdexcept>

compiler::TypeRegistry::TypeRegistry() {}

void compiler::TypeRegistry::registerArrayField(const std::string& arrayFieldName, const Type& arrayFieldType, const Assignability assignability, const uint32_t addressOffset) {
    this->arrayFields.insert({arrayFieldName, FieldInfo{arrayFieldType, assignability, addressOffset}});
}

std::optional<const compiler::FieldInfo*> compiler::TypeRegistry::getFieldInfo(const Type& type, const std::string& fieldName) {
    if (type.dimension == 0) {
        return std::nullopt;
    }

    const auto it = this->arrayFields.find(fieldName);

    if (it == this->arrayFields.end()) {
        return std::nullopt;
    }

    return &it->second;
}


