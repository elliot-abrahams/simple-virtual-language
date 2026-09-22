# Compiler Errors

## Contents

- [1 Overview](#1-overview)
- [2. Error Format](#2-error-format)
- [3. Syntax Errors](#3-syntax-errors)
  - [3.1](#31-unexpected-token)
  - [3.2 Reserved Identifier](#32-reserved-identifier)
  - [3.3 Missing Float Digit](#33-missing-float-digit)
  - [3.4 Missing Float Suffix](#34-missing-float-suffix)
  - [3.5 Value Out of Range](#35-value-out-of-range)
- [4. Semantic Errors](#4-semantic-errors)
  - [4.1 Variable Already Defined](#41-variable-already-defined)
  - [4.2 Undefined Variable](#42-undefined-variable)
  - [4.3 Uninitialised Variable](#43-uninitialised-variable)
  - [4.4 Function Already Defined](#44-function-already-defined)
  - [4.5 Undefined Function](#45-undefined-function)
  - [4.6 Ambiguous Function Call](#46-ambiguous-function-call)
  - [4.7 Missing Return on All Paths](#47-missing-return-on-all-paths)
  - [4.8 Missing Return Value](#48-missing-return-value)
  - [4.9 Return Value From Void Function](#49-return-value-from-void-function)
  - [4.10 Unused Function Return Value](#410-unused-function-return-value)
  - [4.11 Invalid Expression Statement](#411-invalid-expression-statement)
  - [4.12 Return Outside a Function](#412-return-outside-a-function)
  - [4.13 Continue Outside a Loop](#413-continue-outside-a-loop)
  - [4.14 Break Outside a Loop](#414-break-outside-a-loop)
  - [4.15 Too Few Array Initialiser Dimensions](#415-too-few-array-initialiser-dimensions)
  - [4.16 Too Many Array Initialiser Dimensions](#416-too-many-array-initialiser-dimensions)
- [5. Type Errors](#5-type-errors)
  - [5.1 Invalid Assignment](#51-invalid-assignment)
  - [5.2 Invalid Return Type](#52-invalid-return-type)
  - [5.3 Invalid Condition Type](#53-invalid-condition-type)
  - [5.4 Invalid Binary Operator](#54-invalid-binary-operator)
  - [5.5 Invalid Unary Operator](#55-invalid-unary-operator)
  - [5.6 Non-Assignable Expression](#56-non-assignable-expression)
  - [5.7 Invalid Array Index Type](#57-invalid-array-index-type)
  - [5.8 Invalid Array Indexing](#58-invalid-array-indexing)
  - [5.9 Invalid Array Initialiser Element](#59-invalid-array-initialiser-element)
  - [5.10 Invalid Cast](#510-invalid-cast)
   
## 1. Overview

The compiler reports errors when a source program does not satisfy the syntax, semantic, or type rules of the language.

Compiler errors are divided into three categories:
- **Syntax Errors:** the source code does not follow the grammatical or lexical rules of the language.
- **Semantic Errors:** the source code is syntactically valid but violates a rule about the meaning or structure of a program.
- **Type Errors:** an operation uses values or expressions with incompatible or unsupported types.

Each error identifies the location in the source program where the error waas detected.

---

## 2. Error Format

Compiler errors are reported using the following format:

```
<ErrorType>: <message>
  <line> |    <source line>
    |    <caret>
  at <absolute file path>:<line>:<column>
```

For example:

```
SyntaxError: unexpected '|'
  1 |    int x = true | false;
    |                 ^
  at C:\...\svl\examples\test.sv:1:14
```

If the source line cannot be read, the error instead shows:

```
SyntaxError: unexpected '|'
    | source code unavailable
  at C:\...\svl\examples\test.sv:1:14
```

---

## 3. Syntax Errors

Syntax Errors are reported when the source code does not follow the grammatical or lexical rules of the language.

### 3.1 Unexpected Token

**Message:**
```
unexpected '<token image>'
```

**Example:**
```
int x = bool;
```

```
SyntaxError: unexpected 'bool'
```

An expression is required after the assignment operator, but `bool` was encountered instead. 

---

### 3.2 Reserved Identifier

**Message:**
```
identifiers starting with '__' are reserved
```

**Example:**
```
int __x;
```

```
SyntaxError: identifiers starting with '__' are reserved
```

Identifiers beginning with '__' are reserved for use by the compiler.

---

### 3.3 Missing Float Digit

**Message:**
```
expected digit after '.'
```

**Example:**
```
float f = 5.f;
```

```
SyntaxError: expected digit after '.'
```

A floating-point value must contain at least one digit after the decimal point.

---

### 3.4 Missing Float Suffix

**Message:**
```
float value must end with 'f'
```

**Example:**
```
float f = 5.5;
```

```
SyntaxError: float value must end with 'f'
```

Floating-point values must use the `f` suffix.

---

### 3.5 Value Out of Range

**Message:**
```
'<value>' is out of range for type '<type>'
```

**Example:**
```
int x = 2147483648;
```

```
SyntaxError: '2147483648' is out of range for type 'int'
```

The value cannot be represented by the specified type.

---

## 4. Semantic Errors

Semantic errors are reported when the source is syntactically valid but violates a rule about the meaning or structure of the program.

### 4.1 Variable Already Defined

**Message:**
```
variable `<variable_identifier>` is already defined
```

**Example:**
```
int x;
int x;
```

```
SemanticError: variable 'x' is already defined
```

A variable with the same identifier cannot be defined again in the same scope.

---

### 4.2 Undefined Variable

**Message:**
```
variable '<variable_identifier>' is undefined
```

**Example:**
```
print(x);
```

```
SemanticError: variable 'x' is undefined
```

No variable with identifier `x` is visible at the location where it is used.

---

### 4.3 Uninitialised Variable

**Message:**
```
variable `<variable_identifier>` may not have been initialised
```

**Example:**
```
int x;
print(x);
```

```
SemanticError: variable 'x' may not have been initialised
```

The variable

---

### 4.4 Function Already Defined

**Message:**
```
function '<function_signature>' is already defined
```

**Example:**
```
int add(int a, int b) {
    return a + b;
}
int add(int a, int b) {
    return a + b;
}
```

```
SemanticError: function 'add(int,int)' is already defined
```

A function with the same signature has already been defined.

---

### 4.5 Undefined Function

**Message:**
```
function '<function_signature>' is undefined
```

**Example:**
```
foo(5);
```

```
SemanticError: function 'foo(int)' is undefined
```

No function matching the required signature has been defined.

---

### 4.6 Ambiguous Function Call

**Message:**
```
ambiguous call to function '<function_identifier>'
```

**Example:**
```
float add(int a, float b) {
    return a + b;
}
float add(float a, int b) {
    return a + b;
}
add(5, 5);
```

```
SemanticError: ambiguous call to function 'add'
```

Multiple overloaded functions match the call, so a single function cannot be selected.

---

### 4.7 Missing Return on All Paths

**Message:**
```
function '<function_signature>' may not return a value on all paths
```

**Example:**
```
int getVal(bool cond) {
    if (cond) {
        return 10;
    }
}
```

```
SemanticError: function 'getVal(bool)' may not return a value on all paths
```

A non-void function must return a value regardless of which control-flow path is taken.

---

### 4.8 Missing Return value

**Message:**
```
function '<function_signature>' must return a value
```

**Example:**
```
int getVal() {
    return;
}
```

```
SemanticError: function 'getVal()' must return a value
```

A non-void function cannot use a return statement without a value.

---

### 4.9 Return Value From Void Function

**Message:**
```
cannot return a value from void function '<function_signature>'
```

**Example:**
```
void foo() {
    return 10;
}
```

```
SemanticError: cannot return a value from void function 'foo()'
```

A void function cannot return a value.

---

### 4.10 Unused Function Return Value

**Message:**
```
return value of function '<function_signature>' must be used
```

**Example:**
```
int foo() {
    return 10;
}
foo();
```

```
SemanticError: return value of function 'foo()' must be used
```

A non-void function call cannot be used as a statement.

---

### 4.11 Invalid Expression Statement

**Message:**
```
expression cannot be used as a statement
```

**Example:**
```
int x = 10;

x + 5;
```

```
SemanticError: expression cannot be used as a statement
```

The expression does not perform an operation that is permitted as ann expression statement.

---

### 4.12 Return Outside a Function

**Message:**
```
'return' outside a function
```

**Example:**
```
return;
```

```
SemanticError: 'return' outside a function
```

Return can only be used inside a function.

---

### 4.13 Continue Outside a Loop

**Message:**
```
'continue' outside a loop
```

**Example:**
```
continue;
```

```
SemanticError: 'continue' outside a loop
```

Continue can only be used inside a loop.

---

### 4.14 Break Outside a Loop

**Message:**
```
'break' outside a loop
```

**Example:**
```
break;
```

```
SemanticError: 'break' outside a loop
```

break can only be used inside a loop.

---

### 4.15 Too Few Array Initialiser Dimensions

**Message:**
```
array initialiser has too few dimensions for array of type '<type>'
```

**Example:**
```
int[][] arr = new int[3][3]{ 1 };
```

```
SemanticError: array initialiser has too few dimensions for array of type 'int[][]'
```

The structure of the array initialiser does not contain enough nested dimensions for the declared array type.

---

### 4.16 Too Many Array Initialiser Dimensions

**Message:**
```
array initialiser has too many dimensions for array of type '<type>'
```

**Example:**
```
int[] arr = new int[3]{ { 1 } };
```

```
SemanticError: array initialiser has too many dimensions for array of type 'int[]'
```

The array initialiser contains more nested dimensions that the declared array type.

---

## 5. Type Errors

Type errors are reported when an operation is not valid for the type(s) involved.

### 5.1 Invalid Assignment

**Message:**
```
cannot assign '<type>' to type '<type>'
```

**Example:**
```
int x = true;
```

```
TypeError: cannot assign 'bool' to type 'int'
```

The type of the value being assigned is not compatible with declared type.

---

### 5.2 Invalid Return Type

**Message:**
```
cannot return '<type>' from a function returning '<type>'
```

**Example:**
```
bool foo() {
    return 5.5f
}
```

```
TypeError: cannot return 'float' from a function returning 'bool'
```

The type of the returned value is not compatible with the function's return type.

---

### 5.3 Invalid Condition Type

**Message:**
```
cannot use type '<type>' as a condition
```

**Example:**
```
if (5) {}
```

```
TypeError: cannot use type 'int' as a condition
```

A condition must have type `bool`.

---

### 5.4 Invalid Binary Operator

**Message:**
```
cannot apply operator '<binary_operator>' to types '<type>' and '<type>'
```

**Example:**
```
bool value = true + false;
```

```
TypeError: cannot apply operator '+' to types 'bool' and 'bool'
```

The binary operator cannot be applied to the types of its operands.

---

### 5.5 Invalid Unary Operator

**Message:**
```
cannot apply operator '<unary_operator>' to type '<type>'
```

**Example:**
```
bool value = -true;
```

```
TypeError: cannot apply operator '-' to type 'bool'
```

The unary operator cannot be applied to the operand's type.

---

### 5.6 Non-Assignable Expression

**Message:**
```
cannot apply operator '<unary_operator>' to a non-assignable expression
```

**Example:**
```
int x = ++5;
```

```
TypeError: cannot apply operator '++' to a non-assignable expression
```

Increment and decrement require an assignable expression because they modify its value.

---

### 5.7 Invalid Array Index Type

**Message:**
```
cannot use type '<type>' as an array index
```

**Example:**
```
int[] arr = new int[2];
print( arr[ true ] );
```

```
TypeError: cannot use type 'bool' as an array index
```

The array index must have a valid index type.

---

### 5.8 Invalid Array Indexing

**Message:**
```
cannot index a value of type '<type>'
```

**Example:**
```
int x = 5;
int y = x[0];
```

```
TypeError: cannot index a value of type 'int'
```

Indexing can only be applied to an array.

---

### 5.9 Invalid Array Initialiser Element

**Message:**
```
cannot initialise an array element of type '<type>' with type '<type>'
```

**Example:**
```
int[] arr = new int[2]{ 5.5f };
```

```
TypeError: cannot initialise an array element of type 'int' with type 'float'
```

The value used to initialise an array element is not compatible with the array's element type.

---

### 5.10 Invalid Cast

**Message:**
```
cannot cast from type '<type>' to type '<type>'
```

**Example:**
```
bool value = true;
int x = (int) value;
```

```
TypeError: cannot cast from type 'bool' to type 'int'
```

The language does not allow a cast between the specified types.

---
