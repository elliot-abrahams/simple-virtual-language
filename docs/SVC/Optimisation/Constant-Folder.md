# Constant Folder

## Contents

- [1. Overview](#1-overview)
- [2. Expression Folding](#2-expression-folding)
- [3. Type Conversion](#3-type-conversion)
- [4. Error Handling]()
- [5. Examples](#5-examples)

EXPRS
==============
Literals
Binary Expr
Unary Expr
Ternary Expr
Function Calls -> args
Cast Expr
New Expr -> array initialiser element

---

## 1. Overview

Constant folding is an optimisation that evaluates constant expressions at compile time, reducing the work required during runtime.

In SVL, constant folding occurs after semantic analysis and before code generation. It operates on the Abstract Syntax Tree (AST), replacing expressions with literal expressions containing their resulting values.

Folding occurs when an expression can be evaluated using constant operands. Expressions containing non-constant operands are not folded unless their result can be determined without evaluating those operands

---

## 2. Expression Folding

Constant Folding processes the following expressions:
- **Literals:** Already constant and require no modification.
- **Unary expressions:** Folded when their operand is constant.
- **Binary expressions:** Folded when their operands are constant, or when short-circuit evaluation determines the result.
- **Ternary expressions:** Folded when the condition determines which branch is selected.
- **Cast expressions:** Folded when their operand is constant.
- **Function calls:** Function arguments are folded individually.
- **Array initialisers:** Elements are folded individually.

Folding can apply to expressions wherever they occur, including variable initialisers, assignments, conditions, and return statements.

---

## 3. Type Conversion

A folded expression retains the type determined by semantic analysis. Its resulting value is represented by a literal of the appropriate type, accounting for implicit conversions and explicit casts.

For example, the expression in `float x = 5 * 5` is folded to a float literal with the value `25.0f`.

---

## 4. Error Handling

Constant folding detects division and modulo operations by zero when the divisor evaluates to zero at compile time. These operations produce a DivisionByZeroError during compilation.

---

## 5. Examples

**Unary Folding:**

```
int x = -5;
```

Without constant folding:
```
        push i32 #0
        push i32 #5
        sub
        storeG $x
        halt
```

With constant folding:
```
        push i32 #-5
        storeG $x
        halt
```

---

**Arithmetic Folding:**

```
float x = 10 + 5;
```

Without constant folding:
```
        push i32 #10
        push i32 #5
        add
        conv f32
        storeG $x
        halt
```

With constant folding:
```
        push f32 #15.000000
        storeG $x
        halt
```

---

**Short-Circuit Folding:**

```
bool x = false;
bool b = true || x;
```

Without constant folding:
```
        push ui32 #0
        storeG $x
        push ui32 #1
        jnz $logical_or_true_0
        loadG $x
        jnz $logical_or_true_0
        push ui32 #0
        jmp $logical_or_end_1
    $logical_or_true_0:
        push ui32 #1
    $logical_or_end_1:
        storeG $b
        halt
```

With constant folding:
```
        push ui32 #0
        storeG $x
        push ui32 #1
        storeG $b
        halt
```

---

**Ternary Expression Folding:**

```
int expr = 10;
int x = true ? expr : 2;
```

Without constant folding:
```
        push i32 #10
        storeG $expr
        push ui32 #1
        jnz $ternary_true_0
        push i32 #2
        jmp $ternary_end_1
    $ternary_true_0:
        loadG $expr
    $ternary_end_1:
        storeG $x
        halt
```

With constant folding:
```
        push i32 #10
        storeG $expr
        loadG $expr
        storeG $x
        halt
```

---

**Cast Expression Folding:**

```
int x = (int) 5.5f;
```

Without constant folding:
```
        push f32 #5.500000
        conv i32
        storeG $x
        halt
```

With constant folding:
```
        push i32 #5
        storeG $x
        halt
```
