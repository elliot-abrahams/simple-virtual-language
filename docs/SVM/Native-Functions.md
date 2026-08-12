# Native Functions

## Contents

- [1. Overview](#1-overview)
- [2. Native Functions](#2-native-functions)
  - [2.1 exit](#21-exit)

## 1. Overview

Native functions are provided directly by the SVM runtime.

Each native function is identified by a unique 8-bit ID. The `invoke` instruction uses this native function ID to select and execute a native function.

Native functions interact with the operand stack to receive arguments and produce return values.

---

## 2. Native Functions

### 2.1 exit

**Native Ref:** `exit`

**ID:** `0`

**Stack:** `[status] → []`

**Semantics:**
- Pops `status` from the operand stack
- Terminates execution of the SVM
- Uses `status` as the program's exit status

**Rules:**
- `status` must be an integer type