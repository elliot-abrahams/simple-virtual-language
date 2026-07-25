#ifndef SVM_LANGUAGETYPES_H
#define SVM_LANGUAGETYPES_H

namespace compiler {

    enum class Type {
        INT,
        FLOAT,
        BOOL,
        VOID
    };

    enum class AssignmentOperator {
        EQUAL
    };

    enum class BinaryOperator {
        PLUS,
        MINUS,
        MULTIPLY,
        DIVIDE,
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