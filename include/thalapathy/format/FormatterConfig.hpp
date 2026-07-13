#pragma once
#include <cstddef>
#include <string>

namespace thalapathy {

struct FormatterConfig {
    std::size_t indentSize = 4;
    bool insertFinalNewline = true;
};

struct FormatterConfigResult {
    bool success = false;
    FormatterConfig config;
    std::string error;
};

FormatterConfigResult loadFormatterConfig(const std::string& path);

} // namespace thalapathy
