# THALAPATHY Programming Language: Definitive Guide & Reference Manual

THALAPATHY is a gradually-typed, object-oriented, modular programming language designed as a functional fan tribute to the iconic Indian actor and politician Thalapathy Vijay. It maps standard structural and object-oriented paradigms onto keywords derived from Vijay's blockbuster films, fan terms, and political dialogue memes.

---

## Table of Contents
1. [Environment & Installation](#1-environment--installation)
2. [Ecosystem Tools (TDK)](#2-ecosystem-tools-tdk)
3. [Language Basics & Variables](#3-language-basics--variables)
4. [Primitive & Gradual Types](#4-primitive--gradual-types)
5. [Control Flow structures](#5-control-flow-structures)
6. [Collections (Arrays & Maps)](#6-collections-arrays--maps)
7. [Functions & Recursion](#7-functions--recursion)
8. [Object-Oriented Programming (OOP)](#8-object-oriented-programming-oop)
9. [Error & Exception Handling](#9-error--exception-handling)
10. [Module System (`sarkar`)](#10-module-system-sarkar)
11. [Standard Library Reference](#11-standard-library-reference)
12. [Global Built-in Functions](#12-global-built-in-functions)
13. [Personality Engines & Satire Modes](#13-personality-engines--satire-modes)

---

## 1. Environment & Installation

The THALAPATHY Development Kit (TDK) handles compilation, code checking, formatting, and runtime interpretation.

### Installation
To install the TDK distribution into a path of your choice:
```bash
# 1. Build release binaries
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release -G Ninja
cmake --build build-release --config Release

# 2. Deploy distribution layout
cmake --install build-release --prefix "D:\TDK-1.0.0"
```

### Environment Variables
Configure the following environment variables to activate the toolchain:
- **`TDK_HOME`**: Must point to the root directory of the installed TDK (`D:\TDK-1.0.0`).
- **`PATH`**: Add `$TDK_HOME/bin` to your system executable lookup paths.

---

## 2. Ecosystem Tools (TDK)

The TDK packages four main tools under `bin/`:

### `thalapathy` (Language Driver)
The primary execution command.
- **Run directly**: `thalapathy script.tvk`
- **Verify syntax and types**: `thalapathy --check script.tvk`
- **Compile to VEAF bytecode**: `thalapathy --build script.tvk`
- **Emit intermediate representation**: `thalapathy --emit-ir script.tvk`
- **Start interactive REPL**: `thalapathy --repl`

### `tdk` (Project Manager)
Manages localized project builds and environment health checks.
- **`tdk doctor`**: Run toolchain integrity checks.
- **`tdk new <name>`**: Initialize a new project directory scaffolded with `thalapathy.toml`.
- **`tdk check` / `tdk build` / `tdk run`**: Build and execute project packages.

### `vijaydump` (Disassembler)
Inspects compiled `.vijay` binary artifacts.
- **Show headers**: `vijaydump program.vijay --header`
- **Format as JSON**: `vijaydump program.vijay --json`

### `thalafmt` (Formatter)
Auto-formats code conforming to official formatting standards.
- **In-place formatting**: `thalafmt script.tvk --write`
- **Integrity checking**: `thalafmt script.tvk --check`

---

## 3. Language Basics & Variables

### Entry Block Structure
Every executable THALAPATHY script must contain one `thalapathy aarambam` entry block, representing the program's starting point:
```tvk
thalapathy aarambam {
    sollu("Aarambikalama?");
}
```

### Comments
- **Single-line**: `// This is a comment`
- **Multi-line**: `/* This is a block comment */`

### Local Variables
Variables are declared using one of three keywords defining their mutability, or via explicit type annotations:

1. **`nanba`** (Immutable reference):
   ```tvk
   nanba name = "Vijay";
   // name = "Leo"; // Error: Cannot reassign to immutable binding
   ```
2. **`makkal`** (Mutable reference):
   ```tvk
   makkal score = 100;
   score = score + 50; // OK
   ```
3. **`uruthi`** (Compile-time constant):
   ```tvk
   uruthi PI = 3.14159;
   ```
4. **Explicit Type Annotation**:
   ```tvk
   int year = 2026; // Mutably bound integer
   string title = "GOAT"; // Mutably bound string
   ```

---

## 4. Primitive & Gradual Types

THALAPATHY features static type checking combined with gradual type bypasses.

| Type keyword | Representation | Example Value |
|:---|:---|:---|
| `int` | 64-bit signed integer | `42` |
| `float` | 64-bit floating point number | `3.14` |
| `bool` | Boolean state flag | `true` or `false` |
| `char` | Single character | `'V'` |
| `string` | UTF-8 sequence of characters | `"Thalapathy"` |
| `void` | Used for function return signatures | N/A |
| `null` | Unassigned state reference | `null` |
| `any` / `goat` | Gradual override (bypasses static check) | Dynamic type resolution |

---

## 5. Control Flow Structures

### If-Else Statements (`ghilli` / `illana`)
Performs conditional routing logic:
```tvk
int time = 17;
ghilli (time < 12) {
    sollu("Morning!");
} illana {
    sollu("Evening!");
}
```

### C-Style Loops (`bigil`)
Enforces standard three-part iteration bounds:
```tvk
bigil (makkal i = 0; i < 5; i = i + 1) {
    sollu("Count: ", i);
}
```

### Iterator/Range Loops (`vaathi`)
Iterates over index collections or ranges:
```tvk
vaathi i in range(1, 4) {
    sollu("Index: ", i); // Prints 1, 2, 3
}
```

---

## 6. Collections (Arrays & Maps)

### Arrays
Arrays are dynamic list vectors:
```tvk
makkal movies = ["Ghilli", "Theri", "Mersal"];
sollu(movies[0]); // Prints "Ghilli"

movies[1] = "Leo";
movies.push("Master"); // Add to array
sollu(len(movies));    // Prints 4

movies.pop();          // Removes last element
```

### Maps
Maps represent key-value dictionaries:
```tvk
makkal actor = {
    name: "Vijay",
    party: "TVK"
};
sollu(actor.name);      // Dot-accessor prints "Vijay"
sollu(actor["party"]);  // Bracket-accessor prints "TVK"
actor.party = "TVK-HQ"; // Mutation
```

---

## 7. Functions & Recursion

Declared using `mersal` with explicit parameters, parameter types, optional `-> returnType` arrow notations, and `thiruppi` return statements:

```tvk
mersal multiply(int a, int b) -> int {
    thiruppi a * b;
}

thalapathy aarambam {
    sollu(multiply(5, 6)); // Prints 30
}
```

### Recursion Example
Functions support recursive stack frames:
```tvk
mersal factorial(int n) -> int {
    ghilli (n <= 1) {
        thiruppi 1;
    }
    thiruppi n * factorial(n - 1);
}
```

---

## 8. Object-Oriented Programming (OOP)

Classes are the core of THALAPATHY's structure.

### Keywords & OOP Concepts
- **`master`**: Class declaration keyword.
- **`varisu`**: Defines single-class inheritance.
- **`kaththi`**: Specifies interface implementations.
- **`init`**: Special constructor method.
- **`this` / `super`**: References current instance or parent class instance members.
- **`beast` / `leo`**: Private and Protected member visibility modifiers (default is public).
- **`static`**: Declares static variables or static methods.
- **`uruthi`**: In context of classes, marks fields or methods as final (cannot be overridden).
- **`waiting`**: Declares abstract classes or abstract methods.

### Concrete Class Declaration
```tvk
master Animal {
    beast string name; // Private field

    mersal init(string name) {
        this.name = name;
    }

    mersal speak() {
        sollu(this.name, " makes a noise.");
    }
}
```

### Class Inheritance & Super Calls
```tvk
master Dog varisu Animal {
    mersal init(string name) : super(name) {}

    mersal speak() {
        sollu("Dog barks.");
    }
}
```

### Properties (`seyal` properties with `paaru` & `maathu`)
Properties expose custom getter (`paaru`) and setter (`maathu`) accessors encapsulating fields:
```tvk
master Player {
    beast int scoreVal;

    seyal int score {
        paaru {
            thiruppi this.scoreVal;
        }
        maathu(value) {
            ghilli (value >= 0) {
                this.scoreVal = value;
            }
        }
    }
}
```

### Interface Specifications
Interfaces enforce method signature contracts without defining their implementation:
```tvk
kaththi Runnable {
    mersal run() -> void;
}

master Thread kaththi Runnable {
    mersal run() -> void {
        sollu("Thread running!");
    }
}
```

### Operator Overloading
You can override arithmetic and relational operators (`+`, `-`, `*`, `/`, `%`, `<`, `>`) inside class definitions:
```tvk
master Complex {
    float r;
    float i;

    mersal init(float r, float i) {
        this.r = r;
        this.i = i;
    }

    mersal operator+(Complex other) -> Complex {
        thiruppi Complex(this.r + other.r, this.i + other.i);
    }
}
```

---

## 9. Error & Exception Handling

Standard exception throwing uses `theri`, caught via `pokkiri`-`kaavalan` try-catch blocks:

```tvk
master NetworkError {
    string msg;
    mersal init(string msg) {
        this.msg = msg;
    }
}

thalapathy aarambam {
    pokkiri {
        theri NetworkError("Connection lost");
    } kaavalan(e) {
        sollu("Caught network exception: ", e.msg);
    }
}
```

---

## 10. Module System (`sarkar`)

Modular code files can import siblings or standard library scripts using `sarkar`:

```tvk
sarkar math;
sarkar io;

thalapathy aarambam {
    float x = abs(-5.5);
    ezhudhu(string(x)); // Prints 5.5
}
```

### Resolution Order
When `sarkar math;` is encountered:
1. Resolves `math.tvk` in the directory of the current file.
2. Resolves `math.tvk` in the current working directory.
3. Resolves standard library modules under `$TDK_HOME/lib/std/`.

### DFS Cycle Detection
The resolution module graph uses strict tracking states to catch circular imports:
- **UNSEEN**: Module hasn't been scanned.
- **LOADING**: Currently processing dependencies (hits here if recursive loop occurs, throwing `Circular dependency detected`).
- **LOADED**: Safe to reuse resolved nodes.
- **FAILED**: Failures propagate safely.

---

## 11. Standard Library Reference

### 1. `io` (`sarkar io;`)
- **`ezhudhu(string msg)`**: Writes string to standard output.
- **`vaasi() -> string`**: Reads a line of input from standard input.

### 2. `math` (`sarkar math;`)
- **`abs(float x) -> float`**: Returns absolute value.
- **`max(float a, float b) -> float`**: Returns larger value.
- **`min(float a, float b) -> float`**: Returns smaller value.
- **`pow(float base, int exp) -> float`**: Returns base^exp.
- **`fact(int n) -> int`**: Factorial of `n`.

### 3. `text` (`sarkar text;`)
- **`inai(string a, string b) -> string`**: Concatenates two strings.
- **`empty(string s) -> bool`**: Returns `true` if string is empty.

### 4. `collections` (`sarkar collections;`)
- **`Stack`** (Class):
  - **`push(var item)`**: Push element onto stack.
  - **`pop() -> var`**: Pops element.
  - **`isEmpty() -> bool`**: Checks if empty.

### 5. `time` (`sarkar time;`)
- **`now() -> int`**: Returns current epoch time in milliseconds.

---

## 12. Global Built-in Functions

The runtime exposes global built-ins available in any context:

- **`sollu(...)`**: Prints variable parameters separated by space to stdout, ending with a newline.
- **`len(value)`**: Returns element count for arrays, maps, or characters in strings.
- **`range(int start, int end)`**: Returns an array of ints spanning `[start, end)`.
- **`type(value)`**: Returns a string representation of the value type (`int`, `float`, `bool`, `char`, `string`, `array`, `map`, `function`, `class`, `instance`).
- **`string(value)`**: Converts value to string representation.
- **`int(value)`**: Casts numeric/string representation to 64-bit integer.
- **`float(value)`**: Casts numeric/string representation to double.

---

## 13. Personality Engines & Satire Modes

When compiling or running code, diagnostic and compiler outputs are decorated based on the `--mode` flag:

1. **`professional`**: Clean, direct, formal compile messages (Default).
2. **`mass`**: Humorous cinema hype messages, reference dialogues:
   - Successful check: `"Nanba... syntax clean. Compiler green signal kuduthachu. 🔥😂"`
   - Resolver errors decorated with movie style hints.
3. **`political-parody`**: Absurd local political satire, incorporating party funds, news broadcast jokes, and legislative warnings.
