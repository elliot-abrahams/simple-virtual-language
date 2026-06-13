#include "ArithmeticOps.h"

#include "TypeConversions.h"

Value ArithmeticOps::add(const Value& value1, const Value& value2) {

    /*
     * VALID OPERATIONS:
     *
     * i32 + i32 -> i32
     * i32 + ptr -> ptr
     * ui32 + ui32 -> ui32
     * ui32 + ptr -> ptr
     * i64 + i64 -> i64
     * ui64 + ui64 -> ui64
     * f32 + f32 -> f32
     * f64 + f64 -> f64
     * ptr + i32 -> ptr
     * ptr + ui32 -> ptr
     */

    switch (value1.type) {
        case Type::I32: {
            switch (value2.type) {
                case Type::I32: { // i32 + i32
                    const int32_t result = TypeConversions::rawToI32(value1.rawValue) + TypeConversions::rawToI32(value2.rawValue);
                    return Value{Type::I32, TypeConversions::I32ToRaw(result)};
                }
                case Type::PTR: { // i32 + ptr
                    const int64_t result =  static_cast<int64_t>(TypeConversions::rawToI32(value1.rawValue)) +  static_cast<int64_t>(TypeConversions::rawToUI32(value2.rawValue));
                    return Value{Type::PTR, TypeConversions::UI32ToRaw( static_cast<uint32_t>(result))};
                }
                default: throwInvalidOperationOnTypesVMError("add", value2.type, value1.type);
            }
        }
        case Type::UI32: {
            switch (value2.type) {
                case Type::UI32: { // ui32 + ui32
                    const uint32_t result = TypeConversions::rawToUI32(value1.rawValue) + TypeConversions::rawToUI32(value2.rawValue);
                    return Value{Type::UI32, TypeConversions::UI32ToRaw(result)};
                }
                case Type::PTR: { // ui32 + ptr
                    const uint32_t result = TypeConversions::rawToUI32(value1.rawValue) + TypeConversions::rawToUI32(value2.rawValue);
                    return Value{Type::PTR, TypeConversions::UI32ToRaw(result)};
                }
                default: throwInvalidOperationOnTypesVMError("add", value2.type, value1.type);
            }
        }
        case Type::I64: {
            if (value2.type == Type::I64) { // i64 + i64
                const int64_t result = TypeConversions::rawToI64(value1.rawValue) + TypeConversions::rawToI64(value2.rawValue);
                return Value{Type::I64, TypeConversions::I64ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("add", value2.type, value1.type);
        }
        case Type::UI64: {
            if (value2.type == Type::UI64) { // ui64 + ui64
                const uint64_t result = value1.rawValue + value2.rawValue;
                return Value{Type::UI64, result};
            }
            throwInvalidOperationOnTypesVMError("add", value2.type, value1.type);
        }
        case Type::F32: {
            if (value2.type == Type::F32) { // f32 + f32
                const float result = TypeConversions::rawToF32(value1.rawValue) + TypeConversions::rawToF32(value2.rawValue);
                return Value{Type::F32, TypeConversions::F32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("add", value2.type, value1.type);
        }
        case Type::F64: {
            if (value2.type == Type::F64) { // f64 + f64
                const double result = TypeConversions::rawToF64(value1.rawValue) + TypeConversions::rawToF64(value2.rawValue);
                return Value{Type::F64, TypeConversions::F64ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("add", value2.type, value1.type);
        }
        case Type::PTR: {
            switch (value2.type) {
                case Type::I32: { // ptr + i32
                    const int64_t result = static_cast<int64_t>(TypeConversions::rawToUI32(value1.rawValue)) + static_cast<int64_t>(TypeConversions::rawToI32(value2.rawValue));
                    return Value{Type::PTR, TypeConversions::I32ToRaw(static_cast<uint32_t>(result))};
                }
                case Type::UI32: { // ptr + ui32
                    const uint32_t result = TypeConversions::rawToUI32(value1.rawValue) + TypeConversions::rawToUI32(value2.rawValue);
                    return Value{Type::PTR, TypeConversions::UI32ToRaw(result)};
                }
                default: throwInvalidOperationOnTypesVMError("add", value2.type, value1.type);
            }
        }
        default: throwInvalidOperationOnTypesVMError("add", value2.type, value1.type);
    }
    return Value{};
}

Value ArithmeticOps::sub(const Value& value1, const Value& value2) {

    /*
     * VALID OPERATIONS:
     *
     * i32 - i32 -> i32
     * ui32 - ui32 -> ui32
     * i64 - i64 -> i64
     * ui64 - ui64 -> ui64
     * f32 - f32 -> f32
     * f64 - f64 -> f64
     * ptr - ptr -> i64
     * ptr - i32 -> ptr
     * ptr - ui32 -> ptr
     */

    switch (value1.type) {
        case Type::I32: {
            if (value2.type == Type::I32) {
                const int32_t result = TypeConversions::rawToI32(value1.rawValue) - TypeConversions::rawToI32(value2.rawValue);
                return Value{Type::I32, TypeConversions::I32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("sub", value2.type, value1.type);
        }
        case Type::UI32: {
            if (value2.type == Type::UI32) {
                const uint32_t result = TypeConversions::rawToUI32(value1.rawValue) - TypeConversions::rawToUI32(value2.rawValue);
                return Value{Type::UI32, TypeConversions::UI32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("sub", value2.type, value1.type);
        }
        case Type::I64: { // i64 - i64
            if (value2.type == Type::I64) {
                const int64_t result = TypeConversions::rawToI64(value1.rawValue) - TypeConversions::rawToI64(value2.rawValue);
                return Value{Type::I64, TypeConversions::I64ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("sub", value2.type, value1.type);
        }
        case Type::UI64: { // ui64 - ui64
            if (value2.type == Type::UI64) {
                const uint64_t result = value1.rawValue - value2.rawValue;
                return Value{Type::UI64, result};
            }
            throwInvalidOperationOnTypesVMError("sub", value2.type, value1.type);
        }
        case Type::F32: { // f32 - f32
            if (value2.type == Type::F32) {
                const float result = TypeConversions::rawToF32(value1.rawValue) - TypeConversions::rawToF32(value2.rawValue);
                return Value{Type::F32, TypeConversions::F32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("sub", value2.type, value1.type);
        }
        case Type::F64: { // f64 - f64
            if (value2.type == Type::F64) {
                const double result = TypeConversions::rawToF64(value1.rawValue) - TypeConversions::rawToF64(value2.rawValue);
                return Value{Type::F64, TypeConversions::F64ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("sub", value2.type, value1.type);
        }
        case Type::PTR: {
            switch (value2.type) {
                case Type::PTR: { // ptr - ptr
                    const int64_t result = static_cast<int64_t>(TypeConversions::rawToUI32(value1.rawValue)) - static_cast<int64_t>(TypeConversions::rawToUI32(value2.rawValue));
                    return Value{Type::I64, TypeConversions::I64ToRaw(result)};
                }
                case Type::I32: { // ptr - i32
                    const int64_t result = static_cast<int64_t>(TypeConversions::rawToUI32(value1.rawValue)) - static_cast<int64_t>(TypeConversions::rawToI32(value2.rawValue));
                    return Value{Type::PTR, TypeConversions::UI32ToRaw(static_cast<uint32_t>(result))};
                }
                case Type::UI32: { // ptr - ui32
                    const uint32_t result = TypeConversions::rawToUI32(value1.rawValue) - TypeConversions::rawToUI32(value2.rawValue);
                    return Value{Type::PTR, TypeConversions::UI32ToRaw(result)};
                }
                default: throwInvalidOperationOnTypesVMError("sub", value2.type, value1.type);
            }
        }
        default: throwInvalidOperationOnTypesVMError("sub", value2.type, value1.type);
    }
    return Value{};
}

Value ArithmeticOps::mul(const Value &value1, const Value &value2) {

    /*
     * VALID OPERATIONS:
     *
     * i32 * i32 -> i32
     * ui32 * ui32 -> ui32
     * i64 * i64 -> i64
     * ui64 * ui64 -> ui64
     * f32 * f32 -> f32
     * f64 * f64 -> f64
     */

    switch (value1.type) {
        case Type::I32: {
            if (value2.type == Type::I32) {
                const int32_t result = TypeConversions::rawToI32(value1.rawValue) * TypeConversions::rawToI32(value2.rawValue);
                return Value{Type::I32, TypeConversions::I32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("mul", value2.type, value1.type);
        }
        case Type::UI32: {
            if (value2.type == Type::UI32) {
                const uint32_t result = TypeConversions::rawToUI32(value1.rawValue) * TypeConversions::rawToUI32(value2.rawValue);
                return Value{Type::UI32, TypeConversions::UI32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("mul", value2.type, value1.type);
        }
        case Type::I64: {
            if (value2.type == Type::I64) {
                const int64_t result = TypeConversions::rawToI64(value1.rawValue) * TypeConversions::rawToI64(value2.rawValue);
                return Value{Type::I64, TypeConversions::I64ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("mul", value2.type, value1.type);
        }
        case Type::UI64: {
            if (value2.type == Type::UI64) {
                const uint64_t result = value1.rawValue * value2.rawValue;
                return Value{Type::UI64, result};
            }
            throwInvalidOperationOnTypesVMError("mul", value2.type, value1.type);
        }
        case Type::F32: {
            if (value2.type == Type::F32) {
                const float result = TypeConversions::rawToF32(value1.rawValue) * TypeConversions::rawToF32(value2.rawValue);
                return Value{Type::F32, TypeConversions::F32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("mul", value2.type, value1.type);
        }
        case Type::F64: {
            if (value2.type == Type::F64) {
                const double result = TypeConversions::rawToF64(value1.rawValue) * TypeConversions::rawToF64(value2.rawValue);
                return Value{Type::F64, TypeConversions::F64ToRaw(result)};
            }
        }
        default: throwInvalidOperationOnTypesVMError("mul", value2.type, value1.type);
    }
    return Value{};
}

Value ArithmeticOps::div(const Value &value1, const Value &value2) {

    /*
     * VALID OPERATIONS:
     *
     * i32 / i32 -> i32
     * ui32 / ui32 -> ui32
     * i64 / i64 -> i64
     * ui64 / ui64 -> ui64
     * f32 / f32 -> f32
     * f64 / f64 -> f64
     */

    switch (value1.type) {
        case Type::I32: {
            if (value2.type == Type::I32) {
                const int32_t denominator = TypeConversions::rawToI32(value2.rawValue);
                if (denominator == 0) {
                    throwDivisionByZeroVMError();
                }
                const int32_t result = TypeConversions::rawToI32(value1.rawValue) / denominator;
                return Value{Type::I32, TypeConversions::I32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("div", value2.type, value1.type);
        }
        case Type::UI32: {
            if (value2.type == Type::UI32) {
                const uint32_t denominator = TypeConversions::rawToUI32(value2.rawValue);
                if (denominator == 0) {
                    throwDivisionByZeroVMError();
                }
                const uint32_t result = TypeConversions::rawToUI32(value1.rawValue) / denominator;
                return Value{Type::UI32, TypeConversions::UI32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("div", value2.type, value1.type);
        }
        case Type::I64: {
            if (value2.type == Type::I64) {
                const int64_t denominator = TypeConversions::rawToI64(value2.rawValue);
                if (denominator == 0) {
                    throwDivisionByZeroVMError();
                }
                const int64_t result = TypeConversions::rawToI64(value1.rawValue) / denominator;
                return Value{Type::I64, TypeConversions::I64ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("div", value2.type, value1.type);
        }
        case Type::UI64: {
            if (value2.type == Type::UI64) {
                if (value2.rawValue == 0) {
                    throwDivisionByZeroVMError();
                }
                const uint64_t result = value1.rawValue / value2.rawValue;
                return Value{Type::UI64, result};
            }
            throwInvalidOperationOnTypesVMError("div", value2.type, value1.type);
        }
        case Type::F32: {
            if (value2.type == Type::F32) {
                const float denominator = TypeConversions::rawToF32(value1.rawValue);
                if (denominator == 0) {
                    throwDivisionByZeroVMError();
                }
                const float result = TypeConversions::rawToF32(value1.rawValue) / denominator;
                return Value{Type::F32, TypeConversions::F32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("div", value2.type, value1.type);
        }
        case Type::F64: {
            if (value2.type == Type::F64) {
                const double denominator = TypeConversions::rawToF64(value1.rawValue);
                if (denominator == 0) {
                    throwDivisionByZeroVMError();
                }
                const double result = TypeConversions::rawToF64(value1.rawValue) / denominator;
                return Value{Type::F64, TypeConversions::F64ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("div", value2.type, value1.type);
        }
        default: throwInvalidOperationOnTypesVMError("div", value2.type, value1.type);
    }
    return Value{};
}

void ArithmeticOps::throwInvalidOperationOnTypesVMError(const std::string& instructionMnemonic, const Type &type1, const Type &type2) {
    throw VMError(
        std::string("Error: Invalid operation on given types") +
            "\nInstruction: " + instructionMnemonic +
            "\nTypes: " + TypeConversions::typeToString(static_cast<uint8_t>(type1)) + ", " + TypeConversions::typeToString(static_cast<uint8_t>(type2))
        );
}

void ArithmeticOps::throwDivisionByZeroVMError() {
    throw VMError("Error: Division by zero");
}
