#include "MemoryManager.h"

#include <ios>
#include <sstream>


MemoryManager::MemoryManager(uint32_t* HB, const uint32_t* HP, const uint32_t* SP) :
    pageTable(std::unordered_map<uint32_t, Page*>{}),
    startOfDataRegion(0),
    HB(HB),
    HP(HP),
    SP(SP) {}

void MemoryManager::loadBytecodeIntoMemory(std::optional<RuntimeError>* runtimeError, const std::vector<uint8_t>* bytecode, const uint32_t endOfDataAddress) {
    // load bytecode
    for (uint32_t i = BYTECODE_HEADER_SIZE; i <= endOfDataAddress; i++) {
        write8(runtimeError, MemoryAccessScope::ANY, i - BYTECODE_HEADER_SIZE, (*bytecode)[i]);
    }
}

void MemoryManager::setStartOfDataRegion(const uint32_t startOfDataAddress) {
    this->startOfDataRegion = startOfDataAddress;
}

void MemoryManager::write(std::optional<RuntimeError>* runtimeError, const MemoryAccessScope region, const uint32_t address, const Value* value) {
    switch (value->type) {
        case ISA::Type::I32:
        case ISA::Type::UI32:
        case ISA::Type::F32:
        case ISA::Type::PTR: {
            this->write32(runtimeError, region, address, value->rawValue);
            return;
        }
        case ISA::Type::I64:
        case ISA::Type::UI64:
        case ISA::Type::F64: {
            this->write64(runtimeError, region, address, value->rawValue);
        }
    }
}

void MemoryManager::write8(std::optional<RuntimeError>* runtimeError, const MemoryAccessScope region, const uint32_t address, const uint8_t value) {
    this->checkAddressInRegion(runtimeError, region, address, true);
    Page* page = getOrCreatePage(address);
    page->data[getPageOffset(address)] = value;
}

void MemoryManager::write32(std::optional<RuntimeError>* runtimeError, const MemoryAccessScope region, const uint32_t address, const uint32_t value) {
    this->checkAddressInRegion(runtimeError, region, address, true);
    Page* page = getOrCreatePage(address);
    for (int i = 0; i < 4; i++) {
        page->data[getPageOffset(address) + i] = (value >> (i * 8)) & 0xFF;
    }
}

void MemoryManager::write64(std::optional<RuntimeError>* runtimeError, const MemoryAccessScope region, const uint32_t address, const uint64_t value) {
    this->checkAddressInRegion(runtimeError, region, address, true);
    Page* page = getOrCreatePage(address);
    for (int i = 0; i < 8; i++) {
        page->data[getPageOffset(address) + i] = (value >> (i * 8)) & 0xFF;
    }
}

uint64_t MemoryManager::read(std::optional<RuntimeError>* runtimeError, const MemoryAccessScope region, const uint32_t address, const ISA::Type type) const {
    switch (type) {
        case ISA::Type::I32:
        case ISA::Type::UI32:
        case ISA::Type::F32:
        case ISA::Type::PTR: {
            return read32(runtimeError, region, address);
        }
        case ISA::Type::I64:
        case ISA::Type::UI64:
        case ISA::Type::F64: {
            return read64(runtimeError, region, address);
        }
    }
}

uint8_t MemoryManager::read8(std::optional<RuntimeError>* runtimeError, const MemoryAccessScope region, const uint32_t address) const {
    this->checkAddressInRegion(runtimeError, region, address, false);
    Page* page = getPage(address);
    if (page == nullptr) {
        // page is not currently allocated
        return 0;
    }
    return page->data[getPageOffset(address)];
}

uint32_t MemoryManager::read32(std::optional<RuntimeError>* runtimeError, const MemoryAccessScope region, const uint32_t address) const {
    this->checkAddressInRegion(runtimeError, region, address, false);
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

uint64_t MemoryManager::read64(std::optional<RuntimeError>* runtimeError, const MemoryAccessScope region, const uint32_t address) const {
    this->checkAddressInRegion(runtimeError, region, address, false);
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

void MemoryManager::checkAddressInRegion(std::optional<RuntimeError>* runtimeError, const MemoryAccessScope region, const uint32_t address, const bool isWrite) const {
    switch (region) {
        case MemoryAccessScope::CODE: {
            if (this->inCodeRegion(address)) {
                return;
            }
            break;
        }
        case MemoryAccessScope::DATA: {
            if (this->inDataRegion(address)) {
                return;
            }
            break;
        }
        case MemoryAccessScope::HEAP: {
            if (this->inHeapRegion(address)) {
                return;
            }
            break;
        }
        case MemoryAccessScope::CALL_STACK: {
            if (this->inCallStackRegion(address)) {
                return;
            }
            break;
        }
        case MemoryAccessScope::PTR: {
            if (this->inDataRegion(address) || this->inHeapRegion(address)) {
                return;
            }
            break;
        }
        case MemoryAccessScope::ANY: return;
    }
    std::stringstream addressAsHex;
    addressAsHex << "0x" << std::hex << std::uppercase << address;
    const std::string str = isWrite? "write to": "read from";
    const std::string errorMessage = "attempted to " + str + " address " + addressAsHex.str() + " outside the permitted memory region";
    *runtimeError = RuntimeError{
        RuntimeErrorType::INTERNAL,
        errorMessage
    };
}

bool MemoryManager::inCodeRegion(const uint32_t address) const {
    return address < this->startOfDataRegion;
}

bool MemoryManager::inDataRegion(const uint32_t address) const {
    return address >= this->startOfDataRegion && address < *this->HB;
}

bool MemoryManager::inHeapRegion(const uint32_t address) const {
    return address >= *this->HB && address <= *this->HP;
}

bool MemoryManager::inCallStackRegion(const uint32_t address) const {
    return address >= *this->SP;
}
