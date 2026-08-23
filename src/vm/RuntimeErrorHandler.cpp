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

void RuntimeErrorHandler::raiseRuntimeError(const RuntimeError& error) const {

    if (error.type == RuntimeErrorType::INTERNAL || vm->getErrorContext() == ErrorContext::INTERNAL) {
        std::cerr <<  runtimeErrorTypeToString(error.type) + ": " + error.message + "\n\n";
        vm->dumpState();
        return;
    }

    const DebugLineInfo* debugLine = this->getDebugLineInfo(this->vm->getPC() - 1);

    // TODO:: handle debug line not being found
    if (debugLine == nullptr) return;

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
    std::cerr << "  at " + *path + ":" + lineNumber + ":" + std::to_string(debugLine->column) + "\n";

    if (!sourceLine.empty()) {
        std::cerr << "  " + lineNumber + " |    " + sourceLine + "\n";
        std::cerr << spaces.substr(0, (lineNumber.size() + 3)) + "|" + spaces.substr(0, 3 + (debugLine->column - numberOfRemovedSpaces)) + "^" + "\n";

    } else { // unable to read source file
        std::cerr << "    | source code unavailable";
    }
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

std::string RuntimeErrorHandler::runtimeErrorTypeToString(const RuntimeErrorType& errorType) {
    switch (errorType) {
        case RuntimeErrorType::DIVISION_BY_ZERO: return "DivisionByZeroError";
        case RuntimeErrorType::OUT_OF_RANGE: return "OutOfRangeError";
        case RuntimeErrorType::STACK_OVERFLOW: return "StackOverflowError";
        case RuntimeErrorType::INTERNAL: return "InternalError";
    }
    return "";
}
