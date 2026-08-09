#ifndef SIMPLE_VM_MEMORYMANAGER_H
#define SIMPLE_VM_MEMORYMANAGER_H
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "../OperandStack.h"
#include "../../include/ISA.h"


enum class Type : uint8_t;
struct Value;

constexpr size_t PAGE_SIZE = 4096;
constexpr size_t BYTECODE_HEADER_SIZE = 8;

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

    void loadBytecodeIntoMemory(const std::vector<uint8_t>* bytecode);

    void write(const MemoryAccessScope region, uint32_t address, const Value* value);
    void write8(const MemoryAccessScope region, uint32_t address, uint8_t value);
    void write32(const MemoryAccessScope region, uint32_t address, uint32_t value);
    void write64(const MemoryAccessScope region, uint32_t address, uint64_t value);

    uint64_t read(const MemoryAccessScope region, uint32_t address, ISA::Type type) const;
    uint8_t read8(const MemoryAccessScope region, uint32_t address) const;
    uint32_t read32(const MemoryAccessScope region, uint32_t address) const;
    uint64_t read64(const MemoryAccessScope region, uint32_t address) const;

private:
    void allocatePage(uint32_t pageNumber);

    static uint32_t getPageNumber(const uint32_t address);
    static uint16_t getPageOffset(const uint32_t address);
    Page* getOrCreatePage(uint32_t address);
    Page* getPage(uint32_t address) const;

    void checkAddressInRegion(const MemoryAccessScope region, uint32_t address) const;
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