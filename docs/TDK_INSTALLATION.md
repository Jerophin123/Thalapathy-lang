# TDK Installation Guide

**TDK 1.0.0** · Full documentation: [README.md](README.md) · Verify with `tdk doctor` after install.

## Prerequisites

- Windows 10+ or Linux
- C++20 compatible compiler (GCC 12+, Clang 14+, MSVC 2022+)
- CMake 3.20+

## Building from Source

```bash
git clone <repository>
cd THALAPATHY
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -G Ninja
cmake --build build --config Release
```

## Installing

```bash
cmake --install build --prefix /path/to/TDK-1.0.0
```

## Environment Setup

Set `TDK_HOME` to the installation root:

```bash
# Windows PowerShell
$env:TDK_HOME = "D:\TDK-1.0.0"
$env:PATH = "$env:TDK_HOME\bin;$env:PATH"

# Linux/macOS
export TDK_HOME=/opt/TDK-1.0.0
export PATH=$TDK_HOME/bin:$PATH
```

## Verifying Installation

```bash
tdk doctor
```

Expected output:

```
TDK Doctor

[PASS] TDK_HOME directory resolved
[PASS] All TDK executables present (4/4 found)
[PASS] Standard library modules present
[PASS] TDK_HOME/bin is in PATH
[PASS] VERSION file present
[PASS] RELEASE.json valid
[PASS] docs directory present
[PASS] legal directory present
```

## Distribution Layout

```
TDK-1.0.0/
├── bin/                  Executable tools
├── conf/                 Configuration files
├── lib/std/              Standard library modules
├── include/thalapathy/   Public C++ embedding headers
├── tools/                Editor and build integrations
├── docs/                 Documentation
├── examples/             Example programs
├── legal/                License and notices
├── VERSION               Version string
├── RELEASE.json          Distribution metadata
└── README.md             Overview
```

## TDK_HOME Resolution

The TDK resolves its home directory in this order:

1. `TDK_HOME` environment variable (if set and valid)
2. Executable location: `<exe_dir>/../` (when exe is inside `bin/`)

The current working directory is never used as TDK_HOME.
