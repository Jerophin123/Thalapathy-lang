# THALA IR & Serialization Spec

## Artifact Binary Format (.thalac)
Every binary target features:
1. **Magic Header**: Exactly 4 bytes: `T` `H` `L` `A`.
2. **Format Version**: 2 bytes unsigned int: `1`.
3. **Language Version**: 3 bytes representing `0` `1` `0` (v0.1.0).
4. **Constant Pool Size**: 4 bytes unsigned int.
5. **Constant Pool Entries**: Dynamic list of values (type code + byte length + raw payload).
6. **Instructions Size**: 4 bytes unsigned int.
7. **Instructions**: Opcodes (1 byte) followed by operand indices (4 bytes).

## Intermediate Representation Opcodes
- `LOAD_CONST`: Loads constant value onto stack.
- `LOAD_LOCAL`, `STORE_LOCAL`: Handles local bindings.
- `LOAD_GLOBAL`, `STORE_GLOBAL`: Handles global variables.
- `ADD`, `SUB`, `MUL`, `DIV`, `MOD`: Arithmetic actions.
- `EQ`, `NE`, `LT`, `LTE`, `GT`, `GTE`: Comparisons.
- `JUMP`, `JUMP_IF_FALSE`: Control jumps.
- `CALL`: Function calls.
- `RETURN`: Returns from frame.
- `POP`: Clears value stack.
