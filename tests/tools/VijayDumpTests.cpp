#include <catch2/catch_test_macros.hpp>
#include "thalapathy/ir/IR.hpp"
#include <filesystem>
#include <fstream>

using namespace thalapathy;

TEST_CASE("VEAF 3 Binary Formatting and Deserialization", "[ir]") {
    ModuleIR ir;
    ir.constantPool.push_back(100LL);
    ir.constantPool.push_back("Thalapathy");
    ir.instructions.push_back(Instruction{OpCode::LOAD_CONST, 0});
    ir.instructions.push_back(Instruction{OpCode::RETURN, 0});

    std::string testPath = "test_artifact.vijay";
    bool ok = ir.serialize(testPath);
    REQUIRE(ok);

    ModuleIR readIr;
    bool readOk = readIr.deserialize(testPath);
    REQUIRE(readOk);

    CHECK(readIr.constantPool.size() == 2);
    CHECK(std::get<long long>(readIr.constantPool[0]) == 100LL);
    CHECK(std::get<std::string>(readIr.constantPool[1]) == "Thalapathy");
    CHECK(readIr.instructions.size() == 2);
    CHECK(readIr.instructions[0].opcode == OpCode::LOAD_CONST);

    std::filesystem::remove(testPath);
}
