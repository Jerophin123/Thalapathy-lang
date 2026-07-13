#include <catch2/catch_test_macros.hpp>
#include "thalapathy/cli/CLI.hpp"
#include <fstream>
#include <filesystem>

TEST_CASE("CLI Simple Run", "[cli]") {
    std::ofstream out("test_run.tvk");
    out << "sarkar io;\n"
        << "thalapathy aarambam {\n"
        << "    ezhudhu(\"Hello, TDK!\");\n"
        << "}\n";
    out.close();

    std::vector<std::string> args = {"test_run.tvk"};
    int code = thalapathy::CLI::run(args);
    CHECK(code == 0);

    std::filesystem::remove("test_run.tvk");
}
