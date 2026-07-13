# THALAPATHY Toolchain (TDK)

The THALAPATHY Development Kit bundles the compiler front-end, project manager,
formatter, and bytecode inspector.

## Binaries
| Tool | Purpose |
|---|---|
| `thalapathy` | Run, `--check`, `--build` (emit `.vijay`), `--emit-ir`, `--repl`. |
| `tdk` | Project manager (below). |
| `thalafmt` | Source formatter (`thalafmt src/ --write`). |
| `vijaydump` | Inspect compiled `.vijay` artifacts (`--header`). |

## `thalapathy` CLI
```bash
thalapathy app.tvk                 # run
thalapathy --check app.tvk         # semantic verification only
thalapathy --build app.tvk         # compile to app.vijay (VEAF)
thalapathy --emit-ir app.tvk       # print THALA IR disassembly
thalapathy --repl                  # interactive REPL
thalapathy --mode mass app.tvk     # personality: professional|mass|political-parody
```

## `tdk` commands
| Command | Purpose |
|---|---|
| `tdk new <name>` | Scaffold a project (`thalapathy.toml`, `src/main.tvk`). |
| `tdk nadu <a.b.c> [Class]` | Scaffold a Java-style package (dirs + stub `.tvk`). |
| `tdk init` | Initialize the current folder as a project. |
| `tdk check` / `build` / `run` / `clean` | Manifest-driven verify/compile/run/clean. |
| `tdk inspect <pkg>` | Show compiled artifact metadata. |
| `tdk package` | Generate a `.tvkpkg` distribution archive. |
| `tdk doctor` / `home` / `env` / `info` / `list-tools` | Environment diagnostics. |

## Project manifest (`thalapathy.toml`)
```toml
[project]
name = "my-app"
version = "0.1.0"
entry = "src/main.tvk"

[build]
output = "build"
artifact = "my-app.vijay"

[personality]
mode = "mass"
```

## Module & package resolution
- `sarkar io;` — plain module: resolved from the importing file's directory, then
  the standard library.
- `sarkar app.models.hero;` — dotted package path → `app/models/hero.tvk`,
  resolved from the **project root** (entry file's directory), Java-classpath
  style.
- `nadu app.models;` — declares the package a file belongs to.

## Editor support
- VS Code extension (`editors/vscode-thalapathy`): TextMate grammar +
  language configuration (comments, brackets). Syntax highlighting covers all
  keywords including `vagai`, `kadaisi`, `nadu`, `thuppakki`, `thalaivaa`,
  `vazhakku`, `kutty`.
- LSP (completion/hover/diagnostics) and a debugger are **planned**, not yet
  implemented.

## Standard library modules
`io`, `math`, `text`, `time`, `collections` (Stack/List/Queue/Set), `json`,
`http`, `web` (Sarkar framework + Router), `random`, `os`, `db` (VijayDB),
`regex`, `datetime`, `log`.
