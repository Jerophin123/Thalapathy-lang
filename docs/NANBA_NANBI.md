# nanba & nanbi — bindings in THALAPATHY

Part of THALAPATHY **1.0.0**. Doc hub: [README.md](README.md).

Two immutable binding keywords. Both are first-class grammar, implemented across
lexer → parser → resolver → interpreter, verified by
[`examples/mass/nanba_nanbi.tvk`](../examples/mass/nanba_nanbi.tvk) (runs and
builds a VJAY `.vijay`).

## `nanba` — single immutable binding

```tvk
nanba hero = "Vijay";     // inferred, immutable
```
Immutable after init; initializer mandatory; lexically scoped; closure-capturable.

## `nanbi` — immutable pattern (destructuring) binding

The source is **evaluated exactly once**, then bound against the pattern.

```tvk
nanbi [peru, padam, varudam] = ["Vijay", "Ghilli", 2004];  // array/tuple (mixed OK)
nanbi [sum, diff, prod]      = kanakku(10, 5);              // multi-value result
nanbi [first, _, ...rest]    = [1, 2, 3, 4, 5];             // ignore + rest
nanbi { peru, title: pattam } = hero;                       // object + alias
```

Loop form:

```tvk
vaathi nanbi [padam, varudam] ulla padangal {
    sollu(padam + " - " + string(varudam));
}
```

### Pattern elements
| Syntax | Meaning |
|--------|---------|
| `name` | bind element/key to `name` |
| `_` | ignore this position |
| `...rest` | bind remaining elements as a list (array patterns) |
| `key: alias` | object pattern: read `key`, bind as `alias` |

### Diagnostics
| Code | When |
|------|------|
| `THALA-NANBI-002` | pattern expects more values than the source provides |
| `THALA-NANBI-003` | source is not destructurable (array pattern needs a list; object pattern needs a map) |
| `THALA-NANBI-004` | duplicate binding name in one pattern (resolver) |

### Notes / current limits
- Multi-value functions need an explicit `-> any` return type (untyped functions
  default to `void`).
- Heterogeneous array literals (e.g. `["Vijay", 2004]`) are now allowed and typed
  as `any[]` — this powers tuple destructuring.
- IR/VM serialization of `nanbi` is still emit-stubbed; the AST interpreter is the
  execution path today (the VM is emit-only, per [ROADMAP.md](ROADMAP.md)).
