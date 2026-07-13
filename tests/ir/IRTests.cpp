#include <catch2/catch_test_macros.hpp>
#include "thalapathy/ir/IR.hpp"
#include <filesystem>

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
