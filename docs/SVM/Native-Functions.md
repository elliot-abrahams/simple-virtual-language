# Native Functions

## Contents

- [1. Overview](#1-overview)
- [2. Native Functions](#2-native-functions)
  - [2.1 exit](#21-exit)
  - [2.2 print](#22-print)

## 1. Overview

Native functions are provided directly by the SVM runtime.

Each native function is identified by a unique 8-bit ID. The `invoke` instruction uses this native function ID to select and execute a native function.

Native functions interact with the operand stack to receive arguments and produce return values.

---

## 2. Native Functions

### 2.1 exit

**Native Ref:** `exit`

**ID:** `0x00`

**Stack:** `[status] → []`

**Semantics:**
- Pops `status` from the operand stack
- Terminates execution of the SVM
- Uses `status` as the program's exit status

**Type Rules:**
- `status` must be an integer type

---

### 2.2 print

**Native Ref:** `print`

**ID:** `0x01`

**Stack:** `[value]→ []`

**Semantics:**
- Pops `value` from the operand stack
- Outputs `value` to standard output

**Type Rules:**
- `value` must not have type `ptr`

---

### 2.3 print_str

**Native Ref:** `print_str`

**ID:** `0x03`

**Stack:** `[addr]→ []`

**Semantics:**
- Pops `addr` from the operand stack
- Output the string located at the address `addr` in memory to standard output

**Type Rules:**
- `addr` must have type `ptr`