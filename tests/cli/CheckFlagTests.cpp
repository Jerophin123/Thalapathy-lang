#include <catch2/catch_test_macros.hpp>
#include "thalapathy/cli/CLI.hpp"
#include <fstream>
#include <filesystem>

TEST_CASE("CLI Check Flag", "[cli]") {
    std::ofstream out("test_check.tvk");
    out << "sarkar io;\n"
        << "thalapathy aarambam {\n"
        << "    ezhudhu(\"syntax check\");\n"
        << "}\n";
    out.close();

    std::vector<std::string> args = {"--check", "test_check.tvk"};
    int code = thalapathy::CLI::run(args);
    CHECK(code == 0);

    // Test with JSON output
    std::vector<std::string> jsonArgs = {"--check", "test_check.tvk", "--json"};
    int jsonCode = thalapathy::CLI::run(jsonArgs);
    CHECK(jsonCode == 0);

    std::filesystem::remove("test_check.tvk");
}
