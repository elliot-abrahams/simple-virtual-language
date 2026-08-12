# simple-virtual-language

**Simple Virtual Language (SVL)** is a compiler toolchain consisting of a compiler, assembler, and stack-based virtual machine written in C++. It compiles a custom high-level programming language into an intermediate assembly language, then into bytecode, which is executed by a stack-based virtual machine.

---

## Overview

- The compiler **(SVC)** translates `.sv` source files into an intermediate assembly language (`.svma`).
- The assembler **(SVA)** translates this intermediate assembly language into portable bytecode (`.svmb`).
- The virtual machine **(SVM)** executes `.svmb` bytecode

```
  Source Code        (.sv)
       ↓
      SVC            (Simple Virtual Compiler)
       ↓
Assembly Language    (.svma)
       ↓
      SVA            (Simple Virtual Assembler)
       ↓
    Bytecode         (.svmb)
       ↓
      SVM            (Simple Virtual Machine)
```

---

## Features

### Language

- Static type checking
- Global and local variables
- Block scopes
- Functions and function overloading
- Control flow (`if`, `while`, `break`, `continue`)
- Integer, floating-point and boolean types

### Compiler

- Recursive-descent parser
- Semantic analysis
- Abstract Syntax Tree (AST) generation
- Expression parsing with operator precedence
- Source line and column tracking for compile-time errors

### Assembler

- Two-pass assembler
- Label resolution
- Bytecode generation

### Virtual Machine

- Stack-based execution
- Virtual paged memory
- Separate code, data, heap, and call stack regions
- Dynamic heap allocation and deallocation using a free-list allocator

### Testing

Integration tests covering the complete compilation and execution pipeline

Unit tests cover:

- Virtual machine execution
- Lexical analysis (tokeniser)
- Parsing and AST generation
- Semantic analysis

---

## Building

### Requirements

- C++17 compatible compiler
- CMake 3.20+
- Git

Clone the repository:

```bash
git clone https://github.com/elliot-abrahams/simple-virtual-language.git
cd simple-virtual-language
```

Build:

```bash
mkdir build
cd build

cmake ..
cmake --build . --target sv
```

When using the Visual Studio generator, the executable will be located in the `Debug/` or `Release/` directory.

---

## Usage

```text
Commands:
    help                       Display this help message
    execute <file.svmb>        Execute bytecode
    assemble <file.svma>       Assemble assembly into bytecode
    compile <file.sv>          Compile source code into assembly
    run [-a] [-b] <file.sv>    Compile, assemble and execute .sv source code

Run options:
    -a                         Output assembly in .svma file
    -b                         Output bytecode in .svmb file
```

Display the available commands:

```bash
./sv.exe help
```

Example:

```bash
.\sv.exe run ../../examples/example.sv
```

```bash
.\sv.exe run -a ../../examples/example.sv
```

```bash
.\sv.exe run -b ../../examples/example.sv
```

```bash
.\sv.exe run -a -b ../../examples/example.sv
```

```bash
.\sv.exe compile ../../examples/example.sv
.\sv.exe assemble ../../examples/example.svma
.\sv.exe execute ../../examples/example.svmb
```

---

## Example programs

```
int factorial(int n) {
    if (n == 0) {
        return 1;
    }

    return n * factorial(n - 1);
}

print(factorial(5));
```

Output:

```
120
```

---

```
int add(int x, int y) {
    return x + y;
}

float add(float x, float y) {
    return x + y;
}

print(add(1, 5));
print(add(1.5f, 5.75f));
```

Output:

```
67.25
```

---

```
int sumTo(int n) {
    int total = 0;
    int i = 1;
    while (i <= n) {
        total = total + i;
        i = i + 1;
    }
    return total;
}
print(sumTo(10));
```
Output:

```
55
```

---

## Repository Structure

```text
docs         Project documentation
examples/    Example programs 
src          Compiler, assembler, and virtual machine source code
tests        Unit and Integration tests
```

---

## Future Improvements

- Character and string types
- Long and double types
- Arrays
- User-defined types (e.g. structs) 
- Additional built-in functions
- Additional VM native functions
- Multi-file compilation
- Import / library system
