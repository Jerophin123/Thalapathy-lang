# THALAPATHY API LEXICON — Canonical Vijay-Native Vocabulary

**Version 1.0.0** · Companion: [VIJAY_STDLIB_NAMING_MATRIX.md](VIJAY_STDLIB_NAMING_MATRIX.md) · [README.md](README.md)

> "Idhu THALAPATHY da." 🔥
> This document is the **single source of truth** for every public-facing name in
> the THALAPATHY standard library. Nothing gets renamed, aliased, or implemented
> until it is listed here with a collision analysis.

The rule: **the THALAPATHY-native name is canonical.** English names, where they
exist, are *documented compatibility aliases only* — they resolve to the same
implementation, never a duplicate.

---

## 0. How to read this document

Each entry carries:

| Field | Meaning |
|-------|---------|
| **Identifier** | The canonical THALAPATHY name a developer types |
| **Meaning** | The precise technical behaviour |
| **Category** | `builtin` / `module` / `class` / `method` / `keyword` |
| **Inspiration** | The Vijay film / dialogue / thematic root |
| **Collision** | Result of checking against reserved keywords + existing globals |
| **Case** | THALAPATHY identifiers are **case-sensitive** (`sollu` ≠ `Sollu`) |
| **English alias** | Compatibility alias, if any (never the primary API) |

---

## 1. RESERVED KEYWORD AUDIT (collision baseline)

These names are **already lexer keywords** (`src/lexer/Lexer.cpp`). They are
grammar, not identifiers — a module or function **cannot** be named any of these,
because `sarkar kaththi;` would parse `kaththi` as a keyword, not a module name.

| Keyword | Current grammar role |
|---------|----------------------|
| `thalapathy` `aarambam` | program / entry block |
| `mersal` | function declaration |
| `master` | class declaration |
| `varisu` | inheritance (`extends`) |
| `nanba` `makkal` | variable declaration (typed / inferred) |
| `uruthi` | constant / assertion declaration |
| `ghilli` `illana` | if / else |
| `bigil` | for loop |
| `vaathi` | for-each iteration |
| `thiruppi` | return |
| `sollu` | print builtin (lexer-level) |
| `kaththi` | try / guarded block + member modifier |
| `theri` | throw |
| `kaavalan` | catch / protect |
| `vazhakku` | catch case clause |
| `kadaisi` | finally |
| `beast` `leo` | method modifiers (override / virtual) |
| `seyal` `paaru` `maathu` | property / getter / setter |
| `sarkar` | module import |
| `waiting` | async / await construct |
| `goat` | generic type parameter marker |
| `thuppakki` `thalaivaa` `pokkiri` | reserved expression constructs |
| `nadu` `vagai` `kutty` | block / type-alias / lambda constructs |
| `static` `as` `aaguma` `operator` | modifiers / cast / interface-check |
| `int` `float` `bool` `string` `char` `void` `null` `true` `false` `this` `super` `break` `continue` | primitives / literals |

**Design consequence:** module names are chosen from Vijay film titles that are
**NOT** in the table above. Where the spec suggested a reserved word
(`kaththi` for FS, `thuppakki` for HTTP, `thalaivaa` for path, `pokkiri` for
regex), a **collision-free film title** is substituted and the reason recorded.

> ⚠️ **Do not add new lexer keywords for library names.** Every module and method
> below is a plain identifier resolved through the module system — zero new
> `TokenType` entries.

---

## 2. GLOBAL BUILT-INS

Available everywhere, no `sarkar` import. `sollu`, `len`, `range`, `type`, `int`,
`float`, `string`, `bool`, `char` already exist as globals (`Interpreter.cpp`).

