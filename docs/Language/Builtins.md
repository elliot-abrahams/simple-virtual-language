# Built-ins

## Contents

- [1. Overview](#1-overview)
- [2 Built-in Types](#2-built-in-types)
  - [2.1 Array](#21-array)
    - [2.1.1 Length Field](#211-length-field)
- [3. Built-in Functions](#3-built-in-functions)
  - [3.1 exit](#31-exit)
  - [3.2 print](#32-print)
  - [3.3 println](#33-println)

---

## 1. Overview

SV provides a number of built-in types and functions.

Built-in types provide functionality that is available without being explicitly defined by the programmer.

Built-in functions behave like ordinary functions, but their implementations are provided by the compiler.

---

## 2. Built-in Types

### 2.1 Array

Arrays provide indexed storage for a fixed number of elements.

### 2.1.1 Length Field

**Type:** `int`

**Description:** Returns the number of elements in the array.

---

## 3. Built-in Functions

### 3.1 exit

**Signature:**
```
void exit(int)
```

**Description:**

Terminates program execution using the arguments as the exit code.

---

### 3.2 print

**Signature:**
```
void print(int)
void print(float)
void print(bool)
void print(char)
```

**Description:**

Outputs the argument to standard output.
- `int` values are output as decimal integers
- `float` values are output as decimal floating-point values
- `bool` values are output as `true` or `false`
- `char` values are output as their corresponding Unicode character, encoded as UTF-8.

### 3.3 println

**Signature:**
```
void println(int)
void println(float)
void println(bool)
void println(char)
```

**Description:**

Outputs the argument to standard output using the same formatting as the built-in function `print`, followed by a newline.
