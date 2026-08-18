# Lexical Structure

## Contents
- [1. Overview](#1-overview)
- [2. Whitespace](#2-whitespace)
- [3. Delimiters](#3-delimiters)
- [4. Operators](#4-operators)
- [5. Keywords](#5-keywords)
- [6. Identifiers](#6-identifiers)
- [7. Literals](#7-literals)
  - [7.1 Integer Literals](#71-integer-literals)
  - [7.2 Float Literals](#72-float-literals)
  - [7.3 Boolean Literals](#73-boolean-literals)

---

## 1. Overview

The lexical structure of SV defines how source code is divided into tokens.

The tokeniser reads SV source code and converts it into a sequence of tokens. Each token represents a lexical element.

Lexical rules define the valid forms of these elements. The syntactic structure of SV is defined by `LanguageGrammar.ebnf`

---

## 2. Whitespace

Whitespace separates lexical elements and is otherwise ignored by the tokeniser.

Whitespace includes spaces, tabs, and newline.

These two programs produce the same sequence of tokens:
```
int x = 10;
```

```
int         x
       =
    10;
```

---

## 3. Delimiters

SV provides the following delimiters:

| Token   | Character |
|---------|-----------|
| `SEMI`  | `;`       |
| `COMMA` | `,`       |
| `LBR`   | `(`       |
| `RBR`   | `)`       |
| `LCBR`  | `{`       |
| `RCBR`  | `}`       |

---

## 4. Operators

SV provides the following operators

| Token                   | Character |
|-------------------------|-----------|
| `EQUAL`                 | `=`       |
| `PLUS`                  | `+`       |
| `MINUS`                 | `-`       |
| `MULTIPLY`              | `*`       |
| `DIVIDE`                | `/`       |
| `INTEGER_DIVIDE`        | `//`      |
| `MODULO`                | `%`       |
| `LOGICAL_OR`            | `\|\|`    |
| `LOGICAL_AND`           | `&&`      |
| `LOGICAL_NOT`           | `!`       |
| `EQUAL_EQUAL`           | `==`      |
| `NOT_EQUAL`             | `!=`      |
| `LESS_THAN`             | `<`       |
| `LESS_THAN_OR_EQUAL`    | `<=`      |
| `GREATER_THAN`          | `>`       |
| `GREATER_THAN_OR_EQUAL` | `>=`      |

---

## 5. Keywords

The following sequences are reserved as keywords:

| Token          | Keyword    |
|----------------|------------|
| `IF`           | `if`       |
| `ELSE`         | `else`     |
| `WHILE`        | `while`    |
| `CONTINUE`     | `continue` |
| `BREAK`        | `break`    |
| `RETURN`       | `return`   |
| `INTEGER_TYPE` | `int`      |
| `FLOAT_TYPE`   | `float`    |
| `BOOL_TYPE`    | `bool`     |
| `VOID_TYPE`    | `void`     |

Keywords have a predefined meaning in the SV language and cannot be used as identifiers.

---

## 6. Identifiers

Identifiers are used to name variables and functions.

An identifier must begin with an ASCII letter or underscore and may be followed by any number of ASCII letters, digits, or underscores.

The lexical form of an identifier is:
```
(?!__)[a-zA-Z_][a-zA-Z0-9_]*
```

For example:
```
x
value
myVariable
value2
_my_var
```
are valid identifiers.

The following are invalid:
```
2value
my-var
__x
```

Identifiers are case-sensitive.

Identifiers beginning with `__` are reserved for compiler use and cannot be declared by user programs.

---

## 7. Literals

### 7.1 Integer Literals

An integer literal consists of one or more decimal digits.

The lexical form of an integer literal is:
```
[0-9]+
```

Examples:
```
0
10
42
123456
```

Integer literals do not contain a sign.

A sign is represented by a separate unary operator.

### 7.2 Float Literals

A float literal consists of:
1. One or more decimal digits
2. A decimal point `.`
3. One or more decimal digits
4. The suffix `f`

The lexical form of a float literal is:
```
[0-9]+\.[0-9]+f
```

Examples:
```
0.0f
1.5f
10.25f
123.456f
```

Float literals do not contain a sign.

A sign is represented by a separate unary operator.

### 7.3 Boolean Literals

SV provides two boolean literals:
- `true`
- `false`
