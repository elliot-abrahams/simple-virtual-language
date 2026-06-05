#include "VM.h"

VM::VM() :
    PC(0), HP(0), FP(MAX_MEMORY_ADDRESS), SP(MAX_MEMORY_ADDRESS), memoryManager(MemoryManager()) {}

void VM::run(const std::vector<uint8_t> bytecode) {
    // load bytecode into memory
    this->memoryManager.loadBytecodeIntoMemory(bytecode);
}