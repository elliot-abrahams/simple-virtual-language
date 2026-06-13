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

    static void throwInvalidOperationOnTypesVMError(const std::string& instructionMnemonic, const Type& type1, const Type& type2);
    static void throwDivisionByZeroVMError();

};


#endif //SVM_ARITHMETICOPS_H