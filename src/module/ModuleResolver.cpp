#include "thalapathy/module/ModuleResolver.hpp"
#include "thalapathy/installation/TdkHome.hpp"
#include <filesystem>
#include <vector>
#include <algorithm>

namespace thalapathy {

std::string ModuleResolver::resolve(const std::string& logicalName,
                                    const std::string& importingFileDir,
                                    const std::string& entryRootDir) {
    namespace fs = std::filesystem;

    // Java-style dotted package paths map onto nested directories:
    //   sarkar app.models.user;  ->  app/models/user.tvk
    // A plain name (no dots) keeps the flat `<name>.tvk` behaviour.
    bool isPackagePath = logicalName.find('.') != std::string::npos;
    std::string relative = logicalName;
    std::replace(relative.begin(), relative.end(), '.', '/');
    std::string filename = relative + ".tvk";

    // 0. Package (dotted) imports resolve from the project/source root first,
    //    so any file in any sub-package can import `app.models.user` uniformly.
    if (isPackagePath && !entryRootDir.empty()) {
        fs::path rootPath = fs::path(entryRootDir) / filename;
        if (fs::exists(rootPath)) {
            return fs::weakly_canonical(rootPath).string();
        }
    }

    // 1. Check importing file's directory
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

    // 4. Search relative to the running executable. This covers both an installed
    //    TDK layout (bin/../lib/std) and a development build tree where the binary
    //    sits alongside or a few levels below the repository root.
    fs::path exeDir = installation::executableDir();
    if (!exeDir.empty()) {
        std::vector<fs::path> candidates = {
            exeDir / "lib" / "std" / filename,
            exeDir.parent_path() / "lib" / "std" / filename,
        };
        // Walk up a handful of parents looking for lib/std/<name>.tvk.
        fs::path walk = exeDir;
        for (int depth = 0; depth < 6 && !walk.empty(); ++depth) {
            candidates.push_back(walk / "lib" / "std" / filename);
            walk = walk.parent_path();
        }
        for (const auto& c : candidates) {
            if (fs::exists(c)) {
                return fs::weakly_canonical(c).string();
            }
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
