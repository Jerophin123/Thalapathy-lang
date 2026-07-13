# THALAPATHY Diagnostic Systems

Technical errors track positions and draw carets pointing to the exact range of characters.

## Namespace Codes
- `THALA-SOURCE`: File validation errors (e.g. extension check).
- `THALA-LEX`: Scanning anomalies (unterminated strings).
- `THALA-PARSE`: Syntax mistakes.
- `THALA-NAME`: Unknown variable names, duplicates.
- `THALA-MUT`: Reassigning constant/immutable variables.
- `THALA-TYPE`: Incompatible types.
- `THALA-FUNC`: Return structures or calling conventions mismatch.
- `THALA-CLASS`: Cycle detection, invalid `this`/`super` calls.
- `THALA-LOOP`: Loop break/continue checks.
- `THALA-RUNTIME`: Stack, divide-by-zero, array index issues.
```
error[THALA-TYPE-001]: type mismatch
  --> test.tvk:3:11
   |
 3 | int age = "Ghilli";
   |           ^^^^^^^^ expected `int`, found `string`
   |
   = help: use an integer value or change the declared type
```
