# Assembly Generation

# Contents

- [1. Overview](#1-overview)
- [2. Program](#2-program)
- [3. Types](#3-types)
  - [3.1 language Types](#31-language-types)
  - [3.2 Default Values](#32-default-values)
  - [3.3 Implicit Conversion](#33-implicit-conversion-)
- [4. Expressions](#4-expressions)
  - [4.1 Literals](#41-literals)
    - [4.1.1 Integer Literal](#411-integer-literal)
    - [4.1.2 Float Literal](#412-float-literal)
    - [4.1.3 Bool Literal](#413-bool-literal)
  - [4.2 Variable Access](#42-variable-access)
    - [4.2.1 Global Variable Access](#421-global-variable-access)
    - [4.2.2 Local Variable Access](#422-local-variable-access)
  - [4.3 Unary Expressions](#43-unary-expressions)
    - [4.3.1 Positive Sign](#431-positive-sign)
    - [4.3.2 Negative Sign](#432-negative-sign)
    - [4.3.3 Logical NOT](#433-logical-not)
  - [4.4 Arithmetic Expressions](#44-arithmetic-expressions)
    - [4.4.1 Addition](#441-addition)
    - [4.4.2 Subtraction](#442-subtraction)
    - [4.4.3 Multiplication](#443-multiplication)
    - [4.4.4 Division](#444-division)
    - [4.4.5 Modulo](#445-modulo)
  - [4.5 Integer Division](#45-integer-division)
  - [4.6 Comparison Expressions](#46-comparison-expressions)
  - [4.7 Logical Expressions](#47-logical-expressions)
    - [4.7.1 Logical AND](#471-logical-and)
    - [4.7.2 Logical OR](#472-logical-or)
  - [4.8 Function Calls](#48-function-calls)
- [5. Statements](#5-statements)
  - [5.1 Variable Declaration](#51-variable-declaration)
    - [5.1.1 Global Variable Declaration](#511-global-variable-declaration)
    - [5.1.2 Local Variable Declaration](#512-local-variable-declaration)
  - [5.2 Variable Assignment](#52-variable-assignment)
    - [5.2.1 Global Variable Assignment](#521-global-variable-assignment)
    - [5.2.2 Local Variable Assignment](#522-local-variable-assignment)
  - [5.3 Block](#53-block)
  - [5.4 Function Call Statement](#54-function-call-statement)
  - [5.5 Return Statement](#55-return-statement)
    - [5.5.1 Return Statement With Expression](#551-return-with-expression)
    - [5.5.2 Return Statement Without Expression](#552-return-without-expression)
  - [5.6 Conditional Statements](#56-conditional-statements)
    - [5.6.1 If Statement](#561-if-statement)
    - [5.6.2 If-Else Statement](#562-if-else-statement)
    - [5.6.3 Else-If Statement](#563-else-if-statement)
  - [5.7 While Statement](#57-while-statement)
  - [5.8 Break Statement](#58-break-statement)
  - [5.9 Continue Statement](#59-continue-statement)
- [6. Functions](#6-functions)
  - [6.1 Function Declaration](#61-function-declaration)
  - [6.2 Function Labels](#62-function-labels)
  - [6.3 Void Function Default Return](#63-void-function-default-return)
  - [6.4 Scope Functions](#64-scope-functions)
- [7. Builtins](#7-builtins)
  - [7.1 Builtin Functions](#71-builtin-functions)
  - [7.2 Builtin Data](#72-builtin-data)
- [8. Assembly Generation Order](#8-assembly-generation-order)

---

# 1. Overview

The assembly generator translates the semantically analysed abstract syntax tree (AST) into SVMA assembly.

Semantic analysis determines types, symbols, scopes, functions, and other information required by code generation. The assembly generator uses this information to prodoce valid SVMA assembly.

The generated assembly can be executed by the SVM after being assembled by the SVA (Simple Virtual Assembler).

The generated assembly uses the following indentation:
- Instructions are indented by 8 spaces
- Label definitions are indented by 4 spaces
- Top-level definitions and directives are not indented
- Method metadata is indented by 8 spaces
- Data declarations are indented by 4 spaces

---

## 2. Program

The generated assembly consists of the global statements, generated scope functions, user-defined functions, builtin functions, and static data.

```
    [global statements]
        halt
        
[scope functions]

[user-defined functions]

[builtin functions]
        
.data
    [global variable declarations]      
    [builtin data declarations]  
```

Where:
- `[global statements]` represents the assembly generated for the statements in the global scope
- `[generated scope functions]` represents the assembly generated for scope functions
- `[user-defined functions]` represents the assembly generated for user-defined functions
- `[builtin functions]` represents the assembly generated for builtin functions
- `[global variable delcarations]` represents the assembly generated for global variable declarations
- `[builtin data delcarations]` represents the assembly generated for the variable declarations required by builtin functions

---
## 3. Types

### 3.1 Language Types

Language types are represented by the following assembly types:

| Language Type | Assembly Type |
|---------------|---------------|
| `int`         | `i32`         |
| `float`       | `f32`         |
| `bool`        | `ui32`        |

### 3.2 Default Values

| Assembly Type | Default Value |
|---------------|---------------|
| `i32`         | `0`           |
| `f32`         | `0.0`         |
| `ui32`        | `0`           |

### 3.3 Implicit Conversion 

When a value must be converted to another type, the `conv` instruction is generated.

```
    conv [type]
```

Where:
- `[type]` is the assembly type of the target type.

If the value already has the target type, no instruction is generated.

---

## 4. Expressions

Every expression generated assembly which leaves its resulting value on the operand stack.

### 4.1 Literals

#### 4.1.1 Integer Literal

An integer literal is pushed onto the operand stack as an `i32`.

```
    push i32 #[value]
```

Where:
- `[value]` is the integer value

#### 4.1.2 Float Literal

A float literal is pushed onto the operand stack as an `f32`.

```
    push f32 #[value]
```

Where:
- `[value]` is the float value

#### 4.1.3 Bool Literal

A bool literal is pushed onto the operand stack as an `ui32`.

```
    push ui32 #[value]
```

Where:
- `[value]` is `1` when the bool literal is `true`
- `[value]` is `0` when the bool literal is `false`

---

### 4.2 Variable Access

#### 4.2.1 Global Variable Access

A global variable is loaded using `loadG`.

```
    loadG $[identifier]
```

Where:
- `[identifier]` is the identifier of the global variable

#### 4.2.2 Local Variable Access

A local variable is loaded using `loadL`.

```
    loadL [type] #[slot]
```

Where:
- `[type]` is the assembly type of the local variable
- `[slot]` is the local variable's slot

---

### 4.3 unary Expressions

#### 4.3.1 Positive Sign

The positive Sign does not modify the value of its operand.

```
    [expression]
```

Where:
- `[expression]` represents the assembly generated for the operand

#### 4.3.2 Negative Sign

A negative sign is generated by subtracting the operand from zero

```
    push [type] #[zero]
    [expression]
    sub
```

Where:
- `[type]` is the assembly type of the expression
- `[zero]` is the zero value for `[type]`
- `[expression]` represents the assembly generated for the operand

#### 4.3.3 Logical NOT

Logical NOT produces `true` when its operand is false and `false` when its operand is true.

```
    [expression]
    jez $evaluate_to_true_[N0]
    push ui32 #0
    jmp $end_not_[N1]
$evalate_to_true_[N0]:
    push ui32 #1
$end_not_[N1]: 
```

Where:
- `[expression]` represents the assembly generated for the operand
- `[N0]` and `[N1]` are unique label numbers

---

### 4.4 Arithmetic Expressions

The arithmetic operators are generated using teh following instructions:

| Operator | Instruction |
|----------|-------------|
| `+`      | `add`       |
| `-`      | `sub`       |
| `*`      | `mul`       |
| `/`      | `div`       |
| `%`      | `mod`       |

#### 4.4.1 Addition

```
    [left expression]
    [left implicit conversion, if required]
    [right expression]
    [right implicit conversion, if required]
    add
```

#### 4.4.2 Subtraction

```
    [left expression]
    [left implicit conversion, if required]
    [right expression]
    [right implicit conversion, if required]
    sub
```

#### 4.4.3 Multiplication

```
    [left expression]
    [left implicit conversion, if required]
    [right expression]
    [right implicit conversion, if required]
    mul
```

#### 4.4.4 Division

```
    [left expression]
    [left implicit conversion, if required]
    [right expression]
    [right implicit conversion, if required]
    div
```

#### 4.4.5 Modulo

```
    [left expression]
    [left implicit conversion, if required]
    [right expression]
    [right implicit conversion, if required]
    add
```

For the arithmetic operators above:
- `[left expression]` represents the assembly generated for the left operand
- `[right expression]` represented the assembly generated for the right operand
- `[left implicit conversion, if required]` converts the left operand to the resulting type of the expression when required
- `[right implicit conversion, if required]` converts the right operand to the resulting type of the expression when required

---

### 4.5 Integer Division

Integer division uses the `div` instruciton

If either operand is a `float`, both operands are converted to `f32` before division. The result is then converted to `i32`.

```
    [left expression]
    [left implicit conversion, if required]
    [right expression]
    [right implicit conversion, if required]
    div
    [result implicit conversion, if required]
```

Where:
- `[left expression]` represents the assembly generated for the left operand
- `[right expression]` represented the assembly generated for the right operand
- `[left implicit conversion, if required]` converts the left operand to `f32` if either operand is a `float`
- `[right implicit conversion, if required]` converts the right operand to `f32` if either operand is a `float`
- `[result implicit conversion, if required]` converts the result to `i32` if the division was performed using `f32`

---

### 4.6 Comparison Expressions

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
    [left expression]
    [left implicit conversion, if required]
    [right expression]
    [right implicit conversion, if required]
    [comparison instruction]
```

Where:
- `[left expression]` represents the assembly generated for the left operand
- `[right expression]` represented the assembly generated for the right operand
- `[left implicit conversion, if required]` converts the left operand to `f32` if either operand is a `float`
- `[right implicit conversion, if required]` converts the right operand to `f32` if either operand is a `float`
- `[comparison instruction]` is the instruction corresponding to the comparison operator

---

### 4.7 Logical Expressions

Logical expressions use condition jumps to implement short-circuit evaluation.

#### 4.7.1 Logical AND

The right operand is only evaluated if the left operand evaluates to true.

```
    [left expression]
    jez $evaluate_to_false_[N0]
    [right expression]
    jez $evaluate_to_false_[N0]
    push ui32 #1
    jmp $end_and_[N1]
$evaluate_to_false_[N0]:
    push ui32 #0
$end_and_[N1]:
```

Where:
- `[left expression]` represents the assembly generated for the left operand
- `[right expression]` represented the assembly generated for the right operand
- `[N0]` and `[N1]` are unique label numbers

#### 4.7.2 Logical OR

The right operand is only evaluated if the left operand evaluates to false.

```
    [left expression]
    jnz $evaluate_to_true_[N0]
    [right expression]
    jnz $evaluate_to_true_[N0]
    push ui32 #0
    jmp $end_or_[N1]
$evaluate_to_true_[N0]:
    push ui32 #1
$end_or_[N1]:
```

Where:
- `[left expression]` represents the assembly generated for the left operand
- `[right expression]` represented the assembly generated for the right operand
- `[N0]` and `[N1]` are unique label numbers

---

### 4.8 Function Calls

Function arguments are evaluated from left to right.

Each argument is converted to the corresponding parameter type when required.

```
    [argument 1]
    [implicit conversion 1, if required]
    [argument 2]
    [implicit conversion 2, if required]
    ...
    call $[function_label]
```

Where:
- `[argument N]` represents the assembly generated for corresponding argument.
- `[implicit conversion N, if required]` represents the conversion of the argument to the corresponding parameter type.
- `[function_label]` is the label of the called function

If the function returns a value, the value remains on the operand stack.

---

## 5. Statements

### 5.1 Variable Declaration

#### 5.1.1 Global Variable Declaration

A global variable declaration is emitted under the `.data` directive.

```
$[identifier]: [type] [default value]
```

Where:
- `[identifier]` is the identifier of the global variable
- `[type]` is the assembly type of the global variable
- `[default value]` is the default value of the variable's type

A global variable with no initialiser does not generate additional executable assembly.

#### 5.1.2 Local Variable Declaration

A local variable declaration does not directly generate assembly.

Local variables are assigned local slots which are used by local variable access and assignment.

If a local variable has an initialiser, the initialiser is generated and the resulting value is stored in the variable's local slot.

```
    [expression]
    [implicit conversion, if required]
    storeL #[slot]
```

Where:
- `[expression]` represents the assembly generated for the initialiser
- `[implicit conversion, if required]` converts the expression's resulting type to the type of the local variable
- `[slot]` is the local variable's slot

---

### 5.2 Variable Assignment

#### 5.2.1 Global Variable Assignment

A value is stored in a global variable using `storeG`.

```
    [expression]
    [implicit conversion, if required]
    storeG $[identifier]
```

Where:
- `[expression]` represents the assembly generated for the assignment expression
- `[implicit conversion, if required]` converts the expression's resulting type to the type of the global variable
- `[identifier]` is the identifier of the global variable

#### 5.2.2 Local Variable Assignment

A value is stored in a global variable using `storeL`.

```
    [expression]
    [implicit conversion, if required]
    storeL #[slot]
```

Where:
- `[expression]` represents the assembly generated for the assignment expression
- `[implicit conversion, if required]` converts the expression's resulting type to the type of the local variable 
- `[slot]` is the local variable's slot

---

### 5.3 Block

A block contained within a function is generated by generating each statement in the block in source order.

```
    [statement 1]
    [statement 2]
    ...
```

Where:
- `[statement N]` represents the assembly generated for the corresponding statement.

A block whose parent scope is the global scope (not including function scopes) is generated as a scope function instead.

---

### 5.4 Function Call Statement

A function call sued as a statement is generated in the same wat as a function call expression.

```
    [function call]
```

Where:
`[function call]` represents the assembly generated for the function call

---

### 5.5 Return Statement

#### 5.5.1 Return With Expression

```
    [expression]
    [implicit conversion, if required]
    ret
```

Where:
- `[expression]` represents the assembly generated for the return expression
- `[implicit conversion, if required]` converts the expression's resulting type to teh function's return type

#### 5.5.2 Return Without Expression

A `void` function may return without an expression.

```
    ret
```

---

### 5.6 Conditional Statements

#### 5.6.1 If Statement

An `if` statement without an `else` block is generated as:

```
    [condition]
    jez $end_if_[N0]
    [if block]
$end_if_[N0]:
```

Where:
- `[condition]` represented the assembly generated for the condition
- `[if block]` represents the assembly generated for the `if` block
- `[N0]` is a unique label number

#### 5.6.2 If-Else Statement

An `if` statement with an `else` block is generated as:

```
    [condition]
    jez $else_[N0]
    [if block]
    jmp $end_if_[N1]
$else_[N0]:
    [block 2]
$end_if_[N1]:
```

Where:
- `[condition]` represents the assembly generated for the condition
- `[if block]` represents the assembly generated for the if block
- `[else block]` represents the assembly generated for the else block
- `[N0]` and `[N1]` are unique label numbers

#### 5.6.3 Else-If Statement

An `else if` statement is generated as an `if` statement contained within the `else` block of another `if` statement.

---

### 5.7 While Statement

A `while` statement is generated as:

```
$start_while_[N0]
    [condition]
    jez $end_while_[N1]
    [block]
    jmp $start_while_[N0]
$end_while_[N1]:
```

Where:
- `[condition]` represents the assembly generated for the loop condition
- `[block]` represents the assembly generated for the loop body
- `[N0]` and `[N1]` are unique label numbers

---

### 5.8 Break Statement

A `break` statement jumps to the end of the innermost enclosing `while` loop.

```
    jmp $end_while_[N]
```

Where:
- `[N]` identifies the end label of the innermost enclosing `while` loop

--

### 5.9 Continue Statement

A `continue` statement jumps to the start of the innermost enclosing `while` loop.

```
    jmp $start_while_[N]
```

Where:
- `[N]` identifies the start label of the innermost enclosing `while` loop

---

## 6. Functions

### 6.1 Function Declaration

A user-defined function is generated using the `def` directive.

```
def $[function label]:
    args [number of arguments]
    locals [number of local slots]
    
    [function body]
```

Where:
- `[function label]` is the function's assembly label
- `[number of arguments]` is the number of parameters accepted by the function
- `[number of local slots]` is the number of local slots required by teh function
- `[function body]` represented the assembly generated for the function body

---

### 6.2 Function labels

Each user-defined function is assigned a unique assembly label.

The label is generated from the function's identifier and parameter types.

```
$[identifer]([parameter types])
```

Where:
- `[identifier]` is the functions identifier
- `[paratmeter]` is the comma-separated list of the function's parameter types.

For example:
```
int add(int a, float b) {
    return a + b;
}
```

is generated with the assembly label:
```
$add(int,float)
```

### 6.3 Void Function Default Return

A `void` function receives a default `ret` instruction.

```
def $[function label]:
        args [number of arguments]
        locals [number of local slots]
    
    [function body]
        ret
```

Where:
- `[function label]` is the function's assembly label
- `[number of arguments]` is the number of parameters accepted by the function
- `[number of local slots]` is the number of local slots required by teh function
- `[function body]` represented the assembly generated for the function body

---

### 6.4 Scope Functions

A scope function is generated for a block whose parent scope is the global scope (except for function scopes).

The global code calls the generated scope function:

```
    call $__Scope__[N]
```

The scope function is generated as:

```
def $__Scope__[N]:
        args 0
        locals [number of local slots]
        
        [block body]
        ret
```

Where:
- `[N]` is the unique scope function counter
- `[number of local slots]` is the number of local slots required by the block
- `[block body]` represents the assembly generated for the block

Scope functions do not accept arguments and always include a `ret` instruction.

---

## 7. Builtins

### 7.1 Builtin Functions

Builtin functions are emitted after user-defined functions.

Each builtin function has a fixed assembly implementation.

Only the builtin functions that have been called are included in the assembly.

#### 7.1.3 void exit(int)

**Assembly:**
```
def $__Builtin__exit(int):
        args 1
        locals 0

        loadL i32 #1
        native exit
        ret
```

#### 7.1.2 void print(int)

**Assembly:**
```
def $__Builtin__print(int):
        args 1
        locals 0

        loadL i32 #1
        native print
        ret
```

#### 7.1.3 void print(float)

**Assembly:**
```
def $__Builtin__print(float):
        args 1
        locals 0

        loadL f32 #1
        native print
        ret
```

#### 7.1.4 void print(bool)

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
- $__true__string
- $__false__string

---

## 7.2. Builtin Data

Builtin data declarations are emitted under the `.data` directive, after global variable declarations.

Only the builtin data declaration that is required by builtin functions that have been called are included in the assembly.

| Label              | Type  | Value   | used By       |
|--------------------|-------|---------|---------------|
| `$__true__string`  | `str` | `true`  | `print(bool)` |
| `$__false__string` | `str` | `false` | `print(bool)` |

---

## 8. Assembly Generation Order

The assembly generator emits assembly in the following order:
1. Global Statements
2. `halt`
3. Scope functions
4. User defined functions
5. Builtin functions
6. `.data`
7. Global variable declarations
8. Builtin data declarations
