#include <catch2/catch_test_macros.hpp>
#include "thalapathy/lexer/Lexer.hpp"

using namespace thalapathy;

TEST_CASE("Lexer scans keywords and identifiers", "[lexer]") {
    std::string source = "thalapathy aarambam nanba makkal uruthi ghilli illana bigil vaathi mersal thiruppi sollu master varisu theri pokkiri kaavalan Sarkar";
    Lexer lexer("test.tvk", source);
    std::vector<Token> tokens = lexer.tokenize();

    REQUIRE(tokens.size() > 10);
    CHECK(tokens[0].type == TokenType::THALAPATHY);
    CHECK(tokens[1].type == TokenType::AARAMBAM);
    CHECK(tokens[2].type == TokenType::NANBA);
    CHECK(tokens[3].type == TokenType::MAKKAL);
    CHECK(tokens[4].type == TokenType::URUTHI);
    CHECK(tokens[5].type == TokenType::GHILLI);
}

TEST_CASE("Lexer tracks positional metrics", "[lexer]") {
    std::string source = "int year = 2026;\n// comment\nstring name = \"Vijay\";";
    Lexer lexer("test.tvk", source);
    std::vector<Token> tokens = lexer.tokenize();

    REQUIRE(tokens.size() >= 8);
    // int
    CHECK(tokens[0].span.start_line == 1);
    CHECK(tokens[0].span.start_col == 1);
    // name
    // should be line 3
    for (const auto& t : tokens) {
        if (t.value == "name") {
            CHECK(t.span.start_line == 3);
            CHECK(t.span.start_col == 8);
        }
    }
}

TEST_CASE("Lexer reports unterminated string", "[lexer]") {
    std::string source = "nanba text = \"unterminated";
    Lexer lexer("test.tvk", source);
    lexer.tokenize();
    CHECK(!lexer.getErrors().empty());
}
