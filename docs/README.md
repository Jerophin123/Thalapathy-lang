# THALAPATHY Documentation Hub

**Version:** 1.0.0 · **TDK:** 1.0.0 · **VEAF:** 3 · **Extensions:** `.tvk` / `.vijay` · **Magic:** `VJAY`

> *"Once I commit, I never look back."* — This documentation is the official reference for the THALAPATHY programming language and the THALAPATHY Development Kit (TDK). Every page is written for production use: accurate, cross-linked, and honest about what ships today versus what is staged.

---

## In memory & tribute

THALAPATHY exists as an independent fan tribute to **Joseph Vijay Chandrasekhar** — Thalapathy Vijay — the actor whose films shaped a generation and who serves as the Honourable Chief Minister of Tamil Nadu. The language carries his cinema, dialogue rhythm, and fan culture in its syntax — not as endorsement, but as celebration.

Read the full dedication: **[TRIBUTE.md](TRIBUTE.md)**

---

## Start here

| I want to… | Go to |
|------------|-------|
| Install TDK and run my first program | [TDK_INSTALLATION.md](TDK_INSTALLATION.md) → [LANGUAGE_GUIDE.md](LANGUAGE_GUIDE.md) §1–3 |
| See what THALAPATHY code looks like | [../README.md](../README.md) · [`examples/mass/full_language.tvk`](../examples/mass/full_language.tvk) |
| Film → keyword & stdlib cheat sheets | [../README.md#film--keyword--cinema-roots](../README.md#film--keyword--cinema-roots) |
| Look up a keyword or stdlib name | [THALAPATHY_API_LEXICON.md](THALAPATHY_API_LEXICON.md) · [VIJAY_STDLIB_NAMING_MATRIX.md](VIJAY_STDLIB_NAMING_MATRIX.md) |
| Build a web app | [THALAIVALAI_WEB.md](THALAIVALAI_WEB.md) · `tdk new-web <name>` |
| Use CLI tools day-to-day | [TDK_COMMANDS.md](TDK_COMMANDS.md) |
| Embed THALAPATHY in C++ | [TOOLCHAIN.md](TOOLCHAIN.md) · [../include/thalapathy/Toolchain.hpp](../include/thalapathy/Toolchain.hpp) |
| Understand what's shipped vs planned | [ROADMAP.md](ROADMAP.md) |

---

## Language reference

Core grammar, types, and semantics for writing `.tvk` source.

| Document | Contents |
|----------|----------|
| [LANGUAGE_GUIDE.md](LANGUAGE_GUIDE.md) | **Primary tutorial & reference** — installation through OOP, modules, stdlib, diagnostics |
| [LANGUAGE_SPEC.md](LANGUAGE_SPEC.md) | Formal keyword and syntax specification |
| [GRAMMAR.md](GRAMMAR.md) | Parser-level grammar notes |
| [TYPE_SYSTEM.md](TYPE_SYSTEM.md) | Gradual typing, inference, and checking rules |
| [MODULE_SYSTEM.md](MODULE_SYSTEM.md) | `sarkar` imports, `nadu` packages, cycle detection |
| [MEMORY_MODEL.md](MEMORY_MODEL.md) | Values, environments, and object lifetime (interpreter) |
| [DIAGNOSTICS.md](DIAGNOSTICS.md) | Error codes, caret output, personality modes |

### Canonical THALAPATHY identity

| Document | Contents |
|----------|----------|
| [THALAPATHY_API_LEXICON.md](THALAPATHY_API_LEXICON.md) | **Single source of truth** for public API names and collision rules |
| [VIJAY_STDLIB_NAMING_MATRIX.md](VIJAY_STDLIB_NAMING_MATRIX.md) | Full stdlib API matrix with implementation status |
| [THALAPATHY_ACTION_VOCABULARY.md](THALAPATHY_ACTION_VOCABULARY.md) | Action verbs and method naming conventions |
| [THALAPATHY_IDENTITY_AUDIT.md](THALAPATHY_IDENTITY_AUDIT.md) | English → canonical migration audit |
| [THALAPATHY_REFERENCES.md](THALAPATHY_REFERENCES.md) | Filmography and keyword inspiration catalogue |
| [NANBA_NANBI.md](NANBA_NANBI.md) | Immutable bindings and destructuring patterns |
| [POLITICAL_SATIRE.md](POLITICAL_SATIRE.md) | Optional `--mode political-parody` diagnostics (fictional jokes only) |

---

## Standard library

| Document | Contents |
|----------|----------|
| [STANDARD_LIBRARY.md](STANDARD_LIBRARY.md) | Module overview — compat set + Vijay-native canonical modules |
| [VIJAY_STDLIB_NAMING_MATRIX.md](VIJAY_STDLIB_NAMING_MATRIX.md) | Per-method API tables and ✅/⏳ status |
| [THALAPATHY_API_LEXICON.md](THALAPATHY_API_LEXICON.md) | Naming rules and reserved-word collision baseline |

**Shipped Vijay-native modules** (under `$TDK_HOME/lib/std/`):

`vaanganna` · `sivakasi` · `vaseegara` · `kootam` · `kootu` · `velayudham` · `udhaya` · `jilla` · `vettaikaaran` · `deva` · `neram` · `satham` · `arasu` · `villu` · `ozhungu` · `kalanjiyam` · `adayalam` · `yaarunee` · `irukkalaam` · `muthirai` · `thalaivalai` · `bathil`

Legacy compatibility aliases (`io`, `math`, `text`, `collections`, `json`, `http`, `web`, `db`, …) remain available; new code should prefer the canonical names above.

---

## Web development — THALAIVALAI

| Document | Contents |
|----------|----------|
| [THALAIVALAI_WEB.md](THALAIVALAI_WEB.md) | App API, routing, static files, templates, `bathil` responses |
| [`examples/web/02_alangaaram/`](../examples/web/02_alangaaram/) | Curl-verified demo (HTML + CSS + JS + JSON API) |

Quick scaffold: `tdk new-web my-app` → `cd my-app` → `tdk run`

---

## Toolchain & TDK

| Document | Contents |
|----------|----------|
| [TDK.md](TDK.md) | TDK overview, versions, quick start |
| [TDK_INSTALLATION.md](TDK_INSTALLATION.md) | Build, install, `TDK_HOME`, `tdk doctor` |
| [TDK_COMMANDS.md](TDK_COMMANDS.md) | `thalapathy`, `tdk`, `vijaydump`, `thalafmt` reference |
| [TOOLCHAIN.md](TOOLCHAIN.md) | Compiler pipeline, embedding, CMake integration |
| [VIJAY_BINARY_FORMAT.md](VIJAY_BINARY_FORMAT.md) | `.vijay` VEAF 3 bytecode layout |
| [IR_SPEC.md](IR_SPEC.md) | THALA intermediate representation |
| [NATIVE_RUNTIME_BRIDGE.md](NATIVE_RUNTIME_BRIDGE.md) | Native primitives and interpreter bridge |

### CLI at a glance

```bash
thalapathy script.tvk              # run
thalapathy --check script.tvk      # verify
thalapathy --build script.tvk      # compile → .vijay
tdk doctor                         # health check
tdk new my-app                     # console project
tdk new-web my-site                # THALAIVALAI web project
tdk thalapathify src/ -f           # migrate legacy English keywords
thalafmt src/ --write              # format
vijaydump app.vijay --header       # inspect bytecode
```

---

## Architecture & design (staged features)

| Document | Contents |
|----------|----------|
| [ASYNC_DESIGN.md](ASYNC_DESIGN.md) | Staged `varum` / `kaathiru` fiber scheduler design (not faked) |
| [ROADMAP.md](ROADMAP.md) | Phase history, current work, Phase 3 plans |
| [LANGUAGE_AUDIT.md](LANGUAGE_AUDIT.md) | Gap analysis and prioritized backlog |
| [IMPLEMENTATION_REPORT.md](IMPLEMENTATION_REPORT.md) | Implementation status notes |
| [OOP_SYNTAX_COMPATIBILITY_AUDIT.md](OOP_SYNTAX_COMPATIBILITY_AUDIT.md) | OOP syntax compatibility matrix |
| [V010_AUDIT_REPORT.md](V010_AUDIT_REPORT.md) | v1.0.0 release audit report |

---

## Examples

| Path | Demonstrates |
|------|--------------|
| [`examples/hello/`](../examples/hello/) | Hello world |
| [`examples/mass/full_language.tvk`](../examples/mass/full_language.tvk) | Vijay-native stdlib end-to-end |
| [`examples/mass/thalaiva_syntax.tvk`](../examples/mass/thalaiva_syntax.tvk) | Canonical literals, `naan`, `munnadi`, `comeback` |
| [`examples/mass/nanba_nanbi.tvk`](../examples/mass/nanba_nanbi.tvk) | `nanba` / `nanbi` destructuring |
| [`examples/web/02_alangaaram/`](../examples/web/02_alangaaram/) | THALAIVALAI full-stack demo |
| [`examples/oop/`](../examples/oop/) | Classes, inheritance, interfaces |
| [`examples/packages/`](../examples/packages/) | Multi-file `nadu` packages |
| [`examples/hello/`](../examples/hello/) | Hello world |
| [`examples/modules/`](../examples/modules/) | `sarkar` imports |
| [`examples/stdlib/`](../examples/stdlib/) | Legacy stdlib demos |

---

## Legal & project metadata

| Document | Contents |
|----------|----------|
| [../README.md](../README.md) | Project overview, disclaimer, quick links |
| [TRIBUTE.md](TRIBUTE.md) | Dedication to Joseph Vijay Chandrasekhar |
| [../LICENSE](../LICENSE) | Project license |
| [../legal/THIRD_PARTY_NOTICES.md](../legal/THIRD_PARTY_NOTICES.md) | Third-party attributions |
| [../SECURITY.md](../SECURITY.md) | Vulnerability reporting |
| [../CHANGELOG.md](../CHANGELOG.md) | Release history |

---

## Mandatory disclaimer

THALAPATHY is an **independent** programming-language project created as a fan tribute and parody. It is **not** affiliated with, endorsed by, or officially connected to Joseph Vijay Chandrasekhar, TVK, DMK, any political party, film studio, production company, or rights holder. Political references in optional satire diagnostics are fictional programming jokes and must not be interpreted as factual claims.

---

*Idhu Java illa. Python illa. C++ illa. **Idhu THALAPATHY da.** 🔥*
