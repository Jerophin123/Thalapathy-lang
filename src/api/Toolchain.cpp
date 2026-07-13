#include "thalapathy/Toolchain.hpp"
#include "thalapathy/lexer/Lexer.hpp"
#include "thalapathy/parser/Parser.hpp"
#include "thalapathy/semantic/Resolver.hpp"
#include "thalapathy/interpreter/Interpreter.hpp"
#include "thalapathy/ir/IR.hpp"
#include "thalapathy/diagnostics/Diagnostics.hpp"
#include "thalapathy/module/ModuleGraph.hpp"
#include <filesystem>

namespace thalapathy {

static std::vector<PublicDiagnostic> convertDiagnostics(const std::vector<Diagnostic>& diags) {
    std::vector<PublicDiagnostic> result;
    for (const auto& d : diags) {
        PublicDiagnostic pd;
        pd.code = d.code;
        pd.message = d.title;
        pd.location.file = d.span.filename;
        pd.location.line = static_cast<std::size_t>(d.span.start_line);
        pd.location.column = static_cast<std::size_t>(d.span.start_col);
        switch (d.severity) {
            case thalapathy::DiagnosticSeverity::INFO:
                pd.severity = PublicSeverity::Info;
                break;
            case thalapathy::DiagnosticSeverity::WARNING:
                pd.severity = PublicSeverity::Warning;
                break;
            case thalapathy::DiagnosticSeverity::ERROR:
                pd.severity = PublicSeverity::Error;
                break;
        }
        result.push_back(std::move(pd));
    }
    return result;
}

CheckResult Toolchain::check(const std::string& filepath) {
    namespace fs = std::filesystem;
    CheckResult result;

    if (!fs::exists(filepath)) {
        PublicDiagnostic d;
        d.code = "THALA-FILE-001";
        d.severity = PublicSeverity::Error;
        d.message = "source file not found: " + filepath;
        result.diagnostics.push_back(std::move(d));
        return result;
    }

    std::string canonicalEntry = fs::weakly_canonical(filepath).string();
    std::vector<std::string> graphErrors;
    ModuleGraph graph;
    if (!graph.buildGraph(canonicalEntry, graphErrors)) {
        for (const auto& err : graphErrors) {
            PublicDiagnostic d;
            d.code = "THALA-MODULE-001";
            d.severity = PublicSeverity::Error;
            d.message = err;
            result.diagnostics.push_back(std::move(d));
        }
        return result;
    }

    Resolver resolver(filepath, "");
    bool resolved = true;
    for (const auto& modPath : graph.getLoadOrder()) {
        const auto& nodes = graph.getModules().at(modPath)->astNodes;
        bool isEntry = (modPath == canonicalEntry);
        if (!resolver.resolve(nodes, isEntry)) {
            resolved = false;
        }
    }

    if (!resolved) {
        result.diagnostics = convertDiagnostics(resolver.getDiagnostics());
        return result;
    }

    result.success = true;
    return result;
}

BuildResult Toolchain::build(const std::string& filepath, const std::string& outputPath) {
    namespace fs = std::filesystem;
    BuildResult result;

    auto checkResult = check(filepath);
    if (!checkResult.success) {
        result.diagnostics = std::move(checkResult.diagnostics);
        return result;
    }

    std::string canonicalEntry = fs::weakly_canonical(filepath).string();
    std::vector<std::string> graphErrors;
    ModuleGraph graph;
    graph.buildGraph(canonicalEntry, graphErrors);

    // Collect all AST nodes for IR generation
    std::vector<std::unique_ptr<ASTNode>> allNodesCopy;
    // IRGenerator expects unique_ptr references — use the graph directly
    IRGenerator gen;
    // We need a combined vector of unique_ptrs for the generator
    // Gather references from all modules
    std::vector<std::unique_ptr<ASTNode>> combinedNodes;
    for (const auto& modPath : graph.getLoadOrder()) {
        auto& modNodes = graph.getModules().at(modPath)->astNodes;
        for (auto& node : modNodes) {
            // We can't move out of the graph, so we pass each module separately
            (void)node;
        }
    }

    // Generate IR from the entry module (simplified for v0.3.0)
    auto& entryNodes = graph.getModules().at(canonicalEntry)->astNodes;
    ModuleIR ir = gen.generate(entryNodes);

    std::string outPath = outputPath;
    if (outPath.empty()) {
        outPath = fs::path(filepath).replace_extension(".vijay").string();
    }

    ir.serialize(outPath);
    result.success = true;
    result.artifactPath = outPath;
    return result;
}

RunResult Toolchain::run(const std::string& filepath) {
    namespace fs = std::filesystem;
    RunResult result;

    auto checkResult = check(filepath);
    if (!checkResult.success) {
        result.diagnostics = std::move(checkResult.diagnostics);
        return result;
    }

    std::string canonicalEntry = fs::weakly_canonical(filepath).string();
    std::vector<std::string> graphErrors;
    ModuleGraph graph;
    graph.buildGraph(canonicalEntry, graphErrors);

    Interpreter interpreter;
    for (const auto& modPath : graph.getLoadOrder()) {
        const auto& nodes = graph.getModules().at(modPath)->astNodes;
        for (const auto& node : nodes) {
            interpreter.execute(node.get());
        }
    }

    result.success = true;
    result.exitCode = 0;
    return result;
}

} // namespace thalapathy
