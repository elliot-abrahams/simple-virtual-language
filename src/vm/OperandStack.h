#ifndef SIMPLE_VM_OPERANDSTACK_H
#define SIMPLE_VM_OPERANDSTACK_H
#include <cstdint>
#include <cstring>
#include <limits>
#include <optional>
#include <variant>
#include <vector>

#include "TypeConversions.h"
#include "../include/ISA.h"
#include "../include/Error.h"


constexpr size_t MAX_OPERAND_STACK_SIZE = 1024;

using TypedValue = std::variant<int32_t, int64_t, uint32_t, uint64_t, float, double, uint8_t>;

struct Value
{
    ISA::Type type;
    uint64_t rawValue;

    inline static std::optional<RuntimeError>* runtimeError;
    inline static ErrorContext errorContext;

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
                *runtimeError = RuntimeError{
                    RuntimeErrorType::INTERNAL,
                    "unexpected type on the operand stack"
                };
                return TypedValue{};
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
                handleInvalidOperandTypesForConversion(newType);
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
                        handleInvalidOperandTypesForConversion(newType);
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
                            this->handleInvalidOperandTypesForConversion(newType);
                    }
                }, typedValue);
        } catch (std::out_of_range) {
            this->handleOutOfRangeConversionError(newType);
        }

        type = newType;
    }

    void handleInvalidOperandTypesForConversion(const ISA::Type newType) {
        *runtimeError = RuntimeError{
            RuntimeErrorType::INTERNAL,
            "cannot convert value from type " +
                TypeConversions::typeToString(static_cast<uint8_t>(this->type)) +
                " to type " +
                TypeConversions::typeToString(static_cast<uint8_t>(newType))
        };
    }

    void handleOutOfRangeConversionError(const ISA::Type& newType) {
        if (errorContext == ErrorContext::LANGUAGE) {
            *runtimeError = RuntimeError {
                RuntimeErrorType::OUT_OF_RANGE,
                "value cannot be represented by the target type"
            };

        } else {
            *runtimeError = RuntimeError {
                RuntimeErrorType::INTERNAL,
                "out of range during conversion from type " +
                TypeConversions::typeToString(static_cast<uint8_t>(this->type)) +
                " to type " +
                TypeConversions::typeToString(static_cast<uint8_t>(newType))
            };
        }
    }
};

class OperandStack {

public:
    OperandStack();

    Value pop(std::optional<RuntimeError>* runtimeError);

    Value peek(std::optional<RuntimeError>* runtimeError) const;

    void push(std::optional<RuntimeError>* runtimeError, const uint8_t typeOperand, const uint64_t rawValue);
    void push(std::optional<RuntimeError>* runtimeError, const Value value);

    const std::vector<Value>* getStack() const;

private:
    std::vector<Value> stack;
};


#endif //SIMPLE_VM_OPERANDSTACK_H