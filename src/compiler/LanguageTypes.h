#ifndef SVM_LANGUAGETYPES_H
#define SVM_LANGUAGETYPES_H

#include <cstdint>
#include <stdexcept>

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
        EQUAL,
        PLUS_EQUAL,
        MINUS_EQUAL,
        MULTIPLY_EQUAL,
        DIVIDE_EQUAL,
        INTEGER_DIVIDE_EQUAL,
        MODULO_EQUAL,
        BITWISE_OR_EQUAL,
        BITWISE_XOR_EQUAL,
        BITWISE_AND_EQUAL,
        LEFT_SHIFT_EQUAL,
        ARITHMETIC_RIGHT_SHIFT_EQUAL,
        LOGICAL_RIGHT_SHIFT_EQUAL,
        LOGICAL_OR_EQUAL,
        LOGICAL_AND_EQUAL
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

    inline BinaryOperator assignmentOperatorToBinaryOperator(const AssignmentOperator& assignmentOperator) {
        switch (assignmentOperator) {
            case AssignmentOperator::PLUS_EQUAL: return BinaryOperator::ADD;
            case AssignmentOperator::MINUS_EQUAL: return BinaryOperator::SUBTRACT;
            case AssignmentOperator::MULTIPLY_EQUAL: return BinaryOperator::MULTIPLY;
            case AssignmentOperator::DIVIDE_EQUAL: return BinaryOperator::DIVIDE;
            case AssignmentOperator::INTEGER_DIVIDE_EQUAL: return BinaryOperator::INTEGER_DIVIDE;
            case AssignmentOperator::MODULO_EQUAL: return BinaryOperator::MODULO;
            case AssignmentOperator::BITWISE_OR_EQUAL: return BinaryOperator::BITWISE_OR;
            case AssignmentOperator::BITWISE_XOR_EQUAL: return BinaryOperator::BITWISE_XOR;
            case AssignmentOperator::BITWISE_AND_EQUAL: return BinaryOperator::BITWISE_AND;
            case AssignmentOperator::LEFT_SHIFT_EQUAL: return BinaryOperator::LEFT_SHIFT;
            case AssignmentOperator::ARITHMETIC_RIGHT_SHIFT_EQUAL: return BinaryOperator::ARITHMETIC_RIGHT_SHIFT;
            case AssignmentOperator::LOGICAL_RIGHT_SHIFT_EQUAL: return BinaryOperator::LOGICAL_RIGHT_SHIFT;
            case AssignmentOperator::LOGICAL_OR_EQUAL: return BinaryOperator::LOGICAL_OR;
            case AssignmentOperator::LOGICAL_AND_EQUAL: return BinaryOperator::LOGICAL_AND;
            default: throw std::runtime_error("attempted to convert assignment operator 'EQUAL' to binary operator");
        }
    }

}

#endif //SVM_LANGUAGETYPES_H