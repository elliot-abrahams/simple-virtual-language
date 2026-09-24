# Assembler Errors

## Contents

- [1. Overview](#1-overview)
- [2. Error Format](#2-error-format)
- [3. Assembler Errors](#3-assembler-errors)
  - [3.1 File Open Failure](#31-file-open-failure)
  - [3.2 Unexpected End of File](#32-unexpected-end-of-file)
  - [3.3 Unexpected Token](#33-unexpected-token)
  - [3.4 Invalid Keyword](#34-invalid-keyword)
  - [3.5 Invalid Directive](#35-invalid-directive)
  - [3.6 Invalid Label](#36-invalid-label)
  - [3.7 Invalid String](#37-invalid-string)
  - [3.8 Invalid Value for Type](#38-invalid-value-for-type)
  - [3.9 Invalid Instruction Immediate](#39-invalid-instruction-immediate)
  - [3.10 Number of Method Arguments Out of Range](#310-number-of-method-arguments-out-of-range)
  - [3.11 Number of Method Locals Out of Range](#311-number-of-method-locals-out-of-range)
  - [3.12 Duplicate Label](#312-duplicate-label)
  - [3.13 Undefined Label](#313-undefined-label)
  - [3.14 Invalid Source ID](#314-invalid-source-id)
  - [3.15 Invalid Address](#315-invalid-address)
  - [3.16 Invalid Line Number](#316-invalid-line-number)
  - [3.17 Invalid Column Number](#317-invalid-column-number)

---

## 1. Overview

The assembler reports errors when an assembly source file cannot be parsed, validated, or converted into bytecode.

Each error identifies the location in the assembly source program where the error was detected.

## 2. Error Format

Assembler errors are reported using the following format:

```
<ErrorType>: <message>
  <line> |    <source line>
    |    <caret>
  at <absolute file path>:<line>:<column>
```

For example:

```
AssemblerError: unexpected token '#5'
  1 |    push #5
    |         ^
  at C:\...\svl\examples\test.svma:1:14
```

If the source line cannot be read, the error instead shows:

```
AssemblerError: unexpected token '#5'
    | source code unavailable
  at C:\...\svl\examples\test.svma:1:14
```

---

## 3. Assembler Errors

### 3.1 File Open Failure

**Message:**
```
could not open file
```

The assembler could not open the specified assembly source file.

---

### 3.2 Unexpected End of File

**Message:**
```
unexpected 'EOF'
```

**Example:**
```
push
```

```
AssemblerError: unexpected token 'EOF'
```

The assembler encountered the end of the input file while more tokens were expected.

---

### 3.3 Unexpected Token

**Message:**
```
unexpected token '<token_image>'
```

**Example:**
```
pop #1
```

```
AssemblerError: unexpected token '#1'
```

The assembler encountered a token that is not valid in the current context.

---

### 3.4 Invalid Keyword

**Message:**
```
invalid keyword '<keyword>'
```

**Example:**
```
key
```

```
AssemblerError: invalid keyword 'key'
```

The specified keyword is not recognised by the assembler.

---

### 3.5 Invalid Directive

**Message:**
```
invalid directive '<directive>'

```

**Example:**
```
.t
```

```
AssemblerError: invalid directive '.t'
```

The specified directive is not recognised by the assembler.

---

### 3.6 Invalid Label

**Message:**
```
invalid label '<label>'
```

**Example:**
```
$1x:
```

```
AssemblerError: invalid label '$1x:'
```

The specified label does not follow the syntax required for assembly labels.

---

### 3.7 Invalid String

**Message:**
```
invalid string '<string>'
```

**Example:**
```
"a
```

```
AssemblerError: invalid string '"a'
```

The specified string does not follow the syntax required for strings.

---

### 3.8 Invalid Value for Type

**Message:**
```
'<value>' is invalid for type '<type>'
```

**Example:**
```
push ui32 #-1
```

```
AssemblerError: '#-1' is invalid for type 'ui32'
```

The specified value is not valid for the given type.

---

### 3.9 Invalid Instruction Immediate

**Message:**
```
invalid immediate '<value>' for instruction '<instruction>'
```

**Example:**
```
rotD #2
```

```
AssemblerError: invalid immediate '#2' for instruction 'rotD'
```

The specified immediate is not value for the given instruction.

---

### 3.10 Number of Method Arguments Out of Range

**Message:**
```
'<number of args>' is out of range for number of method arguments
```

**Example:**
```
def $foo:
    args 256
    locals 0
```

```
AssemblerError: '256' is out of range for number of method arguments
```

The specified number of method arguments is not valid.

---

### 3.11 Number of Method Locals Out of Range

**Message:**
```
'<number of locals>' is out of range for number of method locals
```

**Example:**
```
def $foo:
    args 0
    locals -1
```

```
AssemblerError: '-1' is out of range for number of method locals
```

The specified number of method locals is not valid.

---

### 3.12 Duplicate Label

**Message:**
```
<label_type> label '<label>' is already defined
```

**Example:**
```
$x:
$x:
```

```
AssemblerError: code label '$x' is already defined
```

A label with the same name has already been defined in the specified label namespace.

---

### 3.13 Undefined Label

**Message:**
```
<label_type> label '<label_name>' is undefined
```

**Example:**
```
jmp $x
```

```
AssemblerError: code label '$x' is undefined
```

The specified label has not been defined in the required label namespace.

---

### 3.14 Invalid Source ID

**Message:**
```
invalid source id '<source_id>'
```

**Example:**
```
.metadata
.sources
 0a 
```

```
AssemblerError: invalid source id '0a'
```

The specified source ID is not valid.

---

### 3.15 Invalid Address

**Message:**
```
invalid address '<address>'
```

**Example:**
```
.metadata
.sources
.functions
 0xFFFFFFFFF
```

```
AssemblerError: invalid address '0xFFFFFFFFF'
```

The specified address is not valid.

---

### 3.16 Invalid Line Number

**Message:**
```
invalid line number '<line_number>'
```

**Example:**
```
.metadata
.sources
.functions
.line_table
0x00000000 0x00000000 0 0a 
```

```
AssemblerError: invalid line number '0a'
```

The specified line number is not valid.

---

### 3.17 Invalid Column Number

**Message:**
```
invalid column number '<column number>'
```

**Example:**
```
.metadata
.sources
.functions
.line_table
0x00000000 0x00000000 0 0 0a 
```

```
AssemblerError: invalid column number '0a'
```

The specified column number is not valid.

---