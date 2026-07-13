#include <catch2/catch_test_macros.hpp>
#include "thalapathy/lexer/Lexer.hpp"
#include "thalapathy/parser/Parser.hpp"
#include "thalapathy/interpreter/Interpreter.hpp"
#include <cstdio>
#include <fstream>

using namespace thalapathy;

static Value runAndGet(const std::string& source, const std::string& varName) {
    Lexer lexer("test.tvk", source);
    auto tokens = lexer.tokenize();
    Parser parser("test.tvk", tokens);
    auto nodes = parser.parse();
    REQUIRE(parser.getErrors().empty());

    Interpreter interpreter;
    interpreter.interpret(nodes);

    bool found = false;
    Value v = interpreter.getEnvironment()->get(varName, found);
    REQUIRE(found);
    return v;
}

TEST_CASE("thuppakki while loop accumulates", "[features][while]") {
    std::string src =
        "makkal total = 0;\n"
        "thalapathy aarambam {\n"
        "    makkal i = 1;\n"
        "    thuppakki (i <= 5) { total = total + i; i = i + 1; }\n"
        "}\n";
    Value v = runAndGet(src, "total");
    CHECK(std::get<long long>(v.val) == 15LL);
}

TEST_CASE("thalaivaa switch selects the matching vazhakku", "[features][switch]") {
    std::string src =
        "makkal label = \"none\";\n"
        "thalapathy aarambam {\n"
        "    nanba day = 2;\n"
        "    thalaivaa (day) {\n"
        "        vazhakku 1: { label = \"one\"; }\n"
        "        vazhakku 2: { label = \"two\"; }\n"
        "        illana:      { label = \"other\"; }\n"
        "    }\n"
        "}\n";
    Value v = runAndGet(src, "label");
    CHECK(std::get<std::string>(v.val) == "two");
}

TEST_CASE("thalaivaa falls through to illana default", "[features][switch]") {
    std::string src =
        "makkal label = \"none\";\n"
        "thalapathy aarambam {\n"
        "    nanba n = 99;\n"
        "    thalaivaa (n) {\n"
        "        vazhakku 1: { label = \"one\"; }\n"
        "        illana:      { label = \"default\"; }\n"
        "    }\n"
        "}\n";
    Value v = runAndGet(src, "label");
    CHECK(std::get<std::string>(v.val) == "default");
}

TEST_CASE("kutty lambda is callable and closes over scope", "[features][lambda]") {
    std::string src =
        "makkal result = 0;\n"
        "thalapathy aarambam {\n"
        "    nanba base = 10;\n"
        "    nanba addBase = kutty (int x) -> int { thiruppi x + base; };\n"
        "    result = addBase(5);\n"
        "}\n";
    Value v = runAndGet(src, "result");
    CHECK(std::get<long long>(v.val) == 15LL);
}

TEST_CASE("array map/filter/reduce higher-order methods", "[features][collections]") {
    std::string src =
        "makkal sum = 0;\n"
        "thalapathy aarambam {\n"
        "    nanba nums = [1, 2, 3, 4, 5];\n"
        "    nanba evens = nums.filter(kutty (n) { thiruppi n % 2 == 0; });\n"
        "    nanba doubled = evens.map(kutty (n) { thiruppi n * 2; });\n"
        "    sum = doubled.reduce(kutty (a, b) { thiruppi a + b; }, 0);\n"
        "}\n";
    Value v = runAndGet(src, "sum");
    // evens = [2,4] -> doubled = [4,8] -> sum = 12
    CHECK(std::get<long long>(v.val) == 12LL);
}

TEST_CASE("string methods work", "[features][string]") {
    std::string src =
        "makkal out = \"\";\n"
        "thalapathy aarambam {\n"
        "    nanba s = \"Thalapathy Vijay\";\n"
        "    out = s.toUpper();\n"
        "}\n";
    Value v = runAndGet(src, "out");
    CHECK(std::get<std::string>(v.val) == "THALAPATHY VIJAY");
}

