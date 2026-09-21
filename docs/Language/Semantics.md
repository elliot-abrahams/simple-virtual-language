# Semantics

## Contents

- [1. Overview](#1-overview)
- [2. Comments](#2-comments)
- [3. Types](#3-types)
  - [3.1 Int Type](#31-int-type)
  - [3.2 Float Type](#32-float-type)
  - [3.3 Bool Type](#33-bool-type)
  - [3.4 Void Type](#34-void-type)
  - [3.5 Array Types](#35-array-types)
  - [3.6 Implicit Conversion](#36-implicit-conversion)
- [4. Variables](#4-variables)
  - [4.1 Variable Declaration](#41-variable-declaration)
  - [4.2 Variable Initialisation](#42-variable-initialisation)
  - [4.3 Variable Assignment](#43-variable-assignment)
  - [4.4 Variable Access](#44-variable-access)
- [5. Scopes](#5-scopes)
  - [5.1 Global Scopes](#51-global-scopes)
  - [5.2 Block Scopes](#52-block-scopes)
  - [5.3 Function Scopes](#53-function-scopes)
  - [5.4 Variable Visibility](#54-variable-visibility)
- [6. Expressions](#6-expressions)
  - [6.1 Literals](#61-literals)
  - [6.2 Variable Access](#62-variable-access)
  - [6.3 Arithmetic Operators](#63-arithmetic-operators)
  - [6.4 Comparison Operators](#64-comparison-operators)
  - [6.5 Equality Operators](#65-equality-operators)
  - [6.6 Logical Operators](#66-logical-operators)
  - [6.7 Unary Operators](#67-unary-operators)
  - [6.8 Postfix Expressions](#68-postfix-expressions)
    - [6.8.1 Indexing](#681-indexing)
    - [6.8.2 Increment and Decrement](#682-increment-and-decrement)
  - [6.9 Function Calls](#69-function-calls)
  - [6.10 Cast Expressions](#610-cast-expressions)
  - [6.11 New Expressions](#611-new-expressions)
- [7. Functions](#7-functions)
  - [7.1 Function Declarations](#71-function-declarations)
  - [7.2 Parameters](#72-parameters)
  - [7.3 Function Overloading](#73-function-overloading)
  - [7.4 Function Resolution](#74-function-resolution)
  - [7.5 Return Types](#75-return-types)
- [8. Statements](#8-statements)
  - [8.1 Statement Rules](#81-statement-rules)
  - [8.2 Blocks](#82-blocks)
  - [8.3 Variable Declarations](#83-variable-declaration)
  - [8.4 Variable Assignment](#84-variable-assignment)
  - [8.5 Expression Statements](#85-expression-statements)
    - [8.5.1 Function Call](#851-function-call)
    - [8.5.2 Increment and Decrement](#852-increment-and-decrement)
- [9. Control Flow](#9-control-flow)
  - [9.1 If Statements](#91-if-statements)
  - [9.2 While Statements](#92-while-statements)
  - [9.3 Break Statements](#93-break-statements)
  - [9.4 Continue Statements](#94-continue-statements)
  - [9.5 Return Statements](#95-return-statements)
  - [9.6 Return Paths](#96-return-paths)

---

## 1. Overview

SV Semantics define the rules that determine whether a syntactically valid SV program is semantically valid.

These rules define the valid use of types, variables, expressions, functions, statements, scopes, and control flow.

A program is semantically valid only if all applicable semantic rules are satisfied.

---

## 2. Comments

SV supports single-line comments.

A comment begins with `#` and continues until the end of the line.

The following code is therefore equivalent:

```
int x = 10;
```

```
# this is a comment
int x = 10; # this is another comment
```

Comments do not change the behaviour of a program

---

## 3. Types

SV provides the following primitive types:
- `int`
- `float`
- `bool`
- `void`

SV also provides array types.

`void` is only permitted as a function return type.

### 3.1 Int Type

`int` represents a signed 32-bit integer.

### 3.2 Float Type

`float` represents a 32-bit floating-point value.

### 3.3 Bool Type

`bool` represents a boolean value.

A boolean value is either `true` or `false`.

### 3.4 Void Type

`void` represents the absence of a value.

`void` may only be used as the return type of a function.

### 3.5 Array Types

An array type consists of an element type and one or more dimensions.

Array types are written by appending `[]` to a type.

The number of `[]` pairs determines the array's dimension.

An array's base type is the primitive type contained at its deepest level.

For example:
```
int[][] arr;
```
has base type `int` and dimension `2`.

### 3.6 Implicit Conversion

SV supports implicit conversion between certain types.

An implicit conversion is performed when an expression is used in a context where its type differs from the expected type and a valid implicit conversion exists.

Array types cannot be implicitly converted to other array types.

The only implicit conversion supported is:

| Source Type | Target Type |
|-------------|-------------|
| `int`       | `float`     |

For example:
```
float x = 5; 
```
The `int` value `5` is implicitly converted to `float`.

Implicit conversion may be required when:
- Initialising a variable
- Assigning a value to a variable
- Passing an argument to a function
- Returning a value from a function
- Combining numeric operands of different types

---

## 4. Variables

A variable has a declared type and may either be initialised or uninitialised.

### 4.1 Variable Declaration

Variable declaration introduces a variable into the current scope.

A variable declaration specifies the variable's type and identifier.

A variable cannot be declared more than once within the same scope.

### 4.2 Variable Initialisation

A variable declaration may include an initialiser.

If an initialiser is provided, the resulting type of the initialiser expression must either match the declared type of be implicitly convertible to it.

For example:
```
int x = 10;
float y = 10;
bool z = true;
int[] arr = new int[3];
```

A variable declared without an initialiser is uninitialised.

A variable becomes initialised after a valid initialiser or assignment has been processed.

A variable's own value cannot be accessed while its initialiser is being evaluated.

For example, the following is invalid:
```
int x = x;
```

Array variables contain a reference to an array rather than the array itself.

For example:
```
int [] arr = new int[2]{1, 2};
int[] copy = arr;

copy[0] = 5;

print(arr[0]); // 5
```
Both `arr` and `copy` refer to the same array. Modifying the array through either variable is therefore visible through the other variable.

### 4.3 Variable Assignment

An assignment stores the resulting value of an expression in an existing variable or array element.

The target variable must be visible from the current scope.

For an array element, the array variable must be visible and the number of indices cannot exceed its array dimension.

The resulting type of the expression must either match the variable's type or be implicitly convertible to it.

A successful assignment initialises the target variable if it was previously uninitialised.

Assigning an array to another array variable copies the array reference rather than the contents of the array.

### 4.4 Variable access

A variable access evaluates to the value stored in the referenced variable.

The variable must be visible from the current scope.

Global variables are visible from function bodies regardless of their position in the source file. However, a global variable must still be initialised before it is accessed.

A variable must be initialised before its value is accessed.

The resulting type is the declared type of the variable.

---

## 5. Scopes

A scope defines the region of a program in which declarations are visible.

### 5.1 Global Scope

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

### 5.2 Block Scope

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

### 5.3 Function Scope

Each function introduces a function scope.

Function parameters and variables declared directly within the function body belong to the function's scope.

A nested block within a function introduces a separate block scope.

Variables declared within a function are not visible outside that function.

### 5.4 Variable Visibility

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

## 6. Expressions

An expression produces a value and has a resulting type.

The resulting type of an expression is determined by the expression's operand(s) and the operator being applied.

An expression cannot have a resulting type of `void`.

### 6.1 Literals

Each literal has a fixed resulting type.

| Literal | Type    |
|---------|---------|
| `10`    | `int`   |
| `10.5f` | `float` |
| `true`  | `bool`  |
| `false` | `bool`  |

### 6.2 Variable Access

A variable access produces the value stored in the referenced variable.

The resulting type is the declared type of the variable.

See [Section 3.4](#34-variable-access) for the rules governing variable access.

### 6.3 Arithmetic Operators

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

### 6.4 Comparison Operators

The comparison operators are:

| Operator | Purpose                  |
|----------|--------------------------|
| `<`      | Less than                |
| `<=`     | Less than or equal to    |
| `>`      | Greater than             |
| `>=`     | Greater than or equal to |

The operands of comparison operators must be numeric.

The resulting type is always `bool`.

### 6.5 Equality Operators

The equality operators are:

| Operator | Purpose      |
|----------|--------------|
| `==`     | Equal to     |
| `!=`     | Not equal to |

Both operands must be `bool`, or both operands must be numeric.

The resulting type is always `bool`.

### 6.6 Logical Operators

The logical operators are:

| Operator | Purpose     |
|----------|-------------|
| `&&`     | Logical AND | 
| `\|\|`   | Logical OR  |

Both operands must be `bool`.

The resulting type is always `bool`.

### 6.7 Unary Operators

The unary operators are:

| Operator | Purpose          |
|----------|------------------|
| `+`      | Positive sign    |
| `-`      | Negative sign    |
| `!`      | Logical NOT      |
| `++`     | Prefix increment |
| `--`     | Prefix decrement |

The `+` and `-` operators can only be applied to numeric operands.

The `!` operator can only be applied to a `bool` operand.

The prefix increment operator `++` increments the value of its base expression by `1`.

The prefix decrement operator `--` decrements the value of its base expression by `1`.

For prefix increment and decrement, the base expression must be an assignable expression.

An expression is assignable when it refers to a variable or array element.

The `++` and `--` operators may only be applied to numeric types.

The `++` and `--` operators produce the value of the base expression after the increment or decrement has been performed.

For example:

```
int x = 5;
[Semantics.md](Semantics.md)
int a = ++x; // x = 6, a = 6
int b = --x; // x = 5, b = 5
```

The resulting type of unary expressions is the resulting type of its base expression.

### 6.8 Postfix Expressions

A postfix expression evaluates its base expression and then applies each postfix operation in order.

#### 6.8.1 Indexing

An index may only be applied to an array.

The index expression must have type `int`.

The index is evaluated at runtime and must be within the bounds of the array. An index outside the bounds results in a runtime error.

Each index reduces the resulting array type by one dimension.

The resulting type of this expression is the type of the selected element.

For example:

```
int[][] arr = new int[2][3];

arr[0]      // has type int[]
arr[0][1]   // has type int 
```

#### 6.8.2 Increment and Decrement

The postfix increment operator `++` increments the value of its base expression by `1`.

The postfix decrement operator `--` decrements the value of its base expression by `1`.

For postfix increment and decrement, the base expression must be an assignable expression.

An expression is assignable when it refers to a variable or array element.

The `++` and `--` operators may only be applied to numeric types.

The `++` and `--` operators produce the value of the base expression before the increment or decrement is performed.

For example:

```
int x = 5;

int a = x++;  // a is 5, x is 6
int b = x--;  // b is 6, x is 5
```

The resulting type of a postfix increment or decrement expression is the resulting type of its base expression.

### 6.9 Function Calls

A function call is an expression that evaluates to the return value of the selected function.

The resulting type of a function call is the return type of the selected function.

A function call with a `void` return type does not produce a value and therefore cannot be used where an expression value is required.

Function selection is defined in [Section 6.4](#64-function-resolution).

### 6.10 Cast Expressions

A cast expression explicitly converts the resulting value of an expression to the specified type.

The resulting type of a cast expression is the type specified by the cast.

Conversions can only be made between numeric types.

Array types cannot be used as either the source or target type of a cast.

### 6.11 New Expressions

A new expression creates an array of the specified type and dimensions.

The type specified by a new expression must be a primitive type followed by one or more array dimensions.

Each array dimension is an expression and must have type `int`.

Each dimension must evaluate to a non-negative value at runtime. A negative dimension results in a runtime error.

Newly allocated array elements are initialised to their default value:

| Element Type | Default Value |
|--------------|---------------|
| `int`        | `0`           |
| `float`      | `0.0`         |
| `bool`       | `false`       |

An array may optionally contain an array initialiser.

For a one-dimensional array, the initialiser contains expressions:
```
int [] arr = new int[3]{1, 2, 3};
```

The expressions must be compatible with the array's element type.

For multidimensional arrays, nested array initialisers are used:
```
int[][] arr = new int[2][2]{
    {1, 2},
    {3, 4}
};
```

An array initialiser must have the correct nesting depth.

For example:
```
int[][] arr = new int[2][2]{1, 2, 3, 4};
```
is invalid.

An initialiser may contain fewer elements than the corresponding array dimension. Elements without an explicitly provided initialiser retain their default value.

For example:
```
int[] arr = new int[3]{1, 2};
```
produces an array equivalent to:
```
{1, 2, 0}
```

A runtime error occurs if the initialiser contains more elements than the corresponding array dimension.

---

## 7. Functions

A function has a name, parameter list, return type, and body.

### 7.1 Function Declarations

A function declaration defines a function with:
- A return type
- An identifier
- Zero or more parameters
- A function body

Two functions cannot have the same identifier and parameter types.

The return type is not part of a function's signature.

Function declarations are available when resolving function calls regardless of their position in the source file.

### 7.2 Parameters

Each parameter has a type and an identifier.

Parameters are considered initialised when the function begins execution.

Parameters are visible throughout the function body unless hidden by a declaration in a nested scope.

When an array is passed as an argument, its array reference is passed to the function. The function therefore refers to the same array as the caller.

### 7.3 Function Overloading

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

### 7.4 Function Resolution

A function call is resolved using the number and resulting type of its arguments.

A function is a valid candidate when:
- It has the same number of parameters as arguments.
- Each argument's resulting type either matches the corresponding parameter or can be implicitly convertible to it.

An exact parameter type match is preferred over a match requiring implicit conversion.

If exactly one function can be selected, that function is selected.

If no function matches the arguments, the call is invalid.

If multiple functions can be selected using implicit conversion and none is preferred, the call is ambiguous and is invalid.

Arguments are implicitly converted to their corresponding parameter type when required.

### 7.5 Return Types

Every function has a return type.

A function with a `non-void` return type must provide a return value on every possible execution path.

When a return statement contains an expression, the resulting type of the expression must either match the function's return type or be implicitly convertible to it.

A `void` function cannot return a value.

---

## 8. Statements

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

### 8.1 Statement Rules

Statements are analysed in source order within a block or function body.

A statement performs an action and does not produce a value.

### 8.2 Blocks

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

### 8.3 Variable Declaration

A variable declaration is a statement that introduces a variable in the current scope.

See [Section 3.1](#31-variable-declaration) and [Section 3.2](#32-variable-initialisation) for the rules governing variable declarations and initialisation.

For example:
```
int x;
float y;
bool z;
int[] arr;
```

### 8.4 Variable Assignment

Variable assignment is a statement that stores the resulting value of an expression in an existing variable.

See [Section 3.3](#33-variable-assignment) for assignment rules.

For example:
```
x = 10;
arr = new int[3];
```

### 8.5 Expression Statements

An expression statement evaluates an expression without producing a value.

Only function call, increment, and decrement expressions may be used as expression statements.

For example:

```
foo();
x++;
--x;
arr[0]--;
```

#### 8.5.1 Function Call

The called function must have a `void` return type.

See [5.9 Function Calls](#59-function-calls) for the semantics of function call expressions.
See [6.4 Function Resolution](#64-function-resolution) for function resolution rules.

For example:
```
void foo() {
    print(5);
}

foo();
```

#### 8.5.2 Increment and Decrement

When used as an expression statement, the increment or decrement is performed without producing the value that would normally be produced by the expression.

The increment or decrement operators increase or decrease the value of its base expression by `1`.

The base expression must be an assignable expression.

An expression is assignable when it refers to a variable or array element.

The increment and decrement operators may only be applied to numeric types.

For example:

```
int x = 5;

x++;    // x becomes 6
++x;    // x becomes 7
x--;    // x becomes 6
--x;    // x becomes 5
```

---

## 9. Control Flow

Control-flow statements determine which statements are executed.

### 9.1 If Statements

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

### 9.2 While Statements

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
### 9.3 Break Statements

A `break` statement terminates the innermost enclosing `while` loop.

A `break` statement must only appear within a loop.

For example:
```
while (true) {
    x = x + 1;
    break;
}
```

### 9.4 Continue Statements

A `continue` statement skips the remainder of the current iteration of the innermost enclosing while loop.

A `continue` statement must only appear within a loop.

For example:

```
while (x < 10) {
    x = x + 1;
    continue;
}
```

### 9.5 Return Statements

A `return` statement terminates execution of the current function and transfers control to the caller.

A `return` statement must only appear within a function.

In a `non-void` function, a `return` statement must contain an expression.

The resulting type of the expression must either match the function's return type or be implicitly convertible to it.

In a `void` function, a `return` statement must not contain an expression.

For example:
```
int foo(int a) {
    return a;
}
```

### 9.6 Return Paths

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
