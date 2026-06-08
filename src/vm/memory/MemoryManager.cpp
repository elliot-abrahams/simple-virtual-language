#include "MemoryManager.h"

#include "../VM.h"

class VM;
MemoryManager::MemoryManager() :
    pageTable(std::unordered_map<uint32_t, Page*>{}),
    dataRegionPointer(0),
    heapRegionPointer(0) {}

void MemoryManager::loadBytecodeIntoMemory(const std::vector<uint8_t>* bytecode) {
    // read header
    for (int i = 0; i < 4; i++) {
        dataRegionPointer = dataRegionPointer | static_cast<uint32_t>((*bytecode)[i]) << (i * 8);
    }

    for (int i = 0; i < 4; i++) {
        heapRegionPointer = heapRegionPointer | static_cast<uint32_t>((*bytecode)[i + 4]) << (i * 8);
    }

    this->dataRegionPointer = this->dataRegionPointer + 1 - BYTECODE_HEADER_SIZE;
    this->heapRegionPointer = this->heapRegionPointer + 1 - BYTECODE_HEADER_SIZE;

    // load bytecode
    for (uint32_t i = 0; i <= bytecode->size() - BYTECODE_HEADER_SIZE - 1; i++) {
        write8(MemoryRegion::ANY, i, (*bytecode)[i + BYTECODE_HEADER_SIZE]);
    }
}

void MemoryManager::write8(const MemoryRegion region, uint32_t address, uint8_t value) {
    this->checkAddressInRegion(region, address);
    Page* page = getOrCreatePage(address);
    page->data[getPageOffset(address)] = value;
}

uint8_t MemoryManager::read8(const MemoryRegion region, uint32_t address) const {
    this->checkAddressInRegion(region, address);
    Page* page = getPage(address);
    if (page == nullptr) {
        // page is not currently allocated
        return 0;
    }
    return page->data[getPageOffset(address)];
}

uint32_t MemoryManager::read32(const MemoryRegion region, uint32_t address) const {
    this->checkAddressInRegion(region, address);
    Page* page = getPage(address);
    if (page == nullptr) {
        // page is not currently allocated
        return 0;
    }
    uint32_t result = 0;
    for (int i = 0; i < 4; i++) {
        result = result | static_cast<uint32_t>(page->data[getPageOffset(address + i)]) << (i * 8);
    }
    return result;
}

uint64_t MemoryManager::read64(const MemoryRegion region, uint32_t address) const {
    this->checkAddressInRegion(region, address);
    Page* page = getPage(address);
    if (page == nullptr) {
        // page is not currently allocated
        return 0;
    }
    uint64_t result = 0;
    for (int i = 0; i < 8; i++) {
        result = result | static_cast<uint32_t>(page->data[getPageOffset(address + i)]) << (i * 8);
    }
    return result;
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

Page* MemoryManager::getPage(uint32_t address) const {
    uint32_t pageNumber = getPageNumber(address);

    auto it = pageTable.find(pageNumber);

    if (it == pageTable.end()) {
        return nullptr;
    }
    return it->second;
}

void MemoryManager::checkAddressInRegion(const MemoryRegion region, uint32_t address) const {
    switch (region) {
        case MemoryRegion::ANY: return;
        case MemoryRegion::CODE: {
            if (address < this->dataRegionPointer) {
                return;
            }
        }
        case MemoryRegion::DATA: {
            if (address >= this->dataRegionPointer && address < this->heapRegionPointer) {
                return;
            }
        }
    }
    throw VMError("Attempted to access address outside the expected region");
}