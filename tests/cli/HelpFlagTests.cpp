#include <catch2/catch_test_macros.hpp>
#include "thalapathy/cli/CLI.hpp"

TEST_CASE("CLI Help Flag", "[cli]") {
    std::vector<std::string> args = {"--help"};
    int code = thalapathy::CLI::run(args);
    CHECK(code == 0);
}
