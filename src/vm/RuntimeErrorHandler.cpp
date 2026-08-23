#include "RuntimeErrorHandler.h"

#include <algorithm>
#include <fstream>
#include <iostream>

#include "VM.h"
#include "../include/Error.h"

RuntimeErrorHandler::RuntimeErrorHandler(VM* vm) : vm(vm) {}

void RuntimeErrorHandler::insertSource(const uint16_t sourceId, const std::string& path) {
    this->sources.insert(std::pair<uint32_t, std::string>(sourceId, path));
}

void RuntimeErrorHandler::insertDebugFunction(const uint32_t startAddress, const uint32_t endAddress, const uint16_t sourceId, const std::string& functionName) {
    this->functions.push_back(
        DebugFunctionInfo{
            startAddress,
            endAddress,
            sourceId,
            functionName
        }
    );
}

void RuntimeErrorHandler::insertDebugLine(const uint32_t startAddress, const uint32_t endAddress, const uint16_t sourceId, const uint32_t lineNumber, const uint32_t columnNumber) {
    this->lineTable.push_back(
        DebugLineInfo{
            startAddress,
            endAddress,
            sourceId,
            lineNumber,
            columnNumber
        }
    );
}

void RuntimeErrorHandler::raiseRuntimeError(const RuntimeError& error, const uint32_t FP) const {
    if (error.type == RuntimeErrorType::INTERNAL || vm->getErrorContext() == ErrorContext::INTERNAL) {
        std::cerr <<  runtimeErrorTypeToString(error.type) + ": " + error.message + "\n\n";
        vm->dumpState();
        return;
    }

    const DebugLineInfo* debugLine = this->getDebugLineInfo(this->vm->getPC() - 1);

    // TODO:: handle debug line not being found
    if (debugLine == nullptr) return;

    // get stack trace
    std::optional<RuntimeError> runtimeErrorWhenBuildingStackTrace;
    std::vector<uint32_t> stackTrace = vm->getCallStackManager()->getStackTrace(&runtimeErrorWhenBuildingStackTrace, FP);
    const DebugFunctionInfo* functionInfo = this->getDebugFunctionInfo(this->vm->getPC() - 1);

    const std::string* path = &this->sources.at(debugLine->sourceId);
    std::ifstream sourceFile(*path);

    // read line in source code at the source and line from debugLine
    std::string sourceLine;
    if (sourceFile) {
        for (size_t i = 1; i <= debugLine->line; ++i) {
            if (!std::getline(sourceFile, sourceLine)) {
                sourceLine.clear();
                break;
            }
        }
    }

    // remove leading spaces from sourceLine
    uint32_t numberOfRemovedSpaces = 0;
    if (!sourceLine.empty()) {
        const auto pos = sourceLine.find_first_not_of(' ');
        if (pos != std::string::npos) {
            sourceLine.erase(0, pos);
            numberOfRemovedSpaces = pos;
        } else {
            sourceLine = " ";
        }
    }

    const std::string spaces(debugLine->column + 4, ' ');
    const std::string lineNumber = std::to_string(debugLine->line);

    std::cerr <<  runtimeErrorTypeToString(error.type) + ": " + error.message + "\n";
    if (stackTrace.empty() || functionInfo == nullptr) {
        std::cerr << "    at " + *path + ":" + lineNumber + ":" + std::to_string(debugLine->column) + "\n";
    } else {
        this->outputStackTraceLine(functionInfo->functionName, functionInfo->sourceId, debugLine->line, debugLine->column);
    }

    if (!sourceLine.empty()) {
        std::cerr << "  " + lineNumber + " |    " + sourceLine + "\n";
        std::cerr << spaces.substr(0, (lineNumber.size() + 3)) + "|" + spaces.substr(0, 3 + (debugLine->column - numberOfRemovedSpaces)) + "^" + "\n";

    } else { // unable to read source file
        std::cerr << "    | source code unavailable";
    }

    if (vm->getCallStackManager()->isCallStackEmpty() || functionInfo == nullptr) {
        return;
    }

    if (runtimeErrorWhenBuildingStackTrace.has_value()) {
        std::cerr << "\n Stack Trace is unavailable";
        return;
    }

    // OUTPUT STACK TRACE

    // trace
    // 0 -> 27
    // 1 -> 16
    // 2 -> 5

    std::cerr << "stack trace:\n";
    for (uint32_t stackTraceIdx = 0; stackTraceIdx < stackTrace.size() - 1; stackTraceIdx++) {
        const DebugLineInfo* lineInfo = this->getDebugLineInfo(stackTrace[stackTraceIdx] - 1);
        const DebugFunctionInfo* functionInfo = this->getDebugFunctionInfo(stackTrace[stackTraceIdx] - 1);

        this->outputStackTraceLine(functionInfo->functionName, functionInfo->sourceId, lineInfo->line, lineInfo->column);
    }
    const DebugLineInfo* lineInfo = this->getDebugLineInfo(stackTrace.back() - 1);
    this->outputStackTraceLine("<global>", lineInfo->sourceId, lineInfo->line, lineInfo->column);
}

const DebugLineInfo* RuntimeErrorHandler::getDebugLineInfo(const uint32_t address) const {
    // search for the line info where start address <= address < end address
    auto it = std::upper_bound(
        this->lineTable.begin(),
        this->lineTable.end(),
        address,
        [](const uint32_t address, const DebugLineInfo& debugLine) {
            return address < debugLine.startAddress;
        }
    );

    if (it == this->lineTable.begin()) {
        return nullptr;
    }

    --it;

    if (address >= it->startAddress && address < it->endAddress) {
        return &(*it);
    }

    return nullptr;
}

const DebugFunctionInfo* RuntimeErrorHandler::getDebugFunctionInfo(const uint32_t address) const {
    // search for the line info where start address <= address < end address
    auto it = std::upper_bound(
        this->functions.begin(),
        this->functions.end(),
        address,
        [](const uint32_t address, const DebugFunctionInfo& function) {
            return address < function.startAddress;
        }
    );

    if (it == this->functions.begin()) {
        return nullptr;
    }

    --it;

    if (address >= it->startAddress && address < it->endAddress) {
        return &(*it);
    }

    return nullptr;
}

void RuntimeErrorHandler::outputStackTraceLine(const std::string& functionName, const uint16_t sourceId, const uint32_t line, const uint16_t column) const {
    std::cerr << "  at " + functionName + " (" + this->sources.at(sourceId) + ":" + std::to_string(line) + ":" + std::to_string(column) + ")\n";
}

std::string RuntimeErrorHandler::runtimeErrorTypeToString(const RuntimeErrorType& errorType) {
    switch (errorType) {
        case RuntimeErrorType::DIVISION_BY_ZERO: return "DivisionByZeroError";
        case RuntimeErrorType::OUT_OF_RANGE: return "OutOfRangeError";
        case RuntimeErrorType::STACK_OVERFLOW: return "StackOverflowError";
        case RuntimeErrorType::INTERNAL: return "InternalError";
    }
    return "";
}
