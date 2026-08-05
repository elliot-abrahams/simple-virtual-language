#include "ArithmeticOps.h"

#include <cmath>

#include "TypeConversions.h"
#include "../include/ISA.h"

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
        case ISA::Type::I32: {
            switch (value2.type) {
                case ISA::Type::I32: { // i32 + i32
                    const int32_t result = TypeConversions::rawToI32(value1.rawValue) + TypeConversions::rawToI32(value2.rawValue);
                    return Value{ISA::Type::I32, TypeConversions::I32ToRaw(result)};
                }
                case ISA::Type::PTR: { // i32 + ptr
                    const int64_t result =  static_cast<int64_t>(TypeConversions::rawToI32(value1.rawValue)) +  static_cast<int64_t>(TypeConversions::rawToUI32(value2.rawValue));
                    return Value{ISA::Type::PTR, TypeConversions::UI32ToRaw( static_cast<uint32_t>(result))};
                }
                default: throwInvalidOperationOnTypesVMError("add", value2.type, value1.type);
            }
        }
        case ISA::Type::UI32: {
            switch (value2.type) {
                case ISA::Type::UI32: { // ui32 + ui32
                    const uint32_t result = TypeConversions::rawToUI32(value1.rawValue) + TypeConversions::rawToUI32(value2.rawValue);
                    return Value{ISA::Type::UI32, TypeConversions::UI32ToRaw(result)};
                }
                case ISA::Type::PTR: { // ui32 + ptr
                    const uint32_t result = TypeConversions::rawToUI32(value1.rawValue) + TypeConversions::rawToUI32(value2.rawValue);
                    return Value{ISA::Type::PTR, TypeConversions::UI32ToRaw(result)};
                }
                default: throwInvalidOperationOnTypesVMError("add", value2.type, value1.type);
            }
        }
        case ISA::Type::I64: {
            if (value2.type == ISA::Type::I64) { // i64 + i64
                const int64_t result = TypeConversions::rawToI64(value1.rawValue) + TypeConversions::rawToI64(value2.rawValue);
                return Value{ISA::Type::I64, TypeConversions::I64ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("add", value2.type, value1.type);
        }
        case ISA::Type::UI64: {
            if (value2.type == ISA::Type::UI64) { // ui64 + ui64
                const uint64_t result = value1.rawValue + value2.rawValue;
                return Value{ISA::Type::UI64, result};
            }
            throwInvalidOperationOnTypesVMError("add", value2.type, value1.type);
        }
        case ISA::Type::F32: {
            if (value2.type == ISA::Type::F32) { // f32 + f32
                const float result = TypeConversions::rawToF32(value1.rawValue) + TypeConversions::rawToF32(value2.rawValue);
                return Value{ISA::Type::F32, TypeConversions::F32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("add", value2.type, value1.type);
        }
        case ISA::Type::F64: {
            if (value2.type == ISA::Type::F64) { // f64 + f64
                const double result = TypeConversions::rawToF64(value1.rawValue) + TypeConversions::rawToF64(value2.rawValue);
                return Value{ISA::Type::F64, TypeConversions::F64ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("add", value2.type, value1.type);
        }
        case ISA::Type::PTR: {
            switch (value2.type) {
                case ISA::Type::I32: { // ptr + i32
                    const int64_t result = static_cast<int64_t>(TypeConversions::rawToUI32(value1.rawValue)) + static_cast<int64_t>(TypeConversions::rawToI32(value2.rawValue));
                    return Value{ISA::Type::PTR, TypeConversions::I32ToRaw(static_cast<uint32_t>(result))};
                }
                case ISA::Type::UI32: { // ptr + ui32
                    const uint32_t result = TypeConversions::rawToUI32(value1.rawValue) + TypeConversions::rawToUI32(value2.rawValue);
                    return Value{ISA::Type::PTR, TypeConversions::UI32ToRaw(result)};
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
        case ISA::Type::I32: {
            if (value2.type == ISA::Type::I32) {
                const int32_t result = TypeConversions::rawToI32(value1.rawValue) - TypeConversions::rawToI32(value2.rawValue);
                return Value{ISA::Type::I32, TypeConversions::I32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("sub", value2.type, value1.type);
        }
        case ISA::Type::UI32: {
            if (value2.type == ISA::Type::UI32) {
                const uint32_t result = TypeConversions::rawToUI32(value1.rawValue) - TypeConversions::rawToUI32(value2.rawValue);
                return Value{ISA::Type::UI32, TypeConversions::UI32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("sub", value2.type, value1.type);
        }
        case ISA::Type::I64: { // i64 - i64
            if (value2.type == ISA::Type::I64) {
                const int64_t result = TypeConversions::rawToI64(value1.rawValue) - TypeConversions::rawToI64(value2.rawValue);
                return Value{ISA::Type::I64, TypeConversions::I64ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("sub", value2.type, value1.type);
        }
        case ISA::Type::UI64: { // ui64 - ui64
            if (value2.type == ISA::Type::UI64) {
                const uint64_t result = value1.rawValue - value2.rawValue;
                return Value{ISA::Type::UI64, result};
            }
            throwInvalidOperationOnTypesVMError("sub", value2.type, value1.type);
        }
        case ISA::Type::F32: { // f32 - f32
            if (value2.type == ISA::Type::F32) {
                const float result = TypeConversions::rawToF32(value1.rawValue) - TypeConversions::rawToF32(value2.rawValue);
                return Value{ISA::Type::F32, TypeConversions::F32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("sub", value2.type, value1.type);
        }
        case ISA::Type::F64: { // f64 - f64
            if (value2.type == ISA::Type::F64) {
                const double result = TypeConversions::rawToF64(value1.rawValue) - TypeConversions::rawToF64(value2.rawValue);
                return Value{ISA::Type::F64, TypeConversions::F64ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("sub", value2.type, value1.type);
        }
        case ISA::Type::PTR: {
            switch (value2.type) {
                case ISA::Type::PTR: { // ptr - ptr
                    const int64_t result = static_cast<int64_t>(TypeConversions::rawToUI32(value1.rawValue)) - static_cast<int64_t>(TypeConversions::rawToUI32(value2.rawValue));
                    return Value{ISA::Type::I64, TypeConversions::I64ToRaw(result)};
                }
                case ISA::Type::I32: { // ptr - i32
                    const int64_t result = static_cast<int64_t>(TypeConversions::rawToUI32(value1.rawValue)) - static_cast<int64_t>(TypeConversions::rawToI32(value2.rawValue));
                    return Value{ISA::Type::PTR, TypeConversions::UI32ToRaw(static_cast<uint32_t>(result))};
                }
                case ISA::Type::UI32: { // ptr - ui32
                    const uint32_t result = TypeConversions::rawToUI32(value1.rawValue) - TypeConversions::rawToUI32(value2.rawValue);
                    return Value{ISA::Type::PTR, TypeConversions::UI32ToRaw(result)};
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
        case ISA::Type::I32: {
            if (value2.type == ISA::Type::I32) {
                const int32_t result = TypeConversions::rawToI32(value1.rawValue) * TypeConversions::rawToI32(value2.rawValue);
                return Value{ISA::Type::I32, TypeConversions::I32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("mul", value2.type, value1.type);
        }
        case ISA::Type::UI32: {
            if (value2.type == ISA::Type::UI32) {
                const uint32_t result = TypeConversions::rawToUI32(value1.rawValue) * TypeConversions::rawToUI32(value2.rawValue);
                return Value{ISA::Type::UI32, TypeConversions::UI32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("mul", value2.type, value1.type);
        }
        case ISA::Type::I64: {
            if (value2.type == ISA::Type::I64) {
                const int64_t result = TypeConversions::rawToI64(value1.rawValue) * TypeConversions::rawToI64(value2.rawValue);
                return Value{ISA::Type::I64, TypeConversions::I64ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("mul", value2.type, value1.type);
        }
        case ISA::Type::UI64: {
            if (value2.type == ISA::Type::UI64) {
                const uint64_t result = value1.rawValue * value2.rawValue;
                return Value{ISA::Type::UI64, result};
            }
            throwInvalidOperationOnTypesVMError("mul", value2.type, value1.type);
        }
        case ISA::Type::F32: {
            if (value2.type == ISA::Type::F32) {
                const float result = TypeConversions::rawToF32(value1.rawValue) * TypeConversions::rawToF32(value2.rawValue);
                return Value{ISA::Type::F32, TypeConversions::F32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("mul", value2.type, value1.type);
        }
        case ISA::Type::F64: {
            if (value2.type == ISA::Type::F64) {
                const double result = TypeConversions::rawToF64(value1.rawValue) * TypeConversions::rawToF64(value2.rawValue);
                return Value{ISA::Type::F64, TypeConversions::F64ToRaw(result)};
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
        case ISA::Type::I32: {
            if (value2.type == ISA::Type::I32) {
                const int32_t denominator = TypeConversions::rawToI32(value2.rawValue);
                if (denominator == 0) {
                    throwDivisionByZeroVMError("div");
                }
                const int32_t result = TypeConversions::rawToI32(value1.rawValue) / denominator;
                return Value{ISA::Type::I32, TypeConversions::I32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("div", value2.type, value1.type);
        }
        case ISA::Type::UI32: {
            if (value2.type == ISA::Type::UI32) {
                const uint32_t denominator = TypeConversions::rawToUI32(value2.rawValue);
                if (denominator == 0) {
                    throwDivisionByZeroVMError("div");
                }
                const uint32_t result = TypeConversions::rawToUI32(value1.rawValue) / denominator;
                return Value{ISA::Type::UI32, TypeConversions::UI32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("div", value2.type, value1.type);
        }
        case ISA::Type::I64: {
            if (value2.type == ISA::Type::I64) {
                const int64_t denominator = TypeConversions::rawToI64(value2.rawValue);
                if (denominator == 0) {
                    throwDivisionByZeroVMError("div");
                }
                const int64_t result = TypeConversions::rawToI64(value1.rawValue) / denominator;
                return Value{ISA::Type::I64, TypeConversions::I64ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("div", value2.type, value1.type);
        }
        case ISA::Type::UI64: {
            if (value2.type == ISA::Type::UI64) {
                if (value2.rawValue == 0) {
                    throwDivisionByZeroVMError("div");
                }
                const uint64_t result = value1.rawValue / value2.rawValue;
                return Value{ISA::Type::UI64, result};
            }
            throwInvalidOperationOnTypesVMError("div", value2.type, value1.type);
        }
        case ISA::Type::F32: {
            if (value2.type == ISA::Type::F32) {
                const float denominator = TypeConversions::rawToF32(value2.rawValue);
                if (denominator == 0) {
                    throwDivisionByZeroVMError("div");
                }
                const float result = TypeConversions::rawToF32(value1.rawValue) / denominator;
                return Value{ISA::Type::F32, TypeConversions::F32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("div", value2.type, value1.type);
        }
        case ISA::Type::F64: {
            if (value2.type == ISA::Type::F64) {
                const double denominator = TypeConversions::rawToF64(value2.rawValue);
                if (denominator == 0) {
                    throwDivisionByZeroVMError("div");
                }
                const double result = TypeConversions::rawToF64(value1.rawValue) / denominator;
                return Value{ISA::Type::F64, TypeConversions::F64ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("div", value2.type, value1.type);
        }
        default: throwInvalidOperationOnTypesVMError("div", value2.type, value1.type);
    }
    return Value{};
}

Value ArithmeticOps::mod(const Value &value1, const Value &value2) {

    /*
     * VALID OPERATIONS:
     *
     * i32 % i32 -> i32
     * ui32 % ui32 -> ui32
     * i64 % i64 -> i64
     * ui64 % ui64 -> ui64
     * f32 % f32 -> f32
     * f64 % f64 -> f64
     */

    switch (value1.type) {
        case ISA::Type::I32: {
            if (value2.type == ISA::Type::I32) {
                const int32_t denominator = TypeConversions::rawToI32(value2.rawValue);
                if (denominator == 0) {
                    throwDivisionByZeroVMError("mod");
                }
                const int32_t result = TypeConversions::rawToI32(value1.rawValue) % denominator;
                return Value{ISA::Type::I32, TypeConversions::I32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("mod", value2.type, value1.type);
        }
        case ISA::Type::UI32: {
            if (value2.type == ISA::Type::UI32) {
                const uint32_t denominator = TypeConversions::rawToUI32(value2.rawValue);
                if (denominator == 0) {
                    throwDivisionByZeroVMError("mod");
                }
                const uint32_t result = TypeConversions::rawToUI32(value1.rawValue) % denominator;
                return Value{ISA::Type::UI32, TypeConversions::UI32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("mod", value2.type, value1.type);
        }
        case ISA::Type::I64: {
            if (value2.type == ISA::Type::I64) {
                const int64_t denominator = TypeConversions::rawToI64(value2.rawValue);
                if (denominator == 0) {
                    throwDivisionByZeroVMError("mod");
                }
                const int64_t result = TypeConversions::rawToI64(value1.rawValue) % denominator;
                return Value{ISA::Type::I64, TypeConversions::I64ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("mod", value2.type, value1.type);
        }
        case ISA::Type::UI64: {
            if (value2.type == ISA::Type::UI64) {
                if (value2.rawValue == 0) {
                    throwDivisionByZeroVMError("mod");
                }
                const uint64_t result = value1.rawValue % value2.rawValue;
                return Value{ISA::Type::UI64, result};
            }
            throwInvalidOperationOnTypesVMError("mod", value2.type, value1.type);
        }
        case ISA::Type::F32: {
            if (value2.type == ISA::Type::F32) {
                const float denominator = TypeConversions::rawToF32(value2.rawValue);
                if (denominator == 0) {
                    throwDivisionByZeroVMError("mod");
                }
                const float result = std::fmod(TypeConversions::rawToF32(value1.rawValue), denominator);
                return Value{ISA::Type::F32, TypeConversions::F32ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("mod", value2.type, value1.type);
        }
        case ISA::Type::F64: {
            if (value2.type == ISA::Type::F64) {
                const double denominator = TypeConversions::rawToF64(value2.rawValue);
                if (denominator == 0) {
                    throwDivisionByZeroVMError("mod");
                }
                const double result = std::fmod(TypeConversions::rawToF64(value1.rawValue), denominator);
                return Value{ISA::Type::F64, TypeConversions::F64ToRaw(result)};
            }
            throwInvalidOperationOnTypesVMError("mod", value2.type, value1.type);
        }
        default: throwInvalidOperationOnTypesVMError("mod", value2.type, value1.type);
    }
    return Value{};
}

Value ArithmeticOps::bitwiseNot(const Value &value) {

    /*
     * VALID OPERATIONS:
     *
     * ~i32 -> i32
     * ~ui32 -> ui32
     * ~i64 -> i64
     * ~ui64 -> ui64
     */

    if (
        value.type == ISA::Type::F32 ||
        value.type == ISA::Type::F64 ||
        value.type == ISA::Type::PTR) {
        throwInvalidOperationOnTypesVMError("not", value.type);
    }

    return Value{value.type, ~value.rawValue};
}

Value ArithmeticOps::bitwiseAnd(const Value &value1, const Value &value2) {

    /*
     * VALID OPERATIONS:
     *
     * i32 & i32 -> i32
     * ui32 & ui32 -> ui32
     * i64 & i64 -> i64
     * ui64 & ui64 -> ui64
     */

    if (
        value1.type != value2.type ||
        value1.type == ISA::Type::F32 ||
        value1.type == ISA::Type::F64 ||
        value1.type == ISA::Type::PTR) {
        throwInvalidOperationOnTypesVMError("and", value1.type, value2.type);
    }

    return Value{value1.type, value1.rawValue & value2.rawValue};
}

Value ArithmeticOps::bitwiseOr(const Value &value1, const Value &value2) {

    /*
     * VALID OPERATIONS:
     *
     * i32 | i32 -> i32
     * ui32 | ui32 -> ui32
     * i64 | i64 -> i64
     * ui64 | ui64 -> ui64
     */

    if (
        value1.type != value2.type ||
        value1.type == ISA::Type::F32 ||
        value1.type == ISA::Type::F64 ||
        value1.type == ISA::Type::PTR) {
        throwInvalidOperationOnTypesVMError("orr", value1.type, value2.type);
        }

    return Value{value1.type, value1.rawValue | value2.rawValue};
}

Value ArithmeticOps::bitwiseXor(const Value &value1, const Value &value2) {

    /*
     * VALID OPERATIONS:
     *
     * i32 ^ i32 -> i32
     * ui32 ^ ui32 -> ui32
     * i64 ^ i64 -> i64
     * ui64 ^ ui64 -> ui64
     */

    if (
        value1.type != value2.type ||
        value1.type == ISA::Type::F32 ||
        value1.type == ISA::Type::F64 ||
        value1.type == ISA::Type::PTR) {
        throwInvalidOperationOnTypesVMError("xor", value1.type, value2.type);
        }

    return Value{value1.type, value1.rawValue ^ value2.rawValue};
}

Value ArithmeticOps::shl(const Value &value1, const Value &value2) {

    /*
     * VALID OPERATIONS:
     *
     * i32 << R -> i32
     * ui32 << R -> ui32
     * i64 << R -> i64
     * ui64 << R -> ui64
     *
     * R -> (i32 | ui32 | i64 | ui64)
     * R is converted to ui32, then masked by (bitWidth - 1)
     */

    if (value2.type == ISA::Type::F32 || value2.type == ISA::Type::F64 || value2.type == ISA::Type::PTR ||
        value1.type == ISA::Type::F32 || value1.type == ISA::Type::F64 || value1.type == ISA::Type::PTR) {
        throwInvalidOperationOnTypesVMError("shr", value2.type, value1.type);
        }

    const uint32_t shift = static_cast<uint32_t>(value2.rawValue) & (getBitWidth(value1.type) - 1);

    uint64_t result = value1.rawValue << shift;

    // truncate
    if (value1.type == ISA::Type::I32 || value1.type == ISA::Type::UI32) {
        result &= 0xFFFFFFFF;
    }

    return Value{value1.type, result};
}

Value ArithmeticOps::shr(const bool isArithmetic, const Value &value1, const Value &value2) {

    /*
     * VALID OPERATIONS:
     *
     * i32 >> R -> i32
     * ui32 >> R -> ui32
     * i64 >> R -> i64
     * ui64 >> R -> ui64
     *
     * R -> (i32 | ui32 | i64 | ui64)
     * R is converted to ui32, then masked by (bitWidth - 1)
     */

    if (value2.type == ISA::Type::F32 || value2.type == ISA::Type::F64 || value2.type == ISA::Type::PTR ||
        value1.type == ISA::Type::F32 || value1.type == ISA::Type::F64 || value1.type == ISA::Type::PTR) {
        throwInvalidOperationOnTypesVMError("shr", value2.type, value1.type);
    }

    const uint32_t shift = static_cast<uint32_t>(value2.rawValue) & (getBitWidth(value1.type) - 1);

    uint64_t result;

    if (isArithmetic) {
        int64_t signedValue;

        if (value1.type == ISA::Type::I32) signedValue = static_cast<int32_t>(value1.rawValue);   // SIGN EXTEND FROM 32-bit
        else signedValue = static_cast<int64_t>(value1.rawValue);

        const int64_t shiftedValue = signedValue >> shift;

        if (value1.type == ISA::Type::I32) {
            result = static_cast<uint32_t>(shiftedValue);
        } else {
            result = static_cast<uint64_t>(shiftedValue);
        }

    } else {
        result = value1.rawValue >> shift;

        // truncate
        if (value1.type == ISA::Type::I32 || value1.type == ISA::Type::UI32) {
            result &= 0xFFFFFFFF;
        }
    }
    return Value{value1.type, result};
}

Value ArithmeticOps::ceq(const Value& value1, const Value& value2) {

    /*
     * VALID OPERATIONS:
     * i32 == i32 -> ui32
     * ui32 == ui32 -> ui32
     * i64 == i64 -> ui32
     * ui64 == ui64 -> ui32
     * f32 == f32 -> ui32
     * f64 == f64 -> ui32
     * ptr == ptr -> ui32
     */

    if (value1.type != value2.type) {
        throwInvalidOperationOnTypesVMError("ceq", value2.type, value1.type);
    }

    uint32_t result = 0;
    value1.rawValue == value2.rawValue ? result = 1 : result = 0;
    return Value{ISA::Type::UI32, result};
}

Value ArithmeticOps::cne(const Value& value1, const Value& value2) {

    /*
     * VALID OPERATIONS:
     * i32 != i32 -> ui32
     * ui32 != ui32 -> ui32
     * i64 != i64 -> ui32
     * ui64 != ui64 -> ui32
     * f32 != f32 -> ui32
     * f64 != f64 -> ui32
     * ptr != ptr -> ui32
     */

    if (value1.type != value2.type) {
        throwInvalidOperationOnTypesVMError("cne", value2.type, value1.type);
    }

    uint32_t result = 0;
    value1.rawValue != value2.rawValue ? result = 1 : result = 0;
    return Value{ISA::Type::UI32, result};
}

Value ArithmeticOps::clt(const Value& value1, const Value& value2) {

    /*
     * VALID OPERATIONS:
     * i32 < i32 -> ui32
     * ui32 < ui32 -> ui32
     * i64 < i64 -> ui32
     * ui64 < ui64 -> ui32
     * f32 < f32 -> ui32
     * f64 < f64 -> ui32
     * ptr < ptr -> ui32
     */

    if (value1.type != value2.type) {
        throwInvalidOperationOnTypesVMError("clt", value2.type, value1.type);
    }

    uint32_t result;
    switch (value1.type) {
        case ISA::Type::UI32:
        case ISA::Type::UI64:
        case ISA::Type::PTR:
            value1.rawValue < value2.rawValue ? result = 1 : result = 0; break;

        case ISA::Type::I32: TypeConversions::rawToI32(value1.rawValue) < TypeConversions::rawToI32(value2.rawValue) ? result = 1 : result = 0; break;
        case ISA::Type::I64: TypeConversions::rawToI64(value1.rawValue) < TypeConversions::rawToI64(value2.rawValue) ? result = 1 : result = 0; break;
        case ISA::Type::F32: TypeConversions::rawToF32(value1.rawValue) < TypeConversions::rawToF32(value2.rawValue) ? result = 1 : result = 0; break;
        case ISA::Type::F64: TypeConversions::rawToF64(value1.rawValue) < TypeConversions::rawToF64(value2.rawValue) ? result = 1 : result = 0; break;
    }
    return Value{ISA::Type::UI32, result};
}

Value ArithmeticOps::cle(const Value& value1, const Value& value2) {

    /*
     * VALID OPERATIONS:
     * i32 <= i32 -> ui32
     * ui32 <= ui32 -> ui32
     * i64 <= i64 -> ui32
     * ui64 <= ui64 -> ui32
     * f32 <= f32 -> ui32
     * f64 <= f64 -> ui32
     * ptr <= ptr -> ui32
     */

    if (value1.type != value2.type) {
        throwInvalidOperationOnTypesVMError("cle", value2.type, value1.type);
    }

    uint32_t result;
    switch (value1.type) {
        case ISA::Type::UI32:
        case ISA::Type::UI64:
        case ISA::Type::PTR:
            value1.rawValue <= value2.rawValue ? result = 1 : result = 0; break;

        case ISA::Type::I32: TypeConversions::rawToI32(value1.rawValue) <= TypeConversions::rawToI32(value2.rawValue) ? result = 1 : result = 0; break;
        case ISA::Type::I64: TypeConversions::rawToI64(value1.rawValue) <= TypeConversions::rawToI64(value2.rawValue) ? result = 1 : result = 0; break;
        case ISA::Type::F32: TypeConversions::rawToF32(value1.rawValue) <= TypeConversions::rawToF32(value2.rawValue) ? result = 1 : result = 0; break;
        case ISA::Type::F64: TypeConversions::rawToF64(value1.rawValue) <= TypeConversions::rawToF64(value2.rawValue) ? result = 1 : result = 0; break;
    }
    return Value{ISA::Type::UI32, result};
}

Value ArithmeticOps::cgt(const Value& value1, const Value& value2) {

    /*
     * VALID OPERATIONS:
     * i32 > i32 -> ui32
     * ui32 > ui32 -> ui32
     * i64 > i64 -> ui32
     * ui64 > ui64 -> ui32
     * f32 > f32 -> ui32
     * f64 > f64 -> ui32
     * ptr > ptr -> ui32
     */

    if (value1.type != value2.type) {
        throwInvalidOperationOnTypesVMError("cgt", value2.type, value1.type);
    }

    uint32_t result;
    switch (value1.type) {
        case ISA::Type::UI32:
        case ISA::Type::UI64:
        case ISA::Type::PTR:
            value1.rawValue > value2.rawValue ? result = 1 : result = 0; break;

        case ISA::Type::I32: TypeConversions::rawToI32(value1.rawValue) > TypeConversions::rawToI32(value2.rawValue) ? result = 1 : result = 0; break;
        case ISA::Type::I64: TypeConversions::rawToI64(value1.rawValue) > TypeConversions::rawToI64(value2.rawValue) ? result = 1 : result = 0; break;
        case ISA::Type::F32: TypeConversions::rawToF32(value1.rawValue) > TypeConversions::rawToF32(value2.rawValue) ? result = 1 : result = 0; break;
        case ISA::Type::F64: TypeConversions::rawToF64(value1.rawValue) > TypeConversions::rawToF64(value2.rawValue) ? result = 1 : result = 0; break;
    }
    return Value{ISA::Type::UI32, result};
}

Value ArithmeticOps::cge(const Value& value1, const Value& value2) {

    /*
     * VALID OPERATIONS:
     * i32 >= i32 -> ui32
     * ui32 >= ui32 -> ui32
     * i64 >= i64 -> ui32
     * ui64 >= ui64 -> ui32
     * f32 >= f32 -> ui32
     * f64 >= f64 -> ui32
     * ptr >= ptr -> ui32
     */

    if (value1.type != value2.type) {
        throwInvalidOperationOnTypesVMError("cge", value2.type, value1.type);
    }

    uint32_t result;
    switch (value1.type) {
        case ISA::Type::UI32:
        case ISA::Type::UI64:
        case ISA::Type::PTR:
            value1.rawValue >= value2.rawValue ? result = 1 : result = 0; break;

        case ISA::Type::I32: TypeConversions::rawToI32(value1.rawValue) >= TypeConversions::rawToI32(value2.rawValue) ? result = 1 : result = 0; break;
        case ISA::Type::I64: TypeConversions::rawToI64(value1.rawValue) >= TypeConversions::rawToI64(value2.rawValue) ? result = 1 : result = 0; break;
        case ISA::Type::F32: TypeConversions::rawToF32(value1.rawValue) >= TypeConversions::rawToF32(value2.rawValue) ? result = 1 : result = 0; break;
        case ISA::Type::F64: TypeConversions::rawToF64(value1.rawValue) >= TypeConversions::rawToF64(value2.rawValue) ? result = 1 : result = 0; break;
    }
    return Value{ISA::Type::UI32, result};
}

uint32_t ArithmeticOps::getBitWidth(const ISA::Type& t) {
    switch (t) {
        case ISA::Type::I32:
        case ISA::Type::UI32:
        case ISA::Type::F32: {
            return 32;
        }
        case ISA::Type::I64:
        case ISA::Type::UI64:
        case ISA::Type::F64: {
            return 64;
        }
    }
    return 0;
}

void ArithmeticOps::throwInvalidOperationOnTypesVMError(const std::string& instructionMnemonic, const ISA::Type &type1, const ISA::Type &type2) {
    throw VMError(
        std::string("Error: Invalid operation on given types") +
            "\nInstruction: " + instructionMnemonic +
            "\nTypes: " + TypeConversions::typeToString(static_cast<uint8_t>(type1)) + ", " + TypeConversions::typeToString(static_cast<uint8_t>(type2))
        );
}

void ArithmeticOps::throwInvalidOperationOnTypesVMError(const std::string &instructionMnemonic, const ISA::Type &type1) {
    throw VMError(
        std::string("Error: Invalid operation on given type") +
        "Instruction: " + instructionMnemonic +
        "\nType: " + TypeConversions::typeToString(static_cast<uint8_t>(type1))
        );
}

void ArithmeticOps::throwDivisionByZeroVMError(const std::string& instructionMnemonic) {
    throw VMError(
        std::string("Error: Division by zero") +
            "\nInstruction: " + instructionMnemonic
        );
}
