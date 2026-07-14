# THALAPATHY Standard Library

The THALAPATHY Development Kit (TDK) distributes a standard library containing utility modules located under `$TDK_HOME/lib/std/`.

> **Canonical API reference:** [VIJAY_STDLIB_NAMING_MATRIX.md](VIJAY_STDLIB_NAMING_MATRIX.md) · [THALAPATHY_API_LEXICON.md](THALAPATHY_API_LEXICON.md) · Doc hub: [README.md](README.md)

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

---

## Vijay-Native Modules (canonical)

The modules above are the original/compat set. The **canonical** THALAPATHY
standard library is Vijay cinema-native: each module is a singleton object, so
you write `sarkar <module>; <module>.<method>(...)`. Full API tables live in
[VIJAY_STDLIB_NAMING_MATRIX.md](VIJAY_STDLIB_NAMING_MATRIX.md); vocabulary and
collision analysis in [THALAPATHY_API_LEXICON.md](THALAPATHY_API_LEXICON.md).

| `sarkar` | Area | Conventional | Example |
|----------|------|--------------|---------|
| `vaanganna` | I/O (input) | stdin | `vaanganna.kelu("Un peru? ")` |
| `sivakasi` | Math | math | `sivakasi.mulai(169.0)` → 13 |
| `vaseegara` | Text | string | `vaseegara.perusaSollu(s)` |
| `kootam` | Collections | stack/queue/set/deque | `kootam.adukku()` |
| `velayudham` | Filesystem | fs | `velayudham.padichu(path)` |
| `udhaya` | Path | path | `udhaya.peru(p)` |
| `jilla` | JSON | json | `jilla.purinjiko(text)` |
| `vettaikaaran` | Regex | regex | `vettaikaaran.thediPidi(pat, txt)` |
| `deva` | Random | random | `deva.number(1, 100)` |
| `neram` | Time | time | `neram.ippo()` |
| `satham` | Logging | logging | `satham.voice("app").satham(msg)` |
| `arasu` | System | os/sys | `arasu.naadu()` |
| `villu` | HTTP | http client | `villu.kelu(url)` |
| `ozhungu` | Algorithms | `<algorithm>` | `ozhungu.ozhunguPannu(list)` |
| `kootu` | Functional | functional | `kootu.vadikattu(list, fn)` |

Runnable end-to-end demo: [`examples/mass/full_language.tvk`](../examples/mass/full_language.tvk).

### Web & data (Vijay-native)

| `sarkar` | Area | Example |
|----------|------|---------|
| `thalaivalai` | Web app framework | `thalaivalai.pudhu()` |
| `bathil` | HTTP responses | `bathil.jilla(obj)` |
| `kalanjiyam` | Document DB | `kalanjiyam.thira("heroes")` |
| `muthirai` | SHA-256 hashing | `muthirai.sha256(data)` |
| `adayalam` | UUID-shaped IDs | `adayalam.pudhu()` |
| `yaarunee` | Value reflection | `yaarunee.enna(x)` |
| `irukkalaam` | Option / Result | `Irukkalaam.irukku(v)` |

Docs: [THALAIVALAI_WEB.md](THALAIVALAI_WEB.md) · API tables: [VIJAY_STDLIB_NAMING_MATRIX.md](VIJAY_STDLIB_NAMING_MATRIX.md).

### Legacy compatibility modules

These English-named modules remain for backward compatibility. **New code should use the Vijay-native names above.**

| Legacy | Prefer |
|--------|--------|
| `io` | `vaanganna` |
| `math` | `sivakasi` |
| `text` | `vaseegara` |
| `collections` | `kootam` |
| `json` | `jilla` |
| `http` | `villu` |
| `web` | `thalaivalai` (new apps) |
| `db` | `kalanjiyam` (new apps) |
| `regex` | `vettaikaaran` |
| `random` | `deva` |
| `time` / `datetime` | `neram` |
| `log` | `satham` |
| `os` | `arasu` |

> Method names are ASCII and avoid reserved grammar tokens (e.g. logging uses
> `kannu`/`pizhai` instead of the reserved `paaru`/`theri`; HTTP PUT is
> `villu.puduppi` because `maathu` is reserved). Printed strings stay Tamil-lish.
