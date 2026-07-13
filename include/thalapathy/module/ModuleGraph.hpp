#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "thalapathy/module/Module.hpp"

namespace thalapathy {

class ModuleGraph {
public:
    ModuleGraph();

    bool buildGraph(const std::string& entryPath, std::vector<std::string>& errors);
    
    const std::unordered_map<std::string, std::unique_ptr<ModuleRecord>>& getModules() const { return modules; }
    const std::vector<std::string>& getLoadOrder() const { return loadOrder; }

private:
    bool buildGraphRecursive(const std::string& canonicalPath, const std::string& logicalName, std::vector<std::string>& importStack, std::vector<std::string>& errors);

    std::unordered_map<std::string, std::unique_ptr<ModuleRecord>> modules;
    std::vector<std::string> loadOrder;
};

} // namespace thalapathy
