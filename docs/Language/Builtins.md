# Built-in Functions

## Contents

- [1. Overview](#1-overview)
- [2. Builtin Functions](#2-builtin-functions)
  - [2.1 exit](#21-exit)
  - [2.2 print](#22-print)

---

## 1. Overview

SV provides a number of built-in functions.

Built-in functions behave like ordinary functions, but their implementations are provided by the compiler.

---

## 2. Builtin Functions

### 2.1 exit

**Signature:**
```
void exit(int)
```

**Description:**

Terminates program execution using the arguments as the exit code.

---

### 2.2 print

**Signature:**
```
void print(int)
void print(float)
void print(bool)
```

**Description:**

Outputs the argument to standard output.
- `int` values are output as decimal integers
- `float` values are output as decimal floating-point values
- `bool` values are output as `true` or `false`

