#include "CallStackManager.h"

CallStackManager::CallStackManager(MemoryManager* memoryManager) :
    memoryManager(memoryManager) {};

void CallStackManager::push(uint32_t& FP, uint32_t& SP, const uint32_t returnAddress, const uint8_t numberOfArguments, const uint32_t numberLocals, const std::vector<Value>& arguments, const uint32_t maxHeapAddress) {
    // calc number of bytes allocated to number of arguments and locals
    const uint32_t sizeOfArguments =  numberOfArguments * 8;
    const uint32_t sizeOfLocals = numberLocals * 8;

    // check if new frame would collide with heap
    const uint32_t sizeOfFrame = sizeOfArguments + 8 + sizeOfLocals;
    if (SP - sizeOfFrame <= maxHeapAddress || sizeOfFrame > SP) {
        throw VMError("Stack overflow");
    }

    // load arguments into memory
    for (int i = 0; i < numberOfArguments; i++) {
        this->memoryManager->write64(MemoryAccessScope::CALL_STACK, SP - ((i + 1) * 8), arguments[i].rawValue);
    }

    // write zero into locals
    for (int i = 0; i < numberLocals; i++) {
        this->memoryManager->write64(MemoryAccessScope::CALL_STACK, SP - (sizeOfArguments + i + 2) * 8, 0);
    }

    // load previous frame pointer value into memory
    this->memoryManager->write32(MemoryAccessScope::CALL_STACK, SP - (sizeOfArguments + 4), FP);

    // load return address into memory
    this->memoryManager->write32(MemoryAccessScope::CALL_STACK, SP - (sizeOfArguments + 8), returnAddress);

    // update FP and SP
    FP = SP - (sizeOfArguments + 8);
    SP = SP - sizeOfFrame;

    // add frame info to stack
    this->frameInfoStack.push(FrameInfo{numberOfArguments, numberLocals});
}

void CallStackManager::pop(uint32_t &FP, uint32_t &SP, uint32_t &PC) {

    // check if stack frame exists
    if (this->frameInfoStack.empty()) {
        throw VMError("ret failed -> no frame on call stack");
    }

    // read return address from memory
    PC = this->memoryManager->read32(MemoryAccessScope::CALL_STACK, FP);

    // set FP to previous frame pointer value read from memory
    FP = this->memoryManager->read32(MemoryAccessScope::CALL_STACK, FP + 4);

    // increase SP by current stack frame size
    const FrameInfo frameInfo = this->frameInfoStack.top();
    SP += (
        (frameInfo.numberOfArguments * 8) +
        8 +
        (frameInfo.numberOfLocals * 8)
    );

    // pop frame info off of stack
    this->frameInfoStack.pop();
}

const FrameInfo* CallStackManager::peekFrameInfo() const {
    if (this->frameInfoStack.empty()) {
        return nullptr;
    }
    return &this->frameInfoStack.top();
}
