#pragma once
#include <string>
#include <memory>
#include <vector>
#include "thalapathy/module/Module.hpp"

namespace thalapathy {

class ModuleLoader {
public:
    static std::unique_ptr<ModuleRecord> load(const std::string& canonicalPath, const std::string& logicalName, std::vector<std::string>& errors);
};

} // namespace thalapathy
