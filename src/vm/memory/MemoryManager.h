#ifndef SIMPLE_VM_MEMORYMANAGER_H
#define SIMPLE_VM_MEMORYMANAGER_H
#include <cstdint>
#include <unordered_map>
#include <vector>

constexpr size_t PAGE_SIZE = 4096;

struct Page {
    uint8_t data[PAGE_SIZE];
};

class MemoryManager {

public:
    MemoryManager();

    void loadBytecodeIntoMemory(const std::vector<uint8_t> bytecode);

    void write8(uint32_t address, uint8_t value);

    uint8_t read8(uint32_t address);

private:
    void allocatePage(uint32_t pageNumber);

    static uint32_t getPageNumber(uint32_t address);
    static uint16_t getPageOffset(uint32_t address);
    Page* getPage(uint32_t address);

    std::unordered_map<uint32_t, Page*> pageTable;
};


#endif //SIMPLE_VM_MEMORYMANAGER_H