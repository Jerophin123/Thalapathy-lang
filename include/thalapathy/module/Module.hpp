#pragma once
#include <string>
#include <vector>
#include <memory>
#include "thalapathy/ast/AST.hpp"

namespace thalapathy {

enum class ModuleState {
    UNSEEN,
    LOADING,
    LOADED,
    FAILED
};

struct ModuleRecord {
    std::string canonicalPath;
    std::string logicalName;
    std::vector<std::unique_ptr<ASTNode>> astNodes;
    ModuleState state = ModuleState::UNSEEN;
    std::vector<std::string> dependencies; // canonical paths of direct imports
};

} // namespace thalapathy
