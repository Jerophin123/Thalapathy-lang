#include "thalapathy/project/ProjectManifest.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace thalapathy {

static std::string trim(const std::string& str) {
    auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) {
        return std::isspace(ch);
    });
    auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {
        return std::isspace(ch);
    }).base();
    return (start < end) ? std::string(start, end) : "";
}

static std::string stripQuotes(const std::string& str) {
    if (str.length() >= 2 && str.front() == '"' && str.back() == '"') {
        return str.substr(1, str.length() - 2);
    }
    return str;
}

bool ProjectLoader::load(const std::string& tomlPath, ProjectManifest& manifest, std::string& error) {
    std::ifstream file(tomlPath);
    if (!file.is_open()) {
        error = "failed to open manifest file: " + tomlPath;
        return false;
    }

    std::string currentSection = "";
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line.front() == '#') {
            continue;
        }

        if (line.front() == '[' && line.back() == ']') {
            currentSection = trim(line.substr(1, line.length() - 2));
            continue;
        }

        size_t eqPos = line.find('=');
        if (eqPos == std::string::npos) {
            error = "invalid syntax in manifest line: " + line;
            return false;
        }

        std::string key = trim(line.substr(0, eqPos));
        std::string val = stripQuotes(trim(line.substr(eqPos + 1)));

        if (currentSection == "project") {
            if (key == "name") manifest.projectName = val;
            else if (key == "version") manifest.projectVersion = val;
            else if (key == "entry") manifest.projectEntry = val;
        } else if (currentSection == "language") {
            if (key == "version") manifest.langVersion = val;
        } else if (currentSection == "build") {
            if (key == "output") manifest.buildOutput = val;
            else if (key == "artifact") manifest.buildArtifact = val;
        } else if (currentSection == "personality") {
            if (key == "mode") manifest.personalityMode = val;
        }
    }

    // Validations
    if (manifest.projectName.empty()) {
        error = "missing required field 'name' in [project] section";
        return false;
    }
    if (manifest.projectVersion.empty()) {
        error = "missing required field 'version' in [project] section";
        return false;
    }
    if (manifest.projectEntry.empty()) {
        error = "missing required field 'entry' in [project] section";
        return false;
    }

    // Path traversal verification
    namespace fs = std::filesystem;
    fs::path entryPath(manifest.projectEntry);
    if (entryPath.is_absolute() || manifest.projectEntry.find("..") != std::string::npos) {
        error = "path traversal or absolute path is not allowed for entry: " + manifest.projectEntry;
        return false;
    }

    if (entryPath.extension() != ".tvk") {
        error = "project entry must have .tvk extension: " + manifest.projectEntry;
        return false;
    }

    if (!manifest.buildArtifact.empty()) {
        fs::path artPath(manifest.buildArtifact);
        if (artPath.is_absolute() || manifest.buildArtifact.find("..") != std::string::npos) {
            error = "path traversal or absolute path is not allowed for build artifact: " + manifest.buildArtifact;
            return false;
        }
        if (artPath.extension() != ".vijay") {
            error = "build artifact must have .vijay extension: " + manifest.buildArtifact;
            return false;
        }
    }

    if (!manifest.personalityMode.empty()) {
        if (manifest.personalityMode != "professional" &&
            manifest.personalityMode != "mass" &&
            manifest.personalityMode != "political-parody") {
            error = "unknown personality mode: " + manifest.personalityMode;
            return false;
        }
    }

    return true;
}

} // namespace thalapathy
