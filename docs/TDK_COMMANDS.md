# TDK Commands Reference

## thalapathy

The main language driver.

```
thalapathy <file.tvk>              Run a THALAPATHY program
thalapathy --check <file.tvk>      Semantic verification only
thalapathy --build <file.tvk>      Compile to .vijay artifact
thalapathy --emit-ir <file.tvk>    Emit THALA IR to stdout
thalapathy --repl                  Start interactive REPL
thalapathy --version               Print version information
thalapathy --help                  Print usage help
```

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
tdk help            Display help text
```

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
