# THALAPATHY Development Kit (TDK)

**TDK** is the official development kit for the THALAPATHY programming language.

> Fan tribute to Joseph Vijay Chandrasekhar (Thalapathy Vijay). See [TRIBUTE.md](TRIBUTE.md).
> Full doc index: [README.md](README.md).

## Overview

TDK provides a complete toolchain for developing, compiling, inspecting, and formatting THALAPATHY source code.

## Core Tools

| Tool | Description |
|------|------------|
| `thalapathy` | Language driver — runs, checks, and compiles `.tvk` source files |
| `tdk` | Project manager — scaffolds, builds, and manages THALAPATHY projects |
| `vijaydump` | Binary inspector — displays `.vijay` artifact metadata and disassembly |
| `thalafmt` | Source formatter — formats `.tvk` files according to style rules |

## Versions

- **Language Version**: 1.0.0
- **TDK Version**: 1.0.0
- **VEAF Format Version**: 3
- **Source Extension**: `.tvk`
- **Artifact Extension**: `.vijay`
- **VEAF Magic**: `VJAY`

## Quick Start

```bash
# Check your installation
tdk doctor

# Create a new project
tdk new my-app

# Run a program
thalapathy hello.tvk

# Check syntax
thalapathy --check hello.tvk

# Compile to .vijay
thalapathy --build hello.tvk
```