| Identifier | Meaning | Inspiration | Collision | English alias |
|------------|---------|-------------|-----------|---------------|
| `sollu` | print line | "Sollu da" — declare it | keyword (exists) | `println` |
| `sollikko` | print without newline | keep talking | free | `print` |
| `vaanganna` | read a line of input | "vaanga vaanga" — receive | free | `input` |
| `yaaru` | runtime type name of a value | "Yaaru nee?" | free | `typeof` |
| `evlo` | length / size | "evlo?" — how much | free | `len` (exists) |
| `varisai` | integer range sequence | rows in order | free | `range` (exists) |
| `maathu` | type conversion (to int/float/string/bool) | reserved keyword → **alias only via `int()/float()`** | keyword | `int`/`float`/`string` |
| `nகல்` → `nகல்edu` | clone / deep copy | duplicate | free | `clone` |
| `orே` | identity check (same reference) | "oree maadhiri" | free | `is` |
| `hash` | hash code of a value | — | free | `hash` |
| `perusu` | maximum of args | "perusu" — the big one | free | `max` |
| `sirusu` | minimum of args | "sirusu" — the small one | free | `min` |
| `mothham` | sum of an iterable | "mothham" — total | free | `sum` |
| `uruthiPannu` | assert(condition, msg) | "uruthi-ah sollu" | free (fn, not `uruthi` kw) | `assert` |
| `adithozha` | panic / abort with message | "adi thozha!" | free | `panic` |
| `mudichidu` | exit process with code | "mudichiko" — finish it | free | `exit` |
| `kaaliya` | null check (is value null) | "kaali-ah?" — empty? | free | `isNull` |
| `koopidalaam` | is value callable | "koopidalaam-a?" | free | `isCallable` |
| `suthalaam` | is value iterable | "suthi paaru" | free | `isIterable` |

> Note: identifiers must be ASCII to be valid tokens. Any Tamil-script sketch
> above (`nகல்`, `orே`) is a placeholder — the **implemented** names use ASCII:
> `nagaledu` (clone), `oree` (identity). Corrected in the naming matrix.

---

## 3. STANDARD MODULE NAME MAP (collision-checked)

Every module name below is verified **not** a reserved keyword.

| Module | Area | Meaning | Inspiration | Why (and spec substitution) |
|--------|------|---------|-------------|------------------------------|
| `vaanganna` | I/O | say / ask / read | "vaanga, sollu" | Spec's chosen I/O identity. Free. |
| `sivakasi` | Math | firepower & numbers | *Sivakasi* (2005) | Spec suggested `mersal`/`thuppakki`/`ghilli` — all **reserved**. Substituted. |
| `vaseegara` | Text/String | dialogue processing | *Vaseegara* song/theme | Spec's suggestion. Free. |
| `kootam` | Collections | crowd / group structures | "kootam" — the mass | Spec's crowd terminology. Free. |
| `velayudham` | Filesystem | guard & store files | *Velayudham* (2011) | Spec suggested `kaththi` — **reserved**. Substituted (weapon/guardian theme). |
| `udhaya` | Path | journey / navigation | *Udhaya* (2004) | Spec's suggestion. Free. |
| `jilla` | JSON | structured records | *Jilla* (2014) | Spec suggested `master`/`sarkar` — **reserved**. Substituted (district = record set). |
| `varisai` | CSV | tabular rows | "varisai" — the line-up | Rows in order. Free (distinct from `varisu` kw). |
| `vettaikaaran` | Regex | hunting patterns | *Vettaikaaran* (2009) | Spec suggested `pokkiri` — **reserved**. Substituted (hunter theme). |
| `deva` | Random | fate / luck | *Deva* (1995) | Unpredictability theme. Free. |
| `neram` | Time/Date | time & clock | "neram" — the time | Free. `waiting` kw kept for async only. |
| `satham` | Logging | mass announcement | "satham" — the voice | Spec suggested `bigil` — **reserved (loop)**. Substituted. |
| `cupmukkiyam` | Testing | "cup is important" | mass victory chant | Spec's testing identity. Free. |
| `nanban` | Networking | connection / friends | *Nanban* (2012) | Spec's suggestion. Free. |
| `villu` | HTTP | fire requests (bow) | *Villu* (2009) | Spec suggested `thuppakki` — **reserved**. Substituted (bow shoots requests). |
| `padai` | Threading | army / team of workers | "padai" — the force | Free. |
| `varum` | Async | "I'm coming" (resume) | dialogue theme | Pairs with `waiting` kw (await). Free. |
| `yaaru` | Reflection | "who are you?" | dialogue theme | Spec's identity. Shared root with builtin `yaaru`. Free. |
| `pothi` | Serialization | wrap / pack | "pothi vai" — wrap it | Free. |
| `ozhungu` | Algorithms | order / arrange | "ozhungu-ah" | Free. |
| `kootu` | Functional | compose / combine | "kootu" — join | Free. |
| `arasu` | System/Process/Env | government = the system | "arasu" — the state | Spec suggested `sarkar` — **reserved**. Substituted. |
| `maatram` | Encoding/Bytes | transform representation | "maatram" — change | Free. |
| `adayalam` | UUID | unique identity mark | "adayalam" — the mark | Free. |

