#include "TDKCommand.hpp"
#include "TDKUtils.hpp"
#include <iostream>

namespace thalapathy {

class HomeCommand : public TDKCommand {
public:
    int execute(const TDKContext& ctx, const std::vector<std::string>& args) override {
        (void)args;
        std::string home = getTdkHome();
        if (ctx.jsonOutput) {
            std::cout << "{\n  \"tdkHome\": \"" << home << "\"\n}\n";
        } else {
            std::cout << home << "\n";
        }
        return 0;
    }
};

} // namespace thalapathy
