#pragma once
#include <string>
#include <vector>

namespace thalapathy {

struct TDKContext {
    std::string tdkHome = "";
    bool jsonOutput = false;
    bool force = false;
};

class TDKCommand {
public:
    virtual ~TDKCommand() = default;
    virtual int execute(const TDKContext& ctx, const std::vector<std::string>& args) = 0;
};

} // namespace thalapathy
