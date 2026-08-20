# Compiler

## Contents

- [1. Overview](#1-overview)
- [2. Input and Output](#2-input-and-output)
- [3. Compilation](#3-compilation)
  - [3.1 Tokenisation and Parsing](#31-tokenisation-and-parsing)
  - [3.2 Built-in Function Registration](#32-built-in-function-registration)
  - [3.3 Semantic Analysis](#33-semantic-analysis)
  - [3.4 Local Slot Assignment](#34-local-slot-assignment)
  - [3.5 Assembly Generation](#35-assembly-generation)
  - [3.6 Assembly Emission](#36-assembly-emission)

---

## 1. Overview

SVC (Simple Virtual Compiler) converts SV source code into SVMA assembly.

The compiler validates SV source code, performs semantic analysis, generates an intermediate representation of the SVMA assembly, and emits SVMA assembly according to the rules defined by `SVA/Assembly-Language.md`.

Compilation succeeds only if the SV source is valid.

---

## 2. Input and Output

The compiler accepts a single SV source file as input.

The source file must conform to the grammar defined in `Language/Grammar.ebnf` and the semantic rules defined in `Language/Semantics.md`.

A successful compilation produces SVMA assembly that can be assembled by the SVA.

When the `compile` or `-a` run option is used, a `.svma` assembly file is produced in the same directory as the input file.

No assembly is produced if compilation fails.

---

## 3. Compilation

The compilation process consists of:
1. Tokenisation and parsing
2. Built-in function registration
3. Semantic analysis
4. Local slot assignment
5. Assembly generation
6. Assembly emission

Each stage must complete successfully before the next stage can proceed.

### 3.1 Tokenisation and Parsing

The tokeniser reads the SV source and produces tokens while the parser constructs the abstract syntax tree (AST).

The tokeniser validates lexical elements according to the lexical rules defined in `Language/Grammar.ebnf`, including:
- Integer Literals
- Float Literals
- Boolean Literals
- Keywords
- Identifiers

The parser validates that the token sequence conforms to the grammar defined in `Language/Grammar.ebnf`.

The output of this stage is an AST representing the SV program.

### 3.2 Built-in Function Registration

The compiler registers its built-in functions in the symbol table.

Built-in functions are treated as predefined functions during semantic analysis and function call resolution.

User-defined functions cannot define a function with a signature that conflicts with an existing built-in function.

### 3.3 Semantic Analysis

The semantic analyser validates the meaning of the program and builds the symbol table that is required by later compilation stages.

The semantic analyser performs:
- Registration of user-defined function signatures
- Assignment of assembly labels to functions
- Variable declaration and scope validation
- Variable initialisation tracking
- Statement and expression type checking
- Function call and overload resolution
- Validation of `return`, `break`, and `continue` statements
- Control-flow return analysis

**Analysis Order**
Semantic analysis is performed in the following order:
1. Register user-defined function signatures
2. Process statements in the global scope
3. Process user-defined function bodies

This allows:
- Functions to be called before their declaration
- Global statements to access only variables declared earlier in the source code
- Function bodies to access global variables regardless of where those variables are declared

### 3.4 Local Slot Assignment

Local variables and function parameters are assigned slots within their containing function.

These slots are used by the generated assembly to access local variables during execution.

### 3.5 Assembly Generation

The assembly generator converts the validated AST and symbol table into an intermediate representation (IR) of the SVMA assembly according to the rules defined in `SVA/Assembly-Language.md`.

The assembly IR represents the assembly items that will be emitted by the compiler.

Assembly generation produces assembly for:
1. Global-scope statements
2. Compiler-generated scope functions
3. User-defined functions
4. User-defined global variables

Built-in functions and built-in data are not inserted into the assembly during this stage. The required Built-in functions and built-in data are tracked by the assembly generator and inserted during assembly emission.

See...........................

### 3.6 Assembly Emission

The assembler emitter converts the IR produced by the assembly generator into textual SVMA assembly.

During emission, the emitted:
- Produces the textual representation of each assembly item
- Inserts the required built-in functions
- Inserts the required built-in data

The resulting SVMA assembly is then available to be assembled by the SVA.
