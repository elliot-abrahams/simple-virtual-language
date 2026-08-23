#ifndef SV_RUNTIMEERRORHANDLER_H
#define SV_RUNTIMEERRORHANDLER_H
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "../include/Error.h"


class VM;

struct DebugLineInfo {
    const uint32_t startAddress;
    const uint32_t endAddress;
    const uint16_t sourceId;
    const uint32_t line;
    const uint32_t column;
};

class RuntimeErrorHandler {
public:
    RuntimeErrorHandler(VM* vm);

    void insertSource(const uint16_t sourceId, const std::string& path);
    void insertDebugLine(const uint32_t startAddress, const uint32_t endAddress, const uint16_t sourceId, const uint32_t lineNumber, const uint32_t columnNumber);

    void raiseRuntimeError(const RuntimeError& error) const;

private:
    const DebugLineInfo* getDebugLineInfo(const uint32_t address) const;
    static std::string runtimeErrorTypeToString(const RuntimeErrorType& errorType);

    VM* vm;

    std::map<uint32_t, std::string> sources;
    std::vector<DebugLineInfo> lineTable;
};


#endif //SV_RUNTIMEERRORHANDLER_H