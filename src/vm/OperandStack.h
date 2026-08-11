#ifndef SIMPLE_VM_OPERANDSTACK_H
#define SIMPLE_VM_OPERANDSTACK_H
#include <cstdint>
#include <cstring>
#include <limits>
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

    int toInt() const {
        switch (type) {
            case ISA::Type::I32:
            case ISA::Type::UI32:
            case ISA::Type::I64:
            case ISA::Type::UI64:
                return static_cast<int>(rawValue);

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
        if (this->type == newType) return;

        TypedValue typedValue = this->toTyped();

        // type and value check if either current or new type is ptr
        if (this->type == ISA::Type::PTR || newType == ISA::Type::PTR) {
            ISA::Type nonPtrType;
            if (this->type == ISA::Type::PTR) {
                nonPtrType = newType;
            } else {
                nonPtrType = this->type;
            }

            // block conversions between ptr and f32 / f64
            if (nonPtrType == ISA::Type::F32 || nonPtrType == ISA::Type::F64) {
                throw VMError(std::string("ERROR: Invalid types for instruction: conv") +
                    "\nTypes: " + TypeConversions::typeToString(static_cast<uint8_t>(this->type)) +
                    ", " + TypeConversions::typeToString(static_cast<uint8_t>(newType))
                );
            }

            if (this->type != ISA::Type::PTR) {
                // check conversions from i32/ui32/i64/ui64 to ptr are in the correct range for ptr (ui32)
                switch (nonPtrType) {
                    case ISA::Type::UI32:
                        break;

                    case ISA::Type::I32:
                        std::visit([&](auto value) {
                            auto v = static_cast<int64_t>(value);

                            if (v < 0 || v > std::numeric_limits<uint32_t>::max())
                                this->handleOutOfRangeConversionError(newType);
                        }, typedValue);
                        break;

                    case ISA::Type::I64:
                        std::visit([&](auto value) {
                            auto v = static_cast<int64_t>(value);

                            if (v < 0 || v > std::numeric_limits<uint32_t>::max())
                                this->handleOutOfRangeConversionError(newType);
                        }, typedValue);
                        break;

                    case ISA::Type::UI64:
                        std::visit([&](auto value) {
                            auto v = static_cast<uint64_t>(value);

                            if (v > std::numeric_limits<uint32_t>::max())
                                this->handleOutOfRangeConversionError(newType);
                        }, typedValue);
                        break;

                    default:
                        throw VMError("Invalid conversion");
                }
            }
        }

        // check value is in range for conversions from f32 / f64 to i32/ui32/i64/ui64
        if (this->type == ISA::Type::F32 || this->type == ISA::Type::F64) {
            std::visit([&](auto value) {

                switch (newType) {
                    case ISA::Type::I32:
                        if (value < static_cast<double>(std::numeric_limits<int32_t>::min()) ||
                            value > static_cast<double>(std::numeric_limits<int32_t>::max())) {
                            handleOutOfRangeConversionError(newType);
                        }
                        break;

                    case ISA::Type::UI32:
                        if (value < 0.0 ||
                            value > static_cast<double>(std::numeric_limits<uint32_t>::max())) {
                            handleOutOfRangeConversionError(newType);
                        }
                        break;

                    case ISA::Type::I64:
                        if (value < static_cast<double>(std::numeric_limits<int64_t>::min()) ||
                            value > static_cast<double>(std::numeric_limits<int64_t>::max())) {
                            handleOutOfRangeConversionError(newType);
                        }
                        break;

                    case ISA::Type::UI64:
                        if (value < 0.0 ||
                            value > static_cast<double>(std::numeric_limits<uint64_t>::max())) {
                            handleOutOfRangeConversionError(newType);
                        }
                        break;

                    default:
                        break;
                }

            }, typedValue);
        }

        try {
            this->rawValue = std::visit(
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
        } catch (std::out_of_range) {
            throw VMError("ERROR: Out of range during conversion");
        }

        type = newType;
    }

    void handleOutOfRangeConversionError(const ISA::Type& newType) {
        std::string errorMessage = "";
        errorMessage += "ERROR: Out of range during conversion from " +
            TypeConversions::typeToString(static_cast<uint8_t>(this->type)) +
            " to " +
            TypeConversions::typeToString(static_cast<uint8_t>(newType));
        throw VMError(errorMessage);
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