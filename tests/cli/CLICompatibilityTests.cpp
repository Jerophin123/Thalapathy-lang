#include <catch2/catch_test_macros.hpp>
#include "thalapathy/cli/CLI.hpp"
#include <fstream>
#include <filesystem>

TEST_CASE("CLI Compatibility Modes", "[cli]") {
    std::ofstream out("test_compat.tvk");
    out << "sarkar io;\n"
        << "thalapathy aarambam {\n"
        << "    ezhudhu(\"compat check\");\n"
        << "}\n";
    out.close();

    // check subcommand
    std::vector<std::string> checkArgs = {"check", "test_compat.tvk"};
    int checkCode = thalapathy::CLI::run(checkArgs);
    CHECK(checkCode == 0);

    // run subcommand
    std::vector<std::string> runArgs = {"run", "test_compat.tvk"};
    int runCode = thalapathy::CLI::run(runArgs);
    CHECK(runCode == 0);

    // build subcommand
    std::vector<std::string> buildArgs = {"build", "test_compat.tvk"};
    int buildCode = thalapathy::CLI::run(buildArgs);
    CHECK(buildCode == 0);
    CHECK(std::filesystem::exists("test_compat.vijay"));

    std::filesystem::remove("test_compat.tvk");
    std::filesystem::remove("test_compat.vijay");
}
