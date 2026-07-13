#include "thalapathy/installation/TdkHome.hpp"
#include <cstdlib>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef ERROR
#undef min
#undef max
#undef VOID
#endif

namespace thalapathy::installation {

static std::filesystem::path getExecutablePath() {
#ifdef _WIN32
    char buf[MAX_PATH];
    GetModuleFileNameA(NULL, buf, MAX_PATH);
    return std::filesystem::path(buf);
#else
    return std::filesystem::path("/usr/local/bin/thalapathy");
#endif
}

std::filesystem::path executableDir() {
    std::error_code ec;
    auto p = std::filesystem::weakly_canonical(getExecutablePath(), ec);
    if (ec) return getExecutablePath().parent_path();
    return p.parent_path();
}

TdkHomeResult resolveTdkHome() {
    namespace fs = std::filesystem;

    // Priority 1: TDK_HOME environment variable
    const char* envHome = std::getenv("TDK_HOME");
    if (envHome && envHome[0] != '\0') {
        fs::path homePath = fs::weakly_canonical(fs::path(envHome));
        if (!fs::exists(homePath)) {
            return {false, {}, "environment", "TDK_HOME points to non-existent directory: " + homePath.string()};
        }
        if (!fs::is_directory(homePath)) {
            return {false, {}, "environment", "TDK_HOME is not a directory: " + homePath.string()};
        }
        // Validate plausibility: check for RELEASE.json or VERSION
        if (fs::exists(homePath / "RELEASE.json") || fs::exists(homePath / "VERSION")) {
            return {true, homePath, "environment", ""};
        }
        // Accept but note it may not be a valid TDK root
        return {true, homePath, "environment", ""};
    }

    // Priority 2: Derive from executable location (bin/..)
    fs::path exePath = getExecutablePath();
    fs::path binDir = exePath.parent_path();
    if (binDir.filename() == "bin") {
        fs::path tdkRoot = fs::weakly_canonical(binDir.parent_path());
        if (fs::exists(tdkRoot) && fs::is_directory(tdkRoot)) {
            return {true, tdkRoot, "executable", ""};
        }
    }

    return {false, {}, "", "TDK_HOME is not set and could not be derived from executable location"};
}

} // namespace thalapathy::installation
