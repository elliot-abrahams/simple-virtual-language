# Semantics

## Contents

- [1. Overview](#1-overview)
- [2. Types](#2-types)
    - [2.1 Int Type](#21-int-type)
    - [2.2 Float Type](#22-float-type)
    - [2.3 Bool Type](#23-bool-type)
    - [2.4 Void Type](#24-void-type)
    - [2.5 Implicit Conversion](#25-implicit-conversion)
- [3. Variables](#3-variables)
  - [3.1 Variable Declaration](#31-variable-declaration)
  - [3.2 Variable Initialisation](#32-variable-initialisation)
  - [3.3 Variable Assignment](#33-variable-assignment)
  - [3.4 Variable Access](#34-variable-access)
- [4. Scopes](#4-scopes)
  - [4.1 Global Scopes](#41-global-scope)
  - [4.2 Block Scopes](#42-block-scope)
  - [4.3 Function Scopes](#43-function-scope)
  - [4.4 Variable Visibility](#44-variable-visibility)
- [5. Expressions](#5-expressions)
  - [5.1 Literals](#51-literals)
  - [5.2 Variable Access](#52-variable-access)
  - [5.3 Function Calls](#53-function-calls)
  - [5.4 Unary Operators](#54-unary-operators)
  - [5.5 Arithmetic Operators](#55-arithmetic-operators)
  - [5.6 Comparison Operators](#56-comparison-operators)
  - [5.7 Equality Operators](#57-equality-operators)
  - [5.8 Logical Operators](#58-logical-operators)
- [6. Functions](#6-functions)
  - [6.1 Function Declarations](#61-function-declarations)
  - [6.2 Parameters](#62-parameters)
  - [6.3 Function Overloading](#63-function-overloading)
  - [6.4 Function Resolution](#64-function-resolution)
  - [6.5 Return Types](#65-return-types)
- [7. Statements](#7-statements)
  - [7.1 Statement Rules](#71-statement-rules)
  - [7.2 Blocks](#72-blocks)
  - [7.3 Variable Declarations](#73-variable-declaration)
  - [7.4 Variable Assignment](#74-variable-assignment)
  - [7.5 Function Call Statements](#75-function-call-statements)
- [8. Control Flow](#8-control-flow)
  - [8.1 If Statements](#81-if-statements)
  - [8.2 While Statements](#82-while-statements)
  - [8.3 Break Statements](#83-break-statements)
  - [8.4 Continue Statements](#84-continue-statements)
  - [8.5 Return Statements](#85-return-statements)
  - [8.6 Return Paths](#86-return-paths)

---

## 1. Overview

SV Semantics define the rules that determine whether a syntactically valid SV program is semantically valid.

These rules define the valid use of types, variables, expressions, functions, statements, scopes, and control flow.

A program is semantically valid only if all applicable semantic rules are satisfied.

---

## 2. Types

SV provides the following types:
- `int`
- `float`
- `bool`
- `void`

`void` is only permitted as a function return type.

### 2.1 Int Type

`int` represents a signed 32-bit integer.

### 2.2 Float Type

`float` represents a 32-bit floating-point value.

### 2.3 Bool Type

`bool` represents a boolean value.

A boolean value is either `true` or `false`.

### 2.4 Void Type

`void` represents the absence of a value.

`void` may only be used as the return type of a function.

### 2.5 Implicit Conversion

SV supports implicit conversion between certain types.

An implicit conversion is performed when an expression is used in a context where its type differs from the expected type and a valid implicit conversion exists.

The only implicit conversions supported is:

| Source Type | Target Type |
|-------------|-------------|
| `int`       | `float`     |

For example:
```
float x = 5; 
```
The `int` value `10` is implicitly converted to `float`.

Implicit conversion may be required when:
- Initialising a variable
- Assigning a value to a variable
- Passing an argument to a function
- Returning a value from a function
- Combining numeric operands of different types

---

## 3. Variables

A variable has a declared type and may either be initialised or uninitialised.

### 3.1 Variable Declaration

Variable declaration introduces a variable into the current scope.

A variable declaration specifies the variable's type and identifier.

A variable cannot be declared more than once within the same scope.

### 3.2 Variable Initialisation

A variable declaration may include an initialiser.

If an initialiser is provided, the resulting type of the initialiser expression must either match the declared type of be implicitly convertible to it.

For example:
```
int x = 10;
float y = 10;
bool z = true;
```

A variable declared without an initialiser is uninitialised.

A variable becomes initialised after a valid initialiser or assignment has been processed.

A variable's own value cannot be accessed while its initialiser is being evaluated.

For example, the following is invalid:
```
int x = x;
```

### 3.3 Variable Assignment

An assignment stores the resulting value of an expression in an existing variable.

The target variable must be visible from the current scope.

The resulting type of the expression must either match the variable's type or be implicitly convertible to it.

A successful assignment initialises the target variable if it was previously uninitialised.

### 3.4 Variable access

A variable access evaluates to the value stored in the referenced variable.

The resulting type of a variable access is the declared type of the variable.

The variable must be visible from the current scope.

A variable must be initialised before its value is accessed.

Global variables are visible from function bodies regardless of their position in the source file. However, a global variable must still be initialised before it is accessed.

---

## 4. Scopes

A scope defines the region of a program in which declarations are visible.

### 4.1 Global Scope

The global scope contains declarations made outside of functions and blocks.

Variables declared in the global scope are global variables.

A global variable is visible from its declaration onwards when accessed from the global scope.

Global variables may be accessed from function bodies regardless of where the global variable is declared in the source file.

For example, the following is valid:
```
int getValue() {
    return x;
}

int x = 10;
```
This is valid because `x` is a global variable.

### 4.2 Block Scope

Each block introduces a new scope.

A variable declared within a block is visible within that block and any nested scopes.

A variable declared within a block is not visible outside of that block.

For example:
```
{
  int x = 10;
  int y = x;   <- valid
}
int y = x;     <- invalid
```

### 4.3 Function Scope

Each function introduces a function scope.

Function parameters and variables declared directly within the function body belong to the function's scope.

A nested block within a function introduces a separate block scope.

Variables declared within a function are not visible outside that function.

### 4.4 Variable Visibility

A variable is visible within its declaring scope and all nested scopes.

When multiple visible declarations have the same identifier, the declaration in the nearest enclosing scope is used.

For example:
```
int x = 10;
{
    int x = 20;
    int y = x;      <- refers to the variable with the value 20
}
```
The block variable `x` takes precedence over the global variable `x`.

A variable must be declared before it is accessed, except when a global variable is accessed from a function body.

---

## 5. Expressions

An expression produces a value and has a resulting type.

The resulting type of an expression is determined by the expression's operand(s) and the operator being applied.

An expression cannot have a resulting type of `void`.

### 5.1 Literals

Each literal has a fixed resulting type.

| Literal | Type    |
|---------|---------|
| `10`    | `int`   |
| `10.5f` | `float` |
| `true`  | `bool`  |
| `false` | `bool`  |

### 5.2 Variable Access

A variable access produces the value stored in the referenced variable.

The resulting type is the declared type of the variable.

See [Section 3.4](#34-variable-access) for the rules governing variable access.

### 5.3 Function Calls

A function call is an expression that evaluates to the return value of the selected function.

The resulting type of a function call is the return type of the selected function.

A function call with a `void` return type does not produce a value and therefore cannot be used where an expression value is required.

Function selection is defined in [Section 6.4](#64-function-resolution).

### 5.4 Unary Operators

The unary operators are:

| Operator | Purpose       |
|----------|---------------|
| `+`      | Positive sign |
| `-`      | Negative sign |
| `!`      | Logical NOT   |

The `+` and `-` operators can only be applied to numeric operands.

The `!` operator can only be applied to a `bool` operand.

The resulting type of unary expressions is the same as the operand's type.

### 5.5 Arithmetic Operators

The arithmetic operators are:

| Operator | Purpose            |
|----------|--------------------|
| `+`      | Addition           |
| `-`      | Subtraction        |
| `*`      | Multiplication     |
| `/`      | Division           |
| `//`     | Integer division   |
| `%`      | Remainder (modulo) | 

Arithmetic operators require numeric operands.

`bool` operands are not permitted.

For `+`, `-`, `*`, `%` operators:
- If either operand is a `float` the resulting type is`float`.
- Otherwise, the resulting type is `int`.

The resulting type of division `/` is always `float`.

The resulting type of integer division `//` is always `int`.

For example:
```
10 + 2       <- int
1- + 2.0f    <- float
10 / 2       <- float
`10 // 2     <- int 
```

### 5.6 Comparison Operators

The comparison operators are:

| Operator | Purpose                  |
|----------|--------------------------|
| `<`      | Less than                |
| `<=`     | Less than or equal to    |
| `>`      | Greater than             |
| `>=`     | Greater than or equal to |

The operands of comparison operators must be numeric.

The resulting type is always `bool`.

### 5.7 Equality Operators

The equality operators are:

| Operator | Purpose      |
|----------|--------------|
| `==`     | Equal to     |
| `!=`     | Not equal to |

Both operands must be `bool`, or both operands must be numeric.

The resulting type is always `bool`.

### 5.8 Logical Operators

The logical operators are:

| Operator | Purpose     |
|----------|-------------|
| `&&`     | Logical AND | 
| `\|\|`   | Logical OR  |

Both operands must be `bool`.

The resulting type is always `bool`.

---

## 6. Functions

A function has a name, parameter list return type, and body.

### 6.1 Function Declarations

A function declaration defines a function with:
- A return type
- An identifier
- Zero or more parameters
- A function body

Two functions cannot have the same identifier and parameter types.

The return type is not part of a function's signature.

Function declarations are available when resolving function calls regardless of their position in the source file.

### 6.2 Parameters

Each parameter has a type and an identifier.

Parameters are considered initialised when the function begins execution.

parameters are visible throughout the function body unless hidden by a declaration in a nested scope.

### 6.3 Function Overloading

SV supports function overloading.

Multiple functions may have the same identifier provided that their parameter type lists are different.

For example:
```
int add(int a, int b) {
    return a + b;
}
float add(float a, float b) {
    return a + b;
}
```

Return types alone cannot distinguish overloaded functions.

### 6.4 Function Resolution

A function call is resolved using the number and resulting type of its arguments.

A function is a valid candidate when:
- It has the same number of parameters as arguments.
- Each argument's resulting type either matches the corresponding parameter or can be implicitly convertible to it.

An exact parameter type match is preferred over a match requiring implicit conversion.

If exactly one function can be selected, that function is selected.

If no function matches the arguments, the call is invalid.

If multiple functions can be selected using implicit conversion and none is preferred, the call is ambiguous and is invalid.

Arguments are implicitly converted to their corresponding parameter type when required.

### 6.5 Return Types

Every function has a return type.

A function with a `non-void` return type must provide a return value on every possible execution path.

When a return statement contains an expression, the resulting type of the expression must either match the function's return type or be implicitly convertible to it.

A `void` function cannot return a value.

---

## 7. Statements

A statement performs an action and does not produce a value.

The following are statements:
- Blocks
- Variable declaration
- Variable assignment
- Function call statements
- `if` statements
- `while` statements
- `break` statements
- `continue` statements
- `return` statements

### 7.1 Statement Rules

Statements are analysed in source order within a block or function body.

### 7.2 Blocks

A block is a sequence of statements enclosed by `{` and `}`.

A block introduces a new scope.

The statements within a block are analysed in order.

See [Section 4.2](#42-block-scope) for block scope rules.

For example:
```
{
    int x = 10;
}
```

### 7.3 Variable Declaration

A variable declaration is a statement that introduces a variable in the current scope.

See [Section 3.1](#31-variable-declaration) and [Section 3.2](#32-variable-initialisation) for the rules governing variable declarations and initialisation.

For example:
```
int x;
float y;
bool z;
```

### 7.4 Variable Assignment

variable assignment is a statement that stores the resulting value of an expression in an existing variable.

See [Section 3.3](#33-variable-assignment) for assignment rules.

For example:
```
x = 10;
```

### 7.5 Function Call Statements

A function call statement evaluates a function call.

The called function must have a `void` return type.

See [Section 6.4](#64-function-resolution) for function resolution rules.

For example:
```
void foo() {
    print(5);
}

foo();
```

---

## 8. Control Flow

Control-flow statements determine which statements are executed.

### 8.1 If Statements

An `if` statement evaluates its condition.

The condition must have a resulting type of `bool`.

If the condition is `true`, the `if` block is executed.

If the condition is `false` and an `else` clause is present, the `else` statement is executed.

An `else` clause is optional.

An `else if` is represented by an `else` containing another `if` statement.

For example:
```
if (true) {
    x = 10;
}
```

```
if (false) {
    x = 10;
} else {
    x = 4;
}
```
```
if (false) {
    x = 10;
} else if (false) {
    x = 4;
} else {
    x = 2;
}
```

### 8.2 While Statements

A `while` statement evaluates its condition.

The condition must have a resulting type of `bool`.

If the condition is `true`, the loop body is executed.

After the body finishes, the condition is evaluated again.

If the condition is `false`, execution continues after the loop.

For example:
```
while (x < 4) {
    x = x + 1;
}
```
### 8.3 Break Statements

A `break` statement terminates the innermost enclosing `while` loop.

A `break` statement must only appear within a loop.

For example:
```
while (true) {
    x = x + 1;
    break;
}
```

### 8.4 Continue Statements

A `continue` statement skips the remainder of the current iteration of the innermost enclosing while loop.

A `continue` statement must only appear within a loop.

For example:

```
while (x < 10) {
    x = x + 1;
    continue;
}
```

### 8.5 Return Statements

A `return` statement terminates execution of the current function and transfers control to the caller.

A `return` statement must only appear within a function.

In a `non-void` function, a `return` statement must contain an expression.

The resulting type of the expression mst either match the function's return type or be implicitly convertible to it.

In a `void` function, a `return` statement must not contain an expression.

For example:
```
int foo(int a) {
    return a;
}
```

### 8.6 Return Paths

A `non-void` function must have a return value on every possible execution path.

A `return` statement provides a returning path.

A `block` provides a returning path if a statement in the block provides a returning path.

An `if` statement provides a returning path only when both of its `if` branch and its `else` branch provide returning paths.

An `if` statement without an `else` branch does not provide a returning path.

A `while` statement does not provide a returning path.

The following does not provide a returning path:
- Variable declaration
- Variable Assignment
- Function call statement
- `break` statement
- `continue` statement
