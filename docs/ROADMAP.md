# THALAPATHY Development Roadmap

**Release 1.0.0** · Documentation hub: [README.md](README.md) · Tribute: [TRIBUTE.md](TRIBUTE.md)

## Phase 1 — Minimal core (v0.1.0) [CURRENT]
- AST tree-walking interpreter.
- Core type check resolver.
- Caret diagnostics.
- Custom binary THALA IR compiler and serializer.
- REPL interactive loop.

## Phase 2 — Language extensions [DONE]
- Standard collection method closures — `map`/`filter`/`reduce`/`forEach` and
  string/map methods. [DONE]
- Class properties and multiple interfaces (`kaththi`). [DONE]
- Dynamic operator overloading. [DONE]
- `thuppakki` while loop, `thalaivaa`/`vazhakku` switch. [DONE]
- `kutty` first-class anonymous functions (closures). [DONE]

## Phase 2.5 — Batteries & backend [DONE]
- Standard library: `json`, `http` (API client), `web` (Sarkar backend
  framework: real HTTP/1.1 server), `random`, `os`. [DONE]
- Real TCP networking layer (winsock / BSD sockets). [DONE]
- Stdlib discovery relative to the executable. [DONE]

## Phase 2.6 — Spring-style backend + persistence [DONE]
- `web.Router`: verb methods (`get`/`post`/`put`/`del`), path parameters
  (`/makkal/:id`), and middleware (`use`). [DONE]
- VijayDB embedded document database (`db` module): collections, CRUD,
  `find`/`get`/`count`, JSON persistence, snapshot isolation. [DONE]
- Full CRUD REST API buildable in pure THALAPATHY. [DONE]
- Reserved words usable as map keys / member names. [DONE]

## Phase 2.7 — Modern language ergonomics [DONE]
- `foreach` (`vaathi x in <array|string|map>`). [DONE]
- Ternary operator `?:`. [DONE]
- Default parameters (functions/methods/constructors/`kutty`). [DONE]
- `kadaisi` finally block + typed `kaavalan` catch. [DONE]
- Enums (`vagai`) with `.name()`/`.ordinal()`, switch + `aaguma` support. [DONE]
- IR constant-folding optimizer pass. [DONE]
- Stdlib: `regex`, `datetime`, `log`, and `collections` (Stack/List/Queue/Set). [DONE]

## Phase 2.8 — Vijay-native stdlib identity [IN PROGRESS]
Canonical vocabulary in [THALAPATHY_API_LEXICON.md](THALAPATHY_API_LEXICON.md)
and [VIJAY_STDLIB_NAMING_MATRIX.md](VIJAY_STDLIB_NAMING_MATRIX.md). Each module is
a **singleton object** (`sarkar sivakasi; sivakasi.mulai(x)`) — dotted, cinema-
native, zero new lexer keywords.
- Shipped & runnable (native-backed, verified via `examples/mass/full_language.tvk`):
  `vaanganna` (io), `sivakasi` (math), `vaseegara` (text), `kootam` (collections),
  `velayudham` (fs), `udhaya` (path), `jilla` (json), `vettaikaaran` (regex),
  `deva` (random), `neram` (time), `satham` (log), `arasu` (system),
  `villu` (http), `ozhungu` (algorithms), `kootu` (functional). [DONE]
- Staged (no fake bodies shipped — native primitive required first):
  `varisai` (CSV, pure-tvk RFC parser — WIP), `cupmukkiyam` (testing — WIP),
  `padai` (threading), `varum` (async — audit `waiting`), `nanban` (sockets),
  `pothi` (serialization), `maatram` (base64/hex), `adayalam` (UUID).

### Interpreter correctness bugs found during stdlib bring-up
- **THALA-LOGIC-001:** ✅ **FIXED** — `&&` / `||` now short-circuit; the right
  operand is evaluated only when the left does not decide the result, so
  `i >= 0 && arr[i] > x` is safe. (`Interpreter::visit(BinaryExpr*)`.)
- **THALA-OOP-001:** ✅ **FIXED** — `super.init(...)` / `super.aarambam(...)` now
  invoke the parent constructor on the current instance (new
  `Interpreter::runConstructorOn`, plus a `CallExpr` interception).
- ~~THALA-GRAMMAR-001~~ — **not a bug**: `static`/`podhu` DO work on fields; they
  require the normal type-prefixed field form (`podhu int total;`). `makkal` is a
  local-variable declarator, not a field declarator, so `static makkal x;` is
  simply invalid syntax.

