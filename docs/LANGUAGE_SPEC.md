# THALAPATHY Language Specification v1.0.0

This document defines the features, keywords, structures, and basic syntax constraints of the THALAPATHY programming language.

> **Companion docs:** [LANGUAGE_GUIDE.md](LANGUAGE_GUIDE.md) (tutorial) · [THALAPATHY_API_LEXICON.md](THALAPATHY_API_LEXICON.md) (canonical names) · [docs/README.md](README.md) (full index)

## Keywords

### Core / declarations
- `thalapathy`: Root program keyword.
- `aarambam`: Entry block declaration.
- `nanba`: Immutable local variable declaration.
- `makkal`: Mutable local variable declaration.
- `uruthi`: Constant definition (also `uruthi master` for a final class).
- `mersal`: Function / method declaration.
- `kutty`: Anonymous function / lambda expression (a closure).
- `thiruppi`: Return statement.
- `sollu`: Print builtin.
- `sarkar`: Module / package import (supports dotted paths).
- `nadu`: Java-style package declaration (`nadu app.models;`).

### Control flow
- `ghilli`: If condition block.
- `illana`: Else block (also the default case inside `thalaivaa`).
- `bigil`: C-style for-loop structure.
- `vaathi`: Range **or foreach** loop (`vaathi i in range(a, b)` or
  `vaathi x in <array|string|map>`).
- `thuppakki`: While-loop structure (`thuppakki (cond) { ... }`).
- `thalaivaa`: Switch/match statement (`thalaivaa (subject) { ... }`).
- `vazhakku`: A case label inside a `thalaivaa` block (`vazhakku value: { ... }`).
- `break` / `continue`: Loop and switch control.
- `theri`: Exception throwing.
- `pokkiri`: Try block.
- `kaavalan`: Catch block (optional type filter: `kaavalan (MyError e)`).
- `kadaisi`: Finally block, runs whether or not an exception occurred.
- `?:` : Ternary conditional (`cond ? whenTrue : whenFalse`).

### Types / data
- `vagai`: Enum declaration (`vagai Color { RED, GREEN, BLUE }`); members expose
  `.name()`, `.ordinal()`, compare by identity, and work in `thalaivaa`.

### Object-oriented
- `master`: Class declaration.
- `waiting master`: Abstract class declaration.
- `uruthi master`: Final class declaration.
- `varisu`: Single inheritance (`master Child varisu Parent`).
- `kaththi`: Interface declaration / implementation list.
- `beast`: Private member access modifier.
- `leo`: Protected member access modifier.
- `static`: Static member.
- `seyal`: Property definition, with `paaru` (getter) and `maathu` (setter) blocks.
- `operator`: Operator overloading (`mersal operator + (T other) -> T`).
- `goat`: Generic type parameter marker (`master Box<goat T>`).
- `this` / `super`: Instance and parent references. A constructor is `mersal init(...)`.
- `as`: Safe cast expression. `aaguma`: Runtime type-test expression.

## Anonymous functions (`kutty`)
A `kutty` is a first-class closure that captures its defining scope:
```
nanba twice = kutty (int x) -> int { thiruppi x * 2; };
```
Parameter types are optional; return type is optional. They can be stored in
variables, passed as arguments, and returned from functions.

## Default parameters
Any function, method, constructor, or `kutty` parameter may have a default,
applied when the caller omits that (trailing) argument:
```
mersal vaazhthu(string per, string mudi = "da nanba") -> string {
    thiruppi "Vanakkam " + per + " " + mudi;
}
vaazhthu("Vijay");            // "Vanakkam Vijay da nanba"
vaazhthu("Vijay", "thalaiva"); // "Vanakkam Vijay thalaiva"
```

## Enums (`vagai`)
```
vagai Grade { PASS, FAIL, GOAT }
nanba g = Grade.GOAT;
g.name();     // "GOAT"
g.ordinal();  // 2
g aaguma Grade; // true
```
Enum members compare by identity and are usable as `thalaivaa` cases.

## foreach
`vaathi` iterates a `range(a, b)` **or** any array, string (by character), or
map (by key):
```
vaathi p in ["Vijay", "Ajith"] { sollu(p); }
vaathi ch in "TVK" { sollu(ch); }
```

