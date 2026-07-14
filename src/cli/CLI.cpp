#include "thalapathy/cli/CLI.hpp"
#include "thalapathy/lexer/Lexer.hpp"
#include "thalapathy/parser/Parser.hpp"
#include "thalapathy/semantic/Resolver.hpp"
#include "thalapathy/interpreter/Interpreter.hpp"
#include "thalapathy/ir/IR.hpp"
#include "thalapathy/diagnostics/Diagnostics.hpp"
#include "thalapathy/personality/Personality.hpp"
#include "thalapathy/module/ModuleGraph.hpp"
#include "thalapathy/version/Version.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <thread>
#include <chrono>
#include <map>
#include "thalapathy/net/Net.hpp"

namespace thalapathy {

static std::string readWholeFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) return "";
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

int CLI::run(const std::vector<std::string>& args) {
    if (args.empty()) {
        printHelp();
        return 0;
    }

    std::string command = "";
    std::string filepath = "";
    std::string customOutPath = "";
    bool jsonOutput = false;
    bool reloadMode = false;
    PersonalityMode mode = PersonalityMode::Professional;

    bool modeNext = false;
    bool outNext = false;

    std::vector<std::string> parsedArgs;
    for (size_t i = 0; i < args.size(); ++i) {
        if (i == 0 && args[i] == "run") continue;
        else if (i == 0 && args[i] == "check") parsedArgs.push_back("--check");
        else if (i == 0 && args[i] == "build") parsedArgs.push_back("--build");
        else if (i == 0 && args[i] == "repl") parsedArgs.push_back("--repl");
        else if (i == 0 && args[i] == "version") parsedArgs.push_back("--version");
        else if (i == 0 && args[i] == "help") parsedArgs.push_back("--help");
        else parsedArgs.push_back(args[i]);
    }

    for (size_t i = 0; i < parsedArgs.size(); ++i) {
        std::string arg = parsedArgs[i];
        if (modeNext) {
            if (arg == "professional") mode = PersonalityMode::Professional;
            else if (arg == "mass") mode = PersonalityMode::Mass;
            else if (arg == "political-parody") mode = PersonalityMode::PoliticalParody;
            else {
                std::cerr << "error: unknown mode '" << arg << "'\n";
                return 1;
            }
            modeNext = false;
        } else if (outNext) {
            customOutPath = arg;
            outNext = false;
        } else if (arg == "--mode") {
            modeNext = true;
        } else if (arg == "-o") {
            outNext = true;
        } else if (arg == "--json") {
            jsonOutput = true;
        } else if (arg == "--reload") {
            reloadMode = true;
        } else if (arg == "--check") {
            if (!command.empty()) { std::cerr << "error: conflicting primary operations\n"; return 1; }
            command = "check";
        } else if (arg == "--build") {
            if (!command.empty()) { std::cerr << "error: conflicting primary operations\n"; return 1; }
            command = "build";
        } else if (arg == "--emit-ir") {
            if (!command.empty()) { std::cerr << "error: conflicting primary operations\n"; return 1; }
            command = "emit-ir";
        } else if (arg == "--repl") {
            if (!command.empty()) { std::cerr << "error: conflicting primary operations\n"; return 1; }
            command = "repl";
        } else if (arg == "--version") {
            if (!command.empty()) { std::cerr << "error: conflicting primary operations\n"; return 1; }
            command = "version";
        } else if (arg == "--help") {
            if (!command.empty()) { std::cerr << "error: conflicting primary operations\n"; return 1; }
            command = "help";
        } else if (arg.rfind("-", 0) == 0) {
            std::cerr << "error: unknown flag '" << arg << "'\n";
            return 1;
        } else {
            if (!filepath.empty()) {
                std::cerr << "error: multiple source files specified\n";
                return 1;
            }
            filepath = arg;
        }
    }

    if (modeNext) { std::cerr << "error: missing argument to --mode\n"; return 1; }
    if (outNext) { std::cerr << "error: missing argument to -o\n"; return 1; }

    PersonalityEngine::setMode(mode);

    if (command == "help") {
        printHelp();
        return 0;
    }
    if (command == "version") {
        printVersion();
        return 0;
    }
    if (command == "repl") {
        if (!filepath.empty()) {
            std::cerr << "error: repl cannot take a file path input\n";
            return 1;
        }
        return startRepl(mode);
    }

    if (command.empty()) {
        if (filepath.empty()) {
            printHelp();
            return 0;
        }
        command = "run";
    } else {
        if (filepath.empty()) {
            std::cerr << "error: no source file specified\n";
            return 1;
        }
    }

    bool onlyCheck = (command == "check");
    bool buildOnly = (command == "build");
    bool emitIr = (command == "emit-ir");

    return executeFile(filepath, mode, onlyCheck, buildOnly, emitIr, customOutPath, jsonOutput, reloadMode);
}

