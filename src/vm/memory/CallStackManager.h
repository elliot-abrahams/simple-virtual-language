#ifndef SVM_CALLSTACK_H
#define SVM_CALLSTACK_H

#pragma once
#include <stack>

#include "MemoryManager.h"
#include <vector>


struct FrameInfo {
    uint32_t sizeOfArguments;
    uint32_t sizeOfLocals;
};

class CallStackManager {

public:
    CallStackManager(MemoryManager* memoryManager);

    void push(
        uint32_t& FP,
        uint32_t& SP,
        const uint32_t returnAddress,
        const uint8_t numberOfArguments,
        const uint32_t numberLocals,
        const std::vector<Value>& arguments,
        const uint32_t maxHeapAddress
        );

    void pop(
        uint32_t& FP,
        uint32_t& SP,
        uint32_t& PC
    );

private:
    MemoryManager *memoryManager;
    std::stack<FrameInfo> frameInfoStack;
};


#endif //SVM_CALLSTACK_H