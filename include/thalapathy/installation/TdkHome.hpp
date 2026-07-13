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

} // namespace thalapathy::installation
