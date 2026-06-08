#ifndef SIMPLE_VM_OPERANDSTACK_H
#define SIMPLE_VM_OPERANDSTACK_H
#include <cstdint>
#include <cstring>
#include <variant>
#include <vector>

#include "VMError.h"


constexpr size_t MAX_OPERAND_STACK_SIZE = 1024;

enum class Type : uint8_t {
    I32 = 0x00,
    UI32 = 0x01,
    I64 = 0x02,
    UI64 = 0x03,
    F32 = 0x04,
    F64 = 0x05,
    PTR = 0x06,
    CHAR = 0x07
};

using TypedValue = std::variant<int32_t, int64_t, uint32_t, uint64_t, float, double, uint8_t>;

struct Value
{
    Type type;
    uint64_t rawValue;

    TypedValue toTyped() const {
        switch (type) {
            case Type::I32: return static_cast<int32_t>(rawValue);
            case Type::I64: return static_cast<int64_t>(rawValue);
            case Type::UI32:
            case Type::PTR:
                return static_cast<uint32_t>(rawValue);
            case Type::UI64: return static_cast<uint64_t>(rawValue);
            case Type::F32: {
                float f;
                std::memcpy(&f, &rawValue, sizeof(float));
                return f;
            }
            case Type::F64: {
                double d;
                std::memcpy(&d, &rawValue, sizeof(double));
                return d;
            }
            case Type::CHAR: return static_cast<uint8_t>(rawValue);

            default:
                throw VMError("Invalid type in operand stack");
        }
    }
};

class OperandStack {

public:
    OperandStack();

    Value pop();

    Value peek() const;

    void push(const uint8_t typeOperand, const uint64_t rawValue);

    const std::vector<Value>* getStack() const;

private:
    std::vector<Value> stack;
};


#endif //SIMPLE_VM_OPERANDSTACK_H