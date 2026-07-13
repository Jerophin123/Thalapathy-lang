#pragma once
#include <string>

namespace thalapathy {

class ModuleResolver {
public:
    // `entryRootDir` is the project/source root (directory of the entry file);
    // dotted package imports (a.b.c) resolve from there, Java-style.
    static std::string resolve(const std::string& logicalName,
                               const std::string& importingFileDir,
                               const std::string& entryRootDir = "");
};

} // namespace thalapathy
