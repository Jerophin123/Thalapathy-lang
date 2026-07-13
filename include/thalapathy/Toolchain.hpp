#pragma once
// THALAPATHY Toolchain Public Embedding API
// Status: EXPERIMENTAL - API may change before THALAPATHY 1.0
// ABI stability is not promised for TDK 0.3.0.
//
// The toolchain is built as a static library (libthalapathy_toolchain.a).
// No shared library / DLL is provided in TDK 0.3.0.

#include <string>
#include <vector>
#include <cstddef>

namespace thalapathy {

enum class PublicSeverity {
    Info,
    Warning,
    Error
};

struct SourceLocation {
    std::string file;
    std::size_t line = 0;
    std::size_t column = 0;
};

struct PublicDiagnostic {
    std::string code;
    PublicSeverity severity = PublicSeverity::Error;
    std::string message;
    SourceLocation location;
};

struct CheckResult {
    bool success = false;
    std::vector<PublicDiagnostic> diagnostics;
};

struct BuildResult {
    bool success = false;
    std::string artifactPath;
    std::vector<PublicDiagnostic> diagnostics;
};

struct RunResult {
    bool success = false;
    int exitCode = 0;
    std::vector<PublicDiagnostic> diagnostics;
};

class Toolchain {
public:
    static CheckResult check(const std::string& filepath);
    static BuildResult build(const std::string& filepath, const std::string& outputPath = "");
    static RunResult run(const std::string& filepath);
};

} // namespace thalapathy
