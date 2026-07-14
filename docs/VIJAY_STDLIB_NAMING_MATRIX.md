# VIJAY STANDARD LIBRARY — CANONICAL API NAMING MATRIX

> Companion to [THALAPATHY_API_LEXICON.md](THALAPATHY_API_LEXICON.md).
> Every public API, its conventional equivalent, and its live status.
> **Canonical = THALAPATHY name.** English column = compatibility alias only.

Legend — **Impl**: ✅ implemented & runnable · ⏳ staged (native primitive
pending) · **Test**: ✅ has example/test · ▫️ pending.

---

## Global built-ins  *(no import)*

| Conventional | THALAPATHY | Meaning | Alias | Impl | Test |
|---|---|---|---|---|---|
| println | `sollu` | print line | println | ✅ | ✅ |
| print | `sollikko` | print, no newline | print | ✅ | ✅ |
| input | `vaanganna` (builtin) | read line | input | ✅ | ✅ |
| typeof | `yaaru` | runtime type name | typeof | ✅ | ✅ |
| len | `evlo` | length/size | len | ✅ | ✅ |
| range | `varisai` | int sequence | range | ✅ | ✅ |
| max | `perusu` | maximum | max | ✅ | ✅ |
| min | `sirusu` | minimum | min | ✅ | ✅ |
| sum | `mothham` | sum iterable | sum | ✅ | ✅ |
| assert | `uruthiPannu` | assert cond,msg | assert | ✅ | ✅ |
| panic | `adithozha` | abort w/ message | panic | ✅ | ✅ |
| exit | `mudichidu` | exit(code) | exit | ✅ | ✅ |
| isNull | `kaaliya` | null check | isNull | ✅ | ✅ |
| clone | `nagaledu` | deep copy | clone | ✅ | ▫️ |
| isCallable | `koopidalaam` | callable? | isCallable | ✅ | ▫️ |

---

## `vaanganna` — I/O

| Conventional | Canonical | Meaning | Impl |
|---|---|---|---|
| print(x) | `sollu(x)` (global) | say — `sollu` is a grammar token, so output stays the global builtin; `vaanganna` owns input | ✅ |
| readLine() | `vaanganna.kelu(prompt)` | ask & read line | ✅ |
| readLine() | `vaanganna.vaasi()` | read line, no prompt | ✅ |
| readInt() | `vaanganna.ennaNumber(prompt?)` | read integer | ✅ |
| readFloat() | `vaanganna.ennaDecimal(prompt?)` | read float | ✅ |
| readBool() | `vaanganna.aamaIlla(prompt?)` | read yes/no → bool | ✅ |

## `sivakasi` — Math

| Conventional | Canonical | Impl |  | Conventional | Canonical | Impl |
|---|---|---|---|---|---|---|
| PI | `sivakasi.PI` | ✅ | | abs | `sivakasi.ver` | ✅ |
| E | `sivakasi.E` | ✅ | | ceil | `sivakasi.melae` | ✅ |
| sqrt | `sivakasi.mulai` | ✅ | | floor | `sivakasi.keezhae` | ✅ |
| cbrt | `sivakasi.moonாமmulai`→`sivakasi.moonmulai` | ✅ | | round | `sivakasi.roundu` | ✅ |
| pow | `sivakasi.power` | ✅ | | min | `sivakasi.sirusu` | ✅ |
| sin | `sivakasi.sin` | ✅ | | max | `sivakasi.perusu` | ✅ |
| cos | `sivakasi.cos` | ✅ | | clamp | `sivakasi.naduvula` | ✅ |
| tan | `sivakasi.tan` | ✅ | | gcd | `sivakasi.pothu` | ✅ |
| log | `sivakasi.log` | ✅ | | lcm | `sivakasi.mடங்கு`→`sivakasi.madangu` | ✅ |
| exp | `sivakasi.exp` | ✅ | | factorial | `sivakasi.fact` | ✅ |
| degrees/radians | `sivakasi.degrees`/`sivakasi.radians` | ✅ | | isPrime | `sivakasi.primeah` | ✅ |

## `vaseegara` — Text / String

