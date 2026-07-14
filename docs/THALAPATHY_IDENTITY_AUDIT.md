# THALAPATHY Language Identity Audit

> STEP 1 of the syntax-THALAPATHIFICATION mission. Grounded in the **actual**
> source (`src/lexer/Lexer.cpp`, `src/parser/Parser.cpp`), not assumptions.
> Canonical = THALAPATHY spelling is primary; English spellings are **legacy
> compatibility aliases** that still compile.

Status legend: ✅ implemented & verified · 🔶 legacy-only (canonical alias
pending) · ⏳ needs new semantics (staged) · ❌ not implemented.

## Keywords & literals

| Category | Current token | Visible in `.tvk` | Canonical THALAPATHY | Legacy alias | Collision risk | Status |
|---|---|---|---|---|---|---|
| Program entry | `thalapathy aarambam` | yes | `thalapathy aarambam` | — | — | ✅ already native |
| Immutable binding | `nanba` | yes | `nanba` | — | — | ✅ native |
| Mutable binding | `makkal` | yes | `makkal` | — | — | ✅ native |
| Constant | `uruthi` | yes | `uruthi` | — | — | ✅ native |
| Function | `mersal` | yes | `mersal` | — | — | ✅ native |
| Return | `thiruppi` | yes | `thiruppi` | — | — | ✅ native |
| If / else | `ghilli`/`illana` | yes | `ghilli`/`illana` | — | — | ✅ native |
| For | `bigil` | yes | `bigil` | — | — | ✅ native |
| Foreach | `vaathi … in` | yes | `vaathi … ulla` | `in` | `ulla`≠`ullam` (parser exact-match) | ✅ **added this pass** |
| While | `thuppakki` | yes | `thuppakki` | — | — | ✅ native |
| Switch | `thalaivaa`/`vazhakku` | yes | `thalaivaa`/`vazhakku` | — | — | ✅ native |
| Class | `master` | yes | `master` | — | — | ✅ native |
| Inherit | `varisu` | yes | `varisu` | — | — | ✅ native |
| Interface | `kaththi` | yes | `kaththi` | — | — | ✅ native |
| Lambda | `kutty` | yes | `kutty` | — | — | ✅ native |
| Generics | `goat` | yes | `goat` | — | — | ✅ native |
| Type test | `aaguma` | yes | `aaguma` | — | — | ✅ native |
| Getter/Setter | `paaru`/`maathu` | yes | `paaru`/`maathu` | — | — | ✅ native |
| Try/Catch/Throw | `pokkiri`/`kaavalan`/`theri` | yes | same | — | — | ✅ native |
| Finally | `kadaisi` | yes | `kadaisi` | (`mudichitu` proposed) | — | ✅ native (rename optional) |
| **true** | `true` | yes | **`aama`** | `true` | `illa`≠`illana` ✓ | ✅ **added this pass** |
| **false** | `false` | yes | **`illa`** | `false` | whole-word ✓ | ✅ **added this pass** |
| **null** | `null` | yes | **`onnumilla`** | `null` | — | ✅ **added this pass** |
| **this** | `this` | yes | **`naan`** | `this` | `naan`≠`naanga` ✓ | ✅ **added this pass** |
| **break** | `break` | yes | **`interval`** | `break` | — | ✅ **added this pass** |
| **continue** | `continue` | yes | **`aduthu`** | `continue` | — | ✅ **added this pass** |
| Cast | `as` | yes | `maathiko` | `as` | — | ✅ alias added (limited by `as` grammar) |
| Static | `static` | yes | `podhu` | `static` | — | ✅ works on fields (`podhu int x;`) and members |

## Staged canonical constructs (need real new semantics — NOT shipped as fake)

| Concept | Canonical | Blocker |
|---|---|---|
| Parent ref / super ctor | `munnadi.aarambam(...)` | `super.init` runtime defect (THALA-OOP-001); `munnadi` also collides with `kootam.munnadi` method — needs contextual resolution |
| Constructor decl | `aarambam(...)` inside `master` | contextual grammar vs `thalapathy aarambam`; currently `init` |
| Override | `comeback` | new modifier + resolver signature check |
| Final | `mudivu` | collides with `satham.mudivu`; needs modifier + resolver |
| Abstract | `waiting` (already) | keep; must NOT reuse for await |
| Async / await | `varum` / `kaathiru` | real scheduler/continuation runtime (no blocking join) |
| Pattern match | `yaaru`/`ivan`/`yaarumilla` | new statement + exhaustiveness |
| Destructuring | `nanbi [...]` | new AST + resolver + interp (see ROADMAP Phase 2.9) |
| Generic constraints | `thaguthi` | resolver constraint validation |

## Interpreter defects found during this audit
- **THALA-LOGIC-001**: `&&`/`||` not short-circuiting — ✅ **FIXED**.
- **THALA-OOP-001**: `super.init(...)` unresolved — ✅ **FIXED** (also
  `super.aarambam(...)`).

(A suspected `static`-field grammar gap turned out to be a false alarm: fields
are type-prefixed, so `podhu int x;` is correct and `static makkal x;` is just
invalid — `makkal` is a local declarator, not a field one.)

Both fixes are recorded in [ROADMAP.md](ROADMAP.md).
