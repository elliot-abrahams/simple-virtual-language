#ifndef SIMPLE_VM_MEMORYMANAGER_H
#define SIMPLE_VM_MEMORYMANAGER_H
#include <cstdint>
#include <unordered_map>
#include <vector>


constexpr size_t PAGE_SIZE = 4096;
constexpr size_t BYTECODE_HEADER_SIZE = 8;

enum class MemoryRegion {
    ANY,
    CODE,
    DATA,
    HEAP,
    CALL_STACK
};

struct Page {
    uint8_t data[PAGE_SIZE];
};

class MemoryManager {

public:
    MemoryManager();

    void loadBytecodeIntoMemory(const std::vector<uint8_t>* bytecode);

    void write8(const MemoryRegion region, uint32_t address, uint8_t value);

    uint8_t read8(const MemoryRegion region, uint32_t address) const;
    uint32_t read32(const MemoryRegion region, uint32_t address) const;
    uint64_t read64(const MemoryRegion region, uint32_t address)const;

private:
    void allocatePage(uint32_t pageNumber);

    static uint32_t getPageNumber(const uint32_t address);
    static uint16_t getPageOffset(const uint32_t address);
    Page* getOrCreatePage(uint32_t address);
    Page* getPage(uint32_t address) const;

    void checkAddressInRegion(const MemoryRegion region, uint32_t address)const;

    std::unordered_map<uint32_t, Page*> pageTable;
    uint32_t dataRegionPointer;
    uint32_t heapRegionPointer;
};


#endif //SIMPLE_VM_MEMORYMANAGER_H