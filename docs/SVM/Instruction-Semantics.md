# Instruction Semantics

## Contents

## Contents

- [1. Core](#1-core)
    - [1.1 nop](#11-nop)
    - [1.2 halt](#12-halt)
- [2. Stack](#2-stack)
    - [2.1 push](#21-push)
    - [2.2 pop](#22-pop)
    - [2.3 dup](#23-dup)
    - [2.4 swap](#24-swap)
- [3. Memory](#3-memory)
    - [3.1 load](#31-load)
    - [3.2 loadB](#32-loadb)
    - [3.3 loadG](#33-loadg)
    - [3.4 loadL](#34-loadl)
    - [3.5 store](#35-store)
    - [3.6 storeB](#36-storeb)
    - [3.7 storeG](#37-storeg)
    - [3.8 storeL](#38-storel)
    - [3.9 alloc](#39-alloc)
    - [3.10 free](#310-free)
- [4. Control](#4-control)
    - [4.1 native](#41-native)
    - [4.2 call](#42-call)
    - [4.3 ret](#43-ret)
    - [4.4 jmp](#44-jmp)
    - [4.5 jez](#45-jez)
    - [4.6 jnz](#46-jnz)
- [5. Arithmetic](#5-arithmetic)
    - [5.1 add](#51-add)
    - [5.2 sub](#52-sub)
    - [5.3 mul](#53-mul)
    - [5.4 div](#54-div)
    - [5.5 mod](#55-mod)
    - [5.6 not](#56-not)
    - [5.7 and](#57-and)
    - [5.8 orr](#58-orr)
    - [5.9 xor](#59-xor)
    - [5.10 shl](#510-shl)
    - [5.11 shr](#511-shr)
    - [5.12 sar](#512-sar)
    - [5.13 ceq](#513-ceq)
    - [5.14 cne](#514-cne)
    - [5.15 clt](#515-clt)
    - [5.16 cle](#516-cle)
    - [5.17 cgt](#517-cgt)
    - [5.18 cge](#518-cge)
- [6. Other](#6-other)
    - [6.3 conv](#61-conv)

---

**Stack notation:** [bottom ... top] → [bottom ... top]

## 1. Core

### 1.1 nop

**Stack:** `[] → []`

**Semantics:**
- Does nothing

---

### 1.2 halt

**Stack:** `[] → []`

**Semantics:**
- Terminates execution

---

## 2. Stack

### 2.1 push

**Operands:** 
- `<type> + <immediate>`
- `ptr + <label>`

**Stack:** `[] → [value]`

**Semantics:**
- If `<type>` is not `ptr`, `<immediate>` is pushed onto the operand stack
- If `<type>` is `ptr`, the address resolved from `<label>` is pushed onto the operand stack

**Type Rules:**
- `value` has type `<type>`

---

### 2.2 pop

**Stack:** `[x] → []`

**Semantics:**
- Removes the top value from the operand stack

---

### 2.3 dup

**Stack:** `[x] → [x, x]`

**Semantics:**
- Duplicates the top value of the operand stack

**Type Rules:**
- The duplicated value has the same type as the original value

---

### 2.4 swap

**Stack:** `[x, y] → [y, x]`

**Semantics:**
- Swaps the top two values of the operand stack

**Type Rules:**
- The types of `x` and `y` are unchanged

---

## 3. Memory

### 3.1 load

**Operands:** `<type>`

**Stack:** `[addr] → [value]`

**Semantics:**
- Loads a `value` from memory address starting at `addr`
- The number of bytes read from memory starting at `addr` is determined by `<type>`
- Pushes the loaded `value` onto the operand stack

**Type Rules:**
- `addr` must have type `ptr` or `ui32`
- `addr` must refer to the statically allocated data or heap region
- `value` has type `<type>`

---

### 3.2 loadB

**Stack:** `[addr] → [value]`

**Semantics:**
- Loads a single byte from memory at address `addr`

**Type Rules:**
- `addr` must have type `ptr` or `ui32`
- `addr` must refer to the statically allocated data or heap region
- `value` has type `ui32`

---

### 3.3 loadG

**Operands:** `<label>`

**Stack:** `[] → [value]`

**Semantics:**
- Loads a `value` from the statically allocated data region at the address resolved from `<label>`

**Type Rules:**
- The address resolved from `<label>` must refer to the statically allocated data region
- `value` has the same type of the data referenced by `<label>`

---

### 3.4 loadL

**Operands:** `<type> + <immediate>`

**Stack:** `[] → [value]`

**Semantics:**
- Loads a `value` from the current stack frame
- `<immediate>` identifies the stack frame slot
- Positive values of `<immediate>` identify method arguments
- Negative values of `<immediate>` identify local variables

**Type Rules:**
- `value` has type `<type>`
- `<immediate>` is a 4 byte value
- `<immediate>` must identify a slot within the current stack frame.

---

### 3.5 store

**Stack:** `[addr, value] → []`

**Semantics:**
- Stores `value` in memory starting at address `addr`
- The number of bytes written to memory starting at `addr` is determined by the type of `value`

**Type Rules:**
- `addr` must have type `ptr` or `ui32`
- `addr` must refer to the statically allocated data or heap region

---

### 3.6 storeB

**Stack:** `[addr, value] → []`

**Semantics:**
- Stores the least significant byte of `value` at address `addr`

**Type Rules:**
- `addr` must have type `ptr` or `ui32`
- `addr` must refer to the statically allocated data or heap region
- `value` must be an integer type

---

### 3.7 storeG

**Operands:** `<label>`

**Stack:** `[value] → []`

**Semantics:**
- Stores a `value` in the statically allocated data region at the address resolved from `<label>`
- The number of bytes that are written to memory starting at the address resolved from `<label>` is determined by the type of the data that `<label>` refers to

**Type Rules:**
- `value` must have the same type as the data referenced by `<label>`
- The address resolved from `<label>` must refer to the statically allocated data region

---

### 3.8 storeL

**Operands:** `<immediate>`

**Stack:** `[value] → []`

**Semantics:**
- Stores a `value` in the current stack frame
- `<immediate>` identifies the stack frame slot
- Positive values of `<immediate>` identify method arguments
- Negative values of `<immediate>` identify local variables

**Type Rules:**
- `<immediate>` is a 4 byte value
- `<immediate>` must identify a slot within the current stack frame.

---

### 3.9 alloc

**Stack:** `[size] → [addr]`

**Semantics:**
- Allocates a contiguous block of heap memory of `4 + size` bytes. (4 bytes for the header)
- A suitable free block is selected using the heap allocator's first-fit strategy
- The allocated memory is zero-initialised
- The block header stores the size of the allocated block (`4 + size`)
- Pushes `addr` which is the first address after the header onto the operand stack

**Type Rules:**
- `size` must have type `ui32`
- `addr` has type `ptr`

**Details:**
See Section 6.2 of `Execution-Model.md` for more details on allocation behaviour.

---

### 3.10 free

**Stack:** `[addr] → []`

**Semantics:**
- `addr` refers to the first byte after the block header
- Deallocates the block that starts at `addr - 4`
- The block is returned to the heap's free-list
- Adjacent free blocks in the free-list are merged

**Type Rules:**
- `addr` must have type `ptr`
- `addr` must refer to the heap region

**Details:**
See Section 6.3 of `Execution-Model.md` for more details on deallocation behaviour.

---

## 4. Control

### 4.1 native

**Operands:** `<native_ref>`

**Stack:** `[...] → [...] (depends on the invoked function)`

**Semantics:**
- Invokes the native function identified by `<native_ref>`
- The native function may pop values from and push values onto the operand stack

**Type Rules:**
- `<native_ref>` must refer to a native function provided by the VM

---

### 4.2 call

**Operands:** `<label>`

**Stack:** `[arg1, arg2, ..., argn] → []`

**Semantics:**
- Reads the number of arguments and number of local variables from memory at the address that is resolved from `<label>`
- Pops the method's arguments from the operand stack
- Creates a new stack frame
- Updates `FP` and `SP`
- Sets `PC` to the first instruction of the called method

**Type Rules:**
- `<label>` must resolve to an address that refers to the code region

**Details:**
See Section 5.2 of `Execution-Model.md` for more details on call behaviour.

---

### 4.3 ret

**Stack:** `[] → []`

**Semantics:**
- Sets `PC` to the return address stored in the current stack frame
- Restores `FP` to its previous value
- Restores `SP` to the value it had before the current stack frame was created

**Type Rules:**
- Stack frame must not be empty

**Details:**
See Section 5.3 of `Execution-Model.md` for more details on return behaviour.

---

### 4.4 jmp

**Operands:** `<label>`

**Stack:** `[] → []`

**Semantics:**
- Sets `PC` to the address that is resolved from `<label>`

**Type Rules:**
- `<label>` must resolve to an address that refers to the code region

---

### 4.5 jez

**Operands:** `<label>`

**Stack:** `[value] → []`

**Semantics:**
- Pops `value` from the operand stack
- If `value` is equal to zero, set `PC` to the address resolved from `<label>`
- Otherwise, execution continues with the next instruction

**Type Rules:**
- `<label>` must resolve to an address that refers to the code region

---

### 4.6 jnz

**Operands:** `<label>`

**Stack:** `[value] → []`

**Semantics:**
- Pops `value` from the operand stack
- If `value` is not equal to zero, set `PC` to the address resolved from `<label>`
- Otherwise, execution continues with the next instruction

**Type Rules:**
- `<label>` must resolve to an address that refers to the code region

---

## 5. Arithmetic

### 5.1 add

**Stack:** `[x, y] → [x + y]`

**Semantics:**
- Pops `x` and `y` from the operand stack
- Adds `x` and `y`
- Pushes the result onto the operand stack

**Type Rules: (non-pointer operands)**
- `x` and `y` must have the same type
- The result has the same type as `x`

**Type Rules: (pointer operands)**

| Operation    | Result type |
|--------------|-------------|
| `i32 + ptr`  | `ptr`       |
| `ptr + i32`  | `ptr`       |
| `ui32 + ptr` | `ptr`       |
| `ptr + ui32` | `ptr`       |

---

### 5.2 sub

**Stack:** `[x, y] → [x - y]`

**Semantics:**
- Pops `x` and `y` from the operand stack
- Subtracts `y` from `x`
- Pushes the result onto the operand stack

**Type Rules: (non-pointer operands)**
- `x` and `y` must have the same type
- The result has the same type as `x`

**Type Rules: (pointer operands)**

| Operation    | Result type |
|--------------|-------------|
| `ptr - ptr`  | `i64`       |
| `ptr - i32`  | `ptr`       |
| `ptr - ui32` | `ptr`       |

---

### 5.3 mul

**Stack:** `[x, y] → [x * y]`

**Semantics:**
- Pops `x` and `y` from the operand stack
- Multiplies `x` by `y`
- Pushes the result onto the operand stack

**Type Rules:**
- `x` and `y` must have the same type
- `x` and `y` must be an integer or floating type
- The result has the same type as `x`

---

### 5.4 div

**Stack:** `[x, y] → [x / y]`

**Semantics:**
- Pops `x` and `y` from the operand stack
- Divides `x` by `y`
- Pushes the result onto the operand stack

**Type Rules:**
- `x` and `y` must have the same type
- `x` and `y` must be an integer or floating type
- The result has the same type as `x`

---

### 5.5 mod

**Stack:** `[x, y] → [x % y]`

**Semantics:**
- Pops `x` and `y` from the operand stack
- Computes the remainder of dividing `x` by `y` (modulo)
- Pushes the result onto the operand stack

**Type Rules:**
- `x` and `y` must have the same type
- `x` and `y` must be an integer or floating type
- The result has the same type as `x`

---

### 5.6 not

**Stack:** `[x] → [~x]`

**Semantics:**
- Pops `x` from the operand stack
- Performs a bitwise NOT on `x`
- Pushes the result onto the operand stack

**Type Rules:**
- `x` must be an integer type
- The result has the same type as `x`

---

### 5.7 and

**Stack:** `[x, y] → [x & y]`

**Semantics:**
- Pops `x` and `y` from the operand stack
- Performs a bitwise AND on `x` and `y`
- Pushes the result onto the operand stack

**Type Rules:**
- `x` and `y` must have the same type
- `x` and `y` must be an integer type
- The result has the same type as `x`

---

### 5.8 orr

**Stack:** `[x, y] → [x | y]`

**Semantics:**
- Pops `x` and `y` from the operand stack
- Performs a bitwise OR on `x` and `y`
- Pushes the result onto the operand stack

**Type Rules:**
- `x` and `y` must have the same type
- `x` and `y` must be an integer type
- The result has the same type as `x`

---

### 5.9 xor

**Stack:** `[x, y] → [x ^ y]`

**Semantics:**
- Pops `x` and `y` from the operand stack
- Performs a bitwise XOR on `x` and `y`
- Pushes the result onto the operand stack

**Type Rules:**
- `x` and `y` must have the same type
- `x` and `y` must be an integer type
- The result has the same type as `x`

---

### 5.10 shl

**Stack:** `[x, y] → [x << y]`

**Semantics:**
- Pops `x` and `y` from the operand stack
- Computes the shift amount as `y & (bitwidth(x) - 1)`
- Performs a logical left shift of `x` by the resulting shift amount
- Pushes the result onto the operand stack

**Type Rules:**
- `x` must be an integer type
- `y` must be an integer type
- `bitwidth(x)` is 32 for `i32` and `ui32`, and 64 for `i64` and `ui64`
- The result has the same type as `x`

---

### 5.11 shr

**Stack:** `[x, y] → [x >> y]`

**Semantics:**
- Pops `x` and `y` from the operand stack
- Computes the shift amount as `y & (bitwidth(x) - 1)`
- Performs a logical right shift of `x` by the resulting shift amount
- Pushes the result onto the operand stack

**Type Rules:**
- `x` must be an integer type
- `y` must be an integer type
- `bitwidth(x)` is 32 for `i32` and `ui32`, and 64 for `i64` and `ui64`
- The result has the same type as `x`

---

### 5.12 sar

**Stack:** `[x, y] → [x >> y]`

**Semantics:**
- Pops `x` and `y` from the operand stack
- Computes the shift amount as `y & (bitwidth(x) - 1)`
- Performs an arithmetic right shift of `x` by the resulting shift amount
- Pushes the result onto the operand stack

**Type Rules:**
- `x` must be an integer type
- `y` must be an integer type
- `bitwidth(x)` is 32 for `i32` and `ui32`, and 64 for `i64` and `ui64`
- The result has the same type as `x`

---

### 5.13 ceq

**Stack:** `[x, y] → [x == y]`

**Semantics:**
- Compares `x` and `y` for equality
- Pushes `1` onto the operand stack if `x == y`, otherwise push `0`

**Type Rules:**
- `x` and `y` must have the same type
- The result has type ui32

---

### 5.14 cne

**Stack:** `[x, y] → [x != y]`

**Semantics:**
- Compares `x` and `y` for inequality
- Pushes `1` onto the operand stack if `x != y`, otherwise push `0`

**Type Rules:**
- `x` and `y` must have the same type
- The result has type ui32

---

### 5.15 clt

**Stack:** `[x, y] → [x < y]`

**Semantics:**
- Test whether `x` is less than `y`
- Pushes `1` onto the operand stack if `x < y`, otherwise push `0`

**Type Rules:**
- `x` and `y` must have the same type
- The result has type ui32

---

### 5.16 cle

**Stack:** `[x, y] → [x <= y]`

**Semantics:**
- Test whether `x` is less than or equal to `y`
- Pushes `1` onto the operand stack if `x <= y`, otherwise push `0`

**Type Rules:**
- `x` and `y` must have the same type
- The result has type ui32

---

### 5.17 cgt

**Stack:** `[x, y] → [x > y]`

**Semantics:**
- Test whether `x` is greater than `y`
- Pushes `1` onto the operand stack if `x > y`, otherwise push `0`

**Type Rules:**
- `x` and `y` must have the same type
- The result has type ui32

---

### 5.18 cge

**Stack:** `[x, y] → [x >= y]`

**Semantics:**
- Test whether `x` is greater than or equal to `y`
- Pushes `1` onto the operand stack if `x >= y`, otherwise push `0`

**Type Rules:**
- `x` and `y` must have the same type
- The result has type ui32

---

## 6. Other

### 6.1 conv

**Operands:** `<type>`

**Stack:** `[x] → [y]`

**Semantics:**
- Pops `x` from the operand stack
- Converts the value of `x` to type `<type>`
- Pushes the result (`y`) onto the operand stack

**Type Rules:**
- `ptr` cannot be converted to or from a floating type
- When converting to `ptr`, `x` must be representable as a 32-bit unsigned integer
- When converting to a floating type, `x` must be representable as `<type>`
- When converting from a floating type, `x` must be representable as `<type>`
- When converting between integer types, values that cannot be represented by `<type>` are wrapped to the range of `<type>`
- `y` has type `<type>`