#include "MemoryManager.h"

#include "../VM.h"

class VM;
MemoryManager::MemoryManager() : pageTable(std::unordered_map<uint32_t, Page*>{}) {}

void MemoryManager::loadBytecodeIntoMemory(const std::vector<uint8_t>* bytecode) {
    // read header
    uint32_t startOfDataSection = 0;
    uint32_t endOfDataSection = 0;

    for (int i = 0; i < 4; i++) {
        startOfDataSection = startOfDataSection | static_cast<uint32_t>((*bytecode)[i]) << (i * 8);
    }

    for (int i = 0; i < 4; i++) {
        endOfDataSection = endOfDataSection | static_cast<uint32_t>((*bytecode)[i + 4]) << (i * 8);
    }

    // load bytecode
    for (uint32_t i = 0; i <= endOfDataSection - BYTECODE_HEADER_SIZE; i++) {
        write8(i, (*bytecode)[i + BYTECODE_HEADER_SIZE]);
    }
}

void MemoryManager::write8(uint32_t address, uint8_t value) {
    Page* page = getOrCreatePage(address);
    page->data[getPageOffset(address)] = value;
}

uint8_t MemoryManager::read8(uint32_t address) {
    Page* page = getPage(address);
    if (page == nullptr) {
        // page is not currently allocated
        return 0;
    }
    return page->data[getPageOffset(address)];
}

void MemoryManager::allocatePage(uint32_t pageNumber) {
    pageTable[pageNumber] = new Page();
}

uint32_t MemoryManager::getPageNumber(const uint32_t address) {
    return address / PAGE_SIZE;
}

uint16_t MemoryManager::getPageOffset(const uint32_t address) {
    return address % PAGE_SIZE;
}

Page* MemoryManager::getOrCreatePage(uint32_t address) {
    uint32_t pageNumber = getPageNumber(address);

    auto it = pageTable.find(pageNumber);

    if (it == pageTable.end()) {
        // allocate new page
        allocatePage(pageNumber);
        it = pageTable.find(pageNumber);
    }
    return it->second;
}

Page* MemoryManager::getPage(uint32_t address) {
    uint32_t pageNumber = getPageNumber(address);

    auto it = pageTable.find(pageNumber);

    if (it == pageTable.end()) {
        return nullptr;
    }
    return it->second;
}