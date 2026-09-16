#ifndef SVM_LANGUAGETYPES_H
#define SVM_LANGUAGETYPES_H

#include <cstdint>

namespace compiler {

    enum class Type {
        INT,
        FLOAT,
        BOOL,
        VOID_RETURN_TYPE
    };

    struct SemanticType {
        Type type;
        unsigned int dimension;

        uint8_t getSize() const {
            return 4;
        }
    };

    enum class AssignmentOperator {
        EQUAL
    };

    enum class BinaryOperator {
        PLUS,
        MINUS,
        MULTIPLY,
        DIVIDE,
        INTEGER_DIVIDE,
        MODULO,

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
        LOGICAL_NOT
    };

}

#endif //SVM_LANGUAGETYPES_H