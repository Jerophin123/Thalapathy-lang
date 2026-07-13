#include <catch2/catch_test_macros.hpp>
#include "thalapathy/diagnostics/Diagnostics.hpp"

using namespace thalapathy;

TEST_CASE("DiagnosticRenderer formats error message", "[diagnostics]") {
    Diagnostic diag;
    diag.code = "THALA-TEST-001";
    diag.severity = DiagnosticSeverity::ERROR;
    diag.title = "test warning";
    diag.span = SourceSpan{"test.tvk", 2, 5, 2, 8, 4, 3}; // starts at col 5, len 3
    diag.explanation = "expected type matches";
    diag.helpMessage = "fix the source expression";

    std::string source = "line 1\nline test line\nline 3";
    std::string output = DiagnosticRenderer::render(diag, source);

    REQUIRE(output.find("error[THALA-TEST-001]: test warning") != std::string::npos);
    REQUIRE(output.find("2 | line test line") != std::string::npos);
    REQUIRE(output.find("^^^ expected type matches") != std::string::npos);
}
