#include <catch2/catch_test_macros.hpp>
#include "thalapathy/lexer/Lexer.hpp"
#include "thalapathy/parser/Parser.hpp"
#include "thalapathy/interpreter/Interpreter.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

using namespace thalapathy;

static Value runAndGet(const std::string& source, const std::string& varName) {
    namespace fs = std::filesystem;

    Lexer lexer("test.tvk", source);
    auto tokens = lexer.tokenize();
    Parser parser("test.tvk", tokens);
    auto nodes = parser.parse();
    if (!parser.getErrors().empty()) {
        for (const auto& err : parser.getErrors()) {
            std::cerr << "Parser error: " << err << "\n";
        }
    }
    REQUIRE(parser.getErrors().empty());

    Interpreter interpreter;

    std::vector<std::unique_ptr<ASTNode>> stdlibNodes;

    // Robustly locate and load thalaivalai.tvk
    fs::path stdlibPath;
    const char* tdkHome = std::getenv("TDK_HOME");
    if (tdkHome) {
        stdlibPath = fs::path(tdkHome) / "lib" / "std" / "thalaivalai.tvk";
    } else {
        stdlibPath = fs::path("..") / "lib" / "std" / "thalaivalai.tvk";
        if (!fs::exists(stdlibPath)) {
            stdlibPath = fs::path("lib") / "std" / "thalaivalai.tvk";
        }
    }

    if (fs::exists(stdlibPath)) {
        std::ifstream in(stdlibPath);
        if (in.is_open()) {
            std::stringstream ss;
            ss << in.rdbuf();
            std::string stdlibSource = ss.str();
            Lexer stdlibLexer(stdlibPath.string(), stdlibSource);
            auto stdlibTokens = stdlibLexer.tokenize();
            Parser stdlibParser(stdlibPath.string(), stdlibTokens);
            stdlibNodes = stdlibParser.parse();
            if (stdlibParser.getErrors().empty()) {
                interpreter.interpret(stdlibNodes);
            } else {
                std::cerr << "Stdlib parser errors:\n";
                for (const auto& err : stdlibParser.getErrors()) std::cerr << err << "\n";
            }
        }
    }

    interpreter.interpret(nodes);

    bool found = false;
    Value v = interpreter.getEnvironment()->get(varName, found);
    REQUIRE(found);
    return v;
}

TEST_CASE("async varum function compiles, resolver resolves, and interpreter throws staged error on execution", "[async]") {
    std::string source =
        "varum mersal punch() -> any {\n"
        "    makkal x = kaathiru 100;\n"
        "    thiruppi x;\n"
        "}\n"
        "makkal ok = illa;\n"
        "thalapathy aarambam {\n"
        "    punch();\n"
        "    ok = aama;\n"
        "}\n";
    
    Value v = runAndGet(source, "ok");
    CHECK(std::get<bool>(v.val) == false);
}

TEST_CASE("async varum lambda compiles and resolves", "[async][lambda]") {
    std::string source =
        "makkal ok = illa;\n"
        "thalapathy aarambam {\n"
        "    nanba f = varum kutty (n) -> any {\n"
        "        thiruppi kaathiru n;\n"
        "    };\n"
        "    ok = aama;\n"
        "}\n";
    Value v = runAndGet(source, "ok");
    CHECK(std::get<bool>(v.val) == true);
}

TEST_CASE("thalaivalai template engine ghilli and vaathi control flow", "[web][template]") {
    std::string source =
        "sarkar thalaivalai;\n"
        "makkal result = \"\";\n"
        "thalapathy aarambam {\n"
        "    makkal html = \"{% ghilli (show) %}Hello {{ name }}{% illana %}Goodbye{% %} - {% vaathi x ulla items %}{{ x }} {% %}\";\n"
        "    makkal nodes = _tv_compileTemplate(html);\n"
        "    makkal data = { show: aama, name: \"Vijay\", items: [\"Ghilli\", \"Leo\"] };\n"
        "    result = _tv_renderNodes(nodes, data);\n"
        "}\n";
    Value v = runAndGet(source, "result");
    CHECK(std::get<std::string>(v.val) == "Hello Vijay - Ghilli Leo ");
}

TEST_CASE("thalaivalai template engine illana branch", "[web][template]") {
    std::string source =
        "sarkar thalaivalai;\n"
        "makkal result = \"\";\n"
        "thalapathy aarambam {\n"
        "    makkal html = \"{% ghilli (show) %}Hello{% illana %}Goodbye{% %}\";\n"
        "    makkal nodes = _tv_compileTemplate(html);\n"
        "    makkal data = { show: illa };\n"
        "    result = _tv_renderNodes(nodes, data);\n"
        "}\n";
    Value v = runAndGet(source, "result");
    CHECK(std::get<std::string>(v.val) == "Goodbye");
}

TEST_CASE("thalaivalai cookie parsing", "[web][cookie]") {
    std::string source =
        "sarkar thalaivalai;\n"
        "makkal result = onnumilla;\n"
        "thalapathy aarambam {\n"
        "    makkal cookies = _tv_parseCookies(\"user=vijay; role=leader\");\n"
        "    result = cookies.get(\"user\") + \"-\" + cookies.get(\"role\");\n"
        "}\n";
    Value v = runAndGet(source, "result");
    CHECK(std::get<std::string>(v.val) == "vijay-leader");
}
