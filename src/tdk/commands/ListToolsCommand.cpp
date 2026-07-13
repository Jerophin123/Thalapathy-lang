#include "TDKCommand.hpp"
#include <iostream>

namespace thalapathy {

class ListToolsCommand : public TDKCommand {
public:
    int execute(const TDKContext& ctx, const std::vector<std::string>& args) override {
        (void)args;
        if (ctx.jsonOutput) {
            std::cout << "[\n"
                      << "  \"thalapathy\",\n"
                      << "  \"tdk\",\n"
                      << "  \"vijaydump\",\n"
                      << "  \"thalafmt\"\n"
                      << "]\n";
        } else {
            std::cout << "TDK Core Tools:\n"
                      << "  - thalapathy (Language driver / execution runner)\n"
                      << "  - tdk        (Development Kit / project manager)\n"
                      << "  - vijaydump  (VEAF binary inspector)\n"
                      << "  - thalafmt   (Source code formatter)\n";
        }
        return 0;
    }
};

} // namespace thalapathy
