#include <catch2/catch_test_macros.hpp>
#include "thalapathy/lexer/Lexer.hpp"
#include "thalapathy/parser/Parser.hpp"

using namespace thalapathy;

TEST_CASE("Parser structures variable declarations", "[parser]") {
    std::string source = "nanba x = 10 + 5 * 2;";
    Lexer lexer("test.tvk", source);
    auto tokens = lexer.tokenize();
    Parser parser("test.tvk", tokens);
    auto nodes = parser.parse();

    REQUIRE(parser.getErrors().empty());
    REQUIRE(nodes.size() == 1);
    
    auto varDecl = dynamic_cast<VarDecl*>(nodes[0].get());
    REQUIRE(varDecl != nullptr);
    CHECK(varDecl->name == "x");
    CHECK(varDecl->isMutable == false);
}

TEST_CASE("Parser evaluates operator precedence climbing", "[parser]") {
    std::string source = "thalapathy aarambam { sollu(1 + 2 * 3); }";
    Lexer lexer("test.tvk", source);
    auto tokens = lexer.tokenize();
    Parser parser("test.tvk", tokens);
    auto nodes = parser.parse();

    REQUIRE(parser.getErrors().empty());
    REQUIRE(nodes.size() == 1);
    auto entry = dynamic_cast<EntryBlockDecl*>(nodes[0].get());
    REQUIRE(entry != nullptr);
}