TEST_CASE("json stringify then parse round-trips", "[features][json]") {
    std::string src =
        "makkal name = \"\";\n"
        "thalapathy aarambam {\n"
        "    nanba obj = { leader: \"Vijay\", year: 2026 };\n"
        "    nanba text = __native_json_stringify(obj);\n"
        "    nanba back = __native_json_parse(text);\n"
        "    name = back[\"leader\"];\n"
        "}\n";
    Value v = runAndGet(src, "name");
    CHECK(std::get<std::string>(v.val) == "Vijay");
}

TEST_CASE("keyword-named map keys and members are allowed", "[features][parser]") {
    std::string src =
        "makkal out = \"\";\n"
        "thalapathy aarambam {\n"
        "    nanba m = { master: \"Vijay\", goat: true, leo: 2023 };\n"
        "    out = m[\"master\"] + \"/\" + string(m[\"leo\"]);\n"
        "}\n";
    Value v = runAndGet(src, "out");
    CHECK(std::get<std::string>(v.val) == "Vijay/2023");
}

TEST_CASE("foreach iterates arrays and strings", "[features][foreach]") {
    std::string src =
        "makkal out = \"\";\n"
        "thalapathy aarambam {\n"
        "    vaathi p in [\"a\", \"b\", \"c\"] { out = out + p; }\n"
        "    vaathi ch in \"XY\" { out = out + ch; }\n"
        "}\n";
    Value v = runAndGet(src, "out");
    CHECK(std::get<std::string>(v.val) == "abcXY");
}

TEST_CASE("ternary operator selects branch", "[features][ternary]") {
    std::string src =
        "makkal out = \"\";\n"
        "thalapathy aarambam {\n"
        "    nanba n = 7;\n"
        "    out = n % 2 == 0 ? \"even\" : \"odd\";\n"
        "}\n";
    Value v = runAndGet(src, "out");
    CHECK(std::get<std::string>(v.val) == "odd");
}

TEST_CASE("default parameters apply when omitted", "[features][defaults]") {
    std::string src =
        "makkal out = \"\";\n"
        "mersal greet(string who, string suffix = \"nanba\") -> string {\n"
        "    thiruppi who + \" \" + suffix;\n"
        "}\n"
        "thalapathy aarambam {\n"
        "    out = greet(\"Vijay\") + \"|\" + greet(\"Vijay\", \"thalaiva\");\n"
        "}\n";
    Value v = runAndGet(src, "out");
    CHECK(std::get<std::string>(v.val) == "Vijay nanba|Vijay thalaiva");
}

TEST_CASE("enum members, ordinal, switch and equality", "[features][enum]") {
    std::string src =
        "vagai Nilai { A, B, C }\n"
        "makkal out = \"\";\n"
        "thalapathy aarambam {\n"
        "    nanba s = Nilai.B;\n"
        "    out = s.name() + string(s.ordinal());\n"
        "    ghilli (s == Nilai.B) { out = out + \"=B\"; }\n"
        "    thalaivaa (s) {\n"
        "        vazhakku Nilai.A: { out = out + \"|A\"; }\n"
        "        vazhakku Nilai.B: { out = out + \"|B\"; }\n"
        "        illana: { out = out + \"|?\"; }\n"
        "    }\n"
        "}\n";
    Value v = runAndGet(src, "out");
    CHECK(std::get<std::string>(v.val) == "B1=B|B");
}

TEST_CASE("finally runs and typed catch filters", "[features][exceptions]") {
    std::string src =
        "makkal out = \"\";\n"
        "thalapathy aarambam {\n"
        "    pokkiri {\n"
        "        theri \"boom\";\n"
        "    } kaavalan (e) {\n"
        "        out = out + \"caught:\" + e;\n"
        "    } kadaisi {\n"
        "        out = out + \"|finally\";\n"
        "    }\n"
        "}\n";
    Value v = runAndGet(src, "out");
    CHECK(std::get<std::string>(v.val) == "caught:boom|finally");
}

