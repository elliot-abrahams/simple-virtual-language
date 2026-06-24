#ifndef SIMPLE_VM_OPERANDSTACK_H
#define SIMPLE_VM_OPERANDSTACK_H
#include <cstdint>
#include <cstring>
#include <variant>
#include <vector>

#include "TypeConversions.h"
#include "VMError.h"
#include "../include/ISA.h"


constexpr size_t MAX_OPERAND_STACK_SIZE = 1024;

using TypedValue = std::variant<int32_t, int64_t, uint32_t, uint64_t, float, double, uint8_t>;

struct Value
{
    ISA::Type type;
    uint64_t rawValue;

    TypedValue toTyped() const {
        switch (type) {
            case ISA::Type::I32: return static_cast<int32_t>(rawValue);
            case ISA::Type::I64: return static_cast<int64_t>(rawValue);
            case ISA::Type::UI32:
            case ISA::Type::PTR:
                return static_cast<uint32_t>(rawValue);
            case ISA::Type::UI64: return static_cast<uint64_t>(rawValue);
            case ISA::Type::F32: {
                float f;
                std::memcpy(&f, &rawValue, sizeof(float));
                return f;
            }
            case ISA::Type::F64: {
                double d;
                std::memcpy(&d, &rawValue, sizeof(double));
                return d;
            }
            default:
                throw VMError("Invalid type in operand stack");
        }
    }

    bool isZero() const {
        switch (type) {
            case ISA::Type::UI32:
            case ISA::Type::UI64:
            case ISA::Type::PTR:
                return rawValue == 0;
            case ISA::Type::I32: return std::get<int32_t>(this->toTyped()) == static_cast<int32_t>(0);
            case ISA::Type::I64: return std::get<int64_t>(this->toTyped()) == static_cast<int64_t>(0);
            case ISA::Type::F32: return std::get<float>(this->toTyped()) == 0.0f;
            case ISA::Type::F64: return std::get<double>(this->toTyped()) == 0.0;
        }
    }

    void convertToType(const ISA::Type& newType) {
        if (type == newType) return;

        TypedValue typedValue = this->toTyped();

        rawValue = std::visit(
            [&](auto value) -> uint64_t {

                switch (newType) {
                    case ISA::Type::I32:
                        return TypeConversions::I32ToRaw(static_cast<int32_t>(value));
                    case ISA::Type::UI32:
                    case ISA::Type::PTR:
                        return TypeConversions::UI32ToRaw(static_cast<uint32_t>(value));
                    case ISA::Type::I64:
                        return TypeConversions::I64ToRaw(static_cast<int64_t>(value));
                    case ISA::Type::UI64:
                        return static_cast<uint64_t>(value);
                    case ISA::Type::F32:
                        return TypeConversions::F32ToRaw(static_cast<float>(value));
                    case ISA::Type::F64:
                        return TypeConversions::F64ToRaw(static_cast<double>(value));
                    default:
                        throw VMError("Invalid conversion type");
                }

            }, typedValue);

        type = newType;
    }
};

class OperandStack {

public:
    OperandStack();

    Value pop();

    Value peek() const;

    void push(const uint8_t typeOperand, const uint64_t rawValue);
    void push(const Value value);

    const std::vector<Value>* getStack() const;

private:
    std::vector<Value> stack;
};


#endif //SIMPLE_VM_OPERANDSTACK_H