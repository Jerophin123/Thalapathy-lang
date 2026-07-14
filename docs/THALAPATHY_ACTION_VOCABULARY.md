# THALAPATHY Controlled Action Vocabulary

> One word per operation, across every module. This prevents "a different Tamil
> word for the same thing in every library." Before any new API name is coined,
> it must reuse a verb from this table (or justify a new family here).

## Core action families

| Verb | Meaning | Used in |
|------|---------|---------|
| `sollu` | output / display | global builtin |
| `kelu` | request/receive (input, HTTP GET) | vaanganna, villu |
| `vaangu` / `vaanganna` | obtain / fetch / receive | io, queue dequeue |
| `anuppu` | send (POST, socket send) | villu, nanban |
| `paaru` | inspect / view / get a visible value | getters, map get |
| `kandupidi` | find (returning the item) | algorithms |
| `thedu` | search (returning position/presence) | regex, algorithms |
| `ozhungu` | order / sort | ozhungu |
| `serthu` | combine / append / join | lists, text, files |
| `pirichu` | split / partition | text, csv, regex |
| `maathu` / `maathidu` | modify / replace / transform | text, http PUT |
| `thiruppu` | reverse / convert-back | text, algorithms |
| `thiruppi` | **function return** (language kw) | grammar |
| `padichu` | read | velayudham |
| `ezhudhu` | write | velayudham |
| `irukka` | exists / contains | fs, collections, text |
| `neelam` / `evlo` | length / size | collections, text |
| `kaaliyaa` / `kaaliya` | empty check | collections |
| `neekku` | remove / delete | lists, fs, http DELETE |
| `nakal` / `nagaledu` | copy | fs, lists |
| `uruthiPannu` | assert / verify | testing |
| `pudhu` | construct a new object (non-`master` factories) | collections, option/result |
| `kaathiru` | suspend / await (staged) | async |
| `varum` | async/task-producing (staged) | async |
| `oduda` | run / execute / start server | web, db |
| `thira` / `moodu` | open / close | db, files |

## Rules
1. **Reuse before invent.** New APIs pick from the table above.
2. **Questions end in `-a`/`-ah`** (`irukka`, `primeah`, `kaaliya`).
3. **Superlatives**: `perusu` (max/big), `sirusu` (min/small), `naduvula` (clamp).
4. **No reserved keyword may be a method name.** Blocked: `sollu paaru maathu
   theri kaththi pokkiri thuppakki thalaivaa mersal master varisu` etc. — see
   [THALAPATHY_API_LEXICON.md](THALAPATHY_API_LEXICON.md). (This is why logging
   uses `kannu`/`pizhai`, HTTP PUT uses `puduppi`.)
5. **Method identifiers stay ASCII.** Only printed strings are Tamil-lish.
6. **One verb, one meaning.** Do not use `maathu` for both "replace" and "PUT"
   without an explicit distinct method (`puduppi` PUT vs `thiruthu` PATCH).
