#include "thalapathy/module/ModuleLoader.hpp"
#include "thalapathy/lexer/Lexer.hpp"
#include "thalapathy/parser/Parser.hpp"
#include <fstream>
#include <sstream>

namespace thalapathy {

static std::string readWholeFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) return "";
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

std::unique_ptr<ModuleRecord> ModuleLoader::load(const std::string& canonicalPath, const std::string& logicalName, std::vector<std::string>& errors) {
    std::string source = readWholeFile(canonicalPath);
    if (source.empty()) {
        errors.push_back("error: failed to read file: " + canonicalPath);
        return nullptr;
    }

    Lexer lexer(canonicalPath, source);
    auto tokens = lexer.tokenize();
    if (!lexer.getErrors().empty()) {
        for (const auto& err : lexer.getErrors()) {
            errors.push_back(err);
        }
        return nullptr;
    }

    Parser parser(canonicalPath, tokens);
    std::vector<std::unique_ptr<ASTNode>> nodes;
    try {
        nodes = parser.parse();
    } catch (...) {}

    if (!parser.getErrors().empty()) {
        for (const auto& err : parser.getErrors()) {
            errors.push_back(err);
        }
        return nullptr;
    }

    auto record = std::make_unique<ModuleRecord>();
    record->canonicalPath = canonicalPath;
    record->logicalName = logicalName;
    record->astNodes = std::move(nodes);
    record->state = ModuleState::UNSEEN;

    // Scan for dependencies
    for (const auto& node : record->astNodes) {
        if (auto imp = dynamic_cast<ImportDecl*>(node.get())) {
            record->dependencies.push_back(imp->moduleName);
        }
    }

    return record;
}

} // namespace thalapathy
