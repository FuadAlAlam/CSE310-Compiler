# CSE 310: Compiler Sessional

A comprehensive, step-by-step implementation of a compiler pipeline for a subset of the **C programming language**, developed across four sequential assignments for the **CSE 310 (Compiler Sessional)** course.

The project incrementally builds from low-level symbol table management to full intermediate code generation targeting **8086 Assembly**, including AST-based semantic analysis and peephole optimization.

---

## 🏗️ Compiler Pipeline Architecture

```
+-------------------------------------------------------------------------+
|                              Source Code (.c)                           |
+-------------------------------------------------------------------------+
                                     │
                                     ▼
+-------------------------------------------------------------------------+
| Assignment 2: Lexical Analyzer (Flex / ANTLR Lexer)                     |
| - Token identification (Keywords, Identifiers, Literals, Operators)     |
| - Comment & string literal processing, error handling                   |
+-------------------------------------------------------------------------+
                                     │ Tokens
                                     ▼
+-------------------------------------------------------------------------+
| Assignment 3: Syntax & Semantic Analyzer (ANTLR4 Parser + Visitor)       |
| - Grammar parsing (Context-Free Grammar)                                |
| - Type checking, scope resolution, function parameter validation        |
| - Backed by Assignment 1: Hierarchical Symbol Table                     |
+-------------------------------------------------------------------------+
                                     │ Annotated AST / Parse Tree
                                     ▼
+-------------------------------------------------------------------------+
| Assignment 4: Intermediate Code Generator (ICG) & Optimizer             |
| - 8086 Assembly code emission (Expressions, Control Flow, Functions)     |
| - Stack frame & memory offset layout                                    |
| - Peephole optimizer (Redundant load/store & jump elimination)          |
+-------------------------------------------------------------------------+
                                     │
                                     ▼
+-------------------------------------------------------------------------+
|                  Target 8086 Assembly Code (.asm)                       |
+-------------------------------------------------------------------------+
```

---

## 📚 Assignments Breakdown

### 1. [Symbol Table](SymbolTable/)
Implementation of a hierarchical, scope-aware symbol table data structure required by all compiler phases to track identifier bindings and declarations.

- **Key Components:**
  - `SymbolInfo`: Stores symbol metadata (name, type, return type, parameter list, array specifiers).
  - `ScopeTable`: Implemented via hash table with separate chaining; dynamically manages unique scope IDs (`1`, `1.1`, `1.2`, etc.).
  - `SymbolTable`: Manages the stack of active `ScopeTable` instances, entering/exiting scopes and performing recursive lookups across nested scopes.
- **Specification:** [SymbolTableSpec.pdf](SymbolTable/Specification/SymbolTableSpec.pdf)

---

### 2. [Lexical Analyzer](LexicalAnalyzer/)
A lexical scanner built using **Flex (Fast Lexical Analyzer)** that processes raw C source code into a stream of tokens.

- **Key Highlights:**
  - Tokenizes keywords, identifiers, integer/floating-point literals, character constants, and operators.
  - Robust handling of multi-line comments (`/* ... */`), single-line comments (`// ...`), and escape sequences in strings/character literals.
  - Integrated symbol table lookups for identifiers upon tokenization.
  - Comprehensive lexical error detection (e.g., unrecognized characters, multi-character constants, unfinished strings/comments).
- **Specification:** [Assignment on Lexical Analysis.pdf](LexicalAnalyzer/Specification/Assignment%20on%20Lexical%20Analysis.pdf)

---

### 3. [Syntax & Semantic Analyzer](Syntax&SemanticAnalyzer/)
A parser and static semantic analyzer implemented using **ANTLR4** with a C++ visitor pattern (`CSubsetVisitorImpl`).

- **Key Highlights:**
  - **Grammar Specification**: Defined context-free grammar rules for C subset constructs in ANTLR4 (`.g4`).
  - **Type Checking & Type Casting**: Validates type compatibility in expressions, assignments, array indexing, and conditional statements.
  - **Scope & Declaration Checks**: Detects undeclared variables/functions, multiple declarations within the same scope, and conflicting function prototypes.
  - **Function Validation**: Verifies function argument counts, types, and return type consistency against declarations and definitions.
  - **Error Logging**: Generates detailed syntax and semantic error reports with line numbers.
- **Specification:** [AntlrSpec.pdf](Syntax&SemanticAnalyzer/Specification/AntlrSpec.pdf) | [ANTLR Reference Guide](Syntax&SemanticAnalyzer/Specification/Terence%20Parr%20-%20The%20Definitive%20ANTLR%204%20Reference,%202nd%20Edition-Pragmatic%20Bookshelf%20(2013).pdf)

---

### 4. [Intermediate Code Generator (ICG)](IntermediateCodeGenerator/)
Generates executable **8086 Assembly** code from the validated parse tree and AST, with automated peephole optimization.

- **Key Highlights:**
  - **Code Generation (`ICGVisitor`)**:
    - Translates expressions (arithmetic, relational, logical) into 8086 register operations (`AX`, `BX`, `CX`, `DX`).
    - Emits structured control flow primitives for `if-else`, `while`, and `for` loops using labels and conditional jumps.
    - Manages function activation records, stack frames (`BP`, `SP`), local variable offsets, and parameter passing.
    - Generates array access logic with dynamic offset computation.
  - **Peephole Optimization (`PeepholeOptimizer`)**:
    - Removes redundant instructions (e.g., consecutive `MOV [a], AX` followed by `MOV AX, [a]`).
    - Eliminates unreachable code and redundant jump instructions.
- **Specification:** [CSE_310_Januay_2026_ICG_Spec.pdf](IntermediateCodeGenerator/Specification/CSE_310_Januay_2026_ICG_Spec.pdf) | [Grammar Specification](IntermediateCodeGenerator/Assignment%204%20ICG%20Resources/P1_Grammar.pdf)

---

## ⚙️ Prerequisites & Environment

- **Operating System**: Linux / Unix-like environment
- **C++ Compiler**: `g++` (supporting C++17 or higher)
- **Lexer Generator**: `Flex` (v2.6+)
- **Parser Generator**: `ANTLR v4` with C++ runtime installed
- **Assembler / Emulator (Optional for testing ICG output)**: `EMU8086` / `DOSBox` with `MASM` or `TASM`

---

## 🚀 Build and Run Instructions

### 1. Symbol Table
```bash
cd SymbolTable
./2205167_build.sh
./symbol_table.out input.txt output.txt
```

### 2. Lexical Analyzer
```bash
cd LexicalAnalyzer
./2205167_build.sh
./lexer.out input1.txt
```

### 3. Syntax & Semantic Analyzer
```bash
cd Syntax&SemanticAnalyzer/2205167
./run-script.sh
```

### 4. Intermediate Code Generator
```bash
cd IntermediateCodeGenerator/2205167
./run.sh input.c
```
*(Clean build artifacts using `./clean-script.sh` when needed)*
