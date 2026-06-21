#include "MemoryManager.h"


MemoryManager::MemoryManager(const uint32_t* endOfHeapRegion, const uint32_t* startOfCallStackRegion) :
    pageTable(std::unordered_map<uint32_t, Page*>{}),
    startOfDataRegion(0),
    startOfHeapRegion(0),
    endOfHeapRegion(endOfHeapRegion),
    startOfCallStackRegion(startOfCallStackRegion) {}

void MemoryManager::loadBytecodeIntoMemory(const std::vector<uint8_t>* bytecode) {
    // read header
    for (int i = 0; i < 4; i++) {
        startOfDataRegion = startOfDataRegion | static_cast<uint32_t>((*bytecode)[i]) << (i * 8);
    }

    for (int i = 0; i < 4; i++) {
        startOfHeapRegion = startOfHeapRegion | static_cast<uint32_t>((*bytecode)[i + 4]) << (i * 8);
    }

    this->startOfDataRegion = this->startOfDataRegion + 1 - BYTECODE_HEADER_SIZE;
    this->startOfHeapRegion = this->startOfHeapRegion + 1 - BYTECODE_HEADER_SIZE;

    // load bytecode
    for (uint32_t i = 0; i <= bytecode->size() - BYTECODE_HEADER_SIZE - 1; i++) {
        write8(MemoryAccessScope::ANY, i, (*bytecode)[i + BYTECODE_HEADER_SIZE]);
    }
}

void MemoryManager::write(const MemoryAccessScope region, uint32_t address, const Value* value) {
    switch (value->type) {
        case ISA::Type::I32:
        case ISA::Type::UI32:
        case ISA::Type::F32:
        case ISA::Type::PTR: {
            this->write32(region, address, value->rawValue);
        }
        case ISA::Type::I64:
        case ISA::Type::UI64:
        case ISA::Type::F64: {
            this->write64(region, address, value->rawValue);
        }
    }
}

void MemoryManager::write8(const MemoryAccessScope region, uint32_t address, uint8_t value) {
    this->checkAddressInRegion(region, address);
    Page* page = getOrCreatePage(address);
    page->data[getPageOffset(address)] = value;
}

void MemoryManager::write32(const MemoryAccessScope region, uint32_t address, uint32_t value) {
    this->checkAddressInRegion(region, address);
    Page* page = getOrCreatePage(address);
    for (int i = 0; i < 4; i++) {
        page->data[getPageOffset(address) + i] = (value >> (i * 8)) & 0xFF;
    }
}

void MemoryManager::write64(const MemoryAccessScope region, uint32_t address, uint64_t value) {
    this->checkAddressInRegion(region, address);
    Page* page = getOrCreatePage(address);
    for (int i = 0; i < 8; i++) {
        page->data[getPageOffset(address) + i] = (value >> (i * 8)) & 0xFF;
    }
}

uint64_t MemoryManager::read(const MemoryAccessScope region, uint32_t address, ISA::Type type) const {
    switch (type) {
        case ISA::Type::I32:
        case ISA::Type::UI32:
        case ISA::Type::F32:
        case ISA::Type::PTR: {
            return read32(region, address);
        }
        case ISA::Type::I64:
        case ISA::Type::UI64:
        case ISA::Type::F64: {
            return read64(region, address);
        }
    }
}

uint8_t MemoryManager::read8(const MemoryAccessScope region, uint32_t address) const {
    this->checkAddressInRegion(region, address);
    Page* page = getPage(address);
    if (page == nullptr) {
        // page is not currently allocated
        return 0;
    }
    return page->data[getPageOffset(address)];
}

uint32_t MemoryManager::read32(const MemoryAccessScope region, uint32_t address) const {
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

uint64_t MemoryManager::read64(const MemoryAccessScope region, uint32_t address) const {
    this->checkAddressInRegion(region, address);
    Page* page = getPage(address);
    if (page == nullptr) {
        // page is not currently allocated
        return 0;
    }
    uint64_t result = 0;
    for (int i = 0; i < 8; i++) {
        result = result | static_cast<uint64_t>(page->data[getPageOffset(address + i)]) << (i * 8);
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

void MemoryManager::checkAddressInRegion(const MemoryAccessScope region, uint32_t address) const {
    switch (region) {
        case MemoryAccessScope::CODE: {
            if (this->inCodeRegion(address)) {
                return;
            }
        }
        case MemoryAccessScope::DATA: {
            if (this->inDataRegion(address)) {
                return;
            }
        }
        case MemoryAccessScope::HEAP: {
            if (this->inHeapRegion(address)) {
                return;
            }
        }
        case MemoryAccessScope::CALL_STACK: {
            if (this->inCallStackRegion(address)) {
                return;
            }
        }
        case MemoryAccessScope::PTR: {
            if (this->inDataRegion(address) || this->inHeapRegion(address)) {
                return;
            }
        }
        case MemoryAccessScope::ANY: return;
    }
    throw VMError("Attempted to access address outside the expected region");
}

bool MemoryManager::inCodeRegion(const uint32_t address) const {
    return address < this->startOfDataRegion;
}

bool MemoryManager::inDataRegion(const uint32_t address) const {
    return address >= this->startOfDataRegion && address < this->startOfHeapRegion;
}

bool MemoryManager::inHeapRegion(const uint32_t address) const {
    return address >= this->startOfHeapRegion && address < *this->endOfHeapRegion;
}

bool MemoryManager::inCallStackRegion(const uint32_t address) const {
    return address >= *this->startOfCallStackRegion;
}
