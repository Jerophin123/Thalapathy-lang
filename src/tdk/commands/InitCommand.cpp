#include "TDKCommand.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace thalapathy {

class InitCommand : public TDKCommand {
public:
    int execute(const TDKContext& ctx, const std::vector<std::string>& args) override {
        (void)args;
        fs::path tomlPath = fs::current_path() / "thalapathy.toml";

        if (fs::exists(tomlPath) && !ctx.force) {
            std::cerr << "error: thalapathy.toml already exists in current directory. Use --force to overwrite.\n";
            return 1;
        }

        std::string projName = fs::current_path().filename().string();
        if (projName.empty()) projName = "unnamed-project";

        fs::create_directories("src");
        fs::create_directories("tests");

        std::ofstream toml(tomlPath);
        if (!toml.is_open()) {
            std::cerr << "error: failed to initialize thalapathy.toml\n";
            return 1;
        }
        toml << "[project]\n"
             << "name = \"" << projName << "\"\n"
             << "version = \"0.1.0\"\n"
             << "entry = \"src/main.tvk\"\n\n"
             << "[language]\n"
             << "version = \"0.3.0\"\n\n"
             << "[build]\n"
             << "output = \"build\"\n"
             << "artifact = \"" << projName << ".vijay\"\n\n"
             << "[personality]\n"
             << "mode = \"mass\"\n";
        toml.close();

        fs::path mainPath = fs::current_path() / "src" / "main.tvk";
        if (!fs::exists(mainPath)) {
            std::ofstream mainFile(mainPath);
            if (mainFile.is_open()) {
                mainFile << "sarkar io;\n\n"
                         << "thalapathy aarambam {\n"
                         << "    ezhudhu(\"Vanakkam Nanba!\");\n"
                         << "}\n";
                mainFile.close();
            }
        }

        if (!ctx.jsonOutput) {
            std::cout << "Initialized THALAPATHY project in " << fs::current_path().string() << "\n";
        }
        return 0;
    }
};

} // namespace thalapathy
