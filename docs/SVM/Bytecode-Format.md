# Bytecode Format

## Contents

- [1. Overview](#1-overview)
- [2. Bytecode Layout](#2-bytecode-layout)
    - [2.1 Byte Sequence](#21-byte-sequence)
    - [2.2 Header](#22-header)
    - [2.3 Code Section](#23-code-section)
    - [2.4 Data Section](#24-data-section)
    - [2.5 Source Metadata Section](#25-source-metadata-section)
    - [2.6 Function Metadata Section](#26-function-metadata-section)
    - [2.7 Line Table Section](#27-line-table-section)
- [3. Addresses](#3-addresses)
    - [3.1 Code Addresses](#31-code-addresses)
    - [3.2 Data Addresses](#32-data-addresses)
    - [3.3 Method Addresses](#33-method-addresses)
    - [3.4 File Index to VM Address](#34-file-index-to-vm-address)
- [4. Value Encoding](#4-value-encoding)
    - [4.1 Byte Order](#41-byte-order)
    - [4.2 Integer Encoding](#42-integer-encoding)
    - [4.3 Floating Point Encoding](#43-floating-point-encoding)
    - [4.4 Pointer Encoding](#44-pointer-encoding)
    - [4.5 String Encoding](#45-string-encoding)
- [5. Instruction Encoding](#5-instruction-encoding)
    - [5.1 Opcode Encoding](#51-opcode-encoding)
    - [5.2 Operand Encoding](#52-operand-encoding)
    - [5.3 Type Encoding](#53-type-encoding)
    - [5.4 Label Encoding](#54-label-encoding)
    - [5.5 Immediate Values](#55-immediate-values)
    - [5.6 Native Reference Encoding](#56-native-reference-encoding)
- [6. Data Encoding](#6-data-encoding)
    - [6.1 Data Entries](#61-data-entries)
    - [6.2 Data Type Encoding](#62-data-type-encoding)
    - [6.3 Numeric Data](#63-numeric-data)
    - [6.4 String Data](#64-string-data)
- [7. Method Encoding](#7-method-encoding)
    - [7.1 Method Layout](#71-method-layout)
    - [7.2 Method Metadata](#72-method-metadata)
    - [7.3 Method Instructions](#73-method-instructions)
- [8. Metadata Encoding](#8-metadata-encoding)
  - [8.1 Metadata Overview](#81-metadata-overview)
  - [8.2 Source Metadata Entries](#82-source-metadata-entries)
  - [8.3 Function Metadata Entries](#83-function-metadata-entries)
  - [8.4 Line Table Entries](#84-line-table-entries)
- [9. Examples](#9-examples)
    - [9.1 Bytecode Layout](#91-bytecode-layout)
    - [9.2 Instruction Encoding](#92-instruction-encoding)
    - [9.3 Data Entry Encoding](#93-data-entry-encoding)
    - [9.4 String Encoding](#94-string-encoding)
    - [9.5 Method Encoding](#95-method-encoding)
    - [9.6 Code Address](#96-code-address)
    - [9.7 Data Address](#97-data-address)
    - [9.8 Method Address](#98-method-address)
    - [9.9 Complete Example](#99-complete-example)
    - [9.10 Complete Example With Metadata](#910-complete-example-with-metadata)

---

## 1. Overview

This document defines the binary format of `.svmb` bytecode files.

`.svmb` files contain bytecode produced by the SVA and executed by the SVM. 

---

## 2. Bytecode Layout

An `.svmb` file consists of a sequence of bytes containing:
- A header
- A code section
- A data section
- A source metadata section
- A function metadata section
- A line table section

The header is used when loading the bytecode, but it is not stored in VM memory.

### 2.1 Byte Sequence

Bytecode is represented as a contiguous sequence of bytes.

Each byte occupies a unique zero-based index within the bytecode.

The first byte has index `0`.

### 2.2 Header

The header is 20 bytes and is located at the beginning of the bytecode.

The header contains five 32-bit unsigned integer values:

| Offset | Size (bytes) | Description                                              |
|--------|--------------|----------------------------------------------------------|
| `0`    | `4`          | Index of the final byte in the code section              |
| `4`    | `4`          | Index of the final byte in the data section              |
| `8`    | `4`          | Index of the final byte in the source metadata section   |
| `12`   | `4`          | Index of the final byte in the function metadata section |
| `16`   | `4`          | Index of the final byte in the bytecode                  |                                                       

The total size of the bytecode is the value of the second field plus `1`.

### 2.3 Code Section

The code section begins immediately after the header.

The first byte of the code section is located at byte index `8`.

The last byte of the code section is specified by the first header field.

### 2.4 Data Section

The data section begins immediately after the code section.

The first byte of the data section is one byte after the final byte of the code section.

The last byte of the data section is specified by the second header field.

### 2.5 Source Metadata Section

The source metadata section begins immediately after the data section.

The first byte of the source metadata section is one byte after the final byte of the data section.

The last byte of the source metadata section is specified by the third header field.

### 2.6 Function Metadata Section

The function metadata section begins immediately after the source metadata section.

The first byte of the function metadata section is one byte of the final byte of the source metadata section.

The last byte of the function metadata section is specified by the fourth header field.

### 2.7 Line Table Section

The line table section begins immediately after the function metadata section.

The first byte of the line table section is one byte of the final byte of the function metadata section.

The last byte of the line table section is specified by the fifth header field.

---

## 3. Addresses

Addresses stored in bytecode represent address in SVM memory.

The bytecode header is not loaded into VM memory. Therefore, bytecode file index `20` corresponds to SVM memory address `0`.

For any bytecode file index `I`, the corresponding SVM memory address is:
`I - 20`

### 3.1 Code Addresses

A code address refers to the first byte of an instruction within the code section.

A code address is calculated from a bytecode file index by subtracting `20`.

For example, a byte at bytecode index `40` has VM memory address `20`.

### 3.2 Data Addresses

A data address refers to the first byte of the data in a data entry, not its data type.

A data address is calculated from the bytecode file index by subtracting `20`.

For example, if a data entry has its type at bytecode index `80` and its value begins at bytecode index `81`, the address of the data is `61`.

### 3.3 Method Addresses

A method address points to the first byte of the method metadata.

A method address is calculated from the bytecode file index by subtracting `20`.

For example, if a method's metadata starts at bytecode index `40`, the address of the method is `20`.

### 3.4 File Index to VM Address

Because the 8-byte header is not loaded into VM memory, file indexes and VM address differ by 20.

| Bytecode File Index | VM Memory index |
|---------------------|-----------------|
| `20`                | `0`             |
| `21`                | `1`             |
| `30`                | `10`            |
| `40`                | `20`            |
| `100`               | `80`            |

The relationship is:
`VM address = bytecode file index - 20`

---

## 4. Value Encoding

### 4.1 Byte Order

Multibyte values are encoded using little-endian.

The least significant byte is stored at the lowest byte index.

### 4.2 Integer Encoding

`i32` and `ui32` values are encoded using 4 bytes.

`i64` and `ui64` values are encoded using 8 bytes.

Signed integer values (`i32`, `i64`) use two's-complement representation.

Unsigned integer values and pointer values (`ui32`, `ui64`, `ptr`) use unsigned binary representation.

### 4.3 Floating Point Encoding

`f32` values are encoded using 4 bytes according to IEEE 754 binary32.

`f64` values are encoded using 8 bytes according to IEEE 754 binary64.

### 4.4 Pointer Encoding

Pointers are encoded as 32-bit unsigned values using 4 bytes.

A pointer represents an address in the SVM memory address space.

### 4.5 String Encoding

A string is encoded as a 32-bit unsigned integer followed by its UTF-8 encoded bytes.

The 32-bit integer specifies the number of bytes occupied by the UTF-8 encoded string.

| Offset | Size (bytes) | Description                         |
|--------|--------------|-------------------------------------|
| `0`    | `4`          | Number of UTF-8 encoded bytes (`N`) |
| `4`    | `N`          | UTF-8 encoded string                |


The stored length (`N`) represents the number of bytes, not the number of characters.

---

## 5. Instruction Encoding

Each instruction begins with a one-byte opcode.

Any operands required by the instruction are encoded immediately after the opcode in the order specified by the instruction.

Instructions may have no operands or multiple operands.

An instruction has the general form:
```
[opcode] [operand1] [operand2]
```

See `Instruction-Set.md` for more details regarding the operands of each instruction.

### 5.1 Opcode Encoding

Instructions are encoded as a single byte.

See `Instruction-Set.md` for the opcode assigned to each instruction.

### 5.2 Operand Encoding

Instruction operands use the following encodings

| Operand        | Size (bytes)               |
|----------------|----------------------------|
| `<type>`       | 1                          |
| `<label>`      | 4                          |
| `<immediate>`  | 4 or 8 (depending on type) |
| `<native_ref>` | 1                          |

Operands are encoded directly after the opcode or proceeding operand.

See `Instruction-Set.md` for the ordering of operands for each instruction.

### 5.3 Type Encoding

A type operand is encoded as a one-byte type identifier.

The type identifier determines the type of the value used by the instruction.

See `Instruction-Set.md` for the identifier assigned to each type.

### 5.4 Label Encoding

A label operand is encoded as a 32-bit unsigned value representing an address.

The label itself is not stored in the bytecode.

The encoded value is the address resolved for the label

### 5.5 Immediate Values

Immediate values are encoded using the number of bytes required by their type.

Immediate values are encoded using their raw binary representation in little-endian byte order.

There are some exceptions to the immediate size for an operand:
- `LoadL` and `storeL` immediate operand has length of 4 bytes

See `Instruction-Set.md` for the number of bytes for each immediate type.

### 5.6 Native Reference Encoding

A native reference is encoded as a 1-byte unsigned native function ID.

The native function ID identifies the VM-provided native function to invoke.

The native function name is not stored in the bytecode.

See `Instruction-Set.md` for the native function ID assigned to each native reference.

For Example:
```
native exit
```

Is encoded as:
```
0x11 0x00
```

---

## 6. Data Encoding

### 6.1 Data Entries

Each data entry consists of a one-byte data type identifier followed by the encoded data.

```
[data type] [data]
```

The size and encoding of the data depend on its data type.

Data entries are stored consecutively in the data section.

### 6.2 Data Type Encoding

The data type is encoded as a one-byte identifier.

See `Instruction-Set.md` for the identifier assigned to each data type.

### 6.3 Numeric Data

Numeric data is encoded according to the encoding defined for its type in section 4.

For example, an `i32` data entry consists of:
```
[data type] [4-byte i32 value]
```
An `f64` data entry consists of:
```
[data type] [8-byte f64 value]
```

### 6.4 String Data

A string data entry consists of:
```
[data type] [4-byte length] [UTF-8 bytes]
```

The length specifies the number of UTF-8 bytes following the length field.

A pointer referring to a string points to the first byte of the string's length field.

---

## 7. Method Encoding

A method consists of method metadata followed by the instructions belonging to the method.

### 7.1 Method Layout

A method's instructions immediately follow its metadata.

A method is encoded as:
```
[number of arguments] [number of locals] [instructions]
```

### 7.2 Method Metadata

Method metadata is encoded as:
```
[number of arguments] [number of locals]
```

| Field               | Size (bytes) |
|---------------------|--------------|
| Number of arguments | 1            |
| Number of Locals    | 4            |

The number of arguments determines how many arguments the method expects

The number of locals determines the number of local variable slots allocated for the method's stack frame.

### 7.3 Method Instructions

The instructions immediately follow the method metadata.

Instructions are encoded according to the rules defined in section 4.

The method's instruction sequence continues until the next method, data section, or the end of the bytecode.

---

## 8. Metadata Encoding

### 8.1 Metadata Overview

The metadata contained in an `.svmb` file allows the SVM to associate VM addresses with source files, functions, and source locations.

The metadata information consists of three sections:
- Source metadata
- Function metadata
- Line table

This metadata is used by the SVM to produce source-level runtime errors and stack traces.

This metadata does not affect the execution of instructions.

### 8.2 Source Metadata Entries

A source metadata entry identifies a source file.

Each source entry is encoded as:
```
[source ID] [path length] [path]
```

| Field                | Size (bytes) |
|----------------------|--------------|
| Source ID            | `2`          |
| Absolute path length | `4`          |
| Absolute path        | `N`          |

The source ID uniquely identifies the source file within the bytecode file.

The path length specified the number of bytes in the absolute path string.

The absolute path is stored as a string of `N` bytes.

The source metadata section contains zero or more source entries.

### 8.3 Function Metadata Entries

A function metadata entry identifies the source-level function corresponding to a range of VM addresses.

Each function entry is encoded as:
```
[start address] [end address] [source ID] [function name length] [function name]
```

| Field                | Size (bytes) |
|----------------------|--------------|
| Start address        | `4`          |
| End address          | `4`          |
| Source ID            | `2`          |
| Function name length | `4`          |
| Function name        | `N`          |

The start address identifiers the first VM address belonging to the function.

The end address identifies the final VM address belonging to the function.

The source ID identifies the source file containing the function.

The function name length specifies the number of bytes in the function name.

The function name is stored as a string of `N` bytes.

### 8.4 Line Table Entries

A line entry associates a range of VM addresses with a source location.

Each entry is encoded as:
```
[start address] [end address] [source ID] [line number] [column number]
```

| Field         | Size (bytes) |
|---------------|--------------|
| Start address | `4`          |
| End address   | `4`          |
| Source ID     | `2`          |
| Line number   | `4`          |
| Column number | `2`          |

The start and end addresses identify the range of VM addresses associated with the source location.

The source ID identifies the source file.

THe line number and column number identify the source location.

The SVM uses this information to report the source location of runtime errors and the call sites contained in stack traces.

---

## 9. Examples

### 9.1 Bytecode Layout

For a bytecode file with:
- Code section ending with byte index `63`
- Bytecode ending at byte index `95`

```
0                19 20                       63 64                 95
+------------------+---------------------------+--------------------+
|      Header      |       Code Section        |    Data Section    |
+------------------+---------------------------+--------------------+
```

The first header field contains `63`.

The second header field contains `95`.

the third fourth and fifth header fields are equal to the second field when there is nothing in the metadata sections.

### 9.2 Instruction Encoding

An instruction containing an opcode, type operand, and immediate value has the following layout:
```
[opcode] [type] [immediate]
```

For example:
```
push i32 #10
```

Is encoded as:
```
0x02 0x00 0x0A 0x00 0x00 0x00
```

### 9.3 Data Entry Encoding

An `i32` data entry containing the value `5` has the following layout:
```
[data type] [4-byte i32 value]
```

This is encoded as:
```
0x00 0x05 0x00 0x00 0x00
```

### 9.4 String Encoding

A string containing `Hello` has the following layout:
```
[data type] [4-byte length] [UTF-8 bytes]
```

This is encoded as:
```
0x07 0x05 0x00 0x00 0x00 0x48 0x65 0x6C 0x6C 0x6F
```

### 9.5 Method Encoding

A method has the following layout:
```
[number of arguments (1 byte)] [number of locals (4 bytes)] [method's instructions]
```

A method with 2 arguments and 3 local variables with 1 instruction `ret` is encoded as:
```
0x02 0x03 0x00 0x00 0x00 0x12
```

### 9.6 Code Address

A code address is calculated from a bytecode file index by subtracting `8`.

The `jmp` instruction has the following layout
```
[0x13] [4-byte address]
```

The following assembly code:
```
$loop:
    jmp $loop
```

Is encoded as:
```
VM Address: Bytecode Index: Bytecode
    
0x00: 08: 0x13    ; jmp
0x01: 09: 0x00    ; $loop
0x02: 10: 0x00
0x03: 11: 0x00
0x04: 12: 0x00
```

### 9.7 Data Address

A data address is calculated from the bytecode file index that refers to the first byte of data by subtracting `8`.

The `loadG` instruction has the following layout:
```
[0x08] [4-byte address]
```

The following assembly code:
```
loadG $x
loadG $y

.data
$x: i32 5
$y: i64 20
```

Is encoded as:
```
VM Address: Bytecode Index: Bytecode
    
0x00: 08: 0x08    ; loadG
0x01: 09: 0x0B    ; $x
0x02: 10: 0x00
0x03: 11: 0x00
0x04: 12: 0x00
0x05: 13: 0x08    ; loadG
0x06: 14: 0x10    ; $y
0x07: 15: 0x00
0x08: 16: 0x00
0x09: 17: 0x00
0x0A: 18: 0x00    ; i32    
0x0B: 19: 0x05    ; 5    
0x0C: 20: 0x00
0x0D: 21: 0x00
0x0E: 22: 0x00
0x0F: 23: 0x02    ; i64
0x10: 24: 0x14    ; 20
0x11: 25: 0x00
0x12: 26: 0x00        
0x13: 27: 0x00       
0x14: 28: 0x00
0x15: 29: 0x00
0x16: 30: 0x00
0x17: 31: 0x00    
```

### 9.8 Method Address

A method address is calculated from the bytecode file index by subtracting `8`.

The `call` instruction has the following layout:
```
[0x11] [4-byte address]
```

The following assembly code:
```
call $foo

def $foo:
    args 0
    locals 2
    
    ret
```

Is encoded as:
```
VM Address: Bytecode Index: Bytecode
    
0x00: 08: 0x11    ; call
0x01: 09: 0x05    ; $foo
0x02: 10: 0x00
0x03: 11: 0x00
0x04: 12: 0x00
0x05: 13: 0x00    ; args 0
0x06: 14: 0x02    ; locals 2
0x07: 15: 0x00
0x08: 16: 0x00
0x09: 17: 0x00
0x0A: 18: 0x12    ; ret
```

### 9.9 Complete Example

The following assembly code:
```
    loadG $x
    push i64 #10
    call $add
    push ptr $msg
    native print_str
    halt

def $add:
    args 2
    locals 0
    
    loadL i64 #1
    loadL i64 #2
    add
    ret
    
.data
$x: i64 200
$msg: str "Hello"
```

Is encoded as:
```

VM Address: Bytecode Index: Bytecode

      00: 0x43    ; header field one (end of code section)
      01: 0x00
      02: 0x00
      03: 0x00
      04: 0x56    ; header field two (end of data section)
      05: 0x00
      06: 0x00
      07: 0x00
      08: 0x56    ; header field two (end of source metadata section)
      09: 0x00
      10: 0x00
      11: 0x00
      12: 0x56    ; header field two (end of function metadata section)
      13: 0x00
      14: 0x00
      15: 0x00
      16: 0x56    ; header field two (bytecode end)
      17: 0x00
      18: 0x00
      19: 0x00
0x00: 20: 0x08    ; loadG
0x01: 21: 0x31    ; $x
0x02: 22: 0x00
0x03: 23: 0x00
0x04: 24: 0x00
0x05: 25: 0x02    ; push
0x06: 26: 0x02    ; i64
0x07: 27: 0x10    ; #10
0x08: 28: 0x00
0x09: 29: 0x00
0x0A: 30: 0x00
0x0B: 31: 0x00
0x0C: 32: 0x00
0x0D: 33: 0x00
0x0E: 34: 0x00
0x0F: 35: 0x11    ; call
0x10: 36: 0x1D    ; $add
0x11: 37: 0x00
0x12: 38: 0x00
0x13: 39: 0x00
0x14: 40: 0x02    ; push
0x15: 41: 0x06    ; str
0x16: 42: 0x3A    ; $msg
0x17: 43: 0x00
0x18: 44: 0x00
0x19: 45: 0x00
0x1A: 46: 0x10    ; native
0x1B: 47: 0x03    ; print_str
0x1C: 48: 0x01    ; halt
0x1D: 49: 0x02    ; args 2
0x1E: 50: 0x00    ; locals 0
0x1F: 51: 0x00
0x20: 52: 0x00
0x21: 53: 0x00
0x22: 54: 0x09    ; loadL
0x23: 55: 0x02    ; i64
0x24: 56: 0x01    ; #1
0x25: 57: 0x00
0x26: 58: 0x00
0x27: 59: 0x00
0x28: 60: 0x09    ; loadL
0x29: 61: 0x02    ; i64
0x2A: 62: 0x02    ; #2
0x2B: 63: 0x00 
0x2C: 64: 0x00
0x2D: 65: 0x00
0x2E: 66: 0x16    ; add
0x2F: 67: 0x12    ; ret
0x30: 68: 0x02    ; i64
0x31: 69: 0xC8    ; #200
0x32: 70: 0x00 
0x33: 71: 0x00
0x34: 72: 0x00
0x35: 73: 0x00 
0x36: 74: 0x00
0x37: 75: 0x00
0x38: 76: 0x00
0x39: 77: 0x07    ; str
0x3A: 78: 0x05    ; length of str
0x3B: 79: 0x00
0x3C: 80: 0x00
0x3D: 81: 0x00
0x3E: 82: 0x48    ; 'H'
0x3F: 83: 0x65    ; 'e'
0x40: 84: 0x6C    ; 'l'
0x41: 85: 0x6C    ; 'l'
0x42: 86: 0x6F    ; 'o'

```

### 9.10 Complete Example With Metadata

The following assembly code:
```
call $push_5:
halt

def $push_5: 
    args 0
    locals 0
    
    push f32 #5.0
    ret
    
.metadata

    .sources
            0    "main.sv"

    .functions
        ;     start          end       source    name
        0x00000006    0x00000011        0        "push_5"

    .line_table
        ;     start          end       source       line       column
        0x00000000    0x00000004        0             1       14
        0x00000004    0x00000005        0             2        1   
 
```

Is encoded as:
```

VM Address: Bytecode Index: Bytecode

      00: 0x25    ; header field one (end of code section)
      01: 0x00
      02: 0x00
      03: 0x00
      04: 0x25    ; header field two (end of data section)
      05: 0x00
      06: 0x00
      07: 0x00
      08: 0x32    ; header field two (end of source metadata section)
      09: 0x00
      10: 0x00
      11: 0x00
      12: 0x46    ; header field two (end of function metadata section)
      13: 0x00
      14: 0x00
      15: 0x00
      16: 0x66    ; header field two (bytecode end)
      17: 0x00
      18: 0x00
      19: 0x00
0x00: 20: 0x11    ; call
0x01: 21: 0x06    ; $push_5
0x02: 22: 0x00
0x03: 23: 0x00
0x04: 24: 0x00
0x05: 25: 0x01    ; halt
0x06: 26: 0x00    ; args 0
0x07: 27: 0x00    ; locals 0
0x08: 28: 0x00
0x09: 29: 0x00
0x0A: 30: 0x00
0x0B: 31: 0x02    ; push
0x0C: 32: 0x04    ; f32
0x0D: 33: 0x00    ; #5.0
0x0E: 34: 0x00    
0x0F: 35: 0xA0
0x10: 36: 0x40
0x11: 37: 0x12    ; ret 
0x12: 38: 0x00    ; source.sourceId
0x13: 39: 0x00
0x14: 40: 0x07    ; length of source.path
0x15: 41: 0x00
0x16: 42: 0x00
0x17: 43: 0x00
0x18: 44: 0x6D    ; 'm' ; source.path
0x19: 45: 0x61    ; 'a'
0x1A: 46: 0x69    ; 'i'
0x1B: 47: 0x6E    ; 'n'
0x1C: 48: 0x2E    ; '.'
0x1D: 49: 0x73    ; 's'
0x1E: 50: 0x76    ; 'v'
0x1F: 51: 0x00    ; function.startAddress
0x20: 52: 0x00
0x21: 53: 0x00
0x22: 54: 0x00
0x23: 55: 0x04    ; function.endAddress
0x24: 56: 0x00
0x25: 57: 0x00
0x26: 58: 0x00
0x27: 59: 0x00    ; function.sourceId
0x28: 60: 0x00
0x29: 61: 0x06    ; length of function.name
0x2A: 62: 0x00
0x2B: 63: 0x00
0x2C: 64: 0x00
0x2D: 65: 0x70    ; 'p' ; function.name
0x2E: 66: 0x75    ; 'u'
0x2F: 67: 0x73    ; 's'
0x30: 68: 0x68    ; 'h'
0x31: 69: 0x5F    ; '_'
0x32: 70: 0x35    ; '5'
0x33: 71: 0x00    ; line_table1.startAddress
0x34: 72: 0x00
0x35: 73: 0x00
0x36: 74: 0x00
0x37: 75: 0x04    ; line_table1.endAddress
0x38: 76: 0x00
0x39: 77: 0x00
0x3A: 78: 0x00
0x3B: 79: 0x00    ; line_table1.sourceId
0x3C: 80: 0x00
0x3D: 81: 0x01    ; line_table1.lineNumber
0x3E: 82: 0x00
0x3F: 83: 0x00
0x40: 84: 0x00
0x41: 85: 0x0E    ; line_table1.columnNumber
0x42: 86: 0x00
0x43: 87: 0x04    ; line_table2.startAddress
0x44: 88: 0x00
0x45: 89: 0x00
0x46: 90: 0x00
0x47: 91: 0x05    ; line_table2.endAddress
0x48: 92: 0x00
0x49: 93: 0x00
0x4A: 94: 0x00
0x4B: 95: 0x00    ; line_table2.sourceId
0x4C: 96: 0x00
0x4D: 97: 0x02    ; line_table2.lineNumber
0x4E: 98: 0x00
0x4F: 99: 0x00
0x51: 100: 0x00
0x52: 101: 0x01    ; line_table2.columnNumber
0x53: 102: 0x00

```