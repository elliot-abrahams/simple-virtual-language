#ifndef SIMPLE_VM_VM_H
#define SIMPLE_VM_VM_H
#include <cstdint>
#include <limits>
#include <vector>

#include "memory/MemoryManager.h"


constexpr uint32_t MAX_MEMORY_ADDRESS = std::numeric_limits<uint32_t>::max();

class VM {
public:
    VM();

    void run(const std::vector<uint8_t>* bytecode);

    void setHP(const uint32_t hp);

private:
    void execute();

    void executeHalt();

    void incrementPC();

    uint32_t PC;
    uint32_t HP;
    uint32_t FP;
    uint32_t SP;

    MemoryManager memoryManager;
    // operand stack

    bool running;
};


#endif //SIMPLE_VM_VM_H