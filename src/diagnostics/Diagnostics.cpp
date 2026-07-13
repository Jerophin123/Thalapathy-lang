#include "thalapathy/diagnostics/Diagnostics.hpp"
#include <sstream>
#include <iostream>

namespace thalapathy {

std::string DiagnosticRenderer::render(const Diagnostic& diag, const std::string& sourceCode) {
    std::stringstream ss;
    
    // Header
    std::string sevStr = "error";
    if (diag.severity == DiagnosticSeverity::WARNING) {
        sevStr = "warning";
    } else if (diag.severity == DiagnosticSeverity::INFO) {
        sevStr = "info";
    }

    ss << sevStr << "[" << diag.code << "]: " << diag.title << "\n";
    ss << "  --> " << diag.span.filename << ":" << diag.span.start_line << ":" << diag.span.start_col << "\n";
    ss << "   |\n";

    // Split source code into lines to find the relevant line
    std::vector<std::string> lines;
    std::stringstream srcStream(sourceCode);
    std::string lineStr;
    while (std::getline(srcStream, lineStr)) {
        lines.push_back(lineStr);
    }

    int targetLine = diag.span.start_line;
    if (targetLine >= 1 && targetLine <= static_cast<int>(lines.size())) {
        std::string activeLine = lines[static_cast<size_t>(targetLine - 1)];
        ss << " " << targetLine << " | " << activeLine << "\n";

        // Caret formatting
        ss << "   | ";
        int caretStart = diag.span.start_col - 1;
        for (int i = 0; i < caretStart; ++i) {
            ss << " ";
        }
        size_t caretLen = diag.span.length;
        if (caretLen == 0) caretLen = 1;
        for (size_t i = 0; i < caretLen; ++i) {
            ss << "^";
        }
        ss << " " << diag.explanation << "\n";
    } else {
        // Fallback if line is missing/out of bounds
        ss << "   | <source unavailable>\n";
    }

    ss << "   |\n";
    if (!diag.helpMessage.empty()) {
        ss << "   = help: " << diag.helpMessage << "\n";
    }

    return ss.str();
}

} // namespace thalapathy
