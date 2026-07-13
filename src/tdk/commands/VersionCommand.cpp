#include "TDKCommand.hpp"
#include "thalapathy/version/Version.hpp"
#include <iostream>

namespace thalapathy {

class VersionCommand : public TDKCommand {
public:
    int execute(const TDKContext& ctx, const std::vector<std::string>& args) override {
        (void)args;
        if (ctx.jsonOutput) {
            std::cout << "{\n"
                      << "  \"tdkVersion\": \"" << getTdkVersion() << "\",\n"
                      << "  \"languageVersion\": \"" << getLanguageVersion() << "\",\n"
                      << "  \"veafVersion\": " << getVeafVersion() << "\n"
                      << "}\n";
        } else {
            std::cout << "TDK Version: " << getTdkVersion() << "\n"
                      << "THALAPATHY Language Version: " << getLanguageVersion() << "\n"
                      << "VEAF Format Version: " << getVeafVersion() << "\n";
        }
        return 0;
    }
};

} // namespace thalapathy
