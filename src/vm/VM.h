#ifndef SIMPLE_VM_VM_H
#define SIMPLE_VM_VM_H
#include <cstdint>
#include <limits>
#include <vector>

#include "OperandStack.h"
#include "VMError.h"
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

    void executePush();
    void executePop();
    void executeDup();
    void executeSwap();

    void executeLoad();
    void executeLoadG();
    void executeLoadL();
    void executeStore();
    void executeStoreG();
    void executeStoreL();

    uint8_t fetchType();
    uint64_t fetchOperand(const uint8_t type);

    void handleVMError(const VMError& e) const;
    void dumpState() const;

    static void checkType(const std::string instruction, const uint8_t expectedType, const uint8_t actualType);
    static std::string typeToString(const uint8_t type);

    static int32_t interpretI32(const uint64_t rawValue);

    uint32_t PC;
    uint32_t HP;
    uint32_t FP;
    uint32_t SP;

    MemoryManager memoryManager;
    OperandStack operandStack;

    bool running;
};


#endif //SIMPLE_VM_VM_H