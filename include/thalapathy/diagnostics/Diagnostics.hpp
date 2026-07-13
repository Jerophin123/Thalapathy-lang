#pragma once
#include <string>
#include <vector>
#include "thalapathy/lexer/Token.hpp"

namespace thalapathy {

enum class DiagnosticSeverity {
    INFO,
    WARNING,
    ERROR
};

struct Diagnostic {
    std::string code;
    DiagnosticSeverity severity;
    std::string title;
    SourceSpan span;
    std::string explanation;
    std::string helpMessage;
};

class DiagnosticRenderer {
public:
    static std::string render(const Diagnostic& diag, const std::string& sourceCode);
};

} // namespace thalapathy