| Conventional | Canonical | Impl |  | Conventional | Canonical | Impl |
|---|---|---|---|---|---|---|
| upper | `vaseegara.perusaSollu` | ✅ | | trim | `vaseegara.trimPannu` | ✅ |
| lower | `vaseegara.sirusaSollu` | ✅ | | reverse | `vaseegara.thiruppu` | ✅ |
| capitalize | `vaseegara.mudhalezhuthu` | ✅ | | repeat | `vaseegara.thirumba` | ✅ |
| contains | `vaseegara.irukka` | ✅ | | split | `vaseegara.pirichu` | ✅ |
| startsWith | `vaseegara.aarambam` (fn) | ✅ | | join | `vaseegara.serthu` | ✅ |
| endsWith | `vaseegara.mudivu` | ✅ | | replace | `vaseegara.maathidu` | ✅ |
| padLeft | `vaseegara.idhuvaaram`→`vaseegara.idamNிரப்பு`→`vaseegara.idamFill` | ✅ | | count | `vaseegara.ennu` | ✅ |
| padRight | `vaseegara.valamFill` | ✅ | | isNumeric | `vaseegara.numberaah` | ✅ |

## `kootam` — Collections

| Conventional | Canonical class | FIFO/LIFO | Impl |
|---|---|---|---|
| Stack | `kootam.Adukku` | LIFO | ✅ |
| Queue | `kootam.Varisai` | FIFO | ✅ |
| Deque | `kootam.IruTalai` | double-ended | ✅ |
| Set (unique) | `kootam.ThaniKootam` | unique members | ✅ |
| Map wrapper | `kootam.Varaipadam` | key→value | ✅ |
| — push/add | `.serthu(x)` | | ✅ |
| — pop/remove | `.eduthudu()` | | ✅ |
| — peek/front | `.munnadi()` | | ✅ |
| — size | `.evlo()` | | ✅ |
| — contains | `.irukka(x)` | | ✅ |

## `velayudham` — Filesystem

| Conventional | Canonical | Impl |
|---|---|---|
| readText | `velayudham.padichu(path)` | ✅ |
| writeText | `velayudham.ezhudhu(path, text)` | ✅ |
| appendText | `velayudham.serthuEzhudhu(path, text)` | ✅ |
| exists | `velayudham.irukka(path)` | ✅ |
| remove | `velayudham.azhi(path)` | ⏳ (native rm pending) |
| copy | `velayudham.nகல்edu`→`velayudham.nagaledu(a,b)` | ✅ (read+write) |
| size | `velayudham.evlo(path)` | ✅ |

## `udhaya` — Path

| Conventional | Canonical | Impl |
|---|---|---|
| join | `udhaya.seru(a, b)` | ✅ |
| parent | `udhaya.periyaVeedu(p)` | ✅ |
| filename | `udhaya.peru(p)` | ✅ |
| extension | `udhaya.vagaipeyar(p)` | ✅ |
| stem | `udhaya.adippeyar(p)` | ✅ |
| normalize | `udhaya.sariseidhu(p)` | ✅ |

## `jilla` — JSON

| Conventional | Canonical | Meaning | Impl |
|---|---|---|---|
| parse | `jilla.purinjiko(text)` | JSON string → value | ✅ |
| stringify | `jilla.solluData(value)` | value → JSON string | ✅ |
| pretty | `jilla.massAhKaatu(value)` | pretty JSON | ✅ |

## `vettaikaaran` — Regex

| Conventional | Canonical | Meaning | Impl |
|---|---|---|---|
| match | `vettaikaaran.pidichacha(pat, txt)` | full match → bool | ✅ |
| search/findAll | `vettaikaaran.thediPidi(pat, txt)` | all matches → list | ✅ |
| replace | `vettaikaaran.maathidu(pat, rep, txt)` | regex replace | ✅ |

## `deva` — Random

| Conventional | Canonical | Impl |
|---|---|---|
| random() | `deva.adhirshtam()` | ✅ |
| randomInt(a,b) | `deva.number(a, b)` | ✅ |
| choice(list) | `deva.eduthuko(list)` | ✅ |
| shuffle(list) | `deva.kalappu(list)` | ✅ |
| seed(n) | `deva.vidhi(n)` | ✅ |

## `neram` — Time / Date

