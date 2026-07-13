#include <catch2/catch_test_macros.hpp>
#include "thalapathy/cli/CLI.hpp"

TEST_CASE("CLI Conflict Flags", "[cli]") {
    std::vector<std::string> args = {"--check", "--build", "some_file.tvk"};
    int code = thalapathy::CLI::run(args);
    CHECK(code != 0);

    std::vector<std::string> args2 = {"--build", "--emit-ir", "some_file.tvk"};
    int code2 = thalapathy::CLI::run(args2);
    CHECK(code2 != 0);
}
