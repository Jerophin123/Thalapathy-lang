#pragma once
#include <string>
#include <vector>
#include "thalapathy/personality/Personality.hpp"

namespace thalapathy {

class CLI {
public:
    static int run(const std::vector<std::string>& args);
    static int executeFile(const std::string& filepath, PersonalityMode mode, bool onlyCheck, bool buildOnly, bool emitIr, const std::string& customOutPath, bool jsonOutput);

private:
    static void printHelp();
    static void printVersion();
    
    static int startRepl(PersonalityMode mode);
};

} // namespace thalapathy