void CLI::printHelp() {
    std::cout << "THALAPATHY Programming Language\n\n"
              << "Usage:\n"
              << "  thalapathy <file.tvk>\n"
              << "  thalapathy --check <file.tvk>\n"
              << "  thalapathy --build <file.tvk>\n"
              << "  thalapathy --emit-ir <file.tvk>\n"
              << "  thalapathy --repl\n"
              << "  thalapathy --version\n"
              << "  thalapathy --help\n\n"
              << "Options:\n"
              << "  --mode <professional|mass|political-parody>\n"
              << "  -o <output>\n"
              << "  --json\n";
}

void CLI::printVersion() {
    std::cout << "THALAPATHY " << getLanguageVersion() << "\n"
              << "TDK " << getTdkVersion() << "\n"
              << "VEAF " << getVeafVersion() << "\n"
              << "Source: " << getSourceExtension() << "\n"
              << "Artifact: " << getArtifactExtension() << "\n"
              << "Magic: " << getArtifactMagic() << "\n";
}

static std::string escapeJsonString(const std::string& input) {
    std::string output = "";
    for (char c : input) {
        if (c == '"') output += "\\\"";
        else if (c == '\\') output += "\\\\";
        else if (c == '\n') output += "\\n";
        else if (c == '\r') output += "\\r";
        else if (c == '\t') output += "\\t";
        else output += c;
    }
    return output;
}

static std::string constantToString(const ConstantVal& val) {
    if (std::holds_alternative<long long>(val)) {
        return std::to_string(std::get<long long>(val));
    } else if (std::holds_alternative<double>(val)) {
        return std::to_string(std::get<double>(val));
    } else {
        return std::get<std::string>(val);
    }
}

static const char* getOpcodeName(OpCode op) {
    switch (op) {
        case OpCode::LOAD_CONST: return "LOAD_CONST";
        case OpCode::LOAD_LOCAL: return "LOAD_LOCAL";
        case OpCode::STORE_LOCAL: return "STORE_LOCAL";
        case OpCode::LOAD_GLOBAL: return "LOAD_GLOBAL";
        case OpCode::STORE_GLOBAL: return "STORE_GLOBAL";
        case OpCode::ADD: return "ADD";
        case OpCode::SUB: return "SUB";
        case OpCode::MUL: return "MUL";
        case OpCode::DIV: return "DIV";
        case OpCode::MOD: return "MOD";
        case OpCode::EQ: return "EQ";
        case OpCode::NE: return "NE";
        case OpCode::LT: return "LT";
        case OpCode::LTE: return "LTE";
        case OpCode::GT: return "GT";
        case OpCode::GTE: return "GTE";
        case OpCode::NOT: return "NOT";
        case OpCode::JUMP: return "JUMP";
        case OpCode::JUMP_IF_FALSE: return "JUMP_IF_FALSE";
        case OpCode::CALL: return "CALL";
        case OpCode::RETURN: return "RETURN";
        case OpCode::POP: return "POP";
        case OpCode::BUILD_ARRAY: return "BUILD_ARRAY";
        case OpCode::INDEX_GET: return "INDEX_GET";
        case OpCode::INDEX_SET: return "INDEX_SET";
        case OpCode::GET_FIELD: return "GET_FIELD";
        case OpCode::SET_FIELD: return "SET_FIELD";
        case OpCode::THROW: return "THROW";
        case OpCode::AWAIT: return "AWAIT";
        default: return "UNKNOWN";
    }
}

