#pragma once
#include <string>

namespace thalapathy {

class ModuleResolver {
public:
    static std::string resolve(const std::string& logicalName, const std::string& importingFileDir);
};

} // namespace thalapathy
