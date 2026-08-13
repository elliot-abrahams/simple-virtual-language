# Assembly Language

## Contents

- [1. Overview](#1-overview)
- [2. SVMA Structure](#2-svma-structure)
    - [2.1 Code Region](#21-code-region)
    - [2.2 Data Region](#22-data-region)
    - [2.3 Region Rules](#23-region-rules)
- [3. Lexical Rules](#3-lexical-rules)
    - [3.1 Whitespace](#31-whitespace)
    - [3.2 Comments](#32-comments)
    - [3.3 Case Sensitivity](#33-case-sensitivity)
- [4. Literals](#4-literals)
    - [4.1 Integer Literals](#41-integer-literals)
    - [4.2 Floating-Point Literals](#42-floating-point-literals)
    - [4.3 String Literals](#43-string-literals)
- [5. Instructions](#5-instructions)
    - [5.1 Instruction Format](#51-instruction-format)
    - [5.2 Instruction Mnemonics](#52-instruction-mnemonics)
    - [5.3 Operands](#53-operands)
    - [5.4 Immediate Values](#54-immediate-values)
    - [5.5 Type Operands](#55-type-operands)
    - [5.6 Native References](#56-native-references)
- [6. Labels](#6-Labels)
    - [6.1 Label Names](#61-label-names)
    - [6.2 Label Definitions](#62-label-definitions)
    - [6.3 Label References](#63-label-references)
    - [6.4 Code Labels](#64-code-labels)
    - [6.5 Method Labels](#65-method-labels)
    - [6.6 Data Labels](#66-data-labels)
    - [6.7 Label Namespaces](#67-label-namespaces)
    - [6.8 Label Resolution](#68-label-resolution)
- [7. Methods](#7-methods)
    - [7.1 Method Definitions](#71-method-definitions)
    - [7.2 Method Metadata](#72-method-metadata)
    - [7.3 Method Body](#73-method-body)
    - [7.4 Method Termination](#74-method-termination)
- [8. Data Definitions](#8-data-definitions)
    - [8.1 Data Definition Format](#81-data-definition-format)
    - [8.2 Data Types](#82-data-types)
    - [8.3 Numeric Data](#83-numeric-data)
    - [8.4 String Data](#84-string-data)
    - [8.5 Pointer Data](#85-pointer-data)
- [9. Program Constraints](#9-program-constraints)
    - [9.1 Region Constraints](#91-region-constraints)
    - [9.2 Label Constraints](#92-label-constraints)
    - [9.3 Method Constraints](#93-method-constraints)
    - [9.4 Instruction Constraints](#94-instruction-constraints)
    - [9.5 Data Definition Constraints](#95-data-definition-constraints)
- [10. Examples](#10-examples)
    - [10.1 Add Two Numbers](#101-add-two-numbers)
    - [10.2 Output String](#102-output-string)
    - [10.3 Method](#103-method)
    - [10.4 Loop](#104-loop)

---

## 1. Overview

SVMA (SVM Assembly) is the assembly language used to represent programs for the SVM (Simple Virtual Machine).

SVMA provides a human-readable representation of SVM instructions, methods, addresses, and statically allocated data.

An SVMA source file is assembled by the SVA into bytecode that can be executed by the SVM.

---

## 2. SVMA Structure

An SVMA source file consists of a code region followed by an optional data region.

### 2.1 Code Region

The code region is the default region and begins at the start of the file.

The code region may contain:
- Instructions
- Code label definitions
- Method definitions
- Method instructions

Example:
```
$start:
    push i32 #5
    native print
    halt
```

### 2.2 Data Region

The data region contains statically allocated data definitions.

It starts from the keyword `.data`.

Example:
```
.data

$message: str "Hello
$value: i32 40
```

### 2.3 Region Rules

An SVMA source file:
- Must contain only one code region
- Must contain zero or one data regions
- Must contain keyword `.data` before any data definitions.
- Must not contain instructions or method definitions in the data region
- Must not contain data definitions in the code region

---

## 3. Lexical Rules

### 3.1 Whitespace

Whitespace separates tokens and is otherwise ignored.

Whitespace includes spaces, tabs, and newlines.

Whitespace within string literals are preserved.

### 3.2 Comments

Comments begin with `;` and continues until the end of the line.

Comments are ignored by the assembler.

```
push i32 #10 ; Push the value 10
```

### 3.3 Case Sensitivity

SVMA keywords, instruction mnemonics, types, and labels are case-sensitive.

For example:
```
push i32 #10
```

is not equivalent to:
```
PUSH I32 #10
```

---

## 4. Literals

### 4.1 Integer Literals

Integer literals represent signed or unsigned integer values

Examples:
```
10
-10
0
255
```

The value must be representable by the type in which it is used.

### 4.2 Floating-Point Literals

Floating-point literals represent `f32` or `f64` values

Examples:
```
5.5
-2.75
0.0
```

The value must be representable by the type in which it is used.

### 4.3 String Literals

String literals represent sequences of UTF-8 encoded characters and are enclosed in double quotes.

Examples:
```
"Hello"
"Hello world"
```

---

### 5. Instructions

## 5.1 Instruction Format

An instruction consists of an instruction mnemonic followed by zero or more operands.

```
[mnemonic] [operand1] [operand2]
```

### 5.2 Instruction Mnemonics

Instruction mnemonics identify the SVM instruction to execute.

Each mnemonic must correspond to an instruction defined by the SVM instruction set.

See `SVM/Instruction-Set.md` for the complete instruction set and operands for each instruction.

### 5.3 Operands

Operands may include:
- Type
- Immediate value
- Label references
- Native references

Operands must appear in the order specified by the instruction.

Example:
```
push i32 #10
```

contains:
- `push` → instruction mnemonic
- `i32` → type
- `#10` → immediate

### 5.4 Immediate values

Immediate values are prefixed with `#`.

Examples:
```
push i32 #10
push i64 #-20
push f64 #3.14
```

The type of an immediate value is determined by the instruction's operand requirements. 

The exceptions to this rule are:
- `loadL`
- `storeL`

See `SVM/Instruction-Semantics.md` for more details on the immediate values that are valid for each instruction.

### 5.5 Type Operands

Types identify the representation of a value used by an instruction.

The supported types are:
```
i32
ui32
i64
ui64
f32
f64
ptr
```

See `SVM/Instruction-Semantics.md` for more details on how the type operand affect each instruction.

### 5.6 Native References

Native references identify VM-provided native functions.

A native reference consists of the name of a native function.

See `SVM/Instruction-Set.md` and `SVM/Native-Functions.md` for more details.

---

## 6. Labels

Labels provide symbolic names for locations within an SVMA program.

SVMA has three categories of labels:
- Code labels
- Method labels
- Data labels

### 6.1 Label Names

A label name must match:
```
\$[a-zA-Z_][a-zA-Z0-9_(),]*
```

Examples:
```
$loop
$add_3 
$value
```

The `$` identifies the symbol as a label.

### 6.2 Label Definitions

A label definition associates a label name with a location within the program.

A label definition has the form:
```
$<label name>:
```

Examples:
```
$loop:
$add_3:
$value:
```

The `:` identifies the label as a definition and is not part of the label's name.

Label definitions do not emit bytecode.

### 6.3 Label References

A label reference refers to a label by its name without the trailing `:`.

The instruction containing the reference determines the category of label that may be referenced.

A label reference may refer to a label defined before or after the reference.

### 6.4 Code Labels

A code label identifies the address of an instruction within the code region.

Example:
```
$loop:
    push i32 #1
    jmp $loop
```

The assembler resolves the reference to the address associated with the label.

The label refers to the address of the instruction immediately following the label definition.

The label definition that the label reference refers to may be defined before or after the reference.

### 6.5 Method Labels

A method label identifies the address of a method.

A method is declared using `def` followed by a label definition.

Example:
```
def $add:
    args 2
    locals 0
```

The method label refers to the beginning of the method's encoded metadata.

### 6.6 Data Labels

A data label identifies the location of a statically allocated data value.

Data labels are defined in the data region.

Example:
```
.data

$message: str
```

A data label refers to the first byte of the encoded data value, immediately after the data type.

### 6.7 Label Namespaces

Code, method, and data labels have separate namespaces.

Therefore, the same label name may be used once in each category.

For example, the following is valid:
```
$loop:
    ...
    
def $loop:
    args 0
    locals 0
    
    ...
    
.data
$loop: i32 10
```

These are three distinct labels

A label name must be unique within each category

For example, two code label with the same name are not permitted.

### 6.8 Label Resolution

The instruction in which a label is referenced determines which label namespace is searched.

| Instruction | Label category required |
|-------------|-------------------------|
| `jmp`       | Code                    |
| `jez`       | Code                    |
| `jnz`       | Code                    |
| `call`      | Method                  |
| `push`      | Data                    |
| `loadG`     | Data                    |
| `storeG`    | Data                    |

If a referenced label does not exist in the required namespace, the assembly is invalid.

---

## 7. Methods

Methods define reusable sequences of instructions that can be invoked by the SVM.

### 7.1 Method Definitions

A method is introduced using the `def` keyword followed by a method label definition.

```
def $add:
    args 2
    locals 0
    
    ...
```

### 7.2 Method Metadata

A method definition must be followed by its metadata.

The supported metadata is:
```
args <number>
locals <number>
```

`args` specifies the number of arguments accepted by the method. 

The number of arguments must be an unsigned 8-bit value, ranging from `0` to `255`.

`locals` specifies the number of local variable slots allocated for the method.

The number of locals must be an unsigned 32-bit value, ranging from `0` to `4,294,967,295`.

### 7.3 Method Body

The method body consists of instructions following the method metadata.

Example:
```
def $add:
    args 2
    locals 0
    
    loadL i32 #1
    loadL i32 #2
    add
    ret
```

### 7.4 Method Termination

The method body continues until another method definition, the data region, or the end of the source code is reached.

A method does not implicitly terminate at the end of the method body. A method can return using `ret`, or execution can be terminated using `halt` or a native function that terminates execution.

---

## 8. Data Definitions

Data definitions declare statically allocated values.

### 8.1 Data definition Format

A data definition has the form:
```
[label definition] [data type] [value]
```

Example:
```
.data

$value: i32 52
$message: str "Hello"
```

### 8.2 Data Types

Data Definitions support the following data types:
```
i32
ui32
i64
ui64
f32
f64
ptr
str
```

See `SVM/Instruction-Set.md` for the encoding of each data type.

### 8.3 Numeric Data

Numeric data is written as a numeric literal.

Examples:
```
$x: i32 -10
$y: ui64 100
$z: f64 -3.14
```

The value must be representable by the specified type.

### 8.4 String Data

String data contains a sequence of characters enclosed in double quotes.

Example:
```
$message: str "Hello World!"
```

String data is encoded using UTF-8 when converted to bytecode.

The address of a string can be pushed onto the operand stack using `push ptr` with the string's data label.

Example
```
push ptr $message
```

### 8.5 Pointer Data

Pointer data is defined using a label reference.

This label reference must refer to a data label definition.

Example:
```
$x: i32 5
$pointer: ptr $x
```

### 8.6 Data Constraints

| Type | Size (Bytes) | Range / Precision                                       | Maximum value | 
|------|--------------|---------------------------------------------------------|---------------|
| i32  | 4            | -2,147,483,648 to 2,147,483,647                         |               |
| ui32 | 4            | 0 to 4,294,967,295                                      |               |
| i64  | 8            | -9,223,372,036,854,775,808 to 9,223,372,036,854,775,807 |               |
| ui64 | 8            | 0 to 18,446,744,073,709,551,615                         |               |
| ptr  | 4            | 0 to 4,294,967,295                                      |               |
| f32  | 4            | IEEE-754 Single Precision                               |
| f64  | 8            | IEEE-754 Double Precision                               |

String data has a maximum size of 4,294,967,295 bytes when encoded as UTF-8.

---

## 9. Program Constraints

The following constraints apply to SVMA programs.

### 9.1 Region Constraints
- Only one `.data` region may be declared.
- The `.data` region must occur after the code region.
- Instructions cannot appear in the data region.
- Data definitions cannot appear in the code region.

### 9.2 Label Constraints
- Labels must follow the required label syntax.
- Labels must be unique within their namespace.
- A label reference must resolve to a label of the appropriate category.

### 9.3 Method Constraints
- Method metadata must immediately follow its method definition.
- The number of arguments must fit within an unsigned 8-bit value.
- The number of locals must fit within an unsigned 32-bit value.
- Method instructions must appear after the method metadata.

### 9.4 Instruction Constraints
- Instructions must use a defined mnemonic.
- The number of operands must match the instruction definition.
- Operands must appear in the order specified by the instruction.
- Operand types must match the requirements of the instruction.
- Label references must refer to the required label category.
- A native reference must refer to a native function provided by the SVM.

### 9.5 Data Definition Constraints
- The data of a data definition must be valid for its data type.

---

## 10. Examples

### 10.1 Add Two Numbers
```
        push i32 #5
        push i32 #10
        add
        native print
        halt
```

### 10.2 Output String
```
        push ptr $message
        native print_str
        halt

.data
$message: str "Hello World!"
```

### 10.3 Method
```
        push f32 #5.2
        push f32 #-2.1
        call $mul
        pop
        halt
    
def $mul:
        args 2
        locals 0
        loadL f32 #0
        loadL f32 #1
        mul
        ret
```

### 10.4 Loop
```
$start_loop:
        loadG $i
        push i32 #5
        clt
        jez $end_loop
    
        loadG $i
        dup
        native print
        push i32 #1
        add
        storeG $i
        
        jmp $start_loop
    
    
$end_loop:
        halt
    
    
.data
$i: i32 0
```