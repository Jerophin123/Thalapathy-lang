# THALAPATHY Standard Library

The THALAPATHY Development Kit (TDK) distributes a standard library containing utility modules located under `$TDK_HOME/lib/std/`.

---

## Standard Modules

### 1. `io` (`sarkar io;`)
Provides basic standard input, output, and token parsing utilities.
- **Dialogue Entry Streams**:
  - `kettutu_varanum`: Standard Input Entry (from *"Entry-ah dhaan kettutu varanum"*).
  - `nammakku_pudicha_madhiri`: Standard Output Entry (from *"Exit nammakku pudicha madhiri"*).
- **Functions**:
  - `ezhudhu(string msg)`: Writes string to standard output.
  - `vaasi()`: Reads a line from standard input.
- **Classes**:
  - `Scanner`: Token parser for streams.
    - `init(any source)`: Initializes the scanner with an input source (e.g. `kettutu_varanum`).
    - `hasNext() -> bool`: Returns `true` if there are more whitespace-separated tokens.
    - `next() -> string`: Returns the next string token.
    - `nextInt() -> int`: Returns the next integer token.
    - `nextFloat() -> float`: Returns the next float token.
    - `nextLine() -> string`: Clears the current token buffer and reads the next full line from the source.

### 2. `math` (`sarkar math;`)
Provides mathematical utility functions.
- `abs(float x)`: Returns the absolute value.
- `max(float a, float b)`: Returns the maximum of two values.
- `min(float a, float b)`: Returns the minimum of two values.
- `pow(float base, int exp)`: Returns base raised to the power of exp.
- `fact(int n)`: Computes the factorial of a number recursively.

### 3. `text` (`sarkar text;`)
Provides string and text helper utilities.
- `inai(string a, string b)`: Concatenates two strings.
- `empty(string s)`: Returns true if the string length is 0.

### 4. `collections` (`sarkar collections;`)
Provides object collections.
- `Stack`: An OOP Stack implementation. Members:
  - `push(var item)`: Pushes item to stack.
  - `pop()`: Pops and returns the top item.
  - `isEmpty()`: Checks if stack is empty.

### 5. `time` (`sarkar time;`)
Provides system time utilities.
- `now()`: Returns the system epoch time in milliseconds.
