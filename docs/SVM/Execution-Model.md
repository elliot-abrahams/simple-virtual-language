# Execution Model

## Contents

- [1. Overview](#1-overview)
- [2. VM Lifecycle](#2-vm-lifecycle)
    - [2.1 Initialisation](#21-initialisation)
    - [2.2 Execution](#22-execution)
        - [Fetch](#fetch)
        - [Decode](#decode)
        - [Execute](#execute)
    - [2.3 Termination](#23-termination)
        - [Normal termination](#normal-termination)
        - [Runtime termination](#runtime-termination)
- [3. Registers](#3-registers)
    - [3.1 Program Counter (PC)](#31-program-counter-pc)
    - [3.2 Heap Base (HB)](#32-heap-base-hb)
    - [3.3 Heap Pointer (HP)](#33-heap-pointer-hp)
    - [3.4 Stack Pointer (SP)](#34-stack-pointer-sp)
    - [3.5 Frame Pointer (FP)](#35-frame-pointer-fp)
- [4. Operand Stack](#4-operand-stack)
- [5. Function Management](#5-function-management)
    - [5.1 Stack Frame](#51-stack-frame)
    - [5.2 Call Behaviour](#52-call-behaviour)
    - [5.3 Return Behaviour](#53-return-behaviour)
    - [5.4 Method Termination](#54-method-termination)
- [6. Heap Management](#6-heap-management)
    - [6.1 Structure](#61-structure)
    - [6.2 Allocation Behaviour](#62-allocation-behaviour)
    - [6.3 Deallocation Behaviour](#63-deallocation-behaviour)

---

## 1. Overview

This document describes the runtime execution model of SVM, including the VM lifecycle, registers, operand stack, function calls, stack frames, and heap management.

---

## 2. VM Lifecycle

### 2.1 Initialisation

1. PC is set to zero
2. HB is set to zero
3. HP is set to zero
4. FP is set to max memory address (2 ^ 32)
5. SP is set to max memory address (2 ^ 32)
6. Bytecode header is read (holds info about size of both code and data regions)
7. Bytecode (without the header) is written into memory
8. HB is set to the first address after the statically allocated data region
9. HP is set to HB

### 2.2 Execution

The VM goes into a loop that runs until it reaches a `halt` instruction or a run-time error

#### Fetch

1. Read opcode from memory at PC
2. Increment PC by one

Example:

```
(In memory)
0x00    02
0x01    00
0x02    05
0x03    00
0x04    00
0x05    00

PC = 0x00

1. Read opcode at PC (value 02 is read which is for the 'push' instruction)
2. PC = 1

PC = 0x01
```

#### Decode

The decode stage depends on the instruction that was fetched
(If necessary)
1.  Read operand from memory at PC
2. Increment PC by the size of the operand

Example:

```
(In memory)
0x00    0a
0x01    00
0x02    05
0x03    00
0x04    00
0x05    00

PC = 0x01

1. Read 'type' operand at PC (value 00 is read which denotes type i32)
2. Increment PC by 1 (size of 'type' operand)

PC = 0x02

In the case of type being i32, 4 bytes are read for the value to be pushed

1. Read 4 bytes from memory starting at PC
2. Increment PC by 4

(This VM uses little endian encoding so the value that was read is 5)

PC = 0x06    
```

#### Execute

Execute the instruction

Example:

```
opcode    -> push
type      -> i32
value     -> 5

The VM pushes the value 5 onto the operand stack
```

### 2.3 Termination

#### Normal termination

When the VM reads the halt instruction:

1. The execution loop ends
2. An exit code of 0 is produced
3. The program terminates

#### Runtime termination

When the VM encounters a runtime error:

1. The execution loop ends.
2. An error message is emitted
3. An exit code of 1 is produced
4. The program terminates

---

## 3. Registers

All registers are 4 bytes in size

### 3.1 Program Counter (PC)

Holds the address of the next instruction.

Initial value of `0`.

Instructions that modify PC:
- call
- ret
- jmp
- jez
- jnz

### 3.2 Heap Base (HB)

Points to the start of the heap region.

Initialised to the start of the heap region.

### 3.3 Heap Pointer (HP)

Points to the highest address currently used by the heap.

Deallocated block are returned to the free-list rather than reducing HP.

Instructions that modify HP:
- call

### 3.4 Stack Pointer (SP)

Points to the top of the current call stack.

Initialised to the top of memory.

Instructions that modify SP:
- call
- ret

### 3.5 Frame Pointer (FP)

Points to the stack-frame slot containing the previous frame pointer.

Initialised to the top of memory.

Instructions that modify FP:
- call
- ret

---

## 4. Operand Stack

The operand stack stores temporary runtime values.

Example:

```
push i32 #10
push i32 #5
add

(Operand Stack during execution)

[] → [10, i32] → [5, i32] → [15, i32]
                 [10, i32]
```

The stack has a maximum capacity of 1024 slots (8 KiB total).

Each slot contains:
- The raw 8 byte value (using little endian encoding)
- the type of the value

---

## 5. Function Management

### 5.1 Stack Frame

| Stack Frame Structure: | Size (Bytes) |
|------------------------|--------------|
| arg 1                  | 8            |
| arg 0                  | 8            |
| return address         | 4            |
| previous frame pointer | 4            |
| local 0                | 8            |
| local 1                | 8            |

Arguments are located towards higher address in memory.
Local variables are located towards lower address in memory.

The current frame pointer (FP) points to the address that stores the previous value of the frame pointer from the stack frame at the very top of the call stack.

### 5.2 Call Behaviour

When `call` instruction is executed:
1. Read `<address>` operand from memory (address of the called method)
2. Read the `<number of arguments>` from memory (read 1 byte at the address of the method)
3. Read the `<number of local variables>` from memory (read 4 bytes at address + 1)
4. Pop `<number of arguments>` slots from the operand stack (These are the arguments for the called method)
5. Write arguments into the call stack (starting at address SP)
6. Write return address and previous value of FP into the call stack
7. Set FP to the address of the location where the previous value of FP is stored
8. Set SP is decremented by the size of the new stack frame
9. Set PC to the first instruction of the method (PC = `<address> + 5`)
10. Track the number of arguments and number of local variables in a stack (used when destroying the stack frame)

### 5.3 Return Behaviour

When `ret` instruction is called:
1. Set PC to the return address from memory at address FP + 4
2. Set FP to the previous value of FP at address FP
3. Set SP to the current SP + size of current stack frame

The VM internally stores the number of arguments and number of local variables of each stack frame to be able to determine the size of the current stack frame.

### 5.4 Method Termination

A method does not have an implicit termination behaviour.

When executing a method:
- `ret` returns execution to the caller
- `halt` terminates VM execution
- A native function may terminate VM execution

If execution reaches the end of a method without encountering a terminating instruction, execution continues into the following bytecode.

Execution continuing beyond a method may cause invalid or unrelated bytecode to be interpreted as instructions.

---

## 6. Heap Management

### 6.1 Structure

Each allocated block of heap has a header of 4 bytes which holds the size (in bytes) of that block.

The heap uses a free-list allocator with a first-fit allocation algorithm.

The VM stores a free-list data structure where each element stores the size (in bytes) the address and the pointer to the next element.

### 6.2 Allocation Behaviour

When `alloc` instruction is called:
1. Pop `<bytesToAllocate>` from operand stack
2. Search the free-list for the first free block where its size is at least `<bytesToAllocate> + 4` (4 bytes for the header)
3. If the highest address of the newly allocated block is higher than SP, Heap exhaustion error is thrown
4. If the highest address of the newly allocated block is higher than the current HP, set HP to the highest address of the newly allocated block
5. Write `<bytesToAllocate>` in memory at the first address of the newly allocated block
6. Write `0` to the rest of the space in the newly allocated block in memory
7. Update the free-list
8. Push the address just after the header in the newly allocated block onto the operand stack

```
0x20    ← Block address (start of block header)
0x21
0x22
0x23
0x24    ← Returned pointer (first address after header)
0x25
```

### 6.3 Deallocation Behaviour

When `free` instruction is called:
1. Pop `<address>` from operand stack
2. Read `<blockAddress>` from memory at address `<address> - 4` (4 bytes for the header)
3. Create new free-list element and insert into the free-list
4. Merge new free-list element if there are any other adjacent free blocks
