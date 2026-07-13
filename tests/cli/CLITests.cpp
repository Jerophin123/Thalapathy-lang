#include <catch2/catch_test_macros.hpp>
#include "thalapathy/cli/CLI.hpp"

using namespace thalapathy;

TEST_CASE("CLI routes options correctly", "[cli]") {
    std::vector<std::string> args = {"version"};
    int code = CLI::run(args);
    CHECK(code == 0);
}

TEST_CASE("CLI rejects non-tvk extension", "[cli]") {
    std::vector<std::string> args = {"run", "test.py"};
    int code = CLI::run(args);
    CHECK(code == 1);
}