TEST_CASE("VijayDB native CRUD round-trip", "[features][db]") {
    std::remove("__unit_test_vijaydb.json");
    std::string src =
        "makkal report = \"\";\n"
        "thalapathy aarambam {\n"
        "    nanba h = __native_db_open(\"__unit_test_vijaydb.json\");\n"
        "    nanba a = __native_db_insert(h, \"t\", { name: \"Vijay\" });\n"
        "    nanba b = __native_db_insert(h, \"t\", { name: \"Ajith\" });\n"
        "    nanba c = __native_db_count(h, \"t\");\n"
        "    nanba found = __native_db_find(h, \"t\", \"name\", \"Vijay\");\n"
        "    __native_db_update(h, \"t\", a[\"_id\"], { name: \"Thalapathy\" });\n"
        "    nanba got = __native_db_get(h, \"t\", a[\"_id\"]);\n"
        "    __native_db_remove(h, \"t\", b[\"_id\"]);\n"
        "    report = string(c) + \"|\" + found[0][\"name\"] + \"|\" + got[\"name\"] + \"|\" + string(__native_db_count(h, \"t\"));\n"
        "}\n";
    Value v = runAndGet(src, "report");
    CHECK(std::get<std::string>(v.val) == "2|Vijay|Thalapathy|1");
    std::remove("__unit_test_vijaydb.json");
}

TEST_CASE("Scanner class reads and parses input streams", "[features][io][scanner]") {
    std::string src =
        "sarkar io;\n"
        "master MockSource {\n"
        "    any lines;\n"
        "    int index;\n"
        "    mersal init(any lines) {\n"
        "        this.lines = lines;\n"
        "        this.index = 0;\n"
        "    }\n"
        "    mersal vaasi() -> any {\n"
        "        ghilli (this.index >= len(this.lines)) {\n"
        "            thiruppi null;\n"
        "        }\n"
        "        makkal l = this.lines[this.index];\n"
        "        this.index = this.index + 1;\n"
        "        thiruppi l;\n"
        "    }\n"
        "}\n"
        "makkal test_word = \"\";\n"
        "makkal test_int = 0;\n"
        "makkal test_float = 0.0;\n"
        "makkal test_line = \"\";\n"
        "makkal has_more = true;\n"
        "thalapathy aarambam {\n"
        "    nanba mock = MockSource([\"42 3.14 hello\", \"  world   \", \"last line\"]);\n"
        "    nanba scanner = Scanner(mock);\n"
        "    test_int = scanner.nextInt();\n"
        "    test_float = scanner.nextFloat();\n"
        "    scanner.next(); // skip 'hello'\n"
        "    test_word = scanner.next();\n"
        "    test_line = scanner.nextLine();\n"
        "    has_more = scanner.hasNext();\n"
        "}\n";

    Lexer lexer("test.tvk", src);
    auto tokens = lexer.tokenize();
    Parser parser("test.tvk", tokens);
    auto nodes = parser.parse();
    REQUIRE(parser.getErrors().empty());

    Interpreter interpreter;

    // Manually load and interpret standard library io.tvk
    const char* tdkHome = std::getenv("TDK_HOME");
    std::string ioPath = tdkHome ? std::string(tdkHome) + "/lib/std/io.tvk" : "../lib/std/io.tvk";
    std::ifstream in(ioPath);
    REQUIRE(in.is_open());
    std::stringstream ss;
    ss << in.rdbuf();
    std::string ioSource = ss.str();
    Lexer ioLexer(ioPath, ioSource);
    auto ioTokens = ioLexer.tokenize();
    Parser ioParser(ioPath, ioTokens);
    auto ioNodes = ioParser.parse();
    REQUIRE(ioParser.getErrors().empty());
    interpreter.interpret(ioNodes);

    // Interpret the test script
    interpreter.interpret(nodes);

    bool found = false;
    Value vInt = interpreter.getEnvironment()->get("test_int", found);
    REQUIRE(found);
    CHECK(std::get<long long>(vInt.val) == 42LL);

    Value vFloat = interpreter.getEnvironment()->get("test_float", found);
    REQUIRE(found);
    CHECK(std::get<double>(vFloat.val) == 3.14);

    Value vWord = interpreter.getEnvironment()->get("test_word", found);
    REQUIRE(found);
    CHECK(std::get<std::string>(vWord.val) == "world");

    Value vLine = interpreter.getEnvironment()->get("test_line", found);
    REQUIRE(found);
    CHECK(std::get<std::string>(vLine.val) == "last line");

    Value vHasMore = interpreter.getEnvironment()->get("has_more", found);
    REQUIRE(found);
    CHECK(std::get<bool>(vHasMore.val) == false);
}

