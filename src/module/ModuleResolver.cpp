#include "thalapathy/module/ModuleResolver.hpp"
#include "thalapathy/installation/TdkHome.hpp"
#include <filesystem>

namespace thalapathy {

std::string ModuleResolver::resolve(const std::string& logicalName, const std::string& importingFileDir) {
    namespace fs = std::filesystem;
    std::string filename = logicalName + ".tvk";

    // 1. Check entry source directory
    if (!importingFileDir.empty()) {
        fs::path localPath = fs::path(importingFileDir) / filename;
        if (fs::exists(localPath)) {
            return fs::weakly_canonical(localPath).string();
        }
    }

    // 2. Check current working directory
    fs::path cwdPath = fs::current_path() / filename;
    if (fs::exists(cwdPath)) {
        return fs::weakly_canonical(cwdPath).string();
    }

    // 3. Check TDK_HOME/lib/std via shared TdkHome discovery
    auto homeResult = installation::resolveTdkHome();
    if (homeResult.success) {
        fs::path stdPath = homeResult.path / "lib" / "std" / filename;
        if (fs::exists(stdPath)) {
            return fs::weakly_canonical(stdPath).string();
        }
    }

    // Fallback: ./lib/std (for development within source repository)
    fs::path stdFallback = fs::path(".") / "lib" / "std" / filename;
    if (fs::exists(stdFallback)) {
        return fs::weakly_canonical(stdFallback).string();
    }

    // Return empty if not found
    return "";
}

} // namespace thalapathy
