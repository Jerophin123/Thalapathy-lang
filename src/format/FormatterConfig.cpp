#include "thalapathy/format/FormatterConfig.hpp"
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>

namespace thalapathy {

static std::string trim(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) start++;
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) end--;
    return s.substr(start, end - start);
}

FormatterConfigResult loadFormatterConfig(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        return {false, {}, "cannot open configuration file: " + path};
    }

    FormatterConfig config;
    std::string line;
    int lineNum = 0;

    while (std::getline(in, line)) {
        lineNum++;
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == ';') continue;

        auto eqPos = trimmed.find('=');
        if (eqPos == std::string::npos) {
            return {false, {}, "syntax error on line " + std::to_string(lineNum) + ": missing '='"};
        }

        std::string key = trim(trimmed.substr(0, eqPos));
        std::string value = trim(trimmed.substr(eqPos + 1));

        if (key == "indent_size") {
            // Validate it is a valid integer
            for (char c : value) {
                if (!std::isdigit(static_cast<unsigned char>(c))) {
                    return {false, {}, "invalid indent_size on line " + std::to_string(lineNum) + ": expected integer, got '" + value + "'"};
                }
            }
            if (value.empty()) {
                return {false, {}, "empty indent_size on line " + std::to_string(lineNum)};
            }
            int parsed = std::stoi(value);
            if (parsed < 1 || parsed > 16) {
                return {false, {}, "indent_size must be between 1 and 16 (got " + std::to_string(parsed) + ") on line " + std::to_string(lineNum)};
            }
            config.indentSize = static_cast<std::size_t>(parsed);
        } else if (key == "insert_final_newline") {
            if (value == "true") {
                config.insertFinalNewline = true;
            } else if (value == "false") {
                config.insertFinalNewline = false;
            } else {
                return {false, {}, "invalid insert_final_newline on line " + std::to_string(lineNum) + ": expected 'true' or 'false', got '" + value + "'"};
            }
        } else {
            return {false, {}, "unknown configuration key '" + key + "' on line " + std::to_string(lineNum)};
        }
    }

    return {true, config, ""};
}

} // namespace thalapathy
