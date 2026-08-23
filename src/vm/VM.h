#ifndef SIMPLE_VM_VM_H
#define SIMPLE_VM_VM_H
#include <cstdint>
#include <limits>
#include <vector>

#include "OperandStack.h"
#include "RuntimeErrorHandler.h"
#include "memory/CallStackManager.h"
#include "memory/HeapManager.h"
#include "memory/MemoryManager.h"


constexpr uint32_t MAX_MEMORY_ADDRESS = std::numeric_limits<uint32_t>::max();

enum class VMTestScenario {
    NONE,
    NEAR_CALL_STACK_OVERFLOW,
    NEAR_HEAP_EXHAUSTION
};

class VM {
public:
    VM();

    void run(const std::vector<uint8_t>* bytecode);
    void run(const std::vector<uint8_t>* bytecode, VMTestScenario testScenario);

    void readBytecode(const std::vector<uint8_t>* bytecode);

    Value popOperandStack();

    OperandStack* getOperandStack();
    CallStackManager* getCallStackManager();

    void handleRuntimeError() const;

    void setExitStatus(int status);
    int getExitStatus() const;
    uint32_t getPC() const;
    ErrorContext getErrorContext() const;
    const std::optional<RuntimeError>* getRuntimeError() const;

    std::string readStringFromMemory(const uint32_t address);

    void checkType(const std::string &instructionMnemonic, const std::vector<uint8_t> expectedTypes, const uint8_t actualType);

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

    void executeNative();
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

    void executeConv();

    uint8_t fetchType();
    uint64_t fetchOperand(const uint8_t type);

    void validateFrameAccess(const int32_t offset);

    uint32_t PC;
    uint32_t HB;
    uint32_t HP;
    uint32_t FP;
    uint32_t SP;

    MemoryManager memoryManager;
    CallStackManager callStackManager;
    HeapManager heapManager;
    OperandStack operandStack;

    RuntimeErrorHandler runtimeErrorHandler;
    std::optional<RuntimeError> runtimeError;
    ErrorContext errorContext;

    bool running;
    int exitCode;
};


#endif //SIMPLE_VM_VM_H