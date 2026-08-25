# Runtime Errors

## Contents

- [1. Overview](#1-overview)
- [2. Error Categories](#2-error-categories)
  - [2.1 Language Runtime Errors](#21-language-runtime-errors)
  - [2.2 Internal Runtime Errors](#22-internal-runtime-errors)
- [3. Error Reference](#3-error-reference)
  - [3.1 DivisionByZeroError](#31-divisionbyzeroerror)
  - [3.2 OutOfRangeError](#32-outofrangeerror)
  - [3.3 StackOverflowError](#33-stackoverflowerror)
  - [3.4 OutOfMemoryError](#34-outofmemoryerror)
  - [3.5 InternalError](#35-internalerror)

---

## 1. Overview

A runtime error occurs while the SVM is executing bytecode.

Runtime errors are divided into two categories:

- **Language runtime errors** — errors that can occur when executing a valid SV program.
- **Internal runtime errors** — errors that cannot occur when executing bytecode produced from valid SV source code by the SVC and SVA.

The category of an error does not depend on where the bytecode came from.

For example, division by zero us always `DivisionByZeroError`. It can occur in both SV programs and manually written assembly or bytecode:
```
push i32 #5
push i32 #0
div
```

An invalid operand type is an `internalError` because valid SV source code cannot produce bytecode with invalid operand types.

---

## 2. Error Categories

### 2.1 Language Runtime Errors

Language runtime errors are errors that are possible during normal execution of an SV program.

The SVC may therefore produce bytecode that can cause these errors.

Examples include:
- division by zero
- modulo by zero
- conversion out of range
- stack overflow
- heap overflow

These errors can also occur when executing bytecode that was not produced by the SVC and SVA.

---

### 2.2 Internal Runtime Errors

Internal errors are errors that cannot occur when executing bytecode produced from valid SV source code by the SVC and SVA.

For example, the SVM may encounter:
```
InternalError: invalid operand type for 'add'
```

when executing manually written assembly that puts invalid operands on the stack.

This cannot be produced by valid SV source code compiled by the SVC.

---

## 3. Error Reference

### 3.1 DivisionByZeroError

***Category***: Language runtime error

Occurs when division or modulo uses zero as the divisor.

**Messages:**
```
division by zero
modulo by zero
```

### 3.2 OutOfRangeError

***Category***: Language runtime error

Occurs when a value cannot be represented by the target type during a conversion.

**Message:**
```
value cannot be represented by the target type
```

---

### 3.3 StackOverflowError

***Category***: Language runtime error

Occurs when the call stack exceeds its available capacity.

**Message:**
```
stack overflow
```

---

### 3.4 OutOfMemoryError

***Category***: Language runtime error

Occurs when the SVM cannot allocate the requested amount of heap memory.

**Messages:**
```
heap memory exhausted
```

---

### 3.5 InternalError

***Category***: Internal runtime error

Occurs when the SVM encounters a condition that cannot be produced by valid SV source code compiled and assembled by the SVC and SVA.

**Empty operand stack pop**

```
attempted to pop from an empty operand stack
```

Occurs when an operation attempts to pop a value from an empty operand stack.

**Empty operand stack peek**

```
attempted to peek at an empty operand stack
```

Occurs when an operation attempts to peek at an empty operand stack.

**Full operand stack push**

```
attempted to push onto a full operand stack
```

Occurs when an operation attempts to push a value onto a full operand stack.

**Invalid operand type**

```
invalid operand type <type> for '<instruction>'
```

Occurs when an instruction receives an operand with an invalid type.

**Invalid operand types**

```
invalid operand types <type1> and <type2> for '<instruction>'
```

Occurs when an instruction receives two operands with invalid types.

**Invalid operand type for instruction with multiple valid types** 

```
invalid operand type for '<instruction>'. expected types(s) <types>, but found <type>
```

Occurs when an instruction accepts multiple types but receives a type that is not one of the accepted types.

**Invalid memory read**

```
attempted to read from address 0x<address> outside the permitted memory regions
```

Occurs when the SVM attempts to read from a memory region that it is not permitted to access.

**Invalid memory write**

```
attempted to write to address 0x<address> outside the permitted memory regions
```

Occurs when the SVM attempts to write to a memory region that it is not permitted to access.

**Invalid local or argument access**

```
attempted to access a local variable or argument outside the current call frame
```

Occurs when `loadL` or `storeL` accesses a local variable or argument outside the current call frame.

**Local or argument access without a call frame**

```
attempted to access a local variable or argument without an active call frame
```

Occurs when `loadL` or `storeL` is executed without an active call frame.

**Invalid return**

```
cannot execute `ret` with an empty call stack
```

Occurs when `ret` is executed without an active call frame.

**Invalid conversion**

```
cannot convert value from type <oldType> to type <newType>
```

Occurs when an unsupported type conversion is attempted.

**Conversion out of range**

```
out of range during conversion from type <oldType> to type <newType>
```

Occurs when the SVM encounters an out-of-range conversion that cannot be produced by valid SV code.

**Zero-byte allocation**

```
attempted to allocate 0 bytes in heap memory
```

Occurs when `alloc` is requested to allocate zero bytes.

**Undefined native function**

```
native call ID <id> does not correspond to a defined function
```

Occurs when `native` uses an undefined native function ID.

**Invalid native function operand type**

```
invalid operand type for native function 'native_function'
```

Occurs when a native function receives an operand with an invalid type.
