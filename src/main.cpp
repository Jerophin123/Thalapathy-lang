#include "thalapathy/cli/CLI.hpp"
#include <vector>
#include <string>

int main(int argc, char* argv[]) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    return thalapathy::CLI::run(args);
}
