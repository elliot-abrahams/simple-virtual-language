# Memory Model

## Contents

- [1. Overview](#1-overview)
- [2. Paging](#2-paging)
- [3. Regions](#3-regions)
    - [3.1 Code Region](#31-code-region)
    - [3.2 Statically Allocated Data Region](#32-statically-allocated-data-region)
    - [3.3 Heap Region](#33-heap-region)
    - [3.4 Call Stack Region](#34-call-stack-region)
- [4. Memory Access Rules](#4-memory-access-rules)

---

## 1. Overview 

SVM uses a 32-bit byte-addressable virtual address space containing `2^32` addressable bytes (4 GiB).

Values are stored in memory as their raw bit representation.

Values are stored using little-endian encoding.

---

## 2. Paging

Memory is divided into fixed-size pages of 4096 bytes.

SVM does not allocate the entire 4 GiB address space. Physical storage for a page is allocated only when the VM first writes to an address within that page.

A virtual address is split into:

| Component   | Calculation    |
|-------------|----------------|
| Page index  | address / 4096 |
| Page offset | address % 4096 |

The memory manager stores a page table mapping page indices to allocated pages.

---

## 3. Regions

|                 Memory Layout:                 |
|:----------------------------------------------:|
|            Call stack <br> grows ↓             |
| Dynamically allocated data (Heap) <br> grows ↑ |
|           Statically allocated data            |
|                Code (Read only)                |

### 3.1 Code Region

Starts at address 0.

Contains executable bytecode.

Read-only during execution.

Region is fixed sized after loading.

The program counter (PC) always points to an address within this region during normal execution.

### 3.2 Statically Allocated Data Region

Located immediately after the code region.
Stores:
- Global variables
- String literals

Region is fixed sized after loading.

### 3.3 Heap Region

Located immediately after the statically allocated data region.

Grows upwards towards the call stack.

The heap allocator prevents allocations from overlapping with the call stack.

Used for dynamically allocated memory.

### 3.4 Call Stack Region

Located at the top of memory.
Grows downwards towards the heap.

Call stack stores stack frames created by function calls.

Each stack frame contains:
- Function arguments
- Return address
- Previous frame pointer
- Local variables

---

## 4 Memory Access Rules

All memory accesses perform bounds checking before reading and writing.

| Operation                                                              | Region Access |
|------------------------------------------------------------------------|---------------|
| Fetch instruction at program counter (PC)                              | Code          |
| Fetch operand of instruction                                           | Code          |
| Reading from memory using the instruction `loadG`                      | Data          |
| Reading from memory using the instructions `load`, `loadB`             | Data, Heap    |
| Reading from memory using the instruction `loadL`                      | Call Stack    |
| Writing to memory using the instruction `storeG`                       | Data          |
| Writing to memory using the instructions `store`, `storeB`             | Data, Heap    |
| Writing to memory using the instruction `storeL`                       | Call Stack    |
| Reading a method's argument / local variable metadata                  | Code          |
| Write stack frame to memory after the instruction `call` is run        | Call Stack    |
| Read stack frame from memory after the instruction `ret` is run        | Call Stack    | 
| Write heap block header to memory after the instruction `alloc` is run | Heap          |
| Read heap block header from memory after the instruction `free` is run | Heap          |

An error is thrown if the address used during an operation is outside the permitted region access.