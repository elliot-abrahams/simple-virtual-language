# Assembler

## Contents

- [1. Overview](#1-overview)
- [2. Input and Output](#2-input-and-output)
- [3. Assembly](#3-assembly)
  - [3.1 Lexical Analysis](#31-lexical-analysis)
  - [3.2 Syntax Analysis](#32-syntax-analysis)
  - [3.3 Label Table Construction](#33-label-table-construction)
  - [3.4 Bytecode Generation](#34-bytecode-generation)

---

## 1. Overview

SVA (Simple Virtual Assembler) converts SVMA source code into SVMB bytecode.

The assembler validates the SVMA source, resolves label references, and generates bytecode according to the rules defined by `SVM/Bytecode-Format.md`.

Assembly succeeds only if the SVMA source is valid.

---

## 2. Input and Output

The assembler accepts a single SVMA source file as input.

The source file must conform to the syntax and constraints defined in `Assembly-Language.md`.

A successful assembly produces bytecode that can be executed by the SVM.

When the `assemble` or `-b` run option is used, a `.svmb` bytecode file is produced in the same directory as the input file.

No bytecode is produced if assembly fails.

---

## 3. Assembly

The assembly process consists of:

1. Lexical analysis
2. Syntax analysis
3. Label table construction
4. Bytecode generation

Each stage must complete successfully before the next stage can proceed.

### 3.1 Lexical Analysis

The lexer converts the SVMA source into a sequence of tokens.

The lexer validates that lexical elements conform to their required formats, including:
- Labels
- Numbers
- Immediate Values
- Strings
- Native References

### 3.2 Syntax Analysis

The parser converts the token stream into a structured representation of the SVMA program.

The parser validates that:
- Instructions have the correct number of operands.
- Operands have the correct kind for the instruction.
- Numeric operands and immediate values have a valid sign and are either integers or decimals as required by their type.
- Method definitions contain valid metadata
- Data definitions contain values matching their declared data type.
- Statements appear in valid locations within the program.

### 3.3 Label Table Construction

The assembler constructs a label table containing the address of each defined label.

During this stage:
- Label definitions are assigned their corresponding bytecode addresses.
- Label references are checked against the label table.
- Undefined label references result in an error.

The label table is used during bytecode generation to resolve label references to their encoded addresses.

### 3.4 Bytecode Generation

The assembler converts the parsed program into SVM bytecode.

During this stage:
- Instructions, operands, methods, and data are encoded according to `Bytecode-Format.md`.
- Numeric values are checked to ensure they are within the range of their required type.
- Label references are replaced with the addresses stored in the label table.
- Native references are replaced with their corresponding native function ID.
