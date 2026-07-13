# THALAPATHY Module System

This document describes the design and specification of the THALAPATHY module import system (`sarkar`).

---

## 1. Import Syntax
Modules are imported using the `sarkar` keyword followed by the logical name of the module:

```tvk
sarkar math;
sarkar io;
```

---

## 2. Module Search Paths
When `sarkar name;` is parsed, the logical name `name` is resolved to a physical file `name.tvk` using the following search order:
1. **Entry source directory**: The folder containing the currently executing file.
2. **Current working directory**: The folder from which the driver executable was invoked.
3. **TDK Standard Library**: The standard library directory at `$TDK_HOME/lib/std/`.

---

## 3. Dependency Cycle Detection & Execution Semantics
To ensure stability and efficiency:
- **Module States**: Every module resolved in the import dependency tree travels through states: `UNSEEN`, `LOADING`, `LOADED`, `FAILED`.
- **Circular Imports**: If a module currently in the `LOADING` state is encountered again during traversal, a cycle is detected and compiling fails with `error[THALA-MODULE-003]: circular module dependency detected`.
- **Execution Once**: Successfully parsed modules are topologically sorted and executed exactly once. Duplicate imports reuse the existing module environment, preserving shared global state.
