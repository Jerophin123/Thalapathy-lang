# THALAPATHY OOP Syntax Compatibility Audit

This audit evaluates the safety of extending existing THALAPATHY keywords for OOP functionality. It categorizes each keyword based on its current implementation status and verifies compatibility to ensure zero regression on existing `.tvk` source code.

---

## Keyword Compatibility Matrix

| Keyword | Current TokenType | Current Parser Usage | Current Semantic Meaning | Safe to Extend | Proposed OOP Meaning | Conflict Status |
|---|---|---|---|---|---|---|
| `thalapathy` | `THALAPATHY` | Root program | Entry declaration prefix | Yes | None (remains root) | No Conflict |
| `aarambam` | `AARAMBAM` | Entry block | Entry block prefix | Yes | None (remains entry) | No Conflict |
| `nanba` | `NANBA` | Variable declaration | Immutable local binding | Yes | None (remains local var) | No Conflict |
| `makkal` | `MAKKAL` | Variable declaration | Mutable local binding | Yes | None (remains local var) | No Conflict |
| `uruthi` | `URUTHI` | Constant declaration | Immutable constant binding | Yes | Contextual `final` class/method modifier | Safe (Contextual) |
| `ghilli` | `GHILLI` | If statement | Conditional branch prefix | Yes | None (remains if) | No Conflict |
| `illana` | `ILLANA` | Else statement | Alternate branch prefix | Yes | None (remains else) | No Conflict |
| `bigil` | `BIGIL` | For loop | C-style loop prefix | Yes | None (remains loop) | No Conflict |
| `vaathi` | `VAATHI` | Iterator loop | Range loop prefix | Yes | None (remains loop) | No Conflict |
| `mersal` | `MERSAL` | Function declaration | Function/Method prefix | Yes | Method declaration prefix | No Conflict |
| `thiruppi` | `THIRUPPI` | Return statement | Return value expression prefix | Yes | Return statement | No Conflict |
| `sollu` | `SOLLU` | Built-in print | Print statement prefix | Yes | Built-in print function | No Conflict |
| `master` | `MASTER` | Class declaration | Class definition prefix | Yes | Class definition prefix | No Conflict |
| `varisu` | `VARISU` | Class inheritance | Parent class indicator | Yes | Single class inheritance prefix | No Conflict |
| `kaththi` | `KATHTHI` | None (lexer only) | Reserved for interfaces | Yes | Interface declaration / list prefix | Safe (Reserved) |
| `theri` | `THERI` | Throw statement | Exception throw prefix | Yes | Throw statement | No Conflict |
| `pokkiri` | `POKKIRI` | Try block | Exception catch boundary | Yes | Exception catch boundary | No Conflict |
| `kaavalan` | `KAAVALAN` | Catch block | Exception handler boundary | Yes | Exception handler boundary | No Conflict |
| `beast` | `BEAST` | None (lexer only) | Unused | Yes | `private` member modifier | Safe (Reserved) |
| `leo` | `LEO` | None (lexer only) | Unused | Yes | `protected` member modifier | Safe (Reserved) |
| `sarkar` | `SARKAR` | None (lexer only) | Reserved for modules | **No** | None (Reserved for modules) | **Avoid Repurposing** |
| `waiting` | `WAITING` | None (lexer only) | Unused | Yes | `abstract` modifier | Safe (Reserved) |
| `goat` | `GOAT` | None (lexer only) | Unused | Yes | Generics type parameter indicator | Safe (Reserved) |
| `this` | `THIS` | Expressions | Current instance lookup | Yes | Current instance lookup | No Conflict |
| `super` | `SUPER` | Expressions | Parent class member access | Yes | Parent constructor/method access | No Conflict |
| `init` | `IDENTIFIER` | Parse constructor | Custom method name | Yes | Constructor name | No Conflict |

---

## Detailed Compatibility Decisions

1. **Access Modifiers**:
   - `sarkar` is reserved for future module/import semantics (e.g. `sarkar math;`) and will **not** be used for access modifiers.
   - Access control is public-by-default.
   - `beast` will map to `private`.
   - `leo` will map to `protected`.
   - Any member declared without a modifier is implicitly `public`.

2. **Abstract Semantics**:
   - `waiting` is currently reserved in the lexer and has no parser rules. It is safe to use as the `abstract` keyword.
   - Usage: `waiting master Shape { ... }` and `waiting mersal draw() -> void;`

3. **Interfaces**:
   - `kaththi` represents interfaces: `kaththi Fighter { mersal fight() -> void; }`.
   - Implementation uses existing `varisu` for parent, and optional `kaththi` lists for interfaces: `master Hero varisu Parent kaththi Interface1, Interface2`.

4. **Generics**:
   - `goat` acts as the type parameter flag: `master Box<goat T> { ... }`.

5. **Safe Casting**:
   - `as` will be introduced as a new keyword for safe downcasting (e.g. `obj as SubClass` returning `null` if incompatible).

6. **Properties**:
   - `seyal` will be introduced as a new keyword for property declarations: `seyal string name { paaru { ... } maathu { ... } }`.
   - `paaru` (getter) and `maathu` (setter) will be introduced as property accessory keywords.

7. **Static Keyword**:
   - `static` will be introduced as a minimal grammar extension for static class members.
