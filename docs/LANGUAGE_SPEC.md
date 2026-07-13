# THALAPATHY Language Specification v0.1.0

This document defines the features, keywords, structures, and basic syntax constraints of the THALAPATHY programming language.

## Keywords
- `thalapathy`: Root program keyword.
- `aarambam`: Entry block declaration.
- `nanba`: Immutable local variable declaration.
- `makkal`: Mutable local variable declaration.
- `uruthi`: Constant definition.
- `ghilli`: If condition block.
- `illana`: Else block.
- `bigil`: C-style for-loop structure.
- `vaathi`: Iterator/range loop structure.
- `mersal`: Function declaration.
- `thiruppi`: Return statement.
- `sollu`: Print builtin.
- `master`: Class declaration.
- `varisu`: Single inheritance definition.
- `theri`: Exception throwing.
- `pokkiri`: Try block.
- `kaavalan`: Catch block.

## Scoping
Shadowing of variables is permitted inside nested block structures `{ ... }`. Variable definitions inside range and loop blocks are localized to that block's scope.
