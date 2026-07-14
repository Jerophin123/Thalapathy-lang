#include "thalapathy/format/Formatter.hpp"
#include "thalapathy/format/FormatterConfig.hpp"
#include "thalapathy/installation/TdkHome.hpp"
#include "thalapathy/version/Version.hpp"
#include "thalapathy/lexer/Lexer.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <algorithm>

// Token-aware legacy->canonical rewrite (shared behaviour with `tdk thalapathify`).
static const char* canonicalSpelling(thalapathy::TokenType t) {
    using T = thalapathy::TokenType;
    switch (t) {
        case T::TRUE_VAL:  return "aama";
        case T::FALSE_VAL: return "illa";
        case T::NULL_VAL:  return "onnumilla";
        case T::THIS:      return "naan";
        case T::SUPER:     return "munnadi";
        case T::BREAK:     return "interval";
        case T::CONTINUE:  return "aduthu";
        case T::STATIC:    return "podhu";
        case T::AS:        return "maathiko";
        default:           return nullptr;
    }
}

static std::string thalapathifySource(const std::string& path, const std::string& src, int& changes) {
    thalapathy::Lexer lexer(path, src);
    auto tokens = lexer.tokenize();
    struct Rep { size_t off; size_t len; std::string to; };
    std::vector<Rep> reps;
    for (const auto& tok : tokens) {
        const char* canon = canonicalSpelling(tok.type);
        if (!canon || tok.value == canon) continue;
        reps.push_back({tok.span.byte_offset, tok.value.size(), canon});
    }
    changes = static_cast<int>(reps.size());
    std::string out = src;
    std::sort(reps.begin(), reps.end(), [](const Rep& a, const Rep& b){ return a.off > b.off; });
    for (const auto& r : reps) out.replace(r.off, r.len, r.to);
    return out;
}

namespace fs = std::filesystem;

static std::string readWholeFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) return "";
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

static bool writeWholeFile(const std::string& path, const std::string& content) {
    std::ofstream out(path);
    if (!out.is_open()) return false;
    out << content;
    return true;
}

int main(int argc, char* argv[]) {
    bool writeMode = false;
    bool checkMode = false;
    bool thalapathifyMode = false;
    std::vector<std::string> paths;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--version") {
            std::cout << "thalafmt " << thalapathy::getTdkVersion() << "\n";
            return 0;
        } else if (arg == "--write") {
            writeMode = true;
        } else if (arg == "--check") {
            checkMode = true;
        } else if (arg == "--thalapathify") {
            thalapathifyMode = true;
        } else if (arg.rfind("-", 0) == 0) {
            std::cerr << "error: unknown option '" << arg << "'\n";
            return 1;
        } else {
            paths.push_back(arg);
        }
    }

    if (paths.empty()) {
        std::cerr << "Usage:\n"
                  << "  thalafmt <file.tvk|dir> [--write] [--check]\n"
                  << "  thalafmt --version\n";
        return 1;
    }

    // Configuration discovery: TDK_HOME/conf/formatter.conf -> defaults
    thalapathy::FormatterConfig fmtConfig;
    auto homeResult = thalapathy::installation::resolveTdkHome();
    if (homeResult.success) {
        fs::path confPath = homeResult.path / "conf" / "formatter.conf";
        if (fs::exists(confPath)) {
            auto configResult = thalapathy::loadFormatterConfig(confPath.string());
            if (configResult.success) {
                fmtConfig = configResult.config;
            } else {
                std::cerr << "warning: failed to load formatter.conf: " << configResult.error << "\n";
            }
        }
    }

    thalapathy::Formatter formatter(fmtConfig);
    bool checkFailed = false;

    // Collect all files (also used by --thalapathify below)
    std::vector<std::string> filesToFormat;
    for (const auto& pathStr : paths) {
        if (!fs::exists(pathStr)) {
            std::cerr << "error: path does not exist: " << pathStr << "\n";
            return 1;
        }
        if (fs::is_directory(pathStr)) {
            for (const auto& entry : fs::recursive_directory_iterator(pathStr)) {
                if (entry.is_regular_file() && entry.path().extension() == ".tvk") {
                    filesToFormat.push_back(entry.path().string());
                }
            }
        } else {
            filesToFormat.push_back(pathStr);
        }
    }

    if (thalapathifyMode) {
        for (const auto& filePath : filesToFormat) {
            std::string original = readWholeFile(filePath);
            int changes = 0;
            std::string rewritten = thalapathifySource(filePath, original, changes);
            if (changes == 0) {
                std::cout << "  " << filePath << ": already canonical\n";
                continue;
            }
            if (writeMode) {
                writeWholeFile(filePath, rewritten);
                std::cout << "thalapathified " << filePath << " (" << changes << " change(s))\n";
            } else if (checkMode) {
                std::cout << "Has legacy syntax: " << filePath << " (" << changes << " change(s))\n";
                checkFailed = true;
            } else {
                std::cout << rewritten;
            }
        }
        return (checkMode && checkFailed) ? 1 : 0;
    }

    for (const auto& filePath : filesToFormat) {
        std::string original = readWholeFile(filePath);
        std::string err;
        std::string formatted = formatter.format(original, err);

        if (original != formatted) {
            if (checkMode) {
                std::cout << "Not formatted: " << filePath << "\n";
                checkFailed = true;
            } else if (writeMode) {
                if (writeWholeFile(filePath, formatted)) {
                    std::cout << "Formatted: " << filePath << "\n";
                } else {
                    std::cerr << "error: failed to write formatted content to " << filePath << "\n";
                    return 1;
                }
            } else {
                // Default prints formatted code to stdout
                std::cout << formatted;
            }
        } else {
            if (checkMode) {
                // Clean output on successful formatting
            }
        }
    }

    if (checkMode && checkFailed) {
        return 1; // non-zero exit code on formatting checks failure
    }

    return 0;
}
