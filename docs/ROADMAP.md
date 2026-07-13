# THALAPATHY Development Roadmap

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

## Phase 3 — High-performance Compiler & runtime [PLANNED]
- Executing bytecode VM for `.vijay` (currently emit/inspect only).
- Garbage collector (cycle collection).
- Concurrency (threads / async / channels).
- Enforced generics, records/tuples, null-safety, richer pattern matching.
- LSP, debugger, package registry.
- Native compilation and LLVM integration.

See `docs/LANGUAGE_AUDIT.md` for the full gap analysis and prioritized backlog.
