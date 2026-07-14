# Changelog

All notable changes to this project are documented here.

The format follows [Keep a Changelog](https://keepachangelog.com/). Version numbers align with [VERSION](../VERSION) and [RELEASE.json](../RELEASE.json).

---

## [1.0.0] - 2026-07-14

### Added
- **Production documentation hub** — [docs/README.md](docs/README.md) indexes the full doc set; [docs/TRIBUTE.md](docs/TRIBUTE.md) dedicates the project to Joseph Vijay Chandrasekhar.
- **Vijay-native standard library** (~20 modules): `vaanganna`, `sivakasi`, `vaseegara`, `kootam`, `kootu`, `velayudham`, `udhaya`, `jilla`, `vettaikaaran`, `deva`, `neram`, `satham`, `arasu`, `villu`, `ozhungu`, `kalanjiyam`, `adayalam`, `yaarunee`, `irukkalaam`, `muthirai`.
- **THALAIVALAI web framework** — routing, static mounts, templates, JSON responses; `tdk new-web`; curl-verified demo at `examples/web/02_alangaaram/`.
- **Canonical THALAPATHY syntax** — `aama`/`illa`/`onnumilla`, `naan`, `munnadi`, `interval`/`aduthu`, `podhu`/`maathiko`; `tdk thalapathify` migration tool.
- **`nanba` / `nanbi`** — immutable bindings and destructuring patterns.
- **Pattern matching** — `yaaru` / `ivan` / `yaarumilla`.
- **OOP enhancements** — `comeback` (override), `mudivu` (final), `aarambam` constructor declaration, `munnadi.aarambam` parent ctor.
- **Lexicon & naming matrix** — [THALAPATHY_API_LEXICON.md](docs/THALAPATHY_API_LEXICON.md), [VIJAY_STDLIB_NAMING_MATRIX.md](docs/VIJAY_STDLIB_NAMING_MATRIX.md).
- **Async design doc** — staged `varum` / `kaathiru` fiber model ([ASYNC_DESIGN.md](docs/ASYNC_DESIGN.md)).

### Fixed
- Short-circuit evaluation for `&&` / `||` (THALA-LOGIC-001).
- `super.init` / `munnadi.aarambam` parent constructor dispatch (THALA-OOP-001).

### Documentation
- README rewritten with full documentation tables and tribute section.
- TDK install/commands docs updated to v1.0.0.
- CMake install ships the complete documentation set.

---

## [0.1.0] - 2026-07-13

### Added
- First public release of THALAPATHY language.
- Lexer, Pratt parser, and Resolver modules.
- Intermediate representation (IR) binary target generation.
- Interactive REPL console.
- Personality decorator.
