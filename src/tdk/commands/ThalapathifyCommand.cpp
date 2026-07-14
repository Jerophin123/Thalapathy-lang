#pragma once
#include "TDKCommand.hpp"
#include "thalapathy/lexer/Lexer.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include <filesystem>

namespace thalapathy {

// `tdk thalapathify <path> [-f]` — rewrite legacy English syntax to canonical
// THALAPATHY spellings. Token-aware (built on the real Lexer), so it never
// touches strings, comments, or identifier substrings — only genuine keyword
// tokens are rewritten. Default previews; `-f` writes.
class ThalapathifyCommand : public TDKCommand {
    static const char* canonicalFor(TokenType t) {
        switch (t) {
            case TokenType::TRUE_VAL:  return "aama";
            case TokenType::FALSE_VAL: return "illa";
            case TokenType::NULL_VAL:  return "onnumilla";
            case TokenType::THIS:      return "naan";
            case TokenType::SUPER:     return "munnadi";
            case TokenType::BREAK:     return "interval";
            case TokenType::CONTINUE:  return "aduthu";
            case TokenType::STATIC:    return "podhu";
            case TokenType::AS:        return "maathiko";
            default:                   return nullptr;
        }
    }

    struct Rep { size_t off; size_t len; std::string to; };

    int processFile(const std::string& path, bool write) {
        std::ifstream in(path, std::ios::binary);
        if (!in) { std::cerr << "error: cannot read " << path << "\n"; return 1; }
        std::stringstream ss; ss << in.rdbuf();
        std::string src = ss.str();
        in.close();

        Lexer lexer(path, src);
        auto tokens = lexer.tokenize();
        std::vector<Rep> reps;
        std::map<std::string, int> counts;
        for (const auto& tok : tokens) {
            const char* canon = canonicalFor(tok.type);
            if (!canon) continue;
            if (tok.value == canon) continue; // already canonical
            reps.push_back({tok.span.byte_offset, tok.value.size(), canon});
            counts[tok.value + std::string(" -> ") + canon]++;
        }
        if (reps.empty()) {
            std::cout << "  " << path << ": already THALAPATHY-canonical. Mass. \n";
            return 0;
        }
        std::cout << (write ? "thalapathified " : "would thalapathify ") << path
                  << " (" << reps.size() << " change(s))\n";
        for (const auto& kv : counts)
            std::cout << "    " << kv.second << "  " << kv.first << "\n";

        if (write) {
            std::sort(reps.begin(), reps.end(),
                      [](const Rep& a, const Rep& b) { return a.off > b.off; });
            for (const auto& r : reps) src.replace(r.off, r.len, r.to);
            std::ofstream out(path, std::ios::binary);
            out << src;
        }
        return 0;
    }

public:
    int execute(const TDKContext& ctx, const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cerr << "usage: tdk thalapathify <path> [-f]\n"
                      << "  previews by default; -f writes the canonical syntax in place\n";
            return 1;
        }
        bool write = ctx.force;
        namespace fs = std::filesystem;
        std::string path = args[0];
        int rc = 0;
        if (fs::is_directory(path)) {
            for (auto& e : fs::recursive_directory_iterator(path)) {
                if (e.is_regular_file() && e.path().extension() == ".tvk")
                    rc |= processFile(e.path().string(), write);
            }
        } else {
            rc = processFile(path, write);
        }
        if (!write)
            std::cout << "\nPreview mattum da. -f kudutha canonical-ah maathidum. 🔥\n";
        return rc;
    }
};

} // namespace thalapathy
