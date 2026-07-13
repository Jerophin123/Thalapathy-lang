#include <catch2/catch_test_macros.hpp>
#include "thalapathy/cli/CLI.hpp"
#include <fstream>
#include <filesystem>

TEST_CASE("CLI Emit IR Flag", "[cli]") {
    std::ofstream out("test_ir.tvk");
    out << "sarkar io;\n"
        << "thalapathy aarambam {\n"
        << "    ezhudhu(\"emit IR test\");\n"
        << "}\n";
    out.close();

    std::vector<std::string> args = {"--emit-ir", "test_ir.tvk", "-o", "test_ir.thalair"};
    int code = thalapathy::CLI::run(args);
    CHECK(code == 0);
    CHECK(std::filesystem::exists("test_ir.thalair"));

    std::filesystem::remove("test_ir.tvk");
    std::filesystem::remove("test_ir.thalair");
}
