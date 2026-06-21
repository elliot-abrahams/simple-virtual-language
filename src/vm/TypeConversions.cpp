#include "TypeConversions.h"

#include "OperandStack.h"

#include <cstdint>
#include <cstring>

int32_t TypeConversions::rawToI32(const uint64_t rawValue) {
    return static_cast<int32_t>(static_cast<uint32_t>(rawValue));
}

uint32_t TypeConversions::rawToUI32(const uint64_t rawValue) {
    return static_cast<uint32_t>(rawValue);
}

int64_t TypeConversions::rawToI64(const uint64_t rawValue) {
    return static_cast<int64_t>(rawValue);
}

float TypeConversions::rawToF32(const uint64_t rawValue) {
    uint32_t bits = static_cast<uint32_t>(rawValue);
    float value;
    std::memcpy(&value, &bits, sizeof(float));
    return value;
}

double TypeConversions::rawToF64(const uint64_t rawValue) {
    double value;
    std::memcpy(&value, &rawValue, sizeof(double));
    return value;
}

uint64_t TypeConversions::I32ToRaw(const int32_t value) {
    uint32_t bits;
    std::memcpy(&bits, &value, sizeof(uint32_t));
    return static_cast<uint64_t>(bits);
}

uint64_t TypeConversions::UI32ToRaw(const uint32_t value) {
    return static_cast<uint64_t>(value);
}

uint64_t TypeConversions::I64ToRaw(const int64_t value) {
    uint64_t bits;
    std::memcpy(&bits, &value, sizeof(uint64_t));
    return bits;
}

uint64_t TypeConversions::F32ToRaw(const float value) {
    uint32_t bits;
    std::memcpy(&bits, &value, sizeof(uint32_t));
    return static_cast<uint64_t>(bits);
}

uint64_t TypeConversions::F64ToRaw(const double value) {
    uint64_t bits;
    std::memcpy(&bits, &value, sizeof(uint64_t));
    return bits;
}

std::string TypeConversions::typeToString(const uint8_t type) {
    switch (type) {
        case static_cast<uint8_t>(ISA::Type::I32): return "i32";
        case static_cast<uint8_t>(ISA::Type::UI32): return "ui64";
        case static_cast<uint8_t>(ISA::Type::I64): return "i64";
        case static_cast<uint8_t>(ISA::Type::UI64): return "ui64";
        case static_cast<uint8_t>(ISA::Type::F32): return "f32";
        case static_cast<uint8_t>(ISA::Type::F64): return "f64";
        case static_cast<uint8_t>(ISA::Type::PTR): return "ptr";
        case static_cast<uint8_t>(ISA::Type::STR): return "str";
        default: return "Unknown";
    }
}
