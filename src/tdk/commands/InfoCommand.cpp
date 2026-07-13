#include "TDKCommand.hpp"
#include "TDKUtils.hpp"
#include "thalapathy/version/Version.hpp"
#include <iostream>

namespace thalapathy {

class InfoCommand : public TDKCommand {
public:
    int execute(const TDKContext& ctx, const std::vector<std::string>& args) override {
        (void)args;
        std::string home = getTdkHome();
        if (ctx.jsonOutput) {
            std::cout << "{\n"
                      << "  \"name\": \"THALAPATHY DEVELOPMENT KIT\",\n"
                      << "  \"version\": \"" << getTdkVersion() << "\",\n"
                      << "  \"home\": \"" << home << "\"\n"
                      << "}\n";
        } else {
            std::cout << "THALAPATHY DEVELOPMENT KIT (TDK)\n"
                      << "Version: " << getTdkVersion() << "\n"
                      << "Home: " << home << "\n";
        }
        return 0;
    }
};

} // namespace thalapathy
