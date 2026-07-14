#pragma once
#include "TDKCommand.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>

namespace thalapathy {

// `tdk new-web <name>` — scaffold a runnable THALAIVALAI web project.
class NewWebCommand : public TDKCommand {
    static void put(const std::filesystem::path& p, const std::string& content) {
        std::ofstream out(p, std::ios::binary);
        out << content;
    }

public:
    int execute(const TDKContext& ctx, const std::vector<std::string>& args) override {
        (void)ctx;
        if (args.empty()) {
            std::cerr << "usage: tdk new-web <project-name>\n";
            return 1;
        }
        namespace fs = std::filesystem;
        fs::path root = args[0];
        if (fs::exists(root)) {
            std::cerr << "error: '" << root.string() << "' already irukku da. Vaera peru kudu.\n";
            return 1;
        }
        for (const char* d : {"src", "paarvai", "alangaaram", "nadanam", "makkal"})
            fs::create_directories(root / d);

        put(root / "thalapathy.toml",
            "[thoguppu]\nname = \"" + root.string() + "\"\nversion = \"0.1.0\"\nentry = \"src/main.tvk\"\n");

        put(root / "src" / "main.tvk",
            "// " + root.string() + " — THALAIVALAI web app. Run:  thalapathy src/main.tvk\n"
            "sarkar thalaivalai;\n"
            "sarkar bathil;\n\n"
            "thalapathy aarambam {\n"
            "    nanba app = thalaivalai.pudhu();\n"
            "    app.sotthu(\"/alangaaram\", \"alangaaram\");\n"
            "    app.sotthu(\"/nadanam\", \"nadanam\");\n"
            "    app.paarvaiIdam(\"paarvai\");\n\n"
            "    app.kelu(\"/\", kutty (kelvi) {\n"
            "        thiruppi app.kaattu(\"veedu.html\", { peru: \"Nanba\" });\n"
            "    });\n\n"
            "    app.kelu(\"/api/nalam\", kutty (kelvi) {\n"
            "        thiruppi bathil.jilla({ nilai: \"mass\" });\n"
            "    });\n\n"
            "    app.oduda(8080);\n"
            "}\n");

        put(root / "paarvai" / "veedu.html",
            "<!DOCTYPE html>\n<html lang=\"ta\">\n<head>\n"
            "  <meta charset=\"utf-8\">\n  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
            "  <title>THALAIVALAI</title>\n  <link rel=\"stylesheet\" href=\"/alangaaram/mass.css\">\n</head>\n"
            "<body>\n  <main class=\"hero\">\n    <h1>Vanakkam {{ peru }}! \xF0\x9F\x94\xA5</h1>\n"
            "    <p>THALAIVALAI-la render aana page.</p>\n  </main>\n"
            "  <script src=\"/nadanam/mass.js\"></script>\n</body>\n</html>\n");

        put(root / "paarvai" / "404.html",
            "<!DOCTYPE html>\n<html><head><meta charset=\"utf-8\"><title>404</title></head>\n"
            "<body><h1>404 \xE2\x80\x94 Intha vazhi kaanom da nanba \xF0\x9F\x98\xAD</h1></body></html>\n");

        put(root / "alangaaram" / "mass.css",
            "body{margin:0;font-family:system-ui,sans-serif;background:#05010a;color:#fff}\n"
            ".hero{min-height:100vh;display:grid;place-items:center;text-align:center;gap:1rem}\n"
            ".hero h1{font-size:clamp(2rem,6vw,4rem);margin:0}\n");

        put(root / "nadanam" / "mass.js",
            "document.addEventListener(\"DOMContentLoaded\",()=>{\n"
            "  console.log(\"Vanakkam Nanba! Nadanam browser-la aarambichiduchu. \\uD83D\\uDD25\");\n});\n");

        put(root / "README.md",
            "# " + root.string() + "\n\nTHALAIVALAI web app.\n\n```\nthalapathy src/main.tvk\n# open http://localhost:8080\n```\n\n"
            "- `paarvai/` views  \n- `alangaaram/` CSS  \n- `nadanam/` JS  \n- `makkal/` static assets\n");

        std::cout << "THALAIVALAI project ready da: " << root.string() << " \xF0\x9F\x94\xA5\n"
                  << "  cd " << root.string() << "\n"
                  << "  thalapathy src/main.tvk   # http://localhost:8080\n";
        return 0;
    }
};

} // namespace thalapathy