---

## 4. IMPLEMENTATION READINESS (honesty column)

Native backing already present in `Interpreter.cpp` → **real wrapper, runnable now.**
No fake returns anywhere.

| Module | Native primitive backing | Status |
|--------|--------------------------|--------|
| `vaanganna` | `__native_read_line`, `sollu` | ✅ ready |
| `sivakasi` | `__native_math_sqrt/sin/cos/ceil/floor` | ✅ ready |
| `vaseegara` | `__native_string_split/replace` + interp | ✅ ready |
| `kootam` | list/map value ops (interp builtins) | ✅ ready |
| `velayudham` | `__native_read_file/write_file/file_exists` | ✅ ready |
| `udhaya` | pure `.tvk` string logic | ✅ ready |
| `jilla` | `__native_json_parse/stringify` | ✅ ready |
| `vettaikaaran` | `__native_regex_match/find_all/replace` | ✅ ready |
| `deva` | `__native_random/random_int/random_seed` | ✅ ready |
| `neram` | `__native_time_millis/now`, `__native_sleep_ms`, `__native_date_*` | ✅ ready |
| `satham` | `__native_log` | ✅ ready |
| `arasu` | `__native_os_name/cwd/exit`, `__native_get_env/set_env`, `__native_sys_command` | ✅ ready |
| `villu` | `__native_http_get/post/request` | ✅ ready |
| `varisai` (CSV) | pure `.tvk` (RFC-aware, no `split(",")`) | ✅ ready |
| `cupmukkiyam` (test) | pure `.tvk` + `adithozha` panic | ✅ ready |
| `ozhungu` / `kootu` | pure `.tvk` + lambda support | ✅ ready |
| `pothi` / `maatram` / `adayalam` | needs new native primitive | ⏳ staged |
| `padai` (threading) | needs `std::thread` bridge | ⏳ staged |
| `varum` (async) | needs event-loop bridge (audit `waiting`) | ⏳ staged |
| `nanban` (sockets) | needs socket bridge | ⏳ staged |

Staged modules are **not** shipped with fake bodies. They are tracked in
[ROADMAP.md](ROADMAP.md) with the exact native primitive required.

---

## 5. NAMING PHILOSOPHY (so new APIs stay coherent)

1. **Action verbs are Tanglish imperatives:** `padichu` (read), `ezhudhu`
   (write), `kelu` (ask/get), `anuppu` (send/post), `kandupidi` (find),
   `maathu`(replace), `serthu`(append/join).
2. **Questions end in `-a`/`-ah`:** `irukka` (exists?), `primeah` (is prime?),
   `kaaliya` (is empty?).
3. **Superlatives:** `perusu` (max/big), `sirusu` (min/small),
   `naduvula` (clamp/middle).
4. **No sentence-length names.** If it doesn't fit as real code, reject it.
5. **Learnability first.** Every themed name carries its conventional equivalent
   in [VIJAY_STDLIB_NAMING_MATRIX.md](VIJAY_STDLIB_NAMING_MATRIX.md).
6. **User-facing *strings* are Tamil-lish** (banners, errors, examples) — but
   **method identifiers stay ASCII** so the language is typeable and diffable.