int CLI::executeFile(const std::string& filepath_orig, PersonalityMode mode, bool onlyCheck, bool buildOnly, bool emitIr, const std::string& customOutPath, bool jsonOutput, bool reloadMode) {
    (void)mode;
    namespace fs = std::filesystem;
    std::string filepath = filepath_orig;
    fs::path path(filepath);

    if (path.extension() == ".thalac") {
        std::cerr << "error[THALA-ARTIFACT-001]:\nlegacy THALA artifact format is not supported by this TDK version\n\n"
                  << "help:\nrebuild the original `.tvk` source to generate a `.vijay` artifact\n";
        return 1;
    }

    // Resolve extensionless or .vijay files to .tvk if possible
    if (path.extension() == "") {
        if (fs::exists(filepath + ".tvk")) {
            filepath = filepath + ".tvk";
            path = fs::path(filepath);
        } else if (fs::exists(filepath + ".vijay")) {
            fs::path tvkPath = filepath + ".tvk";
            if (fs::exists(tvkPath)) {
                filepath = tvkPath.string();
                path = tvkPath;
            }
        }
    } else if (path.extension() == ".vijay") {
        fs::path tvkPath = path;
        tvkPath.replace_extension(".tvk");
        if (fs::exists(tvkPath)) {
            filepath = tvkPath.string();
            path = tvkPath;
        }
    }

    if (path.extension() != ".tvk") {
        std::cerr << "error: unsupported source extension (expected .tvk)\n";
        return 1;
    }

    std::string canonicalEntry = fs::weakly_canonical(filepath).string();
    std::vector<std::string> graphErrors;
    ModuleGraph graph;
    if (!graph.buildGraph(canonicalEntry, graphErrors)) {
        for (const auto& err : graphErrors) {
            std::cerr << err << "\n";
        }
        return 1;
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
        if (jsonOutput) {
            std::cout << "[\n";
            const auto& diags = resolver.getDiagnostics();
            for (size_t i = 0; i < diags.size(); ++i) {
                const auto& d = diags[i];
                std::cout << "  {\n"
                          << "    \"code\": \"" << escapeJsonString(d.code) << "\",\n"
                          << "    \"title\": \"" << escapeJsonString(d.title) << "\",\n"
                          << "    \"explanation\": \"" << escapeJsonString(d.explanation) << "\",\n"
                          << "    \"filepath\": \"" << escapeJsonString(d.span.filename) << "\",\n"
                          << "    \"line\": " << d.span.start_line << ",\n"
                          << "    \"column\": " << d.span.start_col << "\n"
                          << "  }" << (i + 1 < diags.size() ? "," : "") << "\n";
            }
            std::cout << "]\n";
        } else {
            for (const auto& diag : resolver.getDiagnostics()) {
                std::string modSource = readWholeFile(diag.span.filename);
                std::string rendered = DiagnosticRenderer::render(diag, modSource);
                std::string decorated = PersonalityEngine::decorate(diag, rendered);
                std::cerr << decorated;
            }
        }
        return 1;
    }

    if (onlyCheck) {
        if (!jsonOutput) {
            std::cout << "[PASS] Lexer\n[PASS] Parser\n[PASS] Resolver\n[PASS] Type Checker\n\n"
                      << "Nanba... syntax clean.\nCompiler green signal kuduthachu. \U0001F525\U0001F602\n";
        }
        return 0;
    }

    if (buildOnly) {
        if (!customOutPath.empty()) {
            if (fs::path(customOutPath).extension() == ".thalac") {
                std::cerr << "error: output extension cannot be .thalac\n";
                return 1;
            }
        }
        IRGenerator generator;
        ModuleIR ir;
        for (const auto& modPath : graph.getLoadOrder()) {
            const auto& nodes = graph.getModules().at(modPath)->astNodes;
            ModuleIR modIr = generator.generate(nodes);
            ir.constantPool.insert(ir.constantPool.end(), modIr.constantPool.begin(), modIr.constantPool.end());
            ir.instructions.insert(ir.instructions.end(), modIr.instructions.begin(), modIr.instructions.end());
            ir.classes.insert(ir.classes.end(), modIr.classes.begin(), modIr.classes.end());
            ir.interfaces.insert(ir.interfaces.end(), modIr.interfaces.begin(), modIr.interfaces.end());
            ir.properties.insert(ir.properties.end(), modIr.properties.begin(), modIr.properties.end());
            ir.methods.insert(ir.methods.end(), modIr.methods.begin(), modIr.methods.end());
            ir.types.insert(ir.types.end(), modIr.types.begin(), modIr.types.end());
        }

        std::sort(ir.types.begin(), ir.types.end());
        ir.types.erase(std::unique(ir.types.begin(), ir.types.end()), ir.types.end());

        std::string outPath = customOutPath.empty() ? (path.stem().string() + ".vijay") : customOutPath;
        if (ir.serialize(outPath)) {
            std::ifstream testIn(outPath, std::ios::binary);
            char testSig[4] = {0};
            testIn.read(testSig, 4);
            if (testSig[0] != 'V' || testSig[1] != 'J' || testSig[2] != 'A' || testSig[3] != 'Y') {
                std::cerr << "error: build failed signature verification\n";
                return 1;
            }
            if (!jsonOutput) {
                std::cout << PersonalityEngine::getBuildSuccessMessage() << "\n";
            }
            return 0;
        } else {
            std::cerr << "error: failed to write VEAF artifact to " << outPath << "\n";
            return 1;
        }
    }

    if (emitIr) {
        IRGenerator generator;
        ModuleIR ir;
        for (const auto& modPath : graph.getLoadOrder()) {
            const auto& nodes = graph.getModules().at(modPath)->astNodes;
            ModuleIR modIr = generator.generate(nodes);
            ir.constantPool.insert(ir.constantPool.end(), modIr.constantPool.begin(), modIr.constantPool.end());
            ir.instructions.insert(ir.instructions.end(), modIr.instructions.begin(), modIr.instructions.end());
        }

        std::stringstream ss;
        ss << "; THALA IR Disassembly\n";
        ss << "; Constants Pool:\n";
        for (size_t i = 0; i < ir.constantPool.size(); ++i) {
            ss << "const[" << i << "] = " << constantToString(ir.constantPool[i]) << "\n";
        }
        ss << "; Instructions:\n";
        for (size_t i = 0; i < ir.instructions.size(); ++i) {
            const auto& inst = ir.instructions[i];
            ss << i << ": " << getOpcodeName(inst.opcode) << " " << inst.operand << "\n";
        }

        if (!customOutPath.empty()) {
            std::ofstream out(customOutPath);
            if (out.is_open()) {
                out << ss.str();
            } else {
                std::cerr << "error: failed to write IR to " << customOutPath << "\n";
                return 1;
            }
        } else {
            std::cout << ss.str();
        }
        return 0;
    }

    if (reloadMode) {
#ifdef _WIN32
        _putenv_s("THALAIVALAI_ENV", "reload");
#else
        setenv("THALAIVALAI_ENV", "reload", 1);
#endif
    }

    std::map<std::string, std::filesystem::file_time_type> fileTimes;
    if (reloadMode) {
        for (const auto& f : graph.getLoadOrder()) {
            try { fileTimes[f] = fs::last_write_time(f); } catch(...) {}
        }
        net::g_reload_check = [graphFiles = graph.getLoadOrder(), fileTimes, canonicalEntry]() mutable -> bool {
            namespace fs = std::filesystem;
            for (const auto& f : graphFiles) {
                try {
                    auto current = fs::last_write_time(f);
                    auto it = fileTimes.find(f);
                    if (it != fileTimes.end() && current > it->second) {
                        fileTimes[f] = current;
                        std::cout << "\nRasigan mode: File change detected in " << fs::path(f).filename().string() << "! Reloading... \U0001F504\U0001F525\n";
                        return true;
                    }
                } catch (...) {}
            }
            return false;
        };
    }

    bool firstRun = true;
    do {
        if (!firstRun) {
            std::vector<std::string> graphErrors;
            graph = ModuleGraph{}; // reset
            if (!graph.buildGraph(canonicalEntry, graphErrors)) {
                for (const auto& err : graphErrors) std::cerr << err << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                continue;
            }
            // Reset resolver and re-resolve
            Resolver resolver(filepath, "");
            bool resolved = true;
            for (const auto& modPath : graph.getLoadOrder()) {
                const auto& nodes = graph.getModules().at(modPath)->astNodes;
                bool isEntry = (modPath == canonicalEntry);
                if (!resolver.resolve(nodes, isEntry)) resolved = false;
            }
            if (!resolved) {
                for (const auto& diag : resolver.getDiagnostics()) {
                    std::string modSource = readWholeFile(diag.span.filename);
                    std::cerr << PersonalityEngine::decorate(diag, DiagnosticRenderer::render(diag, modSource));
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                continue;
            }
            // Re-setup file times on successful build
            for (const auto& f : graph.getLoadOrder()) {
                try { fileTimes[f] = fs::last_write_time(f); } catch(...) {}
            }
            // Re-create the reload check callback capturing the new load order
            net::g_reload_check = [graphFiles = graph.getLoadOrder(), fileTimes, canonicalEntry]() mutable -> bool {
                namespace fs = std::filesystem;
                for (const auto& f : graphFiles) {
                    try {
                        auto current = fs::last_write_time(f);
                        auto it = fileTimes.find(f);
                        if (it != fileTimes.end() && current > it->second) {
                            fileTimes[f] = current;
                            std::cout << "\nRasigan mode: File change detected in " << fs::path(f).filename().string() << "! Reloading... \U0001F504\U0001F525\n";
                            return true;
                        }
                    } catch (...) {}
                }
                return false;
            };
        }
        firstRun = false;

        Interpreter interpreter;
        try {
            for (const auto& modPath : graph.getLoadOrder()) {
                const auto& nodes = graph.getModules().at(modPath)->astNodes;
                interpreter.interpret(nodes);
            }
            if (reloadMode) {
                while (true) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    if (net::g_reload_check && net::g_reload_check()) {
                        break;
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "error: " << e.what() << "\n";
            if (reloadMode) {
                while (true) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    if (net::g_reload_check && net::g_reload_check()) break;
                }
            } else {
                return 1;
            }
        }
    } while (reloadMode);
    return 0;
}

int CLI::startRepl(PersonalityMode mode) {
    PersonalityEngine::setMode(mode);
    std::cout << PersonalityEngine::getStartupBanner() << "\n";

    Interpreter interpreter;
    Resolver resolver("repl", "");
    std::string currentInput = "";

    while (true) {
        std::cout << ">>> ";
        std::string line;
        if (!std::getline(std::cin, line)) {
            break;
        }

        if (line.empty()) {
            if (mode != PersonalityMode::Professional) {
                std::cout << "I'm waiting...\n";
            }
            continue;
        }

        // Check REPL commands
        if (line == ".exit") {
            break;
        }
        if (line == ".help") {
            std::cout << "REPL Commands:\n"
                      << "  .help               Display this help text\n"
                      << "  .exit               Exit REPL session\n"
                      << "  .clear              Clear screen\n"
                      << "  .reset              Reset local environment variables\n"
                      << "  :set mode <mode>    Change mode (professional | mass | political-parody)\n";
            continue;
        }
        if (line == ".clear") {
#ifdef _WIN32
            (void)std::system("cls");
#else
            (void)std::system("clear");
#endif
            continue;
        }
        if (line == ".reset") {
            interpreter.getEnvironment()->clear();
            std::cout << "Environment bindings reset.\n";
            continue;
        }

        // Mode switching command
        if (line.rfind(":set mode ", 0) == 0) {
            std::string modeStr = line.substr(10);
            if (modeStr == "professional") {
                mode = PersonalityMode::Professional;
                PersonalityEngine::setMode(mode);
            } else if (modeStr == "mass") {
                mode = PersonalityMode::Mass;
                PersonalityEngine::setMode(mode);
            } else if (modeStr == "political-parody") {
                mode = PersonalityMode::PoliticalParody;
                PersonalityEngine::setMode(mode);
            } else {
                std::cout << "Unknown mode: " << modeStr << "\n";
            }
            continue;
        }

        // Run line
        // Lex line
        Lexer lexer("repl", line);
        std::vector<Token> tokens = lexer.tokenize();
        if (!lexer.getErrors().empty()) {
            for (const auto& err : lexer.getErrors()) {
                std::cerr << err << "\n";
            }
            continue;
        }

        // Parse line
        Parser parser("repl", tokens);
        std::vector<std::unique_ptr<ASTNode>> nodes;
        try {
            nodes = parser.parse();
        } catch (...) {}

        if (!parser.getErrors().empty()) {
            // Check if user is typing a command that does not match grammar in mass mode
            if (mode == PersonalityMode::Mass) {
                std::cout << "Nanba, indha command namma script-la illa.\n`.help` paaru.\n";
            } else {
                for (const auto& err : parser.getErrors()) {
                    std::cerr << err << "\n";
                }
            }
            continue;
        }

        // Evaluate or execute
        for (const auto& node : nodes) {
            // Check if it is a statement that does not return value
            if (auto exprStmt = dynamic_cast<ExprStmt*>(node.get())) {
                try {
                    Value result = interpreter.evaluate(exprStmt->expression.get());
                    if (!result.isNull()) {
                        std::cout << result.toString() << "\n";
                    }
                } catch (const ThrowSignal& s) {
                    std::cerr << "Exception thrown: " << s.errorVal.toString() << "\n";
                } catch (const std::exception& e) {
                    std::cerr << "Error: " << e.what() << "\n";
                }
            } else {
                try {
                    interpreter.execute(node.get());
                } catch (const ThrowSignal& s) {
                    std::cerr << "Exception thrown: " << s.errorVal.toString() << "\n";
                } catch (const std::exception& e) {
                    std::cerr << "Error: " << e.what() << "\n";
                }
            }
        }
    }

    return 0;
}

} // namespace thalapathy
