#ifndef SV_TYPEREGISTRY_H
#define SV_TYPEREGISTRY_H
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

#include "../LanguageTypes.h"


namespace compiler {

    struct TypeInfo {
        std::unordered_map<std::string, FieldInfo> fields;
    };

    class TypeRegistry {

    public:
        TypeRegistry();

        void registerArrayField(const std::string& arrayFieldName, const Type& arrayFieldType, const Assignability assignability, const uint32_t addressOffset);

        std::optional<const FieldInfo*> getFieldInfo(const Type& type, const std::string& fieldName);

    private:
        std::unordered_map<std::string, FieldInfo> arrayFields;
    };

}


#endif //SV_TYPEREGISTRY_H