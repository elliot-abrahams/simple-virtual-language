# Assembly Generation

## Contents

- [1. Overview](#1-overview)
- [2 Assembly Notation](#2-assembly-notation)
- [3. Program](#3-program)
- [4. Types](#4-types)
  - [4.1 Language Types](#41-language-types)
  - [4.2 Default Values](#42-default-values)
  - [4.3 Array Representation](#43-array-representation)
    - [4.3.1 Array Allocation size Calculation](#431-array-allocation-size-calculation) 
    - [4.3.2 Array Element Address Calculation](#432-array-element-address-calculation)
    - [4.3.3 Array Index](#433-array-index)
  - [4.4 Implicit Conversion](#44-implicit-conversion)
- [5. Expressions](#5-expressions)
  - [5.1 Literals](#51-literals)
    - [5.1.1 Integer Literal](#511-integer-literal)
    - [5.1.2 Float Literal](#512-float-literal)
    - [5.1.3 Bool Literal](#513-bool-literal)
    - [5.1.4 Char Literal](#514-char-literal)
  - [5.2 Variable Access](#52-variable-access)
    - [5.2.1 Global Variable Access](#521-global-variable-access)
    - [5.2.2 Local Variable Access](#522-local-variable-access)
  - [5.3 Arithmetic Expressions](#53-arithmetic-expressions)
    - [5.3.1 Addition](#531-addition)
    - [5.3.2 Subtraction](#532-subtraction)
    - [5.3.3 Multiplication](#533-multiplication)
    - [5.3.4 Division](#534-division)
    - [5.3.5 Modulo](#535-modulo)
  - [5.4 Integer Division](#54-integer-division)
    - [5.5 Bitwise Expressions](#55-bitwise-expressions)
  - [5.6 Comparison Expressions](#56-comparison-expressions)
  - [5.7 Logical Expressions](#57-logical-expressions)
    - [5.7.1 Logical AND](#571-logical-and)
    - [5.7.2 Logical OR](#572-logical-or)
  - [5.8 Unary Expressions](#58-unary-expressions)
    - [5.8.1 Positive Sign](#581-positive-sign)
    - [5.8.2 Negative Sign](#582-negative-sign)
    - [5.8.3 Logical NOT](#583-logical-not)
    - [5.8.4 Bitwise NOT](#584-bitwise-not)
    - [5.8.5 Prefix Increment](#585-prefix-increment)
    - [5.8.6 Prefix Decrement](#586-prefix-decrement)
  - [5.9 Postfix Expression](#59-postfix-expressions)
    - [5.9.1 Array Access](#591-array-access)
    - [3.9.2 Field Access](#592-field-access)
    - [5.9.3 Increment and Decrement](#593-increment-and-decrement)
  - [5.10 Function Calls](#510-function-calls)
  - [5.11 Cast Expressions](#511-cast-expressions)
  - [5.12 New Expressions](#512-new-expressions)
    - [5.12.1 Array Dimension](#5121-array-dimension)
    - [5.12.2 One Dimensional Array Allocation](#5122-one-dimensional-array-allocation)
    - [5.12.3 Multidimensional Array Allocation](#5123-multidimensional-array-allocation)
    - [5.12.4 Array Initialiser](#5124-array-initialiser)
- [6. Statements](#6-statements)
  - [6.1 Variable Declaration](#61-variable-declaration)
    - [6.1.1 Global Variable Declaration](#611-global-variable-declaration)
    - [6.1.2 Local Variable Declaration](#612-local-variable-declaration)
  - [6.2 Variable Assignment](#62-variable-assignment)
    - [6.2.1 Global Variable Assignment](#621-global-variable-assignment)
    - [6.2.2 Local Variable Assignment](#622-local-variable-assignment)
    - [6.2.3 Global Array Element Assignment](#623-global-array-element-assignment)
    - [6.2.4 Local Array Element Assignment](#624-local-array-element-assignment)
  - [6.3 Block](#63-block)
  - [6.4 Conditional Statements](#64-conditional-statements)
    - [6.4.1 If Statement](#641-if-statement)
    - [6.4.2 If-Else Statement](#642-if-else-statement)
    - [6.4.3 Else-If Statement](#643-else-if-statement)
  - [6.5 While Loop](#65-while-loop)
  - [6.6 For Loop](#66-for-loop)
    - [6.6.1 Range-Based For Loop]()
    - [6.6.2 Range-Based For Loop With Explicit Step]()
    - [6.6.3 Iterable-Based For Loop]()
  - [6.7 Break Statement](#67-break-statement)
  - [6.8 Continue Statement](#68-continue-statement)
  - [6.9 Return Statement](#69-return-statement)
    - [6.9.1 Return Statement With Expression](#691-return-with-expression)
    - [6.9.2 Return Statement Without Expression](#692-return-without-expression)
  - [6.10 Expression Statement](#610-expression-statement)
    - [6.10.1 Function Call](#6101-function-call)
    - [6.10.2 Increment and Decrement](#6102-increment-and-decrement)
- [7. Functions](#7-functions)
  - [7.1 Function Declaration](#71-function-declaration)
  - [7.2 Function Labels](#72-function-labels)
  - [7.3 Void Function Default Return](#73-void-function-default-return)
  - [7.4 Scope Functions](#74-scope-functions)
- [8. Builtins](#8-builtins)
  - [8.1 Builtin Functions](#81-builtin-functions)
  - [8.2 Builtin Data](#82-builtin-data)
- [9. Assembly Generation Order](#9-assembly-generation-order)
- [10. Metadata](#10-metadata)
  - [10.1 Source Metadata Entry](#101-source-metadata-entry)
  - [10.2 Function Metadata Entry](#102-function-metadata-entry)
  - [10.3 Line Table Entry](#103-line-table-entry)

---

## 1. Overview

The assembly generator translates the semantically analysed abstract syntax tree (AST) into an intermediate representation (IR) of the SVMA assembly.

Semantic analysis determines types, symbols, scopes, functions, and other information required by assembly generation.

The assembly IR is subsequently converted into textual SVMA assembly by the assembly emitter. The generated assembly can then be assembled by the SVA.

The assembly generator also records source locations for generated assembly IR. These locations are used by the assembly emitter to generate source, function, and line table metadata.

The generated textual assembly uses the following indentation:
- Instructions are indented by 8 spaces
- Label definitions are indented by 4 spaces
- Method definitions are not indented
- Method metadata is indented by 8 spaces
- Data declarations are indented by 4 spaces
- `.data` and `.metadata` directives are not indented
- `.sources`, `.functions`, and `.line_table` directives are indented by 4 spaces
- All metadata entries are indented by 8 spaces

---

## 2. Assembly Notation

This document uses the following notation when describing generated assembly.

**Generated Assembly**

Double curly brackets `{{ ... }}` represent assembly generated by another compiler operation.

For example:

```
    {{expression}}
    conv f32
```

`{{expression}}` represents the assembly generated for the expression. It is a reference to another assembly-generation operation and is not literal SVMA syntax.

When a reference operation is documented elsewhere, its section is linked explicitly.

For example:
```
    {{Array Index}}
```

Refers to the assembly described in [4.3.3 Array Index](#433-array-index).

**Substituted Values**

Square brackets `[ ... ]` represent values substituted into an assembly template.

For example:

```
    push i32 #[value]
    loadL [type] #[slot]
```

Here:
- `[value]` is replaced by the literal value being pushed
- `[type]` is replaced by the required assembly type
- `[slot]` is replaced by the local variable's slot

For numeric values, the value inside the brackets is represented according to the specified assembly type.

For example:

```
push [type] #[1]
```

Produces:
- `push [type] #1` when `[type]` is `i32`, `ui32`, `i64`, or `ui64`.
- `push [type] #1.0` when `[type]` is `f32` or `f64`.

Square brackets therefore do not represent generated assembly.

**Operand Stack**

Angle brackets `< ... >` represent the operand stack.

Values are ordered from the bottom of the stack to the top, with the rightmost value being the top of the stack.

For example:

```
<array_ptr: ptr, index: ui32>
```

The type of each value is written after a colon.

**Stack Annotations**

Stack states are included only where they clarify an operation, algorithm, loop invariant, or important transition.

They are written as assembly comments:

```
    ; Stack: <array_ptr: ptr> 
    
    {{index expression}} 
    ; Stack: <array_ptr: ptr, index: i32>
```

Stack annotations are explanatory documentation and not part of the generated assembly.

Simple instruction sequences do not require stack annotations when their behaviour is already clear. Complex operations may also show several stack states to explain their implementation.

---

## 3. Program

The generated assembly consists of the global statements, generated scope functions, user-defined functions, builtin functions, and static data.

```
    {{global statements}}
        halt
        
{{scope functions}}

{{user-defined functions}}

{{builtin functions}}
        
.data
    {{global variable declarations}}      
    {{builtin data declarations}} 
    
.metadata

    .sources
        {{source metadata entries}}
        
    .functions
        ;     start          end       source    name
        {{function metadata entries}}
        
    .line_table
        ;     start          end       source       line       column
        {{line table entries}} 
   
```

Global statements are emitted before `halt`.

A block directly contained by the global scope is emitted as a scope function. Scope functions are emitted after the `halt` instruction.

User-defined functions and required built-in functions are emitted after the scope functions.

Static data is emitted under the `.data` directive.

Metadata is emitted under the `.metadata` directive.

---

## 4. Types

### 4.1 Language Types

Language types are represented by the following assembly types:

| Language Type | Assembly Type | Size (bytes) |
|---------------|---------------|--------------|
| `int`         | `i32`         | 4            |
| `float`       | `f32`         | 4            |
| `bool`        | `ui32`        | 4            |
| Array         | `ptr`         | 4            |

---

### 4.2 Default Values

| Assembly Type | Default Value |
|---------------|---------------|
| `i32`         | `0`           |
| `f32`         | `0.0`         |
| `ui32`        | `0`           |
| `ptr`         | `0`           |

---

### 4.3 Array Representation

An array is represented by a pointer to a heap allocation.

The first 4 bytes of an array allocation store the array length. The elements are stored immediately after the length field.

For an array containing `N` elements:

```
[array length (N)]
[element 0]
[elemnt 1]
...
[element N - 1]
```

The array length is stored as `ui32`.


The total allocation size is:

```
allocation size = 4 + (length * element size)
```

The 4 bytes length field is used for runtime bounds checking.

For multidimensional arrays, elements of an outer array are pointers to nested arrays. Each dimension is therefore represented by its own array allocation.

#### 4.3.1 Array Allocation Size Calculation

The total number of bytes required for an array allocation is calculated as:

```
allocation size = 4 + (length * element size)
```

The generated assembly has the following structure:

```
    ; Stack: <length: ui32>
    
    push ui32 #[element_size]
    ; Stack: <length: ui32, element_size: ui32>
    
    mul
    ; Stack: <length * element_size: ui32>
    
    push ui32 #4
    ; Stack: <length * element_size: ui32, 4: ui32>
    
    add
    ; Stack: <allocation_size: ui32>
```

Where
- `[element_size]` is the size in bytes of one element.

For a one-dimensional array of primitive values, the element size is the size of the corresponding primitive assembly type.

For an outer dimension of a multidimensional array, the element size is the size of the `ptr` assembly type.

#### 4.3.2 Array Element Address Calculation

The address of an array element is calculated as:
```
element address = array pointer + 4 + (index * element size)
```

The 4-byte offset skips the array length field.

The generated assembly is:

```
    ; Stack: <array_ptr: ptr, index: ui32>

    push ui32 #[element size] 
    mul
    push ui32 #4
    add 
    add
    
    ; Stack: <array_ptr: ptr>
```

The first `add` produces the byte offset from the beginning of the array allocation.

The second `add` adds this offset to the array pointer.

For multidimensional arrays, the resulting element address is used to load the pointer to the next nested array before processing the next index.

#### 4.3.3 Array Index

`Array Index` performs the runtime bounds checking and address calculation for one array index.

The operation:

1. Loads the array length.
2. Evaluates the index expression.
3. Checks that the index is non-negative.
4. Converts the index to `ui32`.
5. Checks that the index is less than the array length.
6. Calculates the address of the selected element.

The index expression has type `int` after semantic analysis.

The generated assembly is:

```
    ; Stack: <array_ptr: ptr>
    
    dup #0
    load ui32   
    ; Stack: <array_ptr: ptr, array_length: ui32>
    
    {{index expression}}
    ; Stack: <array_ptr: ptr, array_length: ui32, index: i32>
        
    dup #0
    push i32 #0
    clt
    ; Stack: <array_ptr: ptr, array_length: ui32, index: i32, index < 0: i32>
    
    jnz $array_index_out_of_range_[N0]
    ; Stack: <array_ptr: ptr, array_length: ui32, index: i32>
        
    conv ui32
    dup #1
    dup #1    
    cle
    ; Stack: <array_ptr: ptr, array_length: ui32, index: ui32, array_length <= index: ui32>
    
    jnz $array_index_out_of_range_[N0]
    ; Stack: <array_ptr: ptr, array_length: ui32, index: ui32>
    
    jmp $array_index_in_range_[N1]
    ; Stack: <array_ptr: ptr, array_length: ui32, index: ui32>
    
$array_index_out_of_range_[N0]:
    ; Stack: <array_ptr: ptr, array_length: ui32, index: ui32>
    
    throw array_index_out_of_range
    ; Stack: <array_ptr: ptr>
    
$array_index_in_range_[N1]:
    ; Stack: <array_ptr: ptr, array_length: ui32, index: ui32>
    
    swap    
    pop
    ; Stack: <array_ptr: ptr, index: ui32>
        
    {{Array Element Address Calculation}}
    ; Stack: <array_ptr: ptr>
```

Where:
- `[N0]` and `[N1]` are unique label numbers.

The conversion to `ui32` occurs only after the negative-value check so that a negative index cannot be converted into a large unsigned value before its validity is checked.

---

### 4.4 Implicit Conversion

When a value must be converted to another assembly type, the `conv` instruction is generated.

```
    ; Stack: <value: t>

    conv [target_type]
    ; Stack: <value: target_type>
```

If the value already has the target type, no conversion instruction is generated.

---

## 5. Expressions

Expressions normally produce their resulting value on the operand stack.

Some expressions may instead be generated to produce the address of their resulting value when the address is required by the surrounding operation. This allows operations such as increment and decrement to modify the value in memory.

### 5.1 Literals

#### 5.1.1 Integer Literal

An integer literal is pushed onto the operand stack as an `i32`.

```
    push i32 #[value]
    ; Stack: <[value]: i32>
```

Where:
- `[value]` is the integer value represented by the literal.

#### 5.1.2 Float Literal

A float literal is pushed onto the operand stack as an `f32`.

```
    push f32 #[value]
    ; Stack: <[value]: f32>
```

Where:
- `[value]` is the floating-point value represented by the literal.

#### 5.1.3 Bool Literal

A bool literal is pushed onto the operand stack as an `ui32`.

```
    push ui32 #[value]
    ; Stack: <[value]: ui32>
```

Where:
- `[value]` is `1` when the bool literal is `true`.
- `[value]` is `0` when the bool literal is `false`.

#### 5.1.4 Char Literal

A char literal is pushed onto the operand stack as an `ui32`.

```
    push ui32 #[value]
    ; Stack: <[value]: ui32>
```

Where:
- `[value]` is the Unicode code point of the char literal.

---

### 5.2 Variable Access

A variable access expression normally produces the value stored in the referenced variable.

When the variable is used as the target of an operation that modifies its value, the variable access produces the address of the variable instead.

For example:

```
int x = 5;
int y = x++;   // the location of x is required to increment the variable
```

#### 5.2.1 Global Variable Access

A global variable is loaded using `loadG` when its value is required.

```
    loadG $[identifier]
```

When the address of a global variable is required, its address is pushed using `push`.

```
    push ptr $[identifier]
```

Where:
- `[identifier]` is the identifier of the global variable.



The value loaded onto the operand stack has the assembly type of the variable.

#### 5.2.2 Local Variable Access

A local variable is loaded using `loadL` when its value is required.

```
    loadL [type] #[slot]
```

When the address of a local variable is required, it is produced using `addrL`.

```
    addrL #[slot]
```

Where:
- `[type]` is the assembly type of the local variable.
- `[slot]` is the local variable's slot.

---

### 5.3 Arithmetic Expressions

The arithmetic operators are generated using the following instructions:

| Operator | Instruction |
|----------|-------------|
| `+`      | `add`       |
| `-`      | `sub`       |
| `*`      | `mul`       |
| `/`      | `div`       |
| `%`      | `mod`       |

The operands are evaluated from left to right

Each operand is implicitly converted to the resulting type of the arithmetic expression when required.

#### 5.3.1 Addition

```
    {{Left Expression}}    
    {{Left Implicit Conversion}}
    
    {{right Expression}}    
    {{right Implicit Conversion}}
    
    add
```

#### 5.3.2 Subtraction

```
    {{Left Expression}}    
    {{Left Implicit Conversion}}
    
    {{right Expression}}    
    {{right Implicit Conversion}}
    
    sub
```

#### 5.3.3 Multiplication

```
    {{Left Expression}}    
    {{Left Implicit Conversion}}
    
    {{right Expression}}    
    {{right Implicit Conversion}}
    
    mul
```

#### 5.3.4 Division

```
    {{Left Expression}}    
    {{Left Implicit Conversion}}
    
    {{right Expression}}    
    {{right Implicit Conversion}}
    
    div
```

#### 5.3.5 Modulo

```
    {{Left Expression}}    
    {{Left Implicit Conversion}}
    
    {{right Expression}}    
    {{right Implicit Conversion}}
    
    mod
```

---

### 5.4 Integer Division

Integer division uses the `div` instruction

If both operands are integers, the division is performed using `i32`.

If either operand is a `float`, both operands are converted to `f32` before the division. The resulting `f32` value is then converted to `i32`.

```
    {{Left Expression}}    
    {{Left Implicit Conversion}}
    
    {{right Expression}}    
    {{right Implicit Conversion}}
    
    div
    
    {{Result Implicit Conversion}}
```

The result is always an `i32`.

---

### 5.5 Bitwise Expressions

Bitwise expressions use the following instructions:

| Operator | Instruction |
|----------|-------------|
| `&`      | `and`       |
| `\|`     | `orr`       |
| `^`      | `xor`       |
| `<<`     | `shl`       |
| `>>>`    | `shr`       |
| `>>`     | `sar`       |

A bitwise expression is generated as:

```
    {{Left Expression}}    
    {{Left Implicit Conversion}}
    
    {{right Expression}}    
    {{right Implicit Conversion}}
    
    {{Bitwise Expression Instruction}}
```

The result is always an `i32`.

---

### 5.6 Comparison Expressions

Comparison expressions use the following instructions:

| Operator | Instruction |
|----------|-------------|
| `==`     | `ceq`       |
| `!=`     | `cne`       |
| `<`      | `clt`       |
| `<=`     | `cle`       |
| `>`      | `cgt`       |
| `>=`     | `cge`       |

A comparison is generated as:

```
    {{Left Expression}}    
    {{Left Implicit Conversion}}
    
    {{right Expression}}    
    {{right Implicit Conversion}}
    
    {{Comparison Instruction}}
```

The comparison instruction leaves a `ui32` boolean result on the operand stack.

If either operand is a `float`, both operands are converted to `f32`. Otherwise, the operands use their common integer type.

---

### 5.7 Logical Expressions

Logical expressions use conditional jumps to implement short-circuit evaluation.

#### 5.7.1 Logical AND

The right operand is evaluated only when the left operand evaluated to true.

```
    {{Left Expression}}
    
    jez $evaluate_to_false_[N0]
    
    {{Right Expression}}
    
    jez $evaluate_to_false_[N0]
    
    push ui32 #1    
    jmp $end_and_[N1]
    
$evaluate_to_false_[N0]:
    push ui32 #0
    
$end_and_[N1]:
```

Both branches leave a `ui32` result on the operand stack.

Where:
- `[N0]` and `[N1]` are unique label numbers.

#### 5.7.2 Logical OR

The right operand is evaluated only when the left operand evaluates to false.

```
    {{Left Expression}}

    jnz $evaluate_to_true_[N0]
    
    {{Right Expression}}
    
    jnz $evaluate_to_true_[N0]
    
    push ui32 #0    
    jmp $end_or_[N1]
    
$evaluate_to_true_[N0]:
    push ui32 #1
    
$end_or_[N1]:
```

Both branches leave a `ui32` result on the operand stack.

Where:
- `[N0]` and `[N1]` are unique label numbers.

---

### 5.8 Unary Expressions

#### 5.8.1 Positive Sign

The positive sign does not modify the value of its operand.

```
    {{Expression}}
```

No additional instruction is generated.

#### 5.8.2 Negative Sign

A negative sign is generated by subtracting the operand from zero

```
    push [type] #[0]
    {{Expression}}
    sub
```

Where:
- `[type]` is the resulting assembly type of the expression.

#### 5.8.3 Logical NOT

Logical NOT produces `true` when its operand is false and `false` when its operand is true.

```
    {{Expression}}
    
    jez $evaluate_to_true_[N0]
    
    push ui32 #0
    jmp $end_not_[N1]
    
$evalate_to_true_[N0]:
    push ui32 #1
    
$end_not_[N1]:
```

Where:
- `[N0]` and `[N1]` are unique label numbers.

#### 5.8.4 Bitwise NOT

A bitwise not is generated as:

```
    {{Expression}}    
    {{Implicit Conversion}}
    
    not
```

The result is always an `i32`.

#### 5.8.5 Prefix Increment

The prefix increment operator requires the address of the value being modified.

The base expression is therefore generated to produce its address rather than its value. The current value is then loaded from the address, incremented, and stored back at the same address.

The prefix increment operator is generated as:

```
    ; Stack: <ptr: ptr>
    
    dup #0
    load [type]
    ; Stack: <ptr: ptr, val: type>
    
    push [type] #[1]
    add
    ; Stack: <ptr: ptr, val + 1: type>
    
    dup #0
    rotU #3
    ; Stack: <val + 1: type, ptr: ptr, val + 1: type>
    
    store
    ; Stack: <val + 1: type>
```

Where:
- `[type]` is the assembly type of the base expression.

The updated value remains on the operand stack because prefix increment produces the value after the update.

#### 5.8.6 Prefix Decrement

The prefix decrement operator requires the address of the value being modified.

The base expression is therefore generated to produce its address rather than its value. The current value is then loaded from the address, decremented, and stored back at the same address.

The prefix decrement operator is generated as:

```
    ; Stack: <ptr: ptr>
    
    dup #0
    load [type]
    ; Stack: <ptr: ptr, val: type>
    
    push [type] #[1]
    sub
    ; Stack: <ptr: ptr, val - 1: type>
    
    dup #0
    rotU #3
    ; Stack: <val - 1: type, ptr: ptr, val - 1: type>
    
    store
    ; Stack: <val - 1: type>
```

Where:
- `[type]` is the assembly type of the base expression.

The updated value remains on the operand stack because prefix decrement produces the value after the update.

---

### 5.9 Postfix Expressions

A postfix expression evaluates its base expression first, followed by each postfix operation from left to right.

Each postfix operation uses the result of the preceding expression or postfix operation as its input and produces the result used by the next operation.

#### 5.9.1 Array Access

An array access evaluates the index expression, checks that the index is within array bounds, and calculates the address of the selected element.

```
    ; Stack: <ptr: ptr>
    
    {{Array Index}}
    ; Stack: <element_ptr: ptr>
```

If another postfix operation follows the pointer is loaded:

```
    ; Stack: <element_ptr: ptr>
    
    load ptr
    ; Stack: <nested_array_ptr: ptr>
```

When the indexing operation produces the final result and its value is required, the element is loaded:

```
    ; Stack: <element_ptr: ptr>
    
    load [element_type]
     ; Stack: <element: element_type>
```

Where:
- `[element_type]` is the assembly type of the array element.

#### 5.9.2 Field Access

A field access adds the field offset to the current address.

If `[field_offset]` is zero, no assembly is generated. Otherwise:

```
    ; Stack: <ptr: ptr>
    
    push ui32 #[field_offset]
    add
    ; Stack: <field_ptr: ptr>
```

Where:
- `[field_offset]` is the offset of the field.

#### 5.9.3 Increment and Decrement

The increment and decrement operators require the address of the value being modified.

The base expression is therefore generated to produce its address rather than its value. The current value is then loaded from the address, incremented or decremented, and stored back at the same address.

The increment operator is generated as:

```
    ; Stack: <ptr: ptr>
    
    dup #0
    load [type]
    ; Stack: <ptr: ptr, value: type>
    
    swap
    dup #1
    ; Stack: <value: type, ptr: ptr, value: type>
    
    push [type] #[1]
    add
    ; Stack: <value: type, ptr: ptr, value + 1: type>
    
    store
    ; Stack: <value: type>
```

The decrement operator is generated as:

```
    ; Stack: <ptr: ptr>
    
    dup #0
    load [type]
    ; Stack: <ptr: ptr, value: type>
    
    swap
    dup #1
    ; Stack: <value: type, ptr: ptr, value: type>
    
    push [type] #[1]
    sub
    ; Stack: <value: type, ptr: ptr, value - 1: type>
    
    store
    ; Stack: <value: type>
```

Where:
- `[type]` is the assembly type of the base expression.

The old value remains on the operand stack because postfix increment and decrement produce the value before the update.

---

### 5.10 Function Calls

Function arguments are evaluated from left to right.

Each argument is implicitly converted to the corresponding parameter type when required.

```
    {{Argument 1}}
    {{Argument 1 Implicit Conversion}}
    
    {{Argument 2}}
    {{Argument 2 Implicit Conversion}}
    
    ...
    
    call $[function_label]
```

The `call` instruction consumes the arguments.

If the function returns a value, the returned value remains on the operand stack.

Where:
- `[function_label]` is the label of the called function.

---

### 5.11 Cast Expressions

A cast expression uses `conv` to explicitly convert the result of the expression to the specified type.

```
    {{Expression}}
    
    conv [target type]
```

Where:
- `[target type]` is the assembly type to which the value is converted.

If the result of the expression already has the target type, no instruction is generated.

---

### 5.12 New Expressions

A `new` expression allocates an array on the heap and leaves a pointer to the resulting array on the operand stack.

Each dimension is evaluated exactly once. Dimensions are evaluated from right to left so that all required dimensions are available on the operand stack while the nested arrays are allocated.

For example:

```
new int[x][y][z]
```

Evaluates the dimensions in the order:

```
z, y, x
```

The dimensions are checked for negative values and converted to `ui32` before being used for allocation.

A `new` expression is generated in two stages:

1. Allocate and construct the array.
2. If an array initialiser is present and contains at least one element, initialise the allocated array.

The array pointer produced by the allocation stage remains as the result of the `new` expression.

#### 5.12.1 Array Dimension

Each dimension expression is evaluated from right to left.

The resulting value is checked to ensure that it is not negative before being converted to `ui32`.

The generated assembly is:


```
    {{Dimension Expression}}
    
    dup #0
    push [type] #0
    clt
    
    jez $array_size_not_negative_[N]
    
    throw negative_array_size
    
$array_size_not_negative_[N]:
  
    conv ui32    ; instruction only emitted if [type] is not ui32  
    
```

Where:
- `[type]` is the resulting assembly type of the `[dimension expression]`.
- `[N]` is a unique label number


The conversion to `ui32` occurs only after the negative value check. This prevents a negative signed value from being converted into a large unsigned value before its validity has been checked.

#### 5.12.2 One Dimensional Array Allocation

A one-dimensional array is allocated by:

1. Evaluating and validating its dimension.
2. Calculating the number of bytes required for the allocation.
3. Allocating the required memory.
4. Storing the array length at the beginning of the allocation.

See [4.3.1 Array Allocation Size Calculation](#431-array-allocation-size-calculation) and [4.3 Array Representation](#43-array-representation) for the array memory layout.

The generated assembly is:

```
    ; Stack: <dimension: ui32>
    
    dup #0
    ; Stack: <dimension: ui32, dimension: ui32>
    
    {{Array Allocation Size Calculation}}
    ; Stack: <dimension: ui32, allocation_size: ui32>
    
    alloc
    
    dup #0
    rotD #3
    store
    ; Stack: <array_ptr: ptr>
```

The duplicated dimension is stored in the first four bytes of the allocation as the array length.

The result of the operation is therefore:

```
<array_ptr: ptr>
```

#### 5.12.3 Multidimensional Array Allocation

A multidimensional array is constructed recursively.

The outermost array is allocated first. Each element of the outer array contains a pointer to another array representing the next dimension.

For example:

```
new int[x][y]
```

Creates:

```
[array_length (x)]
[nested_array_ptr]
[nested_array_ptr]
...
```

Where the number of nested arrays is determined by `x`, and each nested array has length `y`.

**Allocation Algorithm**

The compiler performs multidimensional allocation using recursive code generation.

For each allocation level:

1. Allocate the current array.
2. Use its length as a counter.
3. Calculate the address of the next element
4. Retrieve the dimension required for the nested allocation.
5. Allocated the nested array.
6. Store the nested array pointer in the current element.
7. Decrement the counter.
8. Repeat until every element has been populated.

Previously evaluated dimensions, excluding the root array's dimension, remain on the operand stack while nested arrays are being allocated.

At each recursive allocation level, the compiler retains the values required to continue allocating the current array. The compiler therefore calculates the appropriate stack depth when retrieving a previously evaluated dimension.

The exact stack depth depends on the current recursive allocation depth.

**Example: Two-Dimensional Array**

For:

```
new int[x][y]
```

After evaluating the dimensions and checked that they are not negative, the is:

```
<y: ui32, x: ui32>
```

The generated structure is:

```
    {{Array Dimension}}
    {{Array Dimension}}
    ; Stack: <nested_array_dimension: ui32, root_array_dimension: ui32>
    
    {{One Dimensional Array Allocation}}
    ; Stack: <nested_array_dimension: ui32, root_array_ptr: ptr>
    
    dup #0
    load ui32
    ; Stack: <nested_array_dimension: ui32, root_array_ptr: ptr, root_array_length: ui32>
        
$nested_array_alloc_loop_[N0]:

    ; root_array_length is used as a counter to allocate root_array_length number of nested arrays
   
    ; Stack: <nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32>

    ; break out of loop if counter is zero
   
    dup #0
    jez $end_nested_array_alloc_loop_[N1]
   
    ; calculate the pointer to the element that will store the nested array
   
    dup #1
    load ui32
    ; Stack: <nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_length: ui32>
   
    dup #1
    sub
    ; Stack: <nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_length - nested_arrays_to_allocate: ui32> 
  
    push ui32 #[element size of root array]
    mul
    ; Stack: <nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, (root_array_length - nested_arrays_to_allocate) * element size of root array: ui32> 

    push ui32 #4
    add
    ; Stack: <nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, offset_from_root_array_ptr: ui32>
    
    dup #2
    add 
    ; Stack: <nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_element_ptr: ptr>

    dup #3  
    ; Stack: <nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_element_ptr: ptr, nested_array_dimension: ui32>

    {{One Dimensional Array Allocation}}
    ; Stack: <nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_element_ptr: ptr, nested_array_ptr: ptr>

    store
    ; Stack: <nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32>

    ; decrement counter

    push ui32 #1
    sub
    ; Stack: <nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate - 1: ui32>

    jmp $nested_array_alloc_loop_[N0]
    
$end_nested_array_alloc_loop_[N1]:
    ; Stack: <nested_array_dimension: ui32, root_array_ptr: ptr, 0: ui32>

    pop
    swap
    pop
    ; Stack: <root_array_ptr: ptr>
```

**Example: Three-Dimensional Array**

For:

```
new int[x][y][z]
```

After evaluating the dimensions and checked that they are not negative, the is:

```
<z: ui32, y: ui32, x: ui32>
```

The generated structure is:

```
    {{Array Dimension}}
    {{Array Dimension}}
    {{Array Dimension}}
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_dimension: ui32>
    
    {{One Dimensional Array Allocation}}
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr>
    
    dup #0
    load ui32
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, root_array_length: ui32>
        
$nested_array_alloc_loop_[N0]:

    ; root_array_length is used as a counter to allocate root_array_length number of nested arrays
   
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32>

    ; break out of loop if counter is zero
   
    dup #0
    jez $end_nested_array_alloc_loop_[N1]
   
    ; calculate the pointer to the element that will store the nested array
   
    dup #1
    load ui32
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_length: ui32>
   
    dup #1
    sub
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_length - nested_arrays_to_allocate: ui32> 
  
    push ui32 #[element size of root array]
    mul
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, (root_array_length - nested_arrays_to_allocate) * element size of root array: ui32> 

    push ui32 #4
    add
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, offset_from_root_array_ptr: ui32>
    
    dup #2
    add 
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_element_ptr: ptr>

    dup #3  
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_element_ptr: ptr, nested_array_dimension: ui32>

    {{One Dimensional Array Allocation}}
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_element_ptr: ptr, nested_array_ptr: ptr>

    dup
    load ui32
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_element_ptr: ptr, nested_array_ptr: ptr, nested_array_length: ui32>

$nested_array_alloc_loop_[N2]:

    ; root_array_length is used as a counter to allocate root_array_length number of nested arrays
   
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_element_ptr: ptr, nested_array_ptr: ptr, nested_2_arrays_to_allocate: ui32>

    ; break out of loop if counter is zero
   
    dup #0
    jez $end_nested_array_alloc_loop_[N3]
   
    ; calculate the pointer to the element that will store the nested array
   
    dup #1
    load ui32
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_element_ptr: ptr, nested_array_ptr: ptr, nested_2_arrays_to_allocate: ui32, nested_array_length: ui32>
   
    dup #1
    sub
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_element_ptr: ptr, nested_array_ptr: ptr, nested_2_arrays_to_allocate: ui32, nested_array_length - nested_2_arrays_to_allocate: ui32>
  
    push ui32 #[element size of root array]
    mul
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_element_ptr: ptr, nested_array_ptr: ptr, nested_2_arrays_to_allocate: ui32, (nested_array_length - nested_2_arrays_to_allocate) * [element size of root array]: ui32>

    push ui32 #4
    add
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_element_ptr: ptr, nested_array_ptr: ptr, nested_2_arrays_to_allocate: ui32, offset_from_nested_array_ptr: ui32>
    
    dup #2
    add 
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_element_ptr: ptr, nested_array_ptr: ptr, nested_2_arrays_to_allocate: ui32, nested_array_element: ptr>

    dup #7
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_element_ptr: ptr, nested_array_ptr: ptr, nested_2_arrays_to_allocate: ui32, nested_array_element: ptr, nested_2_array_dimension: ui32>

    {{One Dimensional Array Allocation}}
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_element_ptr: ptr, nested_array_ptr: ptr, nested_2_arrays_to_allocate: ui32, nested_array_element: ptr, nested_2_array_ptr: ptr>

    store
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_element_ptr: ptr, nested_array_ptr: ptr, nested_2_arrays_to_allocate: ui32>
    
    ; decrement counter
    
    push ui32 #1
    sub
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_element_ptr: ptr, nested_array_ptr: ptr, nested_2_arrays_to_allocate - 1: ui32>

    jmp $nested_array_alloc_loop_[N2]

$end_nested_array_alloc_loop_[N3]:
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_element_ptr: ptr, nested_array_ptr: ptr, nested_2_arrays_to_allocate - 1: ui32>

    pop
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32, root_array_element_ptr: ptr, nested_array_ptr: ptr>
    
    store
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate: ui32>

    ; decrement counter

    push ui32 #1
    sub
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, nested_arrays_to_allocate - 1: ui32>

    jmp $nested_array_alloc_loop_[N0]
    
$end_nested_array_alloc_loop_[N1]:
    ; Stack: <nested_2_array_dimension: ui32, nested_array_dimension: ui32, root_array_ptr: ptr, 0: ui32>

    pop
    rotU #3
    pop
    pop
    ; Stack: <root_array_ptr: ptr>
```

See [5.12.2 One Dimensional Array Allocation](#5122-one-dimensional-array-allocation) for the assembly generated for `[One Dimensional Array Allocation]`.

#### 5.12.4 Array Initialiser

An array initialiser is generated separately from array allocation.

After the array has been allocated, if an initialiser is present and contains at least one element, the compiler duplicates the array pointer and generates the array-initialisation operation.

For example:

```
new int[3] {1, 2, 3};
```

is conceptually generated as:

```
    {{Array Dimension}}
    {{One Dimensional Array Allocation}}
    
    dup #0
    {{Array Initialiser}}
```

The duplicated pointer is consumed by the array initialiser, leaving the original array pointer as the result of the `new` expression.

**Array Initialisation Algorithm**

An array initialiser is generated by:

1. Checking that the number of initialiser elements does not exceed the array length.
2. Calculating the address of the first array element
3. Process each initialiser element in source order. For each element:
   - If it is a nested initialiser, load the pointer to the corresponding nested array and recursively initialise it.
   - If it is an expression, evaluate the expression, apply any required implicit conversion, and store the result.
4. Advance to the next array element when another initialiser element remains.

Elements not explicitly initialised retain their default value.

An initialiser containing more elements than the array can hold causes a runtime error to be thrown.

**Initialiser Size Check**

The number of elements in the initialiser is known during compilation, while the array length is stored at runtime.

The generated assembly therefore compares the runtime array length with the number of elements in the initialiser.

```
    ; Stack: <array_ptr: ptr>
    
    dup #0
    load ui32
    ; Stack: <array_ptr: ptr, array_length: ui32>
    
    push ui32 #[number_of_initialiser_elements]
    clt
    ; Stack: <array_ptr: ptr, array_length < number_of_initialiser_elements: ui32>
    
    jez $valid_array_initialiser_size_[N]
    
    load ui32
    ; Stack: <array_length: ui32>
    
    throw array_initialiser_size
    
$valid_array_initialiser_size_[N]:
```

The `clt` instruction checks whether the array length is less than the number of initialiser elements.

If it is, the initialiser is too large and `ArrayInitialiserSizeError` is thrown.

Where:
- `[number_of_initialiser_elements]` is the number of elements in the initialiser
- `[N]` is a unique label number

**Initialiser Element Pointer**

Once the size has been validated, the pointer is advanced by four bytes to skip the array length field:

```
    push ui32 #4
    add
```

The resulting pointer refers to the first array element.

**Nested Array Initialiser**

When an initialiser element is itself a nested initialiser, the element contains a pointer to the corresponding nested array.

If the nested initialiser has no elements the pointer does not get loaded and instead removed from the stack:

```
    pop
```

If the nested initialiser has at least one element, the nested array pointer is loaded and the assembly to initialise the nested array is produced:

```
    load ptr
    {{Array Initialiser}}
```

The nested initialiser consumes the nested array pointer.

If another element remains in the current initialiser, the parent element pointer is then advanced to the next element.

```
    push ui32 #4
    add
```

**Expression Element**

When an initialiser element is an expression, the current element pointer is retained while the expression is evaluated.

If the pointer refers to an element which is not the last element in the array, the pointer is first duplicated as the `store` instruction consumes the pointer.

```
    dup #0
```

This means the pointer can be incremented to point to the next element and upon reaching the last element, the pointer gets consumed.

The generated assembly for an expression element is:

```
    {{Expression}}
    {{Implicit Conversion}}
    store
```

The expression result is stored at the current element address.

If another initialiser element remains, the element pointer is advanced by the size of one element:

```
    push ui32 #[element_size]
    add
```

**Example: One-Dimensional Initialiser**

For:

```
new int[3] { 10, 20 }
```

The allocation produces an array containing three default-initialised elements.

The array initialiser then writes each expression into the corresponding element:

```
    dup #0
    {{Initialiser Size Check}}
    push ui32 #4
    add
    
    dup #0
    {{Expression: 10}}
    store
    
    push ui32 #4
    add
    
    {{Expression: 20}}
    store
```

This results in the array containing:
```
10, 20, 0
```

**Example: Nested Initialiser**

For:

```
new int[2][2] {
    {},
    {10, 20}
}
```

The resulting assembly is:

```
    {{Initialiser Size Check}}
    push ui32 #4
    add
    
    dup #0
    pop
    
    push ui32 #4
    add
    load ptr
    
    {{Nested Initialiser Size Check 1}}
    push ui32 #4
    add
    
    dup #0
    {{Expression: 10}}
    store
    
    push ui32 #4
    add
    
    {{Expression: 20}}
    store
```

---

## 6. Statements

### 6.1 Variable Declaration

#### 6.1.1 Global Variable Declaration

A global variable declaration is emitted under the `.data` directive.

```
$[identifier]: [type] [default value]
```

Where:
- `[identifier]` is the identifier of the global variable.
- `[type]` is the assembly type of the global variable.
- `[default value]` is the default value of the variable's type.

A global variable with no initialiser does not generate additional executable assembly.

#### 6.1.2 Local Variable Declaration

A local variable declaration does not directly generate assembly.

Local variables are assigned local slots which are used by local variable access and assignment.

If a local variable has an initialiser, the initialiser is generated and the result is stored in the variable's local slot.

```
    {{Expression}}
    {{Implicit Conversion}}
    storeL #[slot]
```

Where:
- `[slot]` is the local variable's slot.

---

### 6.2 Variable Assignment

#### 6.2.1 Global Variable Assignment

A value is stored in a global variable using `storeG`.

```
    {{Expression}}
    {{Implicit Conversion}}
    storeG $[identifier]
```

Where:
- `[identifier]` is the identifier of the global variable.

#### 6.2.2 Local Variable Assignment

A value is stored in a local variable using `storeL`.

```
    {{Expression}}
    {{Implicit Conversion}}
    storeL #[slot]
```

Where:
- `[slot]` is the local variable's slot.

### 6.2.3 Global Array Element Assignment

The array reference is first loaded from the global variable.

```
    loadG $[identifier]
    
    {{Array Index}}
```

For multiple indexes, `{{Array Index}}` is repeated. If the current index does not select the final dimension, the pointer to the nested array is loaded before processing the next index.

After the final index, the operand stack contains the address of the element:

```
<element_ptr: ptr>
```

The right-hand-side expression is then evaluated and converted to the element type:

```
    {{Expression}}
    {{Implicit Conversion}}
    store
```

The complete operation therefore has the following structure

```
    loadG $[identifier]
    
    {{Array Index}}
    load ptr
    
    {{Array Index}}
    load ptr
    
    ...
    
    {{Array Index}}
    
    {{Expression}}
    {{Implicit Conversion}}
    store
```

The `load ptr` instructions are emitted only between indexes.

### 6.2.4 Local Array Element Assignment

The array reference is first loaded from the local variable.

```
    loadL ptr #[slot]
    
    {{Array Index}}
```

As with global array assignment, `Array Index` is repeated for each index. A `load ptr` is emitted between indexes when another dimension remains.

After the final index, the right-hand-side expression is evaluated and stored:

```
    {{Expression}}
    {{Implicit Conversion}}
    store
```

The complete operation has the following structure:

```
    loadL ptr #[slot]
    
    {{Array Index}}
    load ptr
    
    {{Array Index}}
    load ptr
    
    ...
    
    {{Array Index}}
    
    {{Expression}}
    {{Implicit Conversion}}
    store
```

Where
- `[slot]` is the local variable's slot.

---

### 6.3 Block

A block contained within a function is generated by generating each statement in the block in source order.

```
    {{Statement 1}}
    {{Statement 2}}
    ...
```

A block whose parent scope is the global scope, excluding function scopes, is generated as a scope function instead. See [7.4 Scope Functions](#74-scope-functions).

---

### 6.4 Conditional Statements

#### 6.4.1 If Statement

An `if` statement without an `else` block is generated as:

```
    {{Condition Expression}}
    jez $end_if_[N]
    
    {{If Block}}
    
$end_if_[N0]:
```

The condition value is consumed by `jez`.

Where:
- `[N]` is a unique label number.

#### 6.4.2 If-Else Statement

An `if` statement with an `else` block is generated as:

```
    {{Condition Expression}}
    jez $else_[N0]
    
    {{If Block}}
    
    jmp $end_if_[N1]
    
$else_[N0]:
    {{Else Block}}
    
$end_if_[N1]:
```

Where:
- `[N0]` and `[N1]` are unique label numbers.

#### 6.4.3 Else-If Statement

An `else if` statement is generated as an `if` statement contained within the `else` block of the preceding `if` statement.

---

### 6.5 While Loop

A `while` statement is generated as:

```
$start_while_[N0]:
    {{Condition Expression}}
    jez $end_while_[N1]
    
    {{Block}}
    
    jmp $start_while_[N0]
    
$end_while_[N1]:
```

Where:
- `[N0]` and `[N1]` are unique label numbers.

The condition is evaluated at the beginning of every iteration.

---

### 6.6 For Loop

#### 6.6.1 Range-Based For Loop Without Step

A range-based for loop with no explicit step is generated as:

```
    push [type] #[1]
    ; Stack: <1: type>
    
    {{Range End Expression}}
    {{Implicit Conversion to [type]}}
    ; Stack: <1: type, end: type>
    
    {{Loop Variable : PTR}}
    ; Stack: <1: type, end: type, loop_var_ptr: ptr>
    
    {{Range Start Expression}}
    {{Implicit Conversion to [type]}}
    ; Stack: <1: type, end: type, loop_var_ptr: ptr, start: type>
    
$for_loop_start_[N0]:
    ; Stack: <1: type, end: type, loop_var_ptr: ptr, current: type>
    
    dup #2
    dup #1
    cle
    ; Stack: <1: type, end: type, loop_var_ptr: ptr, current: type, end <= current: ui32>
    
    jnz $for_loop_end_[N1]
    ; Stack: <1: type, end: type, loop_var_ptr: ptr, current: type>

    dup #1
    dup #1
    ; Stack: <1: type, end: type, loop_var_ptr: ptr, current: type, loop_var_ptr: ptr, current: type>
    
    store
    ; Stack: <1: type, end: type, loop_var_ptr: ptr, current: type>
    
    {{For Loop Body}}
    ; Stack: <1: type, end: type, loop_var_ptr: ptr, current: type>
    
    dup #3
    add
    ; Stack: <1: type, end: type, loop_var_ptr: ptr, current + 1: type>

    jmp $for_loop_start_[N0]
    
$for_loop_end_[N1]:
    ; Stack: <1: type, end: type, loop_var_ptr: ptr, current: type>
    
    pop
    pop
    pop
    pop
    ; Stack: <>
```

Where:
- `[type]` is the type of the loop variable
- `[N0]` and `[N1]` are unique label numbers.

### 6.6.2 Range-Based For Loop With Step

A range-based for loop with explicit step is generated as:

```
    {{Step Expression}}
    {{Implicit Conversion to [type]}}
    ; Stack: <step: type>
    
    dup #0
    ; Stack: <step: type, step: type>
    
    jnz $for_loop_step_valid_[N0]
    throw zero_range_step

$for_loop_step_valid_[N0]:
    
    ; Stack: <step: type>
    
    dup #0
    ; Stack: <step: type, step: type>
    
    push [type] #[0]
    cge
    ; Stack: <step: type, is_step_positive: ui32>
    
    swap
    ; Stack: <is_step_positive: ui32, step: type>
    
    {{Range End Expression}}
    {{Implicit Conversion to [type]}}
    ; Stack: <is_step_positive: ui32, step: type, end: type>
    
    {{Loop Variable : PTR}}
    ; Stack: <is_step_positive: ui32, step: type, end: type, loop_var_ptr: ptr>
    
    {{Range Start Expression}}
    {{Implicit Conversion to [type]}}
    ; Stack: <is_step_positive: ui32, step: type, end: type, loop_var_ptr: ptr, start: type>
    
$for_loop_start_[N1]:
    dup #2
    dup #1
    ; Stack: <is_step_positive: ui32, step: type, end: type, loop_var_ptr: ptr, current: type, end: type, current: type>

    dup #6
    ; Stack: <is_step_positive: ui32, step: type, end: type, loop_var_ptr: ptr, current: type, end: type, current: type, is_step_positive: ui32>

    jez $for_loop_negative_step_condition_[N2]
    ; Stack: <is_step_positive: ui32, step: type, end: type, loop_var_ptr: ptr, current: type, end: type, current: type>

    cle
    ; Stack: <is_step_positive: ui32, step: type, end: type, loop_var_ptr: ptr, current: type, end <= current: ui32>

    jnz $for_loop_end_[N5]
    ; Stack: <is_step_positive: ui32, step: type, end: type, loop_var_ptr: ptr, current: type>

    jmp $for_loop_condition_join_[N3]
    
$for_loop_negative_step_condition_[N2]:
    ; Stack: <is_step_positive: ui32, step: type, end: type, loop_var_ptr: ptr, current: type, end: type, current: type>

    cge
    ; Stack: <is_step_positive: ui32, step: type, end: type, loop_var_ptr: ptr, current: type, end <= current: ui32>

    jnz $for_loop_end_[N5]
    ; Stack: <is_step_positive: ui32, step: type, end: type, loop_var_ptr: ptr, current: type>

$for_loop_condition_join_[N3]:
    dup #1
    dup #1
    ; Stack: <is_step_positive: ui32, step: type, end: type, loop_var_ptr: ptr, current: type, loop_var_ptr: ptr, current: type>

    store
    ; Stack: <is_step_positive: ui32, step: type, end: type, loop_var_ptr: ptr, current: type>

    {{For Loop Body}}
    ; Stack: <is_step_positive: ui32, step: type, end: type, loop_var_ptr: ptr, current: type>

$for_loop_continue_[N4]:

    dup #3
    add
    ; Stack: <is_step_positive: ui32, step: type, end: type, loop_var_ptr: ptr, current + step: type>

    jmp $for_loop_start_[N1]
    
$for_loop_end_[N5]:
    ; Stack: <is_step_positive: ui32, step: type, end: type, loop_var_ptr: ptr, current: type>
    
    pop
    pop
    pop
    pop
    pop
    ; Stack: <>
```

Where:
- `[type]` is the type of the loop variable
- `[N0]`, `[N1]`, `[N2]`, `[N3]`, `[N4]`, and `[N5]` are unique label numbers.

### 6.6.3 Iterable-Based For Loop

A iterable-based for loop is generated as:

```
    {{Iterable Expression : PTR}}
    dup #0
    ; Stack: <iterable_ptr: ptr, iterable_ptr: ptr>
    
    push ui32 #4
    add
    {{Iterable Expression : PTR}}
    ; Stack: <iterable_ptr: ptr, iterable_first_element_ptr: ptr>
    
    push ui32 #1
    ; Stack: <iterable_ptr: ptr, iterable_first_element_ptr: ptr, 1: ui32>
    
    rotD #3
    ; Stack: <iterable_first_element_ptr: ptr, 1: ui32, iterable_ptr: ptr>
    
    load ui32
    ; Stack: <iterable_first_element_ptr: ptr, 1: ui32, iterable_length: ui32>
    
    {{Loop Variable : PTR}}
    ; Stack: <iterable_first_element_ptr: ptr, 1: ui32, iterable_length: ui32, loop_var_ptr: ptr>
    
    push ui32 #0
    ; Stack: <iterable_first_element_ptr: ptr, 1: ui32, iterable_length: ui32, loop_var_ptr: ptr, 0: ui32>

$for_loop_start_[N0]:
    ; Stack: <iterable_element_ptr: ptr, 1: ui32, iterable_length: ui32, loop_var_ptr: ptr, current: ui32>

    dup #2
    dup #1
    cle
    ; Stack: <iterable_element_ptr: ptr, 1: ui32, iterable_length: ui32, loop_var_ptr: ptr, current: ui32, iterable_length <= current: ui32>

    jnz $for_loop_end_[N2]
    ; Stack: <iterable_element_ptr: ptr, 1: ui32, iterable_length: ui32, loop_var_ptr: ptr, current: ui32>
    
    dup #1
    dup #5
    ; Stack: <iterable_element_ptr: ptr, 1: ui32, iterable_length: ui32, loop_var_ptr: ptr, current: ui32, loop_var_ptr: ptr, iterable_element_ptr: ptr>
    
    load [element_type]
    ; Stack: <iterable_element_ptr: ptr, 1: ui32, iterable_length: ui32, loop_var_ptr: ptr, current: ui32, loop_var_ptr: ptr, iterable_element: element_type>

    store
    ; Stack: <iterable_element_ptr: ptr, 1: ui32, iterable_length: ui32, loop_var_ptr: ptr, current: ui32>

    dotD #5
    ; Stack: <1: ui32, iterable_length: ui32, loop_var_ptr: ptr, current: ui32, iterable_element_ptr: ptr>
    
    push ui32 #[size_of_element_type]
    add
    ; Stack: <1: ui32, iterable_length: ui32, loop_var_ptr: ptr, current: ui32, iterable_element_ptr + size_of_element_type: ptr>

    rotU #5
    ; Stack: <iterable_element_ptr + size_of_element_type: ptr, 1: ui32, iterable_length: ui32, loop_var_ptr: ptr, current: ui32>

    {{For Loop Body}}
    ; Stack: <iterable_element_ptr + size_of_element_type: ptr, 1: ui32, iterable_length: ui32, loop_var_ptr: ptr, current: ui32>

$for_loop_continue_[N1]:

    dup #3
    add
    ; Stack: <iterable_element_ptr + size_of_element_type: ptr, 1: ui32, iterable_length: ui32, loop_var_ptr: ptr, current + 1: ui32>

    jmp $for_loop_start_[N0]
    
$for_loop_end_[N2]:
    ; Stack: <iterable_element_ptr: ptr, 1: ui32, iterable_length: ui32, loop_var_ptr: ptr, current: ui32>

    pop
    pop
    pop
    pop
    pop
    ; Stack: <>
```

Where:
- `[type]` is the type of the loop variable
- `[element_type]` is the type of the elements of the iterable
- `[size_element_type]` is the number of bytes used to store `[element_type]`
- `[N0]`, `[N1]`, and `[N2]` are unique label numbers.

---

### 6.7 Break Statement

A `break` statement jumps to the end of the innermost enclosing loop.

If the innermost enclosing loop is a `while loop` it generates:

```
    jmp $end_while_[N]
```

If the innermost enclosing loop is a `fpr loop` it generates:

```
    jmp for_loop_end[N]
```

Where:
- `[N]` is the unique identifier of the corresponding loop label.

---

### 6.8 Continue Statement

A `continue` statement jumps to the start of the innermost enclosing loop.

If the innermost enclosing loop is a `while loop` it generates:

```
    jmp $start_while_[N]
```

If the innermost enclosing loop is a `for loop` it generates:

```
    jmp $for_loop_continue_[N]
```

Where:
- `[N]` is the unique identifier of the corresponding loop label.

---

### 6.9 Return Statement

#### 6.9.1 Return With Expression

A return statement containing an expression generates:

```
    {{Expression}}
    {{Implicit Conversion}}
    ret
```

The expression is converted to the function's return type when required.

#### 6.9.2 Return Without Expression

A `void` function may return without an expression.

```
    ret
```

---

### 6.10 Expression Statement

Expression Statements evaluate the expression without resulting in a value.

#### 6.10.1 Function Call

A function call statement is generated in the same way as a function call expression.

```
    {{Function Call Expression}}
```

#### 6.10.2 Increment and Decrement

The base expression is generated to produce its address. The current value is then loaded from the address, incremented or decremented, and stored back at the same address. 

The pre increment and post increment operator is generated as:

```
    {{Expression: PTR}}
    ; Stack: <ptr: ptr>
    
    dup #0
    load [type]
    ; Stack: <ptr: ptr, val: type>
    
    push [type] #[1]
    add
    ; Stack: <ptr: ptr, val + 1: type>
    
    store
    ; Stack: <>
```

The pre decrement and post decrement operator is generated as:

```
    {{Expression: PTR}}
    ; Stack: <ptr: ptr>
    
    dup #0
    load [type]
    ; Stack: <ptr: ptr, val: type>
    
    push [type] #[1]
    sub
    ; Stack: <ptr: ptr, val - 1: type>
    
    store
    ; Stack: <>
```

Where:
- `[type]` is the assembly type of the base expression.

---

## 7. Functions

### 7.1 Function Declaration

A user-defined function is generated using the `def` directive.

```
def $[function label]:
    args [number of arguments]
    locals [number of local slots]
    
    {{Function Body}}
```

Where:
- `[function label]` is the function's assembly label.
- `[number of arguments]` is the number of parameters accepted by the function.
- `[number of local slots]` is the number of local slots required by the function.

---

### 7.2 Function labels

Each user-defined function is assigned an assembly label derived from its identifier and parameter types.

The label has the form:

```
$[identifer]([parameter types])
```

Where:
- `[identifier]` is the function's identifier.
- `[paratmeter]` is the comma-separated list of the function's parameter types.

For example:

```
int add(int[][] a, float b) {
    return b;
}
```

is generated with the assembly label:

```
$add(int[][],float)
```

The parameter types distinguish overloaded functions with the same identifier.

### 7.3 Void Function Default Return

A `void` function receives a default `ret` instruction.

```
def $[function label]:
        args [number of arguments]
        locals [number of local slots]
    
        {{Function body}}
        ret
```

Where:
- `[function label]` is the function's assembly label.
- `[number of arguments]` is the number of parameters accepted by the function.
- `[number of local slots]` is the number of local slots required by the function.

---

### 7.4 Scope Functions

A scope function is generated for a block whose parent scope is the global scope, excluding function scopes.

The global code calls the generated scope function:

```
    call $__Scope__[N]
```

The scope function is generated as:

```
def $__Scope__[N]:
        args 0
        locals [number_of_local_slots]
        
        {{Block Body}}
        ret
```

Where:
- `[N]` is the unique scope function counter.
- `[number_of_local_slots]` is the number of local slots required by the block.

Scope functions do not accept arguments and always terminate with `ret`.

---

## 8. Builtins

### 8.1 Builtin Functions

Builtin functions are emitted after user-defined functions.

Each builtin function has a fixed assembly implementation.

Only builtin functions that are called, or are required by a called builtin function, are included in the assembly.

#### 8.1.1 void exit(int)

**Assembly:**
```
def $__Builtin__exit(int):
        args 1
        locals 0

        loadL i32 #1
        native exit
        ret
```

#### 8.1.2 void print(int)

**Assembly:**
```
def $__Builtin__print(int):
        args 1
        locals 0

        loadL i32 #1
        native print
        ret
```

#### 8.1.3 void print(float)

**Assembly:**
```
def $__Builtin__print(float):
        args 1
        locals 0

        loadL f32 #1
        native print
        ret
```

#### 8.1.4 void print(bool)

**Assembly:**
```
def $__Builtin__print(bool):
        args 1
        locals 0

        loadL ui32 #1
        jez $__print(bool)__false
        push ptr $__true__string
        jmp $__print(bool)__print
    $__print(bool)__false:
        push ptr $__false__string
    $__print(bool)__print:
        native print_str
        ret
```

**Required builtin data:**
- `$__true__string`
- `$__false__string`

#### 8.1.5 void print(char)

**Assembly:**
```
def $__Builtin__print(char):
        args 1
        locals 0

        loadL ui32 #1
        native print_char
        ret
```

#### 8.1.6 void println(int)

**Assembly:**
```
def $__Builtin__println(int):
        args 1
        locals 0

        loadL i32 #1
        call $__Builtin__print(int)
        push ui32 #10
        native print_char
        ret
```

#### 8.1.7 void println(float)

**Assembly:**
```
def $__Builtin__println(float):
        args 1
        locals 0

        loadL f32 #1
        call $__Builtin__print(float)
        push ui32 #10
        native print_char
        ret
```

#### 8.1.8 void println(bool)

**Assembly:**
```
def $__Builtin__println(bool):
        args 1
        locals 0

        loadL i32 #1
        call $__Builtin__print(bool)
        push ui32 #10
        native print_char
        ret
```

#### 8.1.9 void println(char)

**Assembly:**
```
def $__Builtin__println(char):
        args 1
        locals 0

        loadL i32 #1
        call $__Builtin__print(char)
        push ui32 #10
        native print_char
        ret
```

---

## 8.2. Builtin Data

Builtin data declarations are emitted under the `.data` directive, after global variable declarations.

Only the builtin data declaration that is required by builtin functions that have been called are included in the assembly.

| Label              | Type  | Value   | used By       |
|--------------------|-------|---------|---------------|
| `$__true__string`  | `str` | `true`  | `print(bool)` |
| `$__false__string` | `str` | `false` | `print(bool)` |

---

## 9. Assembly Generation Order

The assembly generator emits assembly in the following order:
1. Global Statements
2. `halt`
3. Scope functions
4. User-defined functions
5. Required Builtin functions
6. `.data`
7. Global variable declarations
8. Builtin data declarations
9. `.metadata`

The `.metadata` section contains the source, function, and line table information generated from the source locations recorded in the assembly IR.

---

## 10. Metadata

The assembly generator records the source location of most items in the assembly IR. The assembly emitter uses these locations and the current assembly address to generate metadata for the resulting SVMA assembly.

While emitting the code section, the emitter tracks the current byte address of the generated assembly.

Before the line table is emitted, adjacent entries with the same source ID, line, and column are merged into a single entry.

### 10.1 Source Metadata Entry

A source metadata entry is generated as:

```
[source ID]    "[file path]"
```

Where:
- `[source ID]` is the unique ID assigned to each source file. The ID is right-aligned within a field of 5 characters and padded with spaces on the left when necessary.
- `[file path]` is the absolute file path of the source file.

### 10.2 Function Metadata Entry

A function metadata entry is generated as:

```
0x[start address]    0x[end address]    [source ID]    "[function name]"
```

Where:
- `[start address]` is the address of the first byte of the function, represented as an 8-digit hexadecimal value.
- `[end address]` is the address immediately after the last byte of the function, represented as an 8-digit hexadecimal value.
- `[source ID]` is the ID of the source file containing the function. The ID is right-aligned within a field of 5 characters and padded with spaces on the left when necessary.
- `[function name]` is the name of the function.

### 10.3 Line Table Entry

A line table entry is generated as:

```
0x[start address]    0x[end address]    [source ID]    [line]    [column]
```

Where:
- `[start address]` is the address of the first byte of the generated assembly associated with the source location, represented as an 8-digit hexadecimal value.
- `[end address]` is the address immediately after the last byte of the generated assembly associated with the source location, represented as an 8-digit hexadecimal value.
- `[source ID]` is the ID of the source file containing the function. The ID is right-aligned within a field of 5 characters and padded with spaces on the left when necessary.
- `[line]` is the source line number. The line number is right-aligned within a field of 10 characters and padded with spaces on the left when necessary.
- `[column]` is the source column number. The column number is right-aligned within a field of 5 characters and padded with spaces on the left when necessary.
