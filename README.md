# THALAPATHY & TDK (THALAPATHY Development Kit)

Official Source File Extension: `.tvk`  
Official Bytecode Extension: `.vijay`  
Magic Header Signature: `VJAY`  
Current Release Version: `1.0.0`

THALAPATHY is an independent experimental general-purpose, multi-paradigm programming language created as a fan tribute to Thalapathy Vijay. It blends modern programming language design (C, C++, C#, Java, Python, JavaScript) with Vijay cinema culture, dialogue-style humour, and political parody memes.

## Mandatory Disclaimer
"THALAPATHY is an independent programming-language project created as a fan tribute and parody. It is not affiliated with, endorsed by, or officially connected to Vijay, TVK, DMK, any political party, film studio, production company, or rights holder. Political references in optional satire diagnostics are fictional programming jokes and should not be interpreted as factual claims."

---

## Technical Overview
THALAPATHY features a robust structured OOP model, modular imports, and an optimized bytecode runtime:
- **Core Engine**: Built in Modern C++20 with a custom recursive-descent parser, dynamic dispatch resolver, type checker, AST-walking interpreter, and compiler generating VEAF 3 binary bytecode.
- **TDK (THALAPATHY Development Kit)**: An all-in-one environment manager (`tdk`), source code formatter (`thalafmt`), and VEAF inspector (`vijaydump`).
- **Modular Imports (`sarkar`)**: File dependencies are resolved via `sarkar <module>;` with full DFS cycle-detection and path traversal safeguards.
- **Object-Oriented Model**: Supports single inheritance (`varisu`), multiple interface contracts (`kaththi`), property accessors (`paaru` and `maathu` blocks), method overloading, dynamic dispatch, and operator overloading (`mersal operator`).
- **Diagnostics**: Caret-highlighted visual diagnostics with customizable personality modes (`--mode professional|mass|political-parody`).

---

## Build & Installation Guide

Ensure you have CMake 3.20+ and a C++20-capable compiler (GCC 12+, Clang 14+, or MSVC 2022+).

### 1. Compile from Source
```bash
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release -G Ninja
cmake --build build-release --config Release
```

### 2. Install TDK Distribution
```bash
cmake --install build-release --prefix "D:\TDK-1.0.0"
```

---

## Official TDK Distribution Layout

When installed, the kit structures itself as a clean language distribution:

```
TDK-1.0.0/
├── bin/                 # Executables (thalapathy, tdk, vijaydump, thalafmt)
├── conf/                # Toolchain configuration (formatter.conf)
├── lib/                 # Standard Library modules & static embedding library
├── include/             # Public embedding C++ headers (Toolchain.hpp)
├── tools/               # IDE extensions (VS Code) & CMake package exports
├── docs/                # TDK commands, installation, & language spec manuals
├── examples/            # Categorized example scripts (.tvk)
└── legal/               # Legal LICENSE & THIRD_PARTY_NOTICES
```

---

## CLI Usage Reference

Set `TDK_HOME` to your installation directory and add `TDK_HOME/bin` to your system `PATH`.

### Running, Verifying, & Building Code
```bash
# Run a script directly
thalapathy examples/hello/hello.tvk

# Semantic verification only
thalapathy --check examples/oop/classes.tvk

# Compile source to .vijay VEAF bytecode
thalapathy --build examples/hello/hello.tvk
```

### Project Management with TDK
```bash
# Verify environment health
tdk doctor

# Scaffold a new workspace project
tdk new my-app
cd my-app

# Compile and execute project defined by thalapathy.toml
tdk build
tdk run
```

### Source Formatting & Bytecode Disassembly
```bash
# Format source files recursively in place
thalafmt src/ --write

# Inspect compiled bytecode headers
vijaydump hello.vijay --header
```

---

## C++ Embedding API

TDK ships with a linkable static library (`libthalapathy_toolchain.a`) and public headers allowing C++ applications to embed the compiler and interpreter:

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
