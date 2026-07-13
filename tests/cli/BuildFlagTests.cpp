#include <catch2/catch_test_macros.hpp>
#include "thalapathy/cli/CLI.hpp"
#include <fstream>
#include <filesystem>

TEST_CASE("CLI Build Flag", "[cli]") {
    std::ofstream out("test_build.tvk");
    out << "sarkar io;\n"
        << "thalapathy aarambam {\n"
        << "    ezhudhu(\"build compile\");\n"
        << "}\n";
    out.close();

    // Default build
    std::vector<std::string> args = {"--build", "test_build.tvk"};
    int code = thalapathy::CLI::run(args);
    CHECK(code == 0);
    CHECK(std::filesystem::exists("test_build.vijay"));

    // Build to custom path
    std::vector<std::string> customArgs = {"--build", "test_build.tvk", "-o", "custom_test.vijay"};
    int customCode = thalapathy::CLI::run(customArgs);
    CHECK(customCode == 0);
    CHECK(std::filesystem::exists("custom_test.vijay"));

    std::filesystem::remove("test_build.tvk");
    std::filesystem::remove("test_build.vijay");
    std::filesystem::remove("custom_test.vijay");
}
