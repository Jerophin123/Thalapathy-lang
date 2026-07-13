#include "TDKCommand.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace thalapathy {

// `tdk nadu <dotted.package> [ClassName]`
// Creates a Java-style package: nested directories from the dotted name and a
// stub .tvk file that already carries the matching `nadu` declaration.
class NaduCommand : public TDKCommand {
public:
    int execute(const TDKContext& ctx, const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cerr << "Package peru venum da nanba: tdk nadu <nadu.per> [ClassPeru]\n";
            return 1;
        }

        std::string pkg = args[0];

        // Split dotted name into path segments.
        std::vector<std::string> segments;
        std::string current;
        for (char c : pkg) {
            if (c == '.') {
                if (current.empty()) {
                    std::cerr << "Thavaru package peru da nanba: '" << pkg << "'\n";
                    return 1;
                }
                segments.push_back(current);
                current.clear();
            } else {
                current += c;
            }
        }
        if (current.empty()) {
            std::cerr << "Thavaru package peru da nanba: '" << pkg << "'\n";
            return 1;
        }
        segments.push_back(current);

        // Build the directory path from the package segments (rooted at cwd).
        fs::path dir = fs::current_path();
        for (const auto& seg : segments) dir /= seg;

        if (fs::exists(dir) && !ctx.force) {
            std::cerr << "Antha package already irukku da nanba: " << pkg
                      << " ( -f vachu overwrite pannu)\n";
            return 1;
        }
        fs::create_directories(dir);

        // File name: explicit class name, else the last package segment.
        std::string className = (args.size() >= 2) ? args[1] : segments.back();
        std::string fileStem = (args.size() >= 2) ? args[1] : segments.back();
        fs::path filePath = dir / (fileStem + ".tvk");

        if (fs::exists(filePath) && !ctx.force) {
            std::cerr << "Antha file already irukku da nanba: " << filePath.string()
                      << " ( -f vachu overwrite pannu)\n";
            return 1;
        }

        std::ofstream out(filePath, std::ios::trunc);
        if (!out.is_open()) {
            std::cerr << "File create panna mudiyala da nanba: " << filePath.string() << "\n";
            return 1;
        }
        out << "// " << fileStem << ".tvk — package: " << pkg << "\n"
            << "nadu " << pkg << ";\n\n"
            << "master " << className << " {\n"
            << "    mersal init() {\n"
            << "    }\n\n"
            << "    mersal sollu_peru() -> string {\n"
            << "        thiruppi \"" << className << " — nadu " << pkg << "\";\n"
            << "    }\n"
            << "}\n";
        out.close();

        if (ctx.jsonOutput) {
            std::cout << "{\n"
                      << "  \"package\": \"" << pkg << "\",\n"
                      << "  \"path\": \"" << filePath.generic_string() << "\",\n"
                      << "  \"class\": \"" << className << "\"\n"
                      << "}\n";
        } else {
            std::cout << "Nadu ready da nanba! 🔥\n"
                      << "  package : " << pkg << "\n"
                      << "  file    : " << filePath.generic_string() << "\n"
                      << "  import  : sarkar " << pkg << "." << fileStem << ";\n";
        }
        return 0;
    }
};

} // namespace thalapathy
