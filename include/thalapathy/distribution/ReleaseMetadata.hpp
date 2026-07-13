#pragma once
#include <string>
#include <cstdint>
#include <filesystem>

namespace thalapathy::distribution {

struct DistributionComponents {
    bool standardLibrary = false;
    bool vscodeTools = false;
    bool publicHeaders = false;
    bool cmakeIntegration = false;
    bool runtimeResources = false;
    bool veafResources = false;
};

struct ReleaseMetadata {
    std::string tdkVersion;
    std::string languageVersion;
    uint32_t veafVersion = 0;
    std::string artifactMagic;
    std::string sourceExtension;
    std::string artifactExtension;
    DistributionComponents components;
};

struct ReleaseMetadataResult {
    bool success = false;
    ReleaseMetadata metadata;
    std::string error;
};

ReleaseMetadataResult loadReleaseMetadata(const std::filesystem::path& path);

} // namespace thalapathy::distribution
