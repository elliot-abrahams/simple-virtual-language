#include "HeapManager.h"

#include <limits>

HeapManager::HeapManager(MemoryManager* memoryManager) :
    memoryManager(memoryManager) {}

void HeapManager::initialiseHeap(uint32_t* HP) {
    this->freeListHead = new FreeBlock{std::numeric_limits<uint32_t>::max() - *HP, *HP, nullptr};
    this->HP = HP;
}

uint32_t HeapManager::allocateBlock(const uint32_t size, const uint32_t SP) {
    if (size == 0) {
        throw VMError("ERROR: attempted to allocate 0 bytes");
    }

    const uint32_t bytesToAllocate = size + BLOCK_HEADER_SIZE;

    // iterate through free list
    // until first block with a size larger than size to allocate
    // (first fit)

    FreeBlock* currBlock = this->freeListHead;
    FreeBlock* prevBlock = nullptr;

    while (currBlock != nullptr && bytesToAllocate > currBlock->size) {
        prevBlock = currBlock;
        currBlock = currBlock->next;
    }

    // if no sufficiently sized free block was found
    if (currBlock == nullptr) {
        throw VMError("ERROR: Heap exhausted");
    }

    // if allocated space would collide with call stack
    if (currBlock->address + bytesToAllocate >= SP) {
        throw VMError("ERROR: Heap exhausted");
    }

    const uint32_t allocatedAddress = currBlock->address + BLOCK_HEADER_SIZE;

    // update HP if allocated block max address is higher than current HP
    if (currBlock->address + bytesToAllocate > *this->HP) {
        *this->HP = currBlock->address + bytesToAllocate;
    }

    // write header onto the heap
    this->memoryManager->write32(MemoryAccessScope::HEAP, currBlock->address, bytesToAllocate);

    // write all other allocated values to zero
    for (uint32_t addressOffset = 0; addressOffset < size; addressOffset++) {
        const uint32_t address = currBlock->address + BLOCK_HEADER_SIZE + addressOffset;
        this->memoryManager->write8(MemoryAccessScope::HEAP, address, 0);
    }

    // if allocated space takes up the entire free block
    if (currBlock->size == bytesToAllocate) {

        // update next of previous block
        if (prevBlock == nullptr) {
            // update head
            this->freeListHead = currBlock->next;
        } else {
            // update previous block
            prevBlock->next = currBlock->next;
        }

        // delete free block
        delete currBlock;

    } else { // allocated space does not take up entire free block
        currBlock->size -= bytesToAllocate;
        currBlock->address += bytesToAllocate;
    }
    return allocatedAddress;
}

void HeapManager::deallocateBlock(const uint32_t address) {
    const uint32_t blockAddress = address - BLOCK_HEADER_SIZE;

    // read bytesToDeallocate from heap block header
    const uint32_t bytesToDeallocate = this->memoryManager->read32(MemoryAccessScope::HEAP, blockAddress);

    auto* newFreeBlock = new FreeBlock{bytesToDeallocate, blockAddress, nullptr};

    // iterate through free list
    // until next block's address is larger than blockAddress (address of deallocated block)

    FreeBlock* currBlock = this->freeListHead;
    FreeBlock* prevBlock = nullptr;

    while (currBlock != nullptr && currBlock->address < blockAddress) {
        prevBlock = currBlock;
        currBlock = currBlock->next;
    }

    newFreeBlock->next = currBlock;

    // update previous block of newFreeBlock
    if (prevBlock == nullptr) {
        this->freeListHead = newFreeBlock;
    } else {
        prevBlock->next = newFreeBlock;
    }

    // node is used for the result of newFreeBlock being merged
    FreeBlock* node = newFreeBlock;

    if (prevBlock != nullptr) {
        // merge with previous block if adjacent
        if (mergeIfAdjacentBlocks(prevBlock, node)) {
            node = prevBlock;
        }
    }

    if (node->next != nullptr) {
        // merge with next block if adjacent
        mergeIfAdjacentBlocks(node, node->next);
    }
}

bool HeapManager::mergeIfAdjacentBlocks(FreeBlock* block1, FreeBlock* block2) {
    if (block1->address + block1->size == block2->address) {
        block1->size += block2->size;
        block1->next = block2->next;
        delete block2;
        return true;
    }
    return false;
}