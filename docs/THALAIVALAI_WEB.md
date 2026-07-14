# THALAIVALAI — THALAPATHY Web Application Framework 🌐

**Version 1.0.0** · Doc hub: [README.md](README.md) · Scaffold: `tdk new-web <name>`

A Vijay-native web layer on top of the built-in HTTP/1.1 server. Additive — it
changes **no** existing syntax. Verified end-to-end with real HTTP requests
(`examples/web/02_alangaaram/`).

```tvk
sarkar thalaivalai;
sarkar bathil;

thalapathy aarambam {
    nanba app = thalaivalai.pudhu();

    app.sotthu("/alangaaram", "examples/web/02_alangaaram/alangaaram"); // static CSS
    app.sotthu("/nadanam", "examples/web/02_alangaaram/nadanam");       // static JS
    app.paarvaiIdam("examples/web/02_alangaaram/paarvai");              // view dir

    app.kelu("/", kutty (kelvi) { thiruppi app.kaattu("veedu.html", onnumilla); });
    app.kelu("/api/heroes", kutty (kelvi) { thiruppi bathil.jilla([ /* ... */ ]); });
    app.kelu("/hero/<peru>", kutty (kelvi) {
        thiruppi bathil.urai("Vanakkam " + kelvi.vazhiThunai("peru") + " nanba!");
    });

    app.oduda(8080);
}
```

## App API (`thalaivalai.pudhu()`)
| API | Meaning |
|-----|---------|
| `app.kelu(path, h)` | GET route |
| `app.anuppu(path, h)` | POST route |
| `app.muzhusaMaathu(path, h)` | PUT route |
| `app.konjamMaathu(path, h)` | PATCH route |
| `app.neekku(path, h)` | DELETE route |
| `app.naduvila(fn)` | middleware (returns a response to short-circuit, `onnumilla` to continue) |
| `app.sotthu(prefix, dir)` | mount a static directory (`makkal` is a reserved keyword, so the method is `sotthu`) |
| `app.paarvaiIdam(dir)` | set the template/view directory |
| `app.kaattu(name, data)` | render a view with `{{ var }}` interpolation (auto HTML-escaped) |
| `app.oduda(port)` | start the (blocking) server |

Dynamic route params use `<name>` or `:name`; read them via `kelvi.vazhiThunai("name")`.

## `bathil` — responses
| API | Result |
|-----|--------|
| `bathil.urai(text)` | 200 `text/plain` |
| `bathil.kaattu(html)` | 200 `text/html` |
| `bathil.jilla(obj)` | 200 `application/json` |
| `bathil.nilai(code, body)` | custom status HTML |
| `bathil.thiruppuVazhi(loc)` | 302 redirect (`Location` header) |

## `Kelvi` — the request
`murai()` method · `vazhi()` path · `urai()` raw body · `thalai()` headers ·
`jilla()` JSON body · `kelvikuri()` query · `padivam()` form data ·
`vazhiThunai(name)` route param.

## Verified (curl)
```
GET /                    200  text/html                 ✓ renders
GET /alangaaram/mass.css 200  text/css                  ✓
GET /nadanam/mass.js     200  application/javascript    ✓
GET /api/heroes          200  application/json          ✓
GET /hero/Vijay          200  "Vanakkam Vijay nanba!"   ✓ dynamic param
GET /missing             404                             ✓
```
Correct content types mean a real browser renders the HTML, applies the CSS, and
executes the JS.

## Shipped vs staged
- **Shipped & verified:** app + routing (static/dynamic `<param>`), static file
  serving with MIME + `..` traversal guard, `bathil` responses (incl. redirect via
  the new response-`headers` support), `Kelvi` request object, `kaattu` template
  interpolation with HTML escaping, JSON API. Custom response headers now flow
  from `.tvk` (redirect / security headers / ETag).
- **Staged (honest — not shipped):** `{% ghilli %}`/`{% vaathi %}` template
  control-flow (only `{{ }}` interpolation today), sessions (`amarvu`), cookies,
  CSRF, WebSocket (`neradiValai`), SSE (`thodarAnuppu`), `tdk new-web` scaffolding,
  `--reload`/production modes, binary static assets (images — native reads text
  only today), async handlers (depend on the staged `varum`/`kaathiru` runtime).