| Conventional | Canonical | Impl |
|---|---|---|
| now (ms) | `neram.ippo()` | ✅ |
| monotonic (ms) | `neram.odiyathu()` | ✅ |
| sleep(ms) | `neram.kaathiru(ms)` | ✅ |
| dateNow (s) | `neram.indruSeconds()` | ✅ |
| format(fmt) | `neram.kaatu(fmt)` | ✅ |

## `satham` — Logging

| Conventional | Canonical | Level | Impl |
|---|---|---|---|
| logger(name) | `satham.voice(name)` | — | ✅ |
| trace | `log.suvadu(msg)` | TRACE | ✅ |
| debug | `log.kannu(msg)` | DEBUG (renamed: `paaru` is reserved) | ✅ |
| info | `log.satham(msg)` | INFO | ✅ |
| warn | `log.warning(msg)` | WARN | ✅ |
| error | `log.pizhai(msg)` | ERROR (renamed: `theri` is reserved) | ✅ |
| fatal | `log.mudivu(msg)` | FATAL | ✅ |

## `villu` — HTTP

| Conventional | Canonical | Method | Impl |
|---|---|---|---|
| GET | `villu.kelu(url)` | GET | ✅ |
| POST | `villu.anuppu(url, body)` | POST | ✅ |
| PUT | `villu.puduppi(url, body)` | PUT (renamed: `maathu` is reserved) | ✅ |
| PATCH | `villu.thiruthu(url, body)` | PATCH | ✅ |
| DELETE | `villu.azhi(url)` | DELETE | ✅ |

## `arasu` — System / Process / Env

| Conventional | Canonical | Impl |
|---|---|---|
| platform | `arasu.naadu()` | ✅ |
| cwd | `arasu.ippoIdam()` | ✅ |
| getenv | `arasu.kaes(name)` | ✅ |
| setenv | `arasu.vai(name, val)` | ✅ |
| exec | `arasu.aanai(cmd)` | ✅ |
| exit | `arasu.mudichidu(code)` | ✅ |

## `varisai` — CSV  *(RFC-aware, never `split(",")`)*

| Conventional | Canonical | Impl |
|---|---|---|
| parse | `varisai.pirichu(text)` → list of rows | ✅ |
| stringify | `varisai.serthu(rows)` → csv text | ✅ |

## `cupmukkiyam` — Testing

| Conventional | Canonical | Impl |
|---|---|---|
| test(name, fn) | `cupmukkiyam(name, fn)` | ✅ |
| assertEqual | `correctAh(a, b)` | ✅ |
| assertTrue | `aama(x)` | ✅ |
| assertFalse | `illa(x)` | ✅ |
| assertNull | `kaali(x)` | ✅ |
| assertThrows | `theriyuma(fn)` | ✅ |

## `ozhungu` — Algorithms  ·  `kootu` — Functional

| Conventional | Canonical | Module | Impl |
|---|---|---|---|
| sort | `ozhungu.ozhunguPannu(list)` | ozhungu | ✅ |
| binarySearch | `ozhungu.paathiThedal(list, x)` | ozhungu | ✅ |
| reverse | `ozhungu.thiruppu(list)` | ozhungu | ✅ |
| unique | `ozhungu.thaniyaa(list)` | ozhungu | ✅ |
| map | `kootu.maatru(list, fn)` | kootu | ✅ |
| filter | `kootu.vadikattu(list, fn)` | kootu | ✅ |
| reduce | `kootu.suruku(list, fn, init)` | kootu | ✅ |
| pipe | `kootu.thodar(fns)` | kootu | ✅ |

## Staged (native primitive pending — no fake bodies shipped)

| Module | Area | Missing native |
|---|---|---|
| `padai` | Threading | `std::thread`/mutex bridge |
| `varum` | Async | event-loop + `waiting` audit |
| `nanban` | Networking | TCP/UDP socket bridge |
| `pothi` | Serialization | binary encode primitive |
| `maatram` | Encoding | base64/hex primitive |
| `adayalam` | UUID | secure-random UUID primitive |

> Any cell showing a Tamil-script placeholder (e.g. `moonாமmulai`) is normalized
> to its ASCII implemented form in the arrow (`→ moonmulai`); the ASCII form is
> what the interpreter accepts.
