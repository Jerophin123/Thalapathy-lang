#pragma once
#include "thalapathy/installation/TdkHome.hpp"
#include <string>
#include <filesystem>

namespace thalapathy {

inline std::string getTdkHome() {
    auto result = installation::resolveTdkHome();
    if (result.success) {
        return result.path.string();
    }
    // Fallback to current working directory
    return std::filesystem::weakly_canonical(".").string();
}

} // namespace thalapathy
