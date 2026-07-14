# TDK Commands Reference

## thalapathy

The main language driver.

```
thalapathy <file>                  Run a THALAPATHY program
thalapathy --check <file>          Semantic verification only
thalapathy --build <file>          Compile to .vijay artifact
thalapathy --emit-ir <file>        Emit THALA IR to stdout
thalapathy --repl                  Start interactive REPL
thalapathy --version               Print version information
thalapathy --help                  Print usage help
```

### Target Path Resolution
When running files, `thalapathy` supports the following paths:
- **Raw source file**: `thalapathy app.tvk`
- **Extensionless name**: `thalapathy app` (checks and executes `app.tvk` or `app.vijay` transparently)
- **Bytecode target**: `thalapathy app.vijay` (transparently resolves to adjacent source `app.tvk` for execution)


### Options
- `--mode <professional|mass|political-parody>` — Set personality mode
- `-o <output>` — Specify output artifact path
- `--json` — Emit diagnostics as JSON

## tdk

The TDK project manager.

```
tdk version         Print TDK version
tdk doctor          Verify environment health
tdk home            Display active TDK_HOME
tdk env             Display environmental status
tdk info            Display general kit info
tdk list-tools      List TDK core binaries
tdk new <name>      Create a new project
tdk init            Initialize current folder as a project
tdk check           Run semantic verification on manifest entry
tdk build           Compile project entry to .vijay
tdk run             Run project entry directly
tdk clean           Delete project build output safely
tdk inspect <pkg>   Display compiled artifact metadata
tdk package         Generate .tvkpkg distribution archive
tdk thalapathify <path> [-f]  Rewrite legacy English keywords to canonical THALAPATHY
tdk new-web <name>  Scaffold a runnable THALAIVALAI web project
tdk help            Display help text
```

### `tdk thalapathify`

Token-aware migration from legacy English spellings to canonical THALAPATHY syntax. Built on the real Lexer — strings, comments, and identifier substrings are never touched.

```
tdk thalapathify src/              Preview changes (stdout)
tdk thalapathify src/app.tvk -f    Write canonical syntax in place
```

Rewrites: `true→aama`, `false→illa`, `null→onnumilla`, `this→naan`, `super→munnadi`, `break→interval`, `continue→aduthu`, `static→podhu`, `as→maathiko`.

See also [THALAPATHY_IDENTITY_AUDIT.md](THALAPATHY_IDENTITY_AUDIT.md).

### `tdk new-web`

Creates a THALAIVALAI project with `src/main.tvk`, `paarvai/` views, `alangaaram/` CSS, `nadanam/` JS, and `thalapathy.toml`.

```
tdk new-web my-site
cd my-site
tdk run    # serves on port 8080 by default
```

See [THALAIVALAI_WEB.md](THALAIVALAI_WEB.md).

### Options
- `--json` — Format output as structured JSON
- `--force` / `-f` — Force overwrite in init

## vijaydump

Binary artifact inspector.

```
vijaydump <file.vijay>              Display full artifact dump
vijaydump <file.vijay> --header     Display header only
vijaydump <file.vijay> --json       Output as JSON
```

## thalafmt

Source code formatter.

```
thalafmt <file.tvk>                 Print formatted output to stdout
thalafmt <file.tvk> --write         Format file in place
thalafmt <file.tvk> --check         Check if already formatted (exit 1 if not)
thalafmt <directory>                Recursively format all .tvk files
thalafmt --version                  Print version
```
