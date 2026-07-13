#include "thalapathy/module/ModuleGraph.hpp"
#include "thalapathy/module/ModuleLoader.hpp"
#include "thalapathy/module/ModuleResolver.hpp"
#include <filesystem>
#include <algorithm>

namespace thalapathy {

ModuleGraph::ModuleGraph() {}

bool ModuleGraph::buildGraph(const std::string& entryPath, std::vector<std::string>& errors) {
    namespace fs = std::filesystem;
    if (!fs::exists(entryPath)) {
        errors.push_back("error: entry file does not exist: " + entryPath);
        return false;
    }
    std::string canonicalEntry = fs::weakly_canonical(entryPath).string();
    std::vector<std::string> importStack;
    
    return buildGraphRecursive(canonicalEntry, "main", importStack, errors);
}

bool ModuleGraph::buildGraphRecursive(const std::string& canonicalPath, const std::string& logicalName, std::vector<std::string>& importStack, std::vector<std::string>& errors) {
    // 1. Check existing state
    auto it = modules.find(canonicalPath);
    if (it != modules.end()) {
        if (it->second->state == ModuleState::LOADING) {
            // Circular dependency!
            std::string chain = "";
            for (const auto& p : importStack) {
                namespace fs = std::filesystem;
                chain += fs::path(p).stem().string() + " -> ";
            }
            namespace fs = std::filesystem;
            chain += fs::path(canonicalPath).stem().string();

            errors.push_back(
                "error[THALA-MODULE-003]:\n"
                "circular module dependency detected\n\n"
                "import chain:\n" + chain + "\n"
            );
            it->second->state = ModuleState::FAILED;
            return false;
        }
        if (it->second->state == ModuleState::LOADED) {
            return true;
        }
        if (it->second->state == ModuleState::FAILED) {
            return false;
        }
    }

    // 2. Load module record placeholder
    auto recordPtr = std::make_unique<ModuleRecord>();
    recordPtr->canonicalPath = canonicalPath;
    recordPtr->logicalName = logicalName;
    recordPtr->state = ModuleState::LOADING;
    modules[canonicalPath] = std::move(recordPtr);

    importStack.push_back(canonicalPath);

    // Parse AST
    std::vector<std::string> loadErrors;
    auto loadedRecord = ModuleLoader::load(canonicalPath, logicalName, loadErrors);
    if (!loadedRecord) {
        for (const auto& err : loadErrors) {
            errors.push_back(err);
        }
        modules[canonicalPath]->state = ModuleState::FAILED;
        return false;
    }

    // Move nodes into stored record
    auto& storedRecord = modules[canonicalPath];
    storedRecord->astNodes = std::move(loadedRecord->astNodes);
    
    // 3. Resolve and recursively build dependencies
    namespace fs = std::filesystem;
    std::string importingDir = fs::path(canonicalPath).parent_path().string();

    for (const auto& depName : loadedRecord->dependencies) {
        std::string depPath = ModuleResolver::resolve(depName, importingDir);
        if (depPath.empty()) {
            errors.push_back("error: module '" + depName + "' not found (imported by " + canonicalPath + ")");
            storedRecord->state = ModuleState::FAILED;
            return false;
        }
        
        storedRecord->dependencies.push_back(depPath);

        if (!buildGraphRecursive(depPath, depName, importStack, errors)) {
            storedRecord->state = ModuleState::FAILED;
            return false;
        }
    }

    importStack.pop_back();
    storedRecord->state = ModuleState::LOADED;
    
    // Add to load order if not already present
    if (std::find(loadOrder.begin(), loadOrder.end(), canonicalPath) == loadOrder.end()) {
        loadOrder.push_back(canonicalPath);
    }

    return true;
}

} // namespace thalapathy
