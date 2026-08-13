# Instruction Set

## Contents

- [1. Instruction Set](#1-instruction-set)
  - [1.1 Core](#11-core)
  - [1.2 Stack](#12-stack)
  - [1.3 Memory](#13-memory)
  - [1.4 Control](#14-control)
  - [1.5 Arithmetic](#15-arithmetic)
  - [1.6 Other](#16-other)
- [2. Type System](#2-type-system)
  - [2.1 Operand Types](#21-operand-types)
  - [2.2 Data Types](#22-data-types)
- [3. Native References](#3-native-references)

---

## 1. Instruction Set

### 1.1 Core

| Mnemonic | Opcode | Bytes | Operands | Operand Stack [bottom ... top] → [bottom ... top] | Description            |
|----------|--------|-------|----------|---------------------------------------------------|------------------------|
| nop      | 0x00   | 1     |          |                                                   | Does nothing.          |
| halt     | 0x01   | 1     |          |                                                   | Terminates execution.  |

### 1.2 Stack

| Mnemonic | Opcode | Bytes              | Operands             | Operand Stack [bottom ... top] → [bottom ... top] | Description                                                   |
|----------|--------|--------------------|----------------------|---------------------------------------------------|---------------------------------------------------------------|
| push     | 0x02   | 2 + immediate size | <type> + <immediate> | [] → [value]                                      | Pushes `<immediate>` onto the operand stack.                  |
| push     | 0x02   | 6                  | ptr + <label>        | [] → [addr]                                       | Pushes the address of a label onto the operand operand stack. |
| pop      | 0x03   | 1                  |                      | [x] → []                                          | Removes the top value from the operand stack.                 |
| dup      | 0x04   | 1                  |                      | [x] → [x, x]                                      | Duplicates the top value of the operand stack.                |
| swap     | 0x05   | 1                  |                      | [x, y] → [y, x]                                   | Swaps the top two values of the operand stack.                |

### 1.3 Memory

| Mnemonic | Opcode | Bytes | Operands             | Operand Stack [bottom ... top] → [bottom ... top] | Description                                                                     |
|----------|--------|-------|----------------------|---------------------------------------------------|---------------------------------------------------------------------------------|
| load     | 0x06   | 2     | <type>               | [addr] → [value]                                  | Loads a value of the specified `<type>` from memory.                            |
| loadB    | 0x07   | 1     |                      | [addr] → [value]                                  | Loads a single byte from memory.                                                |
| loadG    | 0x08   | 6     | <label>              | [] → [value]                                      | Loads a value from the statically allocated data region.                        |
| loadL    | 0x09   | 5     | <type> + <immediate> | [] → [value]                                      | Loads a values from the current stack frame.                                    |
| store    | 0x0a   | 1     |                      | [addr, value] → []                                | Stores a value in memory.                                                       |
| storeB   | 0x0b   | 1     |                      | [addr, value] → []                                | Stores the least significant byte of `<val>` in memory.                         |
| storeG   | 0x0c   | 5     | <label>              | [value] → []                                      | Stores a value in the statically allocated data region.                         |
| storeL   | 0x0d   | 5     | <immediate>          | [value] → []                                      | Stores a value in the current stack frame.                                      |
| alloc    | 0x0e   | 1     |                      | [size] → [addr]                                   | Allocates a block of heap memory and pushes its address onto the operand stack. |
| free     | 0x0f   | 1     |                      | [addr] → []                                       | Deallocates a previously allocated heap block starting from `<addr>`.           |

### 1.4 Control

| Mnemonic | Opcode | Bytes | Operands     | operand stack [bottom ... top] → [bottom ... top] | Description                                                       |
|----------|--------|-------|--------------|---------------------------------------------------|-------------------------------------------------------------------|
| native   | 0x10   | 2     | <native_ref> | [...] → [...] (depends on the invoked function)   | Invokes a native function provided by the VM                      |
| call     | 0x11   | 5     | <label>      | [arg1, arg2, ..., argn] → []                      | Calls a method and creates a new stack frame.                     |
| ret      | 0x12   | 1     |              |                                                   | Returns from the current method and destroys current stack frame. |
| jmp      | 0x13   | 5     | <label>      |                                                   | Jump to `<label>` unconditionally.                                |
| jez      | 0x14   | 5     | <label>      | [value] → []                                      | Jump to `<label>` if value is zero                                |
| jnz      | 0x15   | 5     | <label>      | [value] → []                                      | Jump to `<label>` if value is not zero.                           |

### 1.5 Arithmetic

| Mnemonic | Opcode | Bytes | Operands | Operand Stack [bottom ... top] → [bottom ... top] | Description                                                  |
|----------|--------|-------|----------|---------------------------------------------------|--------------------------------------------------------------|
| add      | 0x16   | 1     |          | [x, y] → [x + y]                                  | Adds two values.                                             |
| sub      | 0x17   | 1     |          | [x, y] → [x - y]                                  | Subtracts one value from another.                            |
| mul      | 0x18   | 1     |          | [x, y] → [x * y]                                  | Multiplies two values.                                       |
| div      | 0x19   | 1     |          | [x, y] → [x / y]                                  | Divides one value by another.                                |
| mod      | 0x1a   | 1     |          | [x, y] → [x % y]                                  | Computes the remainder of a division.                        |
| not      | 0x1b   | 1     |          | [x] → [~x]                                        | Performs a bitwise NOT.                                      |
| and      | 0x1c   | 1     |          | [x, y] → [x & y]                                  | Performs a bitwise AND.                                      |
| orr      | 0x1d   | 1     |          | [x, y] → [x \| y]                                 | Performs a bitwise OR.                                       |                                             
| xor      | 0x1e   | 1     |          | [x, y] → [x ^ y]                                  | Performs a bitwise XOR.                                      |
| shl      | 0x1f   | 1     |          | [x, y] → [x << y]                                 | Performs a logical left shift.                               |
| shr      | 0x20   | 1     |          | [x, y] → [x >> y]                                 | Performs a logical right shift.                              |
| sar      | 0x21   | 1     |          | [x, y] → [x >> y]                                 | Performs an arithmetic right shift.                          |                                                                   |
| ceq      | 0x22   | 1     |          | [x, y] → [x == y]                                 | Compares two values for equality.                            |
| cne      | 0x23   | 1     |          | [x, y] → [x != y]                                 | Compares two values for inequality.                          |
| clt      | 0x24   | 1     |          | [x, y] → [x < y]                                  | Tests whether one value is less than another.                |
| cle      | 0x25   | 1     |          | [x, y] → [x <= y]                                 | Tests whether one value is less than or equal to another.    |
| cgt      | 0x26   | 1     |          | [x, y] → [x > y]                                  | Tests whether one value is greater than another.             |
| cge      | 0x27   | 1     |          | [x, y] → [x >= y]                                 | Tests whether one value is greater than or equal to another. |

### 1.6 Other

| Mnemonic | Opcode | Bytes | Operands    | Operand Stack [bottom ... top] → [bottom ... top] | Description                                                                                  |
|----------|--------|-------|-------------|---------------------------------------------------|----------------------------------------------------------------------------------------------|
| conv     | 0x28   | 2     | <type>      | [x] → [y]                                         | Converts the top value of the operand stack to the specified `<type>` and pushes the result. |

---

## 2. Type System

### 2.1 Operand Types

| type | Encoding | Bytes   |
|------|----------|---------|
| i32  | 0x00     | 4       |
| ui32 | 0x01     | 4       |
| i64  | 0x02     | 8       |
| ui64 | 0x03     | 8       |
| f32  | 0x04     | 4       |
| f64  | 0x05     | 8       |
| ptr  | 0x06     | 4       |


### 2.2 Data Types

| type | Encoding | Bytes              |
|------|----------|--------------------|
| i32  | 0x00     | 4                  |
| ui32 | 0x01     | 4                  |
| i64  | 0x02     | 8                  |
| ui64 | 0x03     | 8                  |
| f32  | 0x04     | 4                  |
| f64  | 0x05     | 8                  |
| ptr  | 0x06     | 4                  |
| str  | 0x07     | 4 + UTF-8 encoding |

---

## 3. Native References

| native_ref | Encoding |
|------------|----------|
| exit       | 0x00     |
| print      | 0x01     |
| print_str  | 0x03     |