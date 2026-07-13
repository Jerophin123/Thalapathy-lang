#include "TDKCommand.hpp"
#include "thalapathy/project/ProjectManifest.hpp"
#include <iostream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

namespace thalapathy {

class CleanCommand : public TDKCommand {
public:
    int execute(const TDKContext& ctx, const std::vector<std::string>& args) override {
        (void)args;
        fs::path tomlPath = fs::current_path() / "thalapathy.toml";
        if (!fs::exists(tomlPath)) {
            std::cerr << "error: thalapathy.toml not found in current directory\n";
            return 1;
        }

        ProjectManifest manifest;
        std::string err;
        if (!ProjectLoader::load(tomlPath.string(), manifest, err)) {
            std::cerr << "error: failed to load manifest: " << err << "\n";
            return 1;
        }

        fs::path projectRoot = fs::current_path();

        // 1. Clean build output directory
        if (!manifest.buildOutput.empty()) {
            fs::path outDir = fs::weakly_canonical(projectRoot / manifest.buildOutput);

            // Safety validations
            bool isRoot = (outDir == outDir.root_path()) || (outDir.string() == "/") || (outDir.string().length() <= 3);
            bool isProjRoot = (outDir == projectRoot);
            bool isParent = (projectRoot.string().find(outDir.string()) == 0);
            bool hasSource = (outDir == projectRoot / "src") || (outDir == projectRoot / "tests");

            if (isRoot || isProjRoot || isParent || hasSource) {
                std::cerr << "error: clean path is unsafe: " << outDir.string() << "\n";
                return 1;
            }

            if (fs::exists(outDir)) {
                try {
                    fs::remove_all(outDir);
                    if (!ctx.jsonOutput) {
                        std::cout << "Cleaned directory: " << outDir.string() << "\n";
                    }
                } catch (const std::exception& e) {
                    std::cerr << "error: failed to clean directory: " << e.what() << "\n";
                    return 1;
                }
            }
        }

        // 2. Clean build artifact
        std::string artName = manifest.buildArtifact.empty() ? (manifest.projectName + ".vijay") : manifest.buildArtifact;
        fs::path artPath = fs::weakly_canonical(projectRoot / artName);
        if (artPath.extension() == ".vijay" && fs::exists(artPath)) {
            try {
                fs::remove(artPath);
                if (!ctx.jsonOutput) {
                    std::cout << "Cleaned artifact: " << artPath.string() << "\n";
                }
            } catch (const std::exception& e) {
                std::cerr << "error: failed to clean artifact: " << e.what() << "\n";
                return 1;
            }
        }

        return 0;
    }
};

} // namespace thalapathy
