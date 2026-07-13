<div align="center">

<pre style="line-height:1.15; font-weight:bold;">
<span style="color:#E32227">████████╗██╗  ██╗ █████╗ ██╗      █████╗ ██████╗  █████╗ ████████╗██╗  ██╗██╗   ██╗</span>
<span style="color:#FFC300">╚══██╔══╝██║  ██║██╔══██╗██║     ██╔══██╗██╔══██╗██╔══██╗╚══██╔══╝██║  ██║╚██╗ ██╔╝</span>
<span style="color:#E32227">   ██║   ███████║███████║██║     ███████║██████╔╝███████║   ██║   ███████║ ╚████╔╝ </span>
<span style="color:#FFC300">   ██║   ██╔══██║██╔══██║██║     ██╔══██║██╔═══╝ ██╔══██║   ██║   ██╔══██║  ╚██╔╝  </span>
<span style="color:#E32227">   ██║   ██║  ██║██║  ██║███████╗██║  ██║██║     ██║  ██║   ██║   ██║  ██║   ██║   </span>
<span style="color:#FFC300">   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝   ╚═╝   </span>
</pre>

### <span style="color:#FFC300">⚡</span> <span style="color:#E32227">TDK</span> — <span style="color:#FFC300">THALAPATHY</span> <span style="color:#E32227">Development Kit</span> <span style="color:#FFC300">⚡</span>
**"Compiler sonna kaaryam, adhu nadandhu than theerum!"**
*(Whatever the compiler decides, it will happen — no re-runs, no re-takes, just clean builds.)*

