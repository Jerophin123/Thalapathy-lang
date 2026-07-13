#include "TDKCommand.hpp"
#include "thalapathy/project/ProjectManifest.hpp"
#include "thalapathy/cli/CLI.hpp"
#include "thalapathy/personality/Personality.hpp"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

namespace thalapathy {

class RunCommand : public TDKCommand {
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

        PersonalityMode mode = PersonalityMode::Professional;
        if (manifest.personalityMode == "mass") mode = PersonalityMode::Mass;
        else if (manifest.personalityMode == "political-parody") mode = PersonalityMode::PoliticalParody;

        return CLI::executeFile(manifest.projectEntry, mode, false, false, false, "", ctx.jsonOutput);
    }
};

} // namespace thalapathy