## Phase 2.9 — Canonical THALAPATHY syntax [IN PROGRESS]
Docs: [THALAPATHY_IDENTITY_AUDIT.md](THALAPATHY_IDENTITY_AUDIT.md),
[THALAPATHY_ACTION_VOCABULARY.md](THALAPATHY_ACTION_VOCABULARY.md).
- Shipped & verified (`examples/mass/thalaiva_syntax.tvk`, runs + builds VJAY):
  `aama`/`illa`/`onnumilla` literals, `naan` (this), `ulla` (foreach `in`),
  `interval` (break), `aduthu` (continue), `podhu`/`maathiko` aliases. English
  spellings retained as legacy aliases. No new lexer keyword bloat, regression
  green (55/56, unchanged). [DONE]
- ✅ **`nanbi` destructuring SHIPPED** (`examples/mass/nanba_nanbi.tvk`, runs +
  builds VJAY; regression green). Real lexer keyword + `NanbiDecl` AST +
  `NanbiBinding` patterns + parser (`[...]`/`{...}`, `_` ignore, `...rest`,
  `key: alias`) + Resolver (declares bindings, `THALA-NANBI-004` duplicate) +
  Interpreter (source evaluated **exactly once**, `THALA-NANBI-002` arity /
  `THALA-NANBI-003` non-destructurable). Also `vaathi nanbi [a,b] ulla coll`
  loop-pattern form. Heterogeneous array literals now allowed (tuple support).
  *IR/VM serialization of nanbi is still emit-stubbed (VM is emit-only anyway).*
- Also fixed en route: **THALA-OOP-001** (`super.init`/`super.aarambam` parent
  ctor) and **THALA-LOGIC-001** (`&&`/`||` short-circuit).
- ✅ **`munnadi` (super) + `munnadi.aarambam` / `super.aarambam`** parent-ctor
  call SHIPPED (`kootam.munnadi` peek renamed to `.mun`; `aarambam` accepted as a
  super method name). Verified.
- ✅ **`comeback` (override)** SHIPPED — new keyword + method modifier
  (`MethodDecl.isOverride`) + resolver validation `THALA-OVERRIDE-001` (a
  `comeback` method with no matching parent method is rejected). Verified.
- ✅ **`mudivu` (final) SHIPPED** — aliases `URUTHI`, so `mudivu mersal foo()` is a
  final method and `mudivu X = 5` is a constant. Collisions resolved by renaming
  `satham.mudivu`→`.saavu` (FATAL) and `vaseegara.mudivu`→`.mudiyudha` (endsWith).
- ✅ **`aarambam` constructor *declaration* SHIPPED** — `aarambam(...) { }` inside
  a class (no `mersal`) declares a constructor, alongside the still-supported
  `mersal init(...)`. Shared `parseConstructorRest` helper; verified with
  `aarambam` + `munnadi.aarambam` + `comeback` together, and `init` unaffected.
- Staged (need real new semantics): `varum`/`kaathiru` (real async scheduler, no
  blocking join — the one large runtime project left), `thaguthi` generic
  constraints.
- ✅ **Pattern matching SHIPPED** — `yaaru <subject> { ivan <value> { .. } …
  yaarumilla { .. } }`. New tokens/AST (`MatchStmt`/`MatchArm`) + parser +
  resolver + interpreter (subject evaluated once; first equal `ivan` arm runs,
  else `yaarumilla`). Reflection module renamed `yaaru`→`yaarunee` to free the
  keyword. Verified (int + string subjects). Type/destructuring patterns staged.
- ✅ **Migration tool SHIPPED** — `tdk thalapathify <path> [-f]`. Token-aware
  (built on the real Lexer, so strings/comments/identifiers are never touched);
  rewrites `true→aama false→illa null→onnumilla this→naan super→munnadi
  break→interval continue→aduthu static→podhu as→maathiko`. Previews by default,
  `-f` writes; migrated files run identically. `thalafmt --thalapathify` alias
  still staged.

## Phase 2.10 — Batteries-included stdlib (100+ modules) [IN PROGRESS]
Target inspired by Python/Java/C++/C# std libs, all Vijay-native. Map lives in
[VIJAY_STDLIB_NAMING_MATRIX.md](VIJAY_STDLIB_NAMING_MATRIX.md).
- Phase 2.8 foundation (15): io/math/text/collections/fs/path/json/regex/random/
  time/log/system/http/algorithms/functional.
