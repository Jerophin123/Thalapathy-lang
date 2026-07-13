#include "TDKCommand.hpp"
#include "TDKUtils.hpp"
#include "thalapathy/distribution/ReleaseMetadata.hpp"
#include <iostream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace thalapathy {

class DoctorCommand : public TDKCommand {
public:
    int execute(const TDKContext& ctx, const std::vector<std::string>& args) override {
        (void)args;

        auto homeResult = installation::resolveTdkHome();
        bool hasHome = homeResult.success;
        std::string home = hasHome ? homeResult.path.string() : "";

        // Executables check
        fs::path binDir = fs::path(home) / "bin";
        std::vector<std::string> exes = {"thalapathy", "tdk", "vijaydump", "thalafmt"};
        int exePassCount = 0;
        for (const auto& name : exes) {
#ifdef _WIN32
            fs::path p = binDir / (name + ".exe");
#else
            fs::path p = binDir / name;
#endif
            if (fs::exists(p)) exePassCount++;
        }

        // Load RELEASE.json via shared service
        auto releasePath = fs::path(home) / "RELEASE.json";
        auto releaseResult = distribution::loadReleaseMetadata(releasePath);

        // Standard library check
        fs::path stdDir = fs::path(home) / "lib" / "std";
        std::vector<std::string> mods = {"io.tvk", "math.tvk", "text.tvk", "collections.tvk", "time.tvk"};
        int stdPassCount = 0;
        for (const auto& mod : mods) {
            fs::path p = stdDir / mod;
            if (fs::exists(p)) stdPassCount++;
        }

        // PATH check
        const char* pathEnv = std::getenv("PATH");
        std::string pathStr = pathEnv ? pathEnv : "";
        bool inPath = (pathStr.find(home + "\\bin") != std::string::npos || pathStr.find(home + "/bin") != std::string::npos);

        // VERSION check
        bool hasVersion = fs::exists(fs::path(home) / "VERSION");

        // RELEASE.json check
        bool hasRelease = releaseResult.success;

        // Docs and legal checks
        bool hasDocs = fs::exists(fs::path(home) / "docs") && fs::is_directory(fs::path(home) / "docs");
        bool hasLegal = fs::exists(fs::path(home) / "legal") && fs::is_directory(fs::path(home) / "legal");

        // Component-conditional checks (only when RELEASE.json parsed successfully)
        bool publicHeadersOk = true;
        bool cmakeIntegrationOk = true;
        bool checkPublicHeaders = false;
        bool checkCmake = false;

        if (hasRelease) {
            const auto& comp = releaseResult.metadata.components;
            if (comp.publicHeaders) {
                checkPublicHeaders = true;
                publicHeadersOk = fs::exists(fs::path(home) / "include" / "thalapathy");
            }
            if (comp.cmakeIntegration) {
                checkCmake = true;
                cmakeIntegrationOk = fs::exists(fs::path(home) / "tools" / "cmake");
            }
        }

        int passes = 0, warnings = 0, failures = 0;

        auto report = [&](bool pass, bool isWarn, const std::string& label) {
            if (pass) { passes++; }
            else if (isWarn) { warnings++; }
            else { failures++; }
            if (!ctx.jsonOutput) {
                const char* tag = pass ? "[PASS]" : (isWarn ? "[WARN]" : "[FAIL]");
                std::cout << tag << " " << label << "\n";
            }
        };

        if (!ctx.jsonOutput) {
            std::cout << "TDK Doctor\n\n";
        }

        report(hasHome, false, "TDK_HOME directory resolved");
        report(exePassCount == 4, false, "All TDK executables present (" + std::to_string(exePassCount) + "/4 found)");
        report(stdPassCount == 5, false, "Standard library modules present (" + std::to_string(stdPassCount) + "/5 found)");
        report(inPath, true, "TDK_HOME/bin is in PATH");
        report(hasVersion, false, "VERSION file present");
        report(hasRelease, false, "RELEASE.json valid");
        report(hasDocs, false, "docs directory present");
        report(hasLegal, false, "legal directory present");

        if (checkPublicHeaders) {
            report(publicHeadersOk, false, "Public headers installed (include/thalapathy)");
        }
        if (checkCmake) {
            report(cmakeIntegrationOk, false, "CMake integration installed (tools/cmake)");
        }

        if (ctx.jsonOutput) {
            std::cout << "{\n"
                      << "  \"tdkHomeExists\": " << (hasHome ? "true" : "false") << ",\n"
                      << "  \"executablesFound\": " << exePassCount << ",\n"
                      << "  \"stdlibModulesFound\": " << stdPassCount << ",\n"
                      << "  \"binInPath\": " << (inPath ? "true" : "false") << ",\n"
                      << "  \"versionPresent\": " << (hasVersion ? "true" : "false") << ",\n"
                      << "  \"releaseJsonValid\": " << (hasRelease ? "true" : "false") << ",\n"
                      << "  \"docsPresent\": " << (hasDocs ? "true" : "false") << ",\n"
                      << "  \"legalPresent\": " << (hasLegal ? "true" : "false") << ",\n"
                      << "  \"status\": \"" << (failures == 0 ? (warnings > 0 ? "warning" : "healthy") : "failure") << "\"\n"
                      << "}\n";
        } else {
            std::cout << "\n" << passes << " passed\n"
                      << warnings << " warning\n"
                      << failures << " failures\n";
        }

        return failures > 0 ? 2 : (warnings > 0 ? 1 : 0);
    }
};

} // namespace thalapathy
