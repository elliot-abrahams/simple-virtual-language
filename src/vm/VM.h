#ifndef SIMPLE_VM_VM_H
#define SIMPLE_VM_VM_H
#include <cstdint>
#include <limits>
#include <vector>

#include "OperandStack.h"
#include "memory/CallStackManager.h"
#include "memory/HeapManager.h"
#include "memory/MemoryManager.h"


constexpr uint32_t MAX_MEMORY_ADDRESS = std::numeric_limits<uint32_t>::max();

class VM {
public:
    VM();

    void run(const std::vector<uint8_t>* bytecode);

    void setHP(const uint32_t hp);

    Value popOperandStack();

    void handleVMError(const VMError& e) const;
    void dumpState() const;

private:
    void execute();
    void executeHalt();

    void executePush();
    void executePop();
    void executeDup();
    void executeSwap();

    void executeLoad();
    void executeLoadB();
    void executeLoadG();
    void executeLoadL();
    void executeStore();
    void executeStoreB();
    void executeStoreG();
    void executeStoreL();
    void executeAlloc();
    void executeFree();

    void executeCall();
    void executeRet();
    void executeJmp();
    void executeJez();
    void executeJnz();

    void executeAdd();
    void executeSub();
    void executeMul();
    void executeDiv();
    void executeMod();
    void executeNot();
    void executeAnd();
    void executeOrr();
    void executeXor();
    void executeShl();
    void executeShr();
    void executeSar();
    void executeCeq();
    void executeCne();
    void executeClt();
    void executeCle();
    void executeCgt();
    void executeCge();

    void executeOut();
    void executeInn();
    void executeConv();

    std::string readStringFromMemory(const uint32_t address) const;

    uint8_t fetchType();
    uint64_t fetchOperand(const uint8_t type);

    static void checkType(const std::string &instructionMnemonic, const std::vector<uint8_t> expectedTypes, const uint8_t actualType);
    void validateFrameAccess(const int32_t offset) const;
    static std::string formatFloatString(double value);

    uint32_t PC;
    uint32_t HP;
    uint32_t FP;
    uint32_t SP;

    MemoryManager memoryManager;
    CallStackManager callStackManager;
    HeapManager heapManager;
    OperandStack operandStack;

    bool running;
};


#endif //SIMPLE_VM_VM_H