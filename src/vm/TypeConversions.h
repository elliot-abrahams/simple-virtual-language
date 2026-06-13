#ifndef SVM_TYPECONVERSIONS_H
#define SVM_TYPECONVERSIONS_H
#include <cstdint>
#include <string>

class TypeConversions {

public:
    static int32_t rawToI32(const uint64_t rawValue);
    static uint32_t rawToUI32(const uint64_t rawValue);
    static int64_t rawToI64(const uint64_t rawValue);
    static float rawToF32(const uint64_t rawValue);
    static double rawToF64(const uint64_t rawValue);

    static uint64_t I32ToRaw(const int32_t value);
    static uint64_t UI32ToRaw(const uint32_t value);
    static uint64_t I64ToRaw(const int64_t value);
    static uint64_t F32ToRaw(const float value);
    static uint64_t F64ToRaw(const double value);

    static std::string typeToString(const uint8_t type);
};


#endif //SVM_TYPECONVERSIONS_H