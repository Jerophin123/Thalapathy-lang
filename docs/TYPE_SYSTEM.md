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

## Enums (`vagai`)
`vagai Name { A, B, C }` introduces a nominal enum type. Each member is a
distinct value carrying its `typeName`, `name`, and `ordinal`. Members:
- are accessed as `Name.A`,
- expose `.name()`, `.ordinal()`, `.type()`,
- compare by identity with `==` / `!=` and match as `thalaivaa` cases,
- answer runtime type tests: `value aaguma Name`.

Enums are the first user-defined nominal type beyond classes/interfaces.
Generics remain **erased** (`goat T` parses but is not instantiated/checked);
records, tuples, and union types are not yet implemented.

## Gradual Type Compatibility
`any` acts as a gradual type override bypassing strict static checks, letting method calls pass checking and resolving them at runtime. Mutability is statically validated (e.g. reassigning to immutable `nanba` bindings triggers diagnostic `THALA-MUT-001`).
