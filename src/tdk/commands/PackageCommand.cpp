#include "TDKCommand.hpp"
#include "thalapathy/project/ProjectManifest.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace thalapathy {

class PackageCommand : public TDKCommand {
public:
    int execute(const TDKContext& ctx, const std::vector<std::string>& args) override {
        bool includeSource = false;
        for (const auto& arg : args) {
            if (arg == "--include-source") {
                includeSource = true;
            }
        }

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

        std::string pkgName = manifest.projectName + ".tvkpkg";
        fs::path pkgPath = fs::current_path() / pkgName;

        if (fs::exists(pkgPath)) {
            fs::remove_all(pkgPath);
        }
        fs::create_directories(pkgPath);

        fs::path artFile = fs::path(manifest.buildOutput) / (manifest.buildArtifact.empty() ? (manifest.projectName + ".vijay") : manifest.buildArtifact);
        if (!fs::exists(artFile)) {
            artFile = fs::current_path() / (manifest.buildArtifact.empty() ? (manifest.projectName + ".vijay") : manifest.buildArtifact);
        }

        if (!fs::exists(artFile)) {
            std::cerr << "error: compiled artifact not found. Run 'tdk build' first.\n";
            return 1;
        }

        fs::copy_file(artFile, pkgPath / artFile.filename());
        fs::copy_file(tomlPath, pkgPath / "thalapathy.toml");

        std::ofstream pkgJson(pkgPath / "PACKAGE.json");
        if (!pkgJson.is_open()) {
            std::cerr << "error: failed to create PACKAGE.json\n";
            return 1;
        }
        pkgJson << "{\n"
                << "  \"name\": \"" << manifest.projectName << "\",\n"
                << "  \"version\": \"" << manifest.projectVersion << "\",\n"
                << "  \"artifact\": \"" << artFile.filename().string() << "\",\n"
                << "  \"packagedWithSource\": " << (includeSource ? "true" : "false") << "\n"
                << "}\n";
        pkgJson.close();

        if (includeSource) {
            fs::create_directories(pkgPath / "src");
            for (const auto& entry : fs::recursive_directory_iterator(fs::current_path() / "src")) {
                if (entry.is_regular_file()) {
                    fs::path rel = fs::relative(entry.path(), fs::current_path() / "src");
                    fs::create_directories((pkgPath / "src" / rel).parent_path());
                    fs::copy_file(entry.path(), pkgPath / "src" / rel);
                }
            }
        }

        if (!ctx.jsonOutput) {
            std::cout << "Successfully packaged project as: " << pkgName << "\n";
        }
        return 0;
    }
};

} // namespace thalapathy
