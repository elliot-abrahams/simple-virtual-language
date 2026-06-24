#include "CallStack.h"

CallStack::CallStack(MemoryManager* memoryManager) :
    memoryManager(memoryManager) {};

void CallStack::push(uint32_t& FP, uint32_t& SP, const uint32_t returnAddress, const uint8_t numberOfArguments, const uint32_t numberLocals, const std::vector<Value>& arguments) {
    // calc number of bytes allocated to number of arguments and locals
    const uint32_t sizeOfArguments =  numberOfArguments * 8;
    const uint32_t sizeOfLocals = numberLocals * 8;

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
    SP = SP - (sizeOfArguments + 8 + sizeOfLocals);

    // add frame info to stack
    this->frameInfoStack.push(FrameInfo{numberOfArguments, numberLocals});
}

void CallStack::pop(uint32_t &FP, uint32_t &SP, uint32_t &PC) {
    // read return address from memory
    PC = this->memoryManager->read32(MemoryAccessScope::CALL_STACK, FP);

    // set FP to previous frame pointer value read from memory
    FP = this->memoryManager->read32(MemoryAccessScope::CALL_STACK, FP - 4);

    // increase SP by current stack frame size
    const FrameInfo frameInfo = this->frameInfoStack.top();
    SP += (frameInfo.numberOfArguments + 8 + frameInfo.numberOfLocals);

    // pop frame info off of stack
    this->frameInfoStack.pop();
}

