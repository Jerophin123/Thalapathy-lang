#pragma once
#include <string>

namespace thalapathy {

struct ProjectManifest {
    std::string projectName = "";
    std::string projectVersion = "";
    std::string projectEntry = "";
    std::string langVersion = "";
    std::string buildOutput = "";
    std::string buildArtifact = "";
    std::string personalityMode = "";
};

class ProjectLoader {
public:
    static bool load(const std::string& tomlPath, ProjectManifest& manifest, std::string& error);
};

} // namespace thalapathy
