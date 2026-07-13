#include "TDKCommand.hpp"
#include "thalapathy/module/ModuleGraph.hpp"
#include "thalapathy/semantic/Resolver.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace thalapathy {

class NewCommand : public TDKCommand {
public:
    int execute(const TDKContext& ctx, const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cerr << "error: missing project name\n";
            return 1;
        }

        std::string projName = args[0];
        fs::path projPath = fs::current_path() / projName;

        if (fs::exists(projPath)) {
            std::cerr << "error: directory already exists: " << projName << "\n";
            return 1;
        }

        // Create directories
        fs::create_directories(projPath / "src");
        fs::create_directories(projPath / "tests");

        // Write thalapathy.toml
        std::ofstream toml(projPath / "thalapathy.toml");
        if (!toml.is_open()) {
            std::cerr << "error: failed to create thalapathy.toml\n";
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

        // Write src/main.tvk
        std::ofstream mainFile(projPath / "src" / "main.tvk");
        if (!mainFile.is_open()) {
            std::cerr << "error: failed to create src/main.tvk\n";
            return 1;
        }
        mainFile << "sarkar io;\n\n"
                 << "thalapathy aarambam {\n"
                 << "    ezhudhu(\"Vanakkam Nanba!\");\n"
                 << "}\n";
        mainFile.close();

        // Write README.md
        std::ofstream readme(projPath / "README.md");
        if (readme.is_open()) {
            readme << "# " << projName << "\n\n"
                   << "A THALAPATHY project created with TDK.\n\n"
                   << "## Run\n"
                   << "```bash\n"
                   << "tdk run\n"
                   << "```\n";
            readme.close();
        }

        // Write .gitignore
        std::ofstream gitignore(projPath / ".gitignore");
        if (gitignore.is_open()) {
            gitignore << "build/\n"
                      << "*.vijay\n";
            gitignore.close();
        }

        // Internally validate generated main.tvk using parser/resolver pipeline
        std::string mainPath = (projPath / "src" / "main.tvk").string();
        std::vector<std::string> graphErrors;
        ModuleGraph graph;
        if (!graph.buildGraph(mainPath, graphErrors)) {
            std::cerr << "error: generated template failed module parsing\n";
            return 1;
        }

        Resolver resolver(mainPath, "");
        bool resolved = true;
        for (const auto& modPath : graph.getLoadOrder()) {
            const auto& nodes = graph.getModules().at(modPath)->astNodes;
            bool isEntry = (modPath == mainPath);
            if (!resolver.resolve(nodes, isEntry)) {
                resolved = false;
            }
        }

        if (!resolved) {
            std::cerr << "error: generated template failed resolver semantic analysis\n";
            return 1;
        }

        if (!ctx.jsonOutput) {
            std::cout << "Created project: " << projName << "\n"
                      << "Template successfully validated.\n";
        }

        return 0;
    }
};

} // namespace thalapathy
