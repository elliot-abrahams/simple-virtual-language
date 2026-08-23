#ifndef SIMPLE_VM_MEMORYMANAGER_H
#define SIMPLE_VM_MEMORYMANAGER_H
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>

#include "../OperandStack.h"
#include "../../include/Error.h"
#include "../../include/ISA.h"


enum class Type : uint8_t;
struct Value;

constexpr size_t PAGE_SIZE = 4096;
constexpr size_t BYTECODE_HEADER_SIZE = 16;

enum class MemoryAccessScope {
    CODE,
    DATA,
    HEAP,
    CALL_STACK,
    PTR,
    ANY
};

struct Page {
    uint8_t data[PAGE_SIZE];
};

class MemoryManager {

public:
    MemoryManager(uint32_t* HB, const uint32_t* HP, const uint32_t* SP);

    void loadBytecodeIntoMemory(std::optional<RuntimeError>* runtimeError, const std::vector<uint8_t>* bytecode, const uint32_t endOfDataAddress);
    void setStartOfDataRegion(const uint32_t startOfDataAddress);

    void write(std::optional<RuntimeError>* runtimeError, const MemoryAccessScope region, const uint32_t address, const Value* value);
    void write8(std::optional<RuntimeError>* runtimeError, const MemoryAccessScope region, const uint32_t address, const uint8_t value);
    void write32(std::optional<RuntimeError>* runtimeError, const MemoryAccessScope region, const uint32_t address, const uint32_t value);
    void write64(std::optional<RuntimeError>* runtimeError, const MemoryAccessScope region, const uint32_t address, const uint64_t value);

    uint64_t read(std::optional<RuntimeError>* runtimeError, const MemoryAccessScope region, const uint32_t address, const ISA::Type type) const;
    uint8_t read8(std::optional<RuntimeError>* runtimeError, const MemoryAccessScope region, const uint32_t address) const;
    uint32_t read32(std::optional<RuntimeError>* runtimeError, const MemoryAccessScope region, const uint32_t address) const;
    uint64_t read64(std::optional<RuntimeError>* runtimeError, const MemoryAccessScope region, const uint32_t address) const;

private:
    void allocatePage(uint32_t pageNumber);

    static uint32_t getPageNumber(const uint32_t address);
    static uint16_t getPageOffset(const uint32_t address);
    Page* getOrCreatePage(uint32_t address);
    Page* getPage(uint32_t address) const;

    void checkAddressInRegion(std::optional<RuntimeError>* runtimeError, const MemoryAccessScope region, const uint32_t address, const bool isWrite) const;
    bool inCodeRegion(const uint32_t address) const;
    bool inDataRegion(const uint32_t address) const;
    bool inHeapRegion(const uint32_t address) const;
    bool inCallStackRegion(const uint32_t address) const;

    std::unordered_map<uint32_t, Page*> pageTable;
    uint32_t startOfDataRegion;
    uint32_t* HB;
    const uint32_t* HP;
    const uint32_t* SP;
};


#endif //SIMPLE_VM_MEMORYMANAGER_H