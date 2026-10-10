#ifndef SVM_LANGUAGETYPES_H
#define SVM_LANGUAGETYPES_H

#include <cstdint>

namespace compiler {

    using TypeId = uint32_t;

    constexpr TypeId VOID_TYPE_ID  = 0;
    constexpr TypeId INT_TYPE_ID   = 1;
    constexpr TypeId FLOAT_TYPE_ID = 2;
    constexpr TypeId BOOL_TYPE_ID  = 3;
    constexpr TypeId CHAR_TYPE_ID  = 4;

    enum class Assignability {
        NON_ASSIGNABLE,
        ASSIGNABLE
    };

    struct Type {
        TypeId typeId;
        unsigned int dimension;

        Type() :
            typeId(0),
            dimension(0) {}

        Type(const TypeId typeId, const unsigned int dimension) :
            typeId(typeId), dimension(dimension) {}

        uint8_t getSize() const {
            return 4;
        }

        bool isNumeric() const {
            return !this->isArray() && (
                this->typeId == INT_TYPE_ID ||
                this->typeId == FLOAT_TYPE_ID ||
                this->typeId == CHAR_TYPE_ID
                );
        }

        bool isArray() const {
            return dimension > 0;
        }
    };

    struct FieldInfo {
        const Type type;
        const Assignability assignability;
        const uint32_t addressOffset;

        FieldInfo(const Type type, const Assignability assignability, const uint32_t addressOffset) :
            type(type), assignability(assignability), addressOffset(addressOffset) {}
    };

    enum class AssignmentOperator {
        EQUAL
    };

    enum class BinaryOperator {
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        INTEGER_DIVIDE,
        MODULO,

        BITWISE_OR,
        BITWISE_XOR,
        BITWISE_AND,

        LEFT_SHIFT,
        ARITHMETIC_RIGHT_SHIFT,
        LOGICAL_RIGHT_SHIFT,

        LOGICAL_OR,
        LOGICAL_AND,

        EQUAL_EQUAL,
        NOT_EQUAL,

        LESS_THAN,
        LESS_THAN_OR_EQUAL,
        GREATER_THAN,
        GREATER_THAN_OR_EQUAL,
    };

    enum class UnaryOperator {
        PLUS,
        MINUS,
        LOGICAL_NOT,
        BITWISE_NOT,

        INCREMENT,
        DECREMENT
    };

}

#endif //SVM_LANGUAGETYPES_H