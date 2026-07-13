#include <catch2/catch_test_macros.hpp>
#include "thalapathy/ir/IR.hpp"
#include "thalapathy/lexer/Lexer.hpp"
#include "thalapathy/parser/Parser.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>

using namespace thalapathy;

TEST_CASE("IR serialization and deserialization", "[ir]") {
    ModuleIR ir;
    ir.constantPool.push_back(100LL);
    ir.constantPool.push_back(20.5);
    ir.constantPool.push_back(std::string("Thalapathy"));

    ir.instructions.push_back(Instruction{OpCode::LOAD_CONST, 0});
    ir.instructions.push_back(Instruction{OpCode::STORE_LOCAL, 1});
    ir.instructions.push_back(Instruction{OpCode::RETURN, 0});

    std::string testPath = "test_artifact.vijay";
    REQUIRE(ir.serialize(testPath));

    ModuleIR loaded;
    REQUIRE(loaded.deserialize(testPath));

    // Compare
    REQUIRE(loaded.constantPool.size() == 3);
    CHECK(std::get<long long>(loaded.constantPool[0]) == 100LL);
    CHECK(std::get<double>(loaded.constantPool[1]) == 20.5);
    CHECK(std::get<std::string>(loaded.constantPool[2]) == "Thalapathy");

    REQUIRE(loaded.instructions.size() == 3);
    CHECK(loaded.instructions[0].opcode == OpCode::LOAD_CONST);
    CHECK(loaded.instructions[1].opcode == OpCode::STORE_LOCAL);

    std::filesystem::remove(testPath);
}

TEST_CASE("IR generator populates class and method metadata", "[ir][compiler]") {
    std::string src =
        "master Person {\n"
        "    string name;\n"
        "    int age;\n"
        "    mersal greet(string who) -> string {\n"
        "        thiruppi \"Hi \" + who;\n"
        "    }\n"
        "}\n";

    Lexer lexer("test.tvk", src);
    auto tokens = lexer.tokenize();
    Parser parser("test.tvk", tokens);
    auto nodes = parser.parse();
    REQUIRE(parser.getErrors().empty());

    IRGenerator generator;
    ModuleIR ir = generator.generate(nodes);

    // Verify metadata was populated in memory
    REQUIRE(ir.classes.size() == 1);
    CHECK(ir.classes[0].name == "Person");
    REQUIRE(ir.classes[0].fields.size() == 2);
    CHECK(ir.classes[0].fields[0] == "name");
    CHECK(ir.classes[0].fields[1] == "age");
    REQUIRE(ir.classes[0].methods.size() == 1);
    CHECK(ir.classes[0].methods[0] == "greet");

    REQUIRE(ir.methods.size() == 1);
    CHECK(ir.methods[0].name == "greet");
    CHECK(ir.methods[0].returnType == "string");
    REQUIRE(ir.methods[0].paramTypes.size() == 1);
    CHECK(ir.methods[0].paramTypes[0] == "string");

    REQUIRE(ir.properties.size() == 2);
    CHECK(ir.properties[0].name == "name");
    CHECK(ir.properties[0].typeStr == "string");
    CHECK(ir.properties[1].name == "age");
    CHECK(ir.properties[1].typeStr == "int");

    // Verify serialization/deserialization of metadata
    std::string testPath = "test_metadata.vijay";
    REQUIRE(ir.serialize(testPath));

    ModuleIR loaded;
    REQUIRE(loaded.deserialize(testPath));

    REQUIRE(loaded.classes.size() == 1);
    CHECK(loaded.classes[0].name == "Person");
    REQUIRE(loaded.classes[0].fields.size() == 2);
    CHECK(loaded.classes[0].fields[0] == "name");
    REQUIRE(loaded.classes[0].methods.size() == 1);
    CHECK(loaded.classes[0].methods[0] == "greet");

    REQUIRE(loaded.methods.size() == 1);
    CHECK(loaded.methods[0].name == "greet");
    CHECK(loaded.methods[0].returnType == "string");

    REQUIRE(loaded.properties.size() == 2);
    CHECK(loaded.properties[0].name == "name");

    std::filesystem::remove(testPath);
}
