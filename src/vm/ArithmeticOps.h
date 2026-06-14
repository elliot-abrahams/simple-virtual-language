#ifndef SVM_ARITHMETICOPS_H
#define SVM_ARITHMETICOPS_H
#include <cstdint>

#include "OperandStack.h"


class ArithmeticOps {
public:
    static Value add(const Value& value1, const Value& value2);
    static Value sub(const Value& value1, const Value& value2);
    static Value mul(const Value& value1, const Value& value2);
    static Value div(const Value& value1, const Value& value2);
    static Value mod(const Value& value1, const Value& value2);
    static Value bitwiseNot(const Value& value);
    static Value bitwiseAnd(const Value& value1, const Value& value2);
    static Value bitwiseOr(const Value& value1, const Value& value2);
    static Value bitwiseXor(const Value& value1, const Value& value2);
    static Value shl(const Value& value1, const Value& value2);
    static Value shr(const bool isArithmetic, const Value& value1, const Value& value2);

private:
    static uint32_t getBitWidth(const Type& t);

    static void throwInvalidOperationOnTypesVMError(const std::string& instructionMnemonic, const Type& type1, const Type& type2);
    static void throwInvalidOperationOnTypesVMError(const std::string& instructionMnemonic, const Type& type1);
    static void throwDivisionByZeroVMError(const std::string& instructionMnemonic);
};


#endif //SVM_ARITHMETICOPS_H