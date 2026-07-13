#include <catch2/catch_test_macros.hpp>
#include "thalapathy/format/Formatter.hpp"

using namespace thalapathy;

TEST_CASE("Formatter Comment and Space Rules", "[format]") {
    std::string err;
    Formatter formatter; // default config (indentSize=4, insertFinalNewline=true)

    // Test binary operator spacing
    std::string src1 = "var a=1+2*3;";
    std::string expected1 = "var a = 1 + 2 * 3;\n";
    std::string out1 = formatter.format(src1, err);
    CHECK(out1 == expected1);

    // Test comma normalization
    std::string src2 = "fun foo(x,y,z) { }";
    std::string expected2 = "fun foo(x, y, z) {\n}\n";
    std::string out2 = formatter.format(src2, err);
    CHECK(out2 == expected2);

    // Test block structure indentation and semicolons
    std::string src3 = "thalapathy aarambam {\nvar a = 5;\nif(a > 2){\nsollu(\"ok\");\n}\n}";
    std::string expected3 = "thalapathy aarambam {\n    var a = 5;\n    if (a > 2) {\n        sollu(\"ok\");\n    }\n}\n";
    std::string out3 = formatter.format(src3, err);
    CHECK(out3 == expected3);

    // Test comment preservation
    std::string src4 = "// single comment\nvar a = 1; /* block comment */";
    std::string expected4 = "// single comment\nvar a = 1; /* block comment */\n";
    std::string out4 = formatter.format(src4, err);
    CHECK(out4 == expected4);
}

TEST_CASE("Formatter Config indentSize=2", "[format]") {
    FormatterConfig config;
    config.indentSize = 2;
    Formatter formatter(config);
    std::string err;

    std::string src = "ghilli (true) {\nsollu(\"hi\");\n}";
    std::string out = formatter.format(src, err);
    CHECK(out == "ghilli (true) {\n  sollu(\"hi\");\n}\n");
}
