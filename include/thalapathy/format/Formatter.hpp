#pragma once
#include <string>
#include "thalapathy/format/FormatterConfig.hpp"

namespace thalapathy {

class Formatter {
public:
    explicit Formatter(FormatterConfig config = {});

    std::string format(const std::string& source, std::string& error);

private:
    FormatterConfig config_;
};

} // namespace thalapathy
