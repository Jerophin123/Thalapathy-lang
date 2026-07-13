# THALAPATHY Type System Spec

THALAPATHY is statically verified via a resolver.

## Primitive Types
- `int`: Signed 64-bit integers.
- `float`: 64-bit IEEE 754 floating point numbers.
- `bool`: boolean `true` / `false`.
- `string`: UTF-8 string values.
- `char`: character constants.
- `void`: empty return indicators.
- `null`: empty value type.

## Gradual Type Compatibility
`any` acts as a gradual type override bypassing strict static checks, letting method calls pass checking and resolving them at runtime. Mutability is statically validated (e.g. reassigning to immutable `nanba` bindings triggers diagnostic `THALA-MUT-001`).