[![Version](https://img.shields.io/badge/release-1.0.0-E32227?style=for-the-badge&labelColor=FFC300&logo=git&logoColor=black)](#)
[![Extension](https://img.shields.io/badge/source-.tvk-FFC300?style=for-the-badge&labelColor=E32227)](#)
[![Bytecode](https://img.shields.io/badge/bytecode-.vijay-E32227?style=for-the-badge&labelColor=FFC300&logoColor=black)](#)
[![Engine](https://img.shields.io/badge/engine-C%2B%2B20-FFC300?style=for-the-badge&labelColor=E32227)](#)
[![Mode](https://img.shields.io/badge/mode-mass%20%7C%20professional%20%7C%20political--parody-E32227?style=for-the-badge&labelColor=FFC300&logoColor=black)](#)
[![Build](https://img.shields.io/badge/build-passing-FFC300?style=for-the-badge&labelColor=E32227)](#)
[![License](https://img.shields.io/badge/license-see%20legal%2F-E32227?style=for-the-badge&labelColor=FFC300&logoColor=black)](#)

*A general-purpose, multi-paradigm programming language built as a fan tribute — where every compile is a "Vaathi Coming" moment and every runtime error gets its own punch dialogue.*

</div>

---

## <span style="color:#E32227">🎬 Mandatory</span> <span style="color:#FFC300">Disclaimer</span>

> THALAPATHY is an independent programming-language project created as a fan tribute and parody. It is **not affiliated with, endorsed by, or officially connected to** Vijay, TVK, DMK, any political party, film studio, production company, or rights holder. Political references in optional satire diagnostics are **fictional programming jokes** and should not be interpreted as factual claims. This is a compiler, not a manifesto. Vote with your ballot, not your build system.

---

## <span style="color:#E32227">🐯 What Is</span> <span style="color:#FFC300">THALAPATHY?</span>

THALAPATHY (working title: *"Leo of Languages"*) is an independent experimental programming language that blends serious language-design engineering (à la C, C++, C#, Java, Python, JavaScript) with unapologetic Vijay-movie energy. Think of it as what would happen if a compiler theory textbook and a mass-hero interval block had a baby, and that baby grew up to have opinions about static typing.

- Write code in `.tvk` files.
- Compile it down to `.vijay` bytecode, stamped with the `VJAY` magic header.
- Get roasted, encouraged, or lectured by the compiler depending on your chosen `--mode`.
- Feel like the hero of your own second-half twist every time a build passes.

---

## <span style="color:#E32227">🔥 Technical</span> <span style="color:#FFC300">Overview</span>

THALAPATHY is not just vibes — under the *maasu* hood is a genuinely serious toolchain:

- **Core Engine** — Modern C++20, with a custom recursive-descent parser, dynamic dispatch resolver, static type checker, AST-walking interpreter, and a compiler that emits **VEAF 3** binary bytecode. No shortcuts, no duplicate-role gimmicks.
- **TDK (THALAPATHY Development Kit)** — a full environment manager (`tdk`), a source formatter (`thalafmt`), and a bytecode inspector (`vijaydump`). One kit to rule the whole shooting schedule.
- **Modular Imports (`sarkar`)** — resolve dependencies with `sarkar <module>;`, backed by full DFS cycle-detection and path-traversal safeguards. `sarkar` never lets a module go missing on your watch.
- **Object-Oriented Model:**
  - `varisu` → single inheritance ("legacy," literally)
  - `kaththi` → multiple interface contracts (sharp, precise, no blade left unused)
  - `paaru` / `maathu` → property getter/setter blocks ("look" / "change")
  - Method overloading, dynamic dispatch, and operator overloading via `mersal operator`
- **Diagnostics** — caret-highlighted visual error messages with switchable personality via `--mode professional|mass|political-parody`.

---

## <span style="color:#E32227">🎭 Compiler</span> <span style="color:#FFC300">Personality Modes</span>

THALAPATHY's diagnostics engine doesn't just tell you what broke — it tells you *how it feels* about it.

```bash
thalapathy run script.tvk --mode mass
```

```
error[E0308]: mismatched types at script.tvk:14:9
   |
14 |     let count: int = "vaathi";
   |                       ^^^^^^^ expected `int`, found `string`
   |
   = note: Naanga romba pesa maatom, but your types don't match. Fix பண்ணு.
```

```bash
thalapathy run script.tvk --mode political-parody
```

```
error[E0499]: cannot borrow `budget` as mutable more than once
   |
22 |     let promise = budget.borrow_mut();
   |                          ^^^^^^^^^^^^ second mutable borrow occurs here
   |
   = note: [Opposition Party™, fictional in-universe rival faction]
           has released a press statement condemning this borrow checker violation.
           A special commission will investigate after the next by-election.
```

*(Yes, the second one is a joke about a made-up, generic "Opposition Party™" — not a real organization. The compiler is equal-opportunity about which fictional party it teases; it just picked one at random today.)*

```bash
thalapathy run script.tvk --mode professional
```

```
error[E0308]: mismatched types
  --> script.tvk:14:9
   |
14 |     let count: int = "vaathi";
   |                      ^^^^^^^^ expected `int`, found `&str`
```

Boring, correct, HR-approved. Use this one for your actual job interviews.

---

## <span style="color:#E32227">🏗️ Build &</span> <span style="color:#FFC300">Installation Guide</span>

Ensure you have **CMake 3.20+** and a **C++20-capable compiler** (GCC 12+, Clang 14+, or MSVC 2022+). No shortcuts here — this is a proper release pipeline, not a single-day box-office number.

### 1. Compile from Source

```bash
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release -G Ninja
cmake --build build-release --config Release
```

### 2. Install the TDK Distribution

```bash
cmake --install build-release --prefix "D:\TDK-1.0.0"
```

*"Build ஒரு தடவை பண்ணா, run everywhere."*

---

## 📦 Official TDK Distribution Layout

```
TDK-1.0.0/
├── bin/                 # Executables: thalapathy, tdk, vijaydump, thalafmt
├── conf/                # Toolchain configuration (formatter.conf)
├── lib/                 # Standard Library modules & static embedding library
├── include/             # Public embedding C++ headers (Toolchain.hpp)
├── tools/               # IDE extensions (VS Code) & CMake package exports
├── docs/                # TDK commands, installation, & language spec manuals
├── examples/            # Categorized example scripts (.tvk)
└── legal/               # Legal LICENSE & THIRD_PARTY_NOTICES
```

Clean, structured, no loose files lying around — this distribution runs a tighter ship than most film production houses.

---

## 🛠️ CLI Usage Reference

Set `TDK_HOME` to your installation directory and add `TDK_HOME/bin` to your system `PATH`.

### Running, Verifying, & Building Code

```bash
# Run a script directly
thalapathy examples/hello/hello.tvk

# Semantic verification only ("check pannu, don't release yet")
thalapathy --check examples/oop/classes.tvk

# Compile source to .vijay VEAF bytecode
thalapathy --build examples/hello/hello.tvk
```

### Project Management with TDK

```bash
# Verify environment health — "system check panren"
tdk doctor

# Scaffold a new workspace project — "first day first show"
tdk new my-app
cd my-app

# Compile and execute project defined by thalapathy.toml
tdk build
tdk run
```

### Source Formatting & Bytecode Disassembly

```bash
# Format source files recursively in place — "makeover time"
thalafmt src/ --write

# Inspect compiled bytecode headers — "background verification"
vijaydump hello.vijay --header
```

---

## 👋 Hello, World — THALAPATHY Style

```tvk
sarkar io;

vaathi hello() {
    io.sollu("Vanakkam, THALAPATHY!");
}

hello();
```

Output:

```
Vanakkam, THALAPATHY!
```

Short, punchy, and it lands the line on the first try — just like it should.

---

## 🧩 C++ Embedding API

TDK ships with a linkable static library (`libthalapathy_toolchain.a`) and public headers, so C++ applications can embed the compiler and interpreter directly — no need to wait for a sequel.

```cpp
#include <iostream>
#include <thalapathy/Toolchain.hpp>

int main() {
    auto result = thalapathy::Toolchain::check("main.tvk");
    if (result.success) {
        std::cout << "Verification successful!\n";
        return 0;
    }
    return 1;
}
```

Link downstream targets using CMake:

```cmake
find_package(THALAPATHY CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE THALAPATHY::toolchain)
```

---

---

## <span style="color:#E32227">📚 Documentation</span>

The `docs/` folder in the TDK distribution has the full deep-dive — grammar, runtime internals, and yes, the satire disclaimer gets its own file too. Start with `LANGUAGE_GUIDE.md` if you're new, or jump straight to whatever you're debugging.

| Doc | What's Inside |
|---|---|
| [`docs/LANGUAGE_GUIDE.md`](docs/LANGUAGE_GUIDE.md) | Beginner-friendly walkthrough — start here |
| [`docs/LANGUAGE_SPEC.md`](docs/LANGUAGE_SPEC.md) | Full formal language specification |
| [`docs/GRAMMAR.md`](docs/GRAMMAR.md) | Formal grammar / EBNF reference |
| [`docs/TYPE_SYSTEM.md`](docs/TYPE_SYSTEM.md) | Static typing rules & type-checker behavior |
| [`docs/MEMORY_MODEL.md`](docs/MEMORY_MODEL.md) | Memory ownership & lifetime semantics |
| [`docs/IR_SPEC.md`](docs/IR_SPEC.md) | Intermediate representation used by the compiler |
| [`docs/VIJAY_BINARY_FORMAT.md`](docs/VIJAY_BINARY_FORMAT.md) | `.vijay` bytecode / VEAF format reference |
| [`docs/NATIVE_RUNTIME_BRIDGE.md`](docs/NATIVE_RUNTIME_BRIDGE.md) | Interop bridge to native/C++ runtime |
| [`docs/MODULE_SYSTEM.md`](docs/MODULE_SYSTEM.md) | `sarkar` imports, resolution & cycle detection |
| [`docs/OOP_SYNTAX_COMPATIBILITY_AUDIT.md`](docs/OOP_SYNTAX_COMPATIBILITY_AUDIT.md) | OOP feature audit (`varisu`, `kaththi`, etc.) |
| [`docs/STANDARD_LIBRARY.md`](docs/STANDARD_LIBRARY.md) | Standard library modules & APIs |
| [`docs/TDK.md`](docs/TDK.md) | TDK toolchain overview |
| [`docs/TDK_INSTALLATION.md`](docs/TDK_INSTALLATION.md) | Detailed installation instructions |
| [`docs/TDK_COMMANDS.md`](docs/TDK_COMMANDS.md) | Full CLI command reference |
| [`docs/DIAGNOSTICS.md`](docs/DIAGNOSTICS.md) | Error/warning catalog & diagnostic modes |
| [`docs/THALAPATHY_REFERENCES.md`](docs/THALAPATHY_REFERENCES.md) | Glossary of movie-inspired keywords & terms |
| [`docs/POLITICAL_SATIRE.md`](docs/POLITICAL_SATIRE.md) | Scope & guardrails for `--mode political-parody` |
| [`docs/V010_AUDIT_REPORT.md`](docs/V010_AUDIT_REPORT.md) | v1.0.0 release audit report |
| [`docs/ROADMAP.md`](docs/ROADMAP.md) | Planned features & future direction |

---

## <span style="color:#FFC300">🤝 Contributing</span>

Pull requests are welcome — think of every PR review as a script narration session. Bring your best scenes (clean commits), keep the runtime tight (no bloated diffs), and be ready for at least one dialogue-worthy code review comment.

1. Fork the repo
2. Create a feature branch (`git checkout -b feature/vaathi-mode`)
3. Commit your changes with a message worthy of an interval block
4. Open a PR and wait for the compiler — and the maintainers — to approve

---

## 📜 License & Legal

See `legal/LICENSE` and `legal/THIRD_PARTY_NOTICES` in the distribution for full terms. This project is a parody/tribute work; all trademarks, film titles, and names referenced belong to their respective owners.

---

<div align="center">

**<span style="color:#E32227">THALAPATHY</span> & <span style="color:#FFC300">TDK</span>** — because your code deserves a mass entry too.

*Naan periya doctor illa, but naan sonna Segment Fault-a fix pandreadhu confirm.*

</div>
