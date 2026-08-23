#ifndef SVM_ARITHMETICOPS_H
#define SVM_ARITHMETICOPS_H
#include <cstdint>
#include <optional>

#include "OperandStack.h"
#include "../include/Error.h"
#include "../include/ISA.h"


class ArithmeticOps {
public:
    static Value add(std::optional<RuntimeError>* runtimeError, const Value& value1, const Value& value2);
    static Value sub(std::optional<RuntimeError>* runtimeError, const Value& value1, const Value& value2);
    static Value mul(std::optional<RuntimeError>* runtimeError, const Value& value1, const Value& value2);
    static Value div(std::optional<RuntimeError>* runtimeError, const Value& value1, const Value& value2);
    static Value mod(std::optional<RuntimeError>* runtimeError, const Value& value1, const Value& value2);
    static Value bitwiseNot(std::optional<RuntimeError>* runtimeError, const Value& value);
    static Value bitwiseAnd(std::optional<RuntimeError>* runtimeError, const Value& value1, const Value& value2);
    static Value bitwiseOr(std::optional<RuntimeError>* runtimeError, const Value& value1, const Value& value2);
    static Value bitwiseXor(std::optional<RuntimeError>* runtimeError, const Value& value1, const Value& value2);
    static Value shl(std::optional<RuntimeError>* runtimeError, const Value& value1, const Value& value2);
    static Value shr(std::optional<RuntimeError>* runtimeError, const bool isArithmetic, const Value& value1, const Value& value2);
    static Value ceq(std::optional<RuntimeError>* runtimeError, const Value& value1, const Value& value2);
    static Value cne(std::optional<RuntimeError>* runtimeError, const Value& value1, const Value& value2);
    static Value clt(std::optional<RuntimeError>* runtimeError, const Value& value1, const Value& value2);
    static Value cle(std::optional<RuntimeError>* runtimeError, const Value& value1, const Value& value2);
    static Value cgt(std::optional<RuntimeError>* runtimeError, const Value& value1, const Value& value2);
    static Value cge(std::optional<RuntimeError>* runtimeError, const Value& value1, const Value& value2);

private:
    static uint32_t getBitWidth(const ISA::Type& t);

    static void raiseInvalidOperationOnTypesRuntimeError(std::optional<RuntimeError>* runtimeError, const std::string& instructionMnemonic, const ISA::Type& type1, const ISA::Type& type2);
    static void raiseInvalidOperationOnTypesRuntimeError(std::optional<RuntimeError>* runtimeError, const std::string& instructionMnemonic, const ISA::Type& type1);
};


#endif //SVM_ARITHMETICOPS_H