- ✅ Newly shipped & verified: `kalanjiyam` (document DB over VijayDB natives —
  thira/serthu/ellam/thedu/edu/puduppi/neekku/ennikkai), `adayalam` (UUID-shaped
  ids via random; non-crypto, documented), `yaaru` (value reflection over
  `type()`), `irukkalaam` (Option `Irukkalaam` + Result `MudivuBathil` with
  factories). ~19 modules live.
- Next real slices: `kottai` (wrap existing `web` server / merge with THALAIVALAI).
- ✅ Newly shipped: `muthirai` (hashing) — real **standard SHA-256 (FIPS 180-4)**
  native `__native_sha256`, verified against the published `abc`/empty-string test
  vectors. ~20 modules live.
- Staged needing native work: threading `padai`/sockets `nanban` (Phase 3).

## Phase 2.11 — THALAIVALAI web framework [IN PROGRESS]
Vijay-native web layer over the built-in HTTP/1.1 server; additive, no syntax
changes. Docs: [THALAIVALAI_WEB.md](THALAIVALAI_WEB.md).
- Shipped & **curl-verified** (`examples/web/02_alangaaram/`): `thalaivalai.pudhu()`
  app; routing (`kelu`/`anuppu`/`muzhusaMaathu`/`konjamMaathu`/`neekku`), dynamic
  `<param>` routes; static mounts (`app.sotthu`) with MIME + `..` traversal guard;
  `bathil` responses (text/html/json/redirect); `Kelvi` request object; `kaattu`
  template `{{ }}` interpolation with HTML escaping; 404. Correct content types
  verified for HTML/CSS/JS/JSON. Added custom response `headers` support in the
  native serve→interpreter bridge (redirects / security headers / ETag). [DONE]
- ✅ **`tdk new-web <name>` SHIPPED** — scaffolds a runnable THALAIVALAI project
  (src/main.tvk, paarvai/ views, alangaaram/ CSS, nadanam/ JS, thalapathy.toml).
  Verified: generated app passes `--check`, runs, and curl confirms the template
  `{{ peru }}` renders, the JSON route responds, and CSS serves with the right
  content type.
- Staged (honest — not shipped): `{% ghilli %}`/`{% vaathi %}` template control
  flow (only `{{ }}` interpolation today), sessions/cookies/CSRF, WebSocket
  (`neradiValai`), SSE (`thodarAnuppu`), `--reload`/`--production`, binary static
  assets, and async handlers (blocked on the staged `varum`/`kaathiru` runtime).
  `thalafmt --thalapathify` migration flag is now shipped alongside `tdk
  thalapathify`.

### Tests
New feature coverage in `tests/interpreter/FeaturesTests.cpp` (8 cases, all
green; suite 63/64, the one failure is a pre-existing unrelated
"unsupported source extension" case): canonical literals + `naan`, `&&`/`||`
short-circuit, `nanbi` array/rest/object, `yaaru` pattern matching (arm +
default), `comeback` override + `munnadi.aarambam`.

## Phase 2.12 — Real async (`varum` / `kaathiru`) [DESIGN — deliberately not faked]
`waiting` already means *abstract*, so async uses `varum` (async fn) + `kaathiru`
(await). Doing this **correctly** (non-blocking, no OS-thread-per-suspension —
both explicit requirements) needs true suspension/resumption, which a tree-walking
interpreter cannot express without one of:
1. **Stackful coroutines** (fibers / `ucontext` / Boost.Context) — each `varum`
   call runs on a coroutine stack; `kaathiru` yields to a scheduler event loop.
2. **CPS / state-machine transform** of `varum` functions in the compiler.
Both are substantial runtime projects. Shipping a `thread.join()`-style blocking
version is explicitly forbidden and would be fake, so async is **staged with this
design** rather than shipped hollow. Recommended path: option (1) — a
single-threaded scheduler + stackful coroutines + a `Varum`/`Pani` task value.

## Phase 3 — High-performance Compiler & runtime [PLANNED]
- Executing bytecode VM for `.vijay` (currently emit/inspect only).
- Garbage collector (cycle collection).
- Concurrency (threads / async / channels).
- Enforced generics, records/tuples, null-safety, richer pattern matching.
- LSP, debugger, package registry.
- Native compilation and LLVM integration.

See `docs/LANGUAGE_AUDIT.md` for the full gap analysis and prioritized backlog.
