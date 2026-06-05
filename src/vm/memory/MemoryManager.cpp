#include "MemoryManager.h"

MemoryManager::MemoryManager() : pageTable(std::unordered_map<uint32_t, Page*>{}) {}

void MemoryManager::loadBytecodeIntoMemory(const std::vector<uint8_t> bytecode) {
    // read header
    uint32_t startOfDataSection = 0;
    uint32_t endOfDataSection = 0;

    for (int i = 0; i < 4; i++) {
        startOfDataSection = startOfDataSection | static_cast<uint32_t>(bytecode[i]) << (i * 8);
    }

    for (int i = 0; i < 4; i++) {
        endOfDataSection = endOfDataSection | static_cast<uint32_t>(bytecode[i + 4]) << (i * 8);
    }

    // load bytecode
    for (uint32_t i = 0; i < endOfDataSection - 8; i++) {
        write8(i, bytecode[i + 8]);
    }
}

void MemoryManager::write8(uint32_t address, uint8_t value) {
    Page* page = getPage(address);
    page->data[getPageOffset(address)] = value;
}

void MemoryManager::allocatePage(uint32_t pageNumber) {
    pageTable[pageNumber] = new Page();
}

uint32_t MemoryManager::getPageNumber(uint32_t address) {
    return address / PAGE_SIZE;
}

uint16_t MemoryManager::getPageOffset(uint32_t address) {
    return address % PAGE_SIZE;
}

Page* MemoryManager::getPage(uint32_t address) {

    uint32_t pageNumber = getPageNumber(address);

    auto it = pageTable.find(pageNumber);

    if (it == pageTable.end()) {
        // allocate new page
        allocatePage(pageNumber);
        it = pageTable.find(pageNumber);
    }
    return it->second;
}