#pragma once
#include <filesystem>
#include <string>

namespace thalapathy::installation {

struct TdkHomeResult {
    bool success;
    std::filesystem::path path;
    std::string source;
    std::string error;
};

TdkHomeResult resolveTdkHome();

// Absolute directory containing the running executable (empty on failure).
std::filesystem::path executableDir();

} // namespace thalapathy::installation
