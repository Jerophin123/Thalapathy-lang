#include "TDKCommand.hpp"
#include "TDKUtils.hpp"
#include <iostream>
#include <cstdlib>

namespace thalapathy {

class EnvCommand : public TDKCommand {
public:
    int execute(const TDKContext& ctx, const std::vector<std::string>& args) override {
        (void)args;
        std::string home = getTdkHome();
        const char* pathEnv = std::getenv("PATH");
        std::string pathStr = pathEnv ? pathEnv : "";
        bool inPath = (pathStr.find(home + "\\bin") != std::string::npos || pathStr.find(home + "/bin") != std::string::npos);

        if (ctx.jsonOutput) {
            std::cout << "{\n"
                      << "  \"TDK_HOME\": \"" << home << "\",\n"
                      << "  \"TDK_HOME_bin_in_PATH\": " << (inPath ? "true" : "false") << ",\n"
                      << "  \"PATH\": \"" << (pathEnv ? pathEnv : "") << "\"\n"
                      << "}\n";
        } else {
            std::cout << "TDK_HOME=" << home << "\n"
                      << "TDK_HOME/bin in PATH: " << (inPath ? "YES" : "NO") << "\n";
        }
        return 0;
    }
};

} // namespace thalapathy
