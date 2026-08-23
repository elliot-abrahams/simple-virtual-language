#include "RuntimeErrorHandler.h"

#include <algorithm>
#include <fstream>
#include <iostream>

#include "VM.h"
#include "../include/Error.h"

RuntimeErrorHandler::RuntimeErrorHandler(VM* vm) : vm(vm) {}

void RuntimeErrorHandler::insertSource(const uint16_t sourceId, const std::string& path) {
    this->sourceMetadata.insert(std::pair<uint32_t, std::string>(sourceId, path));
}

void RuntimeErrorHandler::insertFunctionMetadata(const uint32_t startAddress, const uint32_t endAddress, const uint16_t sourceId, const std::string& functionName) {
    this->functionMetadata.push_back(
        FunctionMetadata{
            startAddress,
            endAddress,
            sourceId,
            functionName
        }
    );
}

void RuntimeErrorHandler::insertLineTableMetadata(const uint32_t startAddress, const uint32_t endAddress, const uint16_t sourceId, const uint32_t lineNumber, const uint32_t columnNumber) {
    this->lineTableMetadata.push_back(
        LineTableMetadata{
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

    const LineTableMetadata* lineMetadata = this->getLineTableMetadata(this->vm->getPC() - 1);

    // TODO:: handle line metadata not being found
    if (lineMetadata == nullptr) return;

    // get stack trace
    std::optional<RuntimeError> runtimeErrorWhenBuildingStackTrace;
    std::vector<uint32_t> stackTrace = vm->getCallStackManager()->getStackTrace(&runtimeErrorWhenBuildingStackTrace, FP);
    const FunctionMetadata* functionInfo = this->getFunctionMetadata(this->vm->getPC() - 1);

    const std::string* path = &this->sourceMetadata.at(lineMetadata->sourceId);
    std::ifstream sourceFile(*path);

    // read line in source code at the source and line from line metadata
    std::string sourceLine;
    if (sourceFile) {
        for (size_t i = 1; i <= lineMetadata->line; ++i) {
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

    const std::string spaces(lineMetadata->column + 4, ' ');
    const std::string lineNumber = std::to_string(lineMetadata->line);

    std::cerr <<  runtimeErrorTypeToString(error.type) + ": " + error.message + "\n";
    if (stackTrace.empty() || functionInfo == nullptr) {
        std::cerr << "    at " + *path + ":" + lineNumber + ":" + std::to_string(lineMetadata->column) + "\n";
    } else {
        this->outputStackTraceLine(functionInfo->functionName, functionInfo->sourceId, lineMetadata->line, lineMetadata->column);
    }

    if (!sourceLine.empty()) {
        std::cerr << "  " + lineNumber + " |    " + sourceLine + "\n";
        std::cerr << spaces.substr(0, (lineNumber.size() + 3)) + "|" + spaces.substr(0, 3 + (lineMetadata->column - numberOfRemovedSpaces)) + "^" + "\n";

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
        const LineTableMetadata* lineInfo = this->getLineTableMetadata(stackTrace[stackTraceIdx] - 1);
        const FunctionMetadata* functionInfo = this->getFunctionMetadata(stackTrace[stackTraceIdx] - 1);

        this->outputStackTraceLine(functionInfo->functionName, functionInfo->sourceId, lineInfo->line, lineInfo->column);
    }
    const LineTableMetadata* lineInfo = this->getLineTableMetadata(stackTrace.back() - 1);
    this->outputStackTraceLine("<global>", lineInfo->sourceId, lineInfo->line, lineInfo->column);
}

const LineTableMetadata* RuntimeErrorHandler::getLineTableMetadata(const uint32_t address) const {
    // search for the line info where start address <= address < end address
    auto it = std::upper_bound(
        this->lineTableMetadata.begin(),
        this->lineTableMetadata.end(),
        address,
        [](const uint32_t address, const LineTableMetadata& lineMetadata) {
            return address < lineMetadata.startAddress;
        }
    );

    if (it == this->lineTableMetadata.begin()) {
        return nullptr;
    }

    --it;

    if (address >= it->startAddress && address < it->endAddress) {
        return &(*it);
    }

    return nullptr;
}

const FunctionMetadata* RuntimeErrorHandler::getFunctionMetadata(const uint32_t address) const {
    // search for the line info where start address <= address < end address
    auto it = std::upper_bound(
        this->functionMetadata.begin(),
        this->functionMetadata.end(),
        address,
        [](const uint32_t address, const FunctionMetadata& function) {
            return address < function.startAddress;
        }
    );

    if (it == this->functionMetadata.begin()) {
        return nullptr;
    }

    --it;

    if (address >= it->startAddress && address < it->endAddress) {
        return &(*it);
    }

    return nullptr;
}

void RuntimeErrorHandler::outputStackTraceLine(const std::string& functionName, const uint16_t sourceId, const uint32_t line, const uint16_t column) const {
    std::cerr << "  at " + functionName + " (" + this->sourceMetadata.at(sourceId) + ":" + std::to_string(line) + ":" + std::to_string(column) + ")\n";
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
