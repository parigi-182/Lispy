# Lispy
Lispy is a minimal Lisp interpreter written in C, following the tutorial ["Build Your Own Lisp"](http://www.buildyourownlisp.com/) by Daniel Holden.

## Fetaures
- **Lisp-like syntax** with S-Expressions and Q-Expressions
- **Dynamic typing** with different value types:
  - Numbers (integers)
  - Symbols
  - Strings
  - Lists (S-Expressions and Q-Expressions)
  - Functions (built-in and user-defined)
- **Lexical scoping** with environments
- **File loading** capability
- **Error handling** with messages
- **Standard library** of built-in functions:
  - Arithmetic operations (`+`, `-`, `*`, `/`)
  - Comparison operations (`>`, `<`, `==`, etc.)
  - List operations (`head`, `tail`, `join`, `eval`)
  - Variable definition (`def`, `=`)
  - Lambda functions (`\`)
  - Control flow (`if`)
  - I/O (`print`, `error`, `load`)


## Screenshots
![none](images/img01.png)
![none](images/img02.png)


## How It Works

Lispy follows the traditional interpreter architecture:

1. **Parsing**: Input strings are parsed into abstract syntax trees using the [mpc](https://github.com/orangeduck/mpc) parser combinator library.
2. **Evaluation**: The parsed expressions are evaluated according to Lisp semantics:
   - Symbols are looked up in the environment
   - S-Expressions are evaluated as function calls
   - Q-Expressions are treated as literal lists
3. **Environment**: Variables and functions are stored in environments that support lexical scoping.

### Key Components

- **lval**: The value system that represents all possible types in the language
- **lenv**: The environment system that handles variable storage and lookup
- **Builtins**: Core functions implemented in C
- **REPL**: The interactive read-eval-print loop