## Ternary and finally
```
nanba label = age > 40 ? "Senior" : "Young";
pokkiri { risky(); } kaavalan (e) { sollu(e); } kadaisi { cleanup(); }
```

## Higher-order collection methods
Arrays support `map`, `filter`, `reduce`, `forEach`, `push`, `pop`, `join`,
`contains`, `indexOf`, `reverse`, `size`/`length`, `get`, `isEmpty`.
Strings support `length`, `toUpper`, `toLower`, `trim`, `substring`, `charAt`,
`contains`, `indexOf`, `startsWith`, `endsWith`, `replace`, `split`, `repeat`.
Maps support `size`, `isEmpty`, `has`, `get`, `set`, `remove`, `keys`, `values`.

## Standard library modules
Imported with `sarkar <name>;`:
- `io`, `math`, `text`, `collections`, `time` — core utilities.
- `json` — `parse(text)`, `stringify(value)`.
- `http` — `get(url)`, `post(url, body)`, `request(verb, url, body)` (API client).
- `web` — the "Sarkar" backend web framework:
  - `serve(port, handler)` — blocking HTTP/1.1 server.
  - `router()` → a `Router` with `get`/`post`/`put`/`del`/`use` and path
    parameters (`/makkal/:id` → `req["params"]["id"]`) plus middleware.
  - `app_handler(router)` adapts a router for `serve`.
  - `reply_ok`, `reply_json`, `reply_json_status`, `reply_status` response
    builders; `parse_query`, `body_json` request helpers.
- `db` — VijayDB embedded document database. `open(path)` → a `Database` with
  `insert`, `all`, `find`, `get`, `update`, `remove`, `count`. Documents are
  maps with an auto-assigned integer `_id`; state persists to a JSON file.
- `random` — `random()`, `randint(lo, hi)`, `seed(n)`, `choice(items)`.
- `os` — `name()`, `cwd()`, `exit(code)`, `millis()`, `sleep(ms)`, `getenv`,
  `setenv`, `run(cmd)`.
- `regex` — `porundhu(text, pattern)` (match), `ellam(text, pattern)` (find all),
  `pathil(text, pattern, replacement)` (replace).
- `datetime` — `ippo()` (epoch seconds), `format(seconds, pattern)`,
  `ippo_string()`.
- `log` — `seidhi` (INFO), `eccharikkai` (WARN), `pizhai` (ERROR),
  `vivaram` (DEBUG); timestamped.
- `collections` — `Stack`, `List`, `Queue`, `Set` classes.

## Building a REST API (Spring-style)
```
sarkar web;
sarkar db;

thalapathy aarambam {
    nanba store = open("makkal.db");
    nanba app = router();

    app.get("/makkal/:id", kutty (req) {
        nanba person = store.get("makkal", int(req["params"]["id"]));
        thiruppi reply_json(person);
    });
    app.post("/makkal", kutty (req) {
        thiruppi reply_json_status(201, store.insert("makkal", body_json(req)));
    });

    serve(8080, app_handler(app));
}
```

Any identifier position (map keys, member names) also accepts reserved words,
so `{ master: "Vijay", goat: true }` and `req.body` are valid.

## Packages (Java-style)
A file may declare the package it belongs to, and packages map onto nested
directories:
```
// file: app/models/hero.tvk
nadu app.models;
master Hero { ... }
```
Import from any other file with a dotted path — `sarkar app.models.hero;` —
which resolves to `app/models/hero.tvk`. Dotted (package) imports resolve from
the project root (the entry file's directory), so any sub-package can import any
other uniformly, exactly like a Java classpath. Plain names (`sarkar io;`) keep
the flat lookup (importing dir, then the standard library).

Scaffold a package with the toolkit: `tdk nadu app.models Hero` creates the
directories and a stub `Hero.tvk` already carrying `nadu app.models;`.

## Scoping
Shadowing of variables is permitted inside nested block structures `{ ... }`. Variable definitions inside range and loop blocks are localized to that block's scope. A `kutty` closure captures the environment in which it is defined.
