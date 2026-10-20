# SVC Documentation

The SVC documentation describes the design and operation of the SV compiler.

## Assembly Generation

`Assembly-Generation.md`

Describes how the semantically analysed AST is translated into SVMA assembly.

- Program Generation
- Types
- Expressions
- Statements
- Functions
- Built-in Functions
- Built-in Data
- Metadata

---

## Compiler

`Compiler.md`

Describes the overall compilation process and the stages performed by SVC.

---

## Compiler Errors

`Compiler-Errors.md`

Describes the errors that can be reported during compilation.

- Syntax Errors
- Semantic Errors
- Type Errors

## Lexical Structure

`Lexical-Structure.md`

Describes how SV source code is converted into tokens.

- Whitespace
- Delimiters
- Operators
- Keywords
- Identifiers
- Literals

## Optimisation

`optimisation/`

Describes the optimisation passes performed on the AST before assembly generation.

- `Constant-Folder.md`