#include <catch2/catch_test_macros.hpp>
#include "thalapathy/lexer/Lexer.hpp"
#include "thalapathy/parser/Parser.hpp"
#include "thalapathy/semantic/Resolver.hpp"

using namespace thalapathy;

TEST_CASE("Resolver catches immutable reassignment", "[resolver]") {
    std::string source = "thalapathy aarambam { nanba x = 10; x = 20; }";
    Lexer lexer("test.tvk", source);
    auto tokens = lexer.tokenize();
    Parser parser("test.tvk", tokens);
    auto nodes = parser.parse();

    Resolver resolver("test.tvk", source);
    bool resolved = resolver.resolve(nodes);
    CHECK(resolved == false);

    REQUIRE(!resolver.getDiagnostics().empty());
    CHECK(resolver.getDiagnostics()[0].code == "THALA-MUT-001");
}

TEST_CASE("Resolver catches duplicate definitions", "[resolver]") {
    std::string source = "thalapathy aarambam { makkal x = 10; makkal x = 20; }";
    Lexer lexer("test.tvk", source);
    auto tokens = lexer.tokenize();
    Parser parser("test.tvk", tokens);
    auto nodes = parser.parse();

    Resolver resolver("test.tvk", source);
    bool resolved = resolver.resolve(nodes);
    CHECK(resolved == false);
    CHECK(resolver.getDiagnostics()[0].code == "THALA-NAME-001");
}

TEST_CASE("Resolver catches break outside loops", "[resolver]") {
    std::string source = "thalapathy aarambam { break; }";
    Lexer lexer("test.tvk", source);
    auto tokens = lexer.tokenize();
    Parser parser("test.tvk", tokens);
    auto nodes = parser.parse();

    Resolver resolver("test.tvk", source);
    bool resolved = resolver.resolve(nodes);
    CHECK(resolved == false);
    CHECK(resolver.getDiagnostics()[0].code == "THALA-LOOP-001");
}
