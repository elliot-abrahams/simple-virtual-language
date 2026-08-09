#ifndef SVM_HEAP_H
#define SVM_HEAP_H
#include <cstdint>

#include "MemoryManager.h"


static constexpr uint32_t BLOCK_HEADER_SIZE = 4;

struct FreeBlock {
    uint32_t size;
    uint32_t address;
    FreeBlock* next;
};

class HeapManager {

public:
    HeapManager(MemoryManager* memoryManager);

    void initialiseHeap(uint32_t* HP);
    uint32_t allocateBlock(const uint32_t size, const uint32_t SP);
    void deallocateBlock(const uint32_t address);

private:
    static bool mergeIfAdjacentBlocks(FreeBlock* block1, FreeBlock* block2);

    MemoryManager* memoryManager;
    FreeBlock* freeListHead;
    uint32_t* HP;
};


#endif //SVM_HEAP_H