# THALAPATHY Standard Library

The THALAPATHY Development Kit (TDK) distributes a standard library containing utility modules located under `$TDK_HOME/lib/std/`.

---

## Standard Modules

### 1. `io` (`sarkar io;`)
Provides basic standard input and output utilities.
- `ezhudhu(string msg)`: Writes string to stdout.
- `vaasi()`: Reads a line from stdin.

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
