#include "RuntimeErrorHandler.h"

#include <algorithm>
#include <filesystem>
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
        std::cerr << "InternalError: " + error.message + "\n\n";
        vm->dumpState();
        return;
    }

    const LineTableMetadata* lineMetadata = this->getLineTableMetadata(this->vm->getPC() - 1);

    if (lineMetadata == nullptr) return;

    const FunctionMetadata* functionInfo = this->getFunctionMetadata(this->vm->getPC() - 1);

    const std::string* path = &this->sourceMetadata.at(lineMetadata->sourceId);

    std::cerr << error.generateMessage(std::filesystem::path(*path), lineMetadata->line, lineMetadata->column) << std::endl;

    // OUTPUT STACK TRACE
    std::cerr << "stack trace:\n";

    if (vm->getCallStackManager()->isCallStackEmpty() || functionInfo == nullptr) {
        this->outputStackTraceLine("<global>", lineMetadata->sourceId, lineMetadata->line, lineMetadata->column);
        return;
    }

    // get stack trace
    std::vector<uint32_t> stackTrace;
    try {
        stackTrace = vm->getCallStackManager()->getStackTrace(FP);
    } catch (const RuntimeError& e) {
        std::cerr << "\n Stack Trace is unavailable";
        return;
    }

    this->outputStackTraceLine(functionInfo->functionName, functionInfo->sourceId, lineMetadata->line, lineMetadata->column);

    bool errorInsideScopeFunction = false;
    for (uint32_t stackTraceIdx = 0; stackTraceIdx < stackTrace.size() - 1; stackTraceIdx++) {
        const LineTableMetadata* lineInfo = this->getLineTableMetadata(stackTrace[stackTraceIdx] - 1);
        const FunctionMetadata* functionInfo = this->getFunctionMetadata(stackTrace[stackTraceIdx] - 1);
        if (functionInfo != nullptr) this->outputStackTraceLine(functionInfo->functionName, functionInfo->sourceId, lineInfo->line, lineInfo->column);
        else {
            this->outputStackTraceLine("<global>", lineInfo->sourceId, lineInfo->line, lineInfo->column);
            errorInsideScopeFunction = true;
        }
    }
    if (!errorInsideScopeFunction) {
        const LineTableMetadata* lineInfo = this->getLineTableMetadata(stackTrace.back() - 1);
        this->outputStackTraceLine("<global>", lineInfo->sourceId, lineInfo->line, lineInfo->column);
    }
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
