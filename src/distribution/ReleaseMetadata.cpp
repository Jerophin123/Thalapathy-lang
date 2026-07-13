#include "thalapathy/distribution/ReleaseMetadata.hpp"
#include <fstream>
#include <sstream>
#include <cctype>
#include <map>
#include <variant>

namespace thalapathy::distribution {

// Minimal JSON value types for parsing RELEASE.json
using JsonValue = std::variant<std::string, int64_t, double, bool, std::nullptr_t>;
using JsonObject = std::map<std::string, JsonValue>;
using JsonObjectNested = std::map<std::string, std::variant<std::string, int64_t, double, bool, std::nullptr_t, std::map<std::string, JsonValue>>>;

static void skipWhitespace(const std::string& s, size_t& pos) {
    while (pos < s.size() && std::isspace(static_cast<unsigned char>(s[pos]))) pos++;
}

static bool parseJsonString(const std::string& s, size_t& pos, std::string& out) {
    if (pos >= s.size() || s[pos] != '"') return false;
    pos++; // skip opening "
    out.clear();
    while (pos < s.size()) {
        if (s[pos] == '"') { pos++; return true; }
        if (s[pos] == '\\' && pos + 1 < s.size()) {
            pos++;
            switch (s[pos]) {
                case '"': out += '"'; break;
                case '\\': out += '\\'; break;
                case '/': out += '/'; break;
                case 'n': out += '\n'; break;
                case 'r': out += '\r'; break;
                case 't': out += '\t'; break;
                default: out += s[pos]; break;
            }
            pos++;
        } else {
            out += s[pos++];
        }
    }
    return false; // unterminated string
}

static bool parseJsonBool(const std::string& s, size_t& pos, bool& out) {
    if (s.compare(pos, 4, "true") == 0) { out = true; pos += 4; return true; }
    if (s.compare(pos, 5, "false") == 0) { out = false; pos += 5; return true; }
    return false;
}

static bool parseJsonInt(const std::string& s, size_t& pos, int64_t& out) {
    size_t start = pos;
    if (pos < s.size() && s[pos] == '-') pos++;
    if (pos >= s.size() || !std::isdigit(static_cast<unsigned char>(s[pos]))) { pos = start; return false; }
    while (pos < s.size() && std::isdigit(static_cast<unsigned char>(s[pos]))) pos++;
    // If we hit a '.', this is a float not an int
    if (pos < s.size() && s[pos] == '.') { pos = start; return false; }
    out = std::stoll(s.substr(start, pos - start));
    return true;
}

// Parse a flat JSON object (string/int/bool values only, no nesting)
static bool parseFlatObject(const std::string& s, size_t& pos, JsonObject& obj) {
    skipWhitespace(s, pos);
    if (pos >= s.size() || s[pos] != '{') return false;
    pos++; // skip {
    skipWhitespace(s, pos);
    if (pos < s.size() && s[pos] == '}') { pos++; return true; }
    while (pos < s.size()) {
        skipWhitespace(s, pos);
        std::string key;
        if (!parseJsonString(s, pos, key)) return false;
        skipWhitespace(s, pos);
        if (pos >= s.size() || s[pos] != ':') return false;
        pos++; // skip :
        skipWhitespace(s, pos);
        // Parse value
        if (s[pos] == '"') {
            std::string val;
            if (!parseJsonString(s, pos, val)) return false;
            obj[key] = val;
        } else if (s[pos] == 't' || s[pos] == 'f') {
            bool val;
            if (!parseJsonBool(s, pos, val)) return false;
            obj[key] = val;
        } else if (std::isdigit(static_cast<unsigned char>(s[pos])) || s[pos] == '-') {
            int64_t val;
            if (!parseJsonInt(s, pos, val)) return false;
            obj[key] = val;
        } else {
            return false; // unsupported value type
        }
        skipWhitespace(s, pos);
        if (pos < s.size() && s[pos] == ',') { pos++; continue; }
        if (pos < s.size() && s[pos] == '}') { pos++; return true; }
        return false;
    }
    return false;
}

// Parse the top-level JSON object which may contain one nested "components" object
struct ParsedRelease {
    JsonObject root;
    JsonObject components;
    bool hasComponents = false;
};

static bool parseReleaseJson(const std::string& s, ParsedRelease& result) {
    size_t pos = 0;
    skipWhitespace(s, pos);
    if (pos >= s.size() || s[pos] != '{') return false;
    pos++;
    skipWhitespace(s, pos);
    if (pos < s.size() && s[pos] == '}') { pos++; return true; }
    while (pos < s.size()) {
        skipWhitespace(s, pos);
        std::string key;
        if (!parseJsonString(s, pos, key)) return false;
        skipWhitespace(s, pos);
        if (pos >= s.size() || s[pos] != ':') return false;
        pos++;
        skipWhitespace(s, pos);
        if (key == "components") {
            if (!parseFlatObject(s, pos, result.components)) return false;
            result.hasComponents = true;
        } else if (s[pos] == '"') {
            std::string val;
            if (!parseJsonString(s, pos, val)) return false;
            result.root[key] = val;
        } else if (s[pos] == 't' || s[pos] == 'f') {
            bool val;
            if (!parseJsonBool(s, pos, val)) return false;
            result.root[key] = val;
        } else if (std::isdigit(static_cast<unsigned char>(s[pos])) || s[pos] == '-') {
            int64_t val;
            if (!parseJsonInt(s, pos, val)) return false;
            result.root[key] = val;
        } else {
            return false;
        }
        skipWhitespace(s, pos);
        if (pos < s.size() && s[pos] == ',') { pos++; continue; }
        if (pos < s.size() && s[pos] == '}') { pos++; return true; }
        return false;
    }
    return false;
}

static bool getString(const JsonObject& obj, const std::string& key, std::string& out, std::string& error) {
    auto it = obj.find(key);
    if (it == obj.end()) { error = "missing required field: " + key; return false; }
    if (!std::holds_alternative<std::string>(it->second)) { error = "field '" + key + "' must be a string"; return false; }
    out = std::get<std::string>(it->second);
    return true;
}

static bool getInt(const JsonObject& obj, const std::string& key, int64_t& out, std::string& error) {
    auto it = obj.find(key);
    if (it == obj.end()) { error = "missing required field: " + key; return false; }
    if (!std::holds_alternative<int64_t>(it->second)) { error = "field '" + key + "' must be an integer"; return false; }
    out = std::get<int64_t>(it->second);
    return true;
}

static bool getBool(const JsonObject& obj, const std::string& key, bool& out, std::string& error) {
    auto it = obj.find(key);
    if (it == obj.end()) { out = false; return true; } // optional booleans default false
    if (!std::holds_alternative<bool>(it->second)) { error = "field '" + key + "' must be a boolean"; return false; }
    out = std::get<bool>(it->second);
    return true;
}

ReleaseMetadataResult loadReleaseMetadata(const std::filesystem::path& path) {
    namespace fs = std::filesystem;

    if (!fs::exists(path)) {
        return {false, {}, "RELEASE.json not found: " + path.string()};
    }

    std::ifstream in(path);
    if (!in.is_open()) {
        return {false, {}, "cannot open RELEASE.json: " + path.string()};
    }

    std::stringstream ss;
    ss << in.rdbuf();
    std::string content = ss.str();

    ParsedRelease parsed;
    if (!parseReleaseJson(content, parsed)) {
        return {false, {}, "invalid JSON in RELEASE.json"};
    }

    ReleaseMetadata meta;
    std::string err;

    if (!getString(parsed.root, "tdkVersion", meta.tdkVersion, err)) return {false, {}, err};
    if (!getString(parsed.root, "languageVersion", meta.languageVersion, err)) return {false, {}, err};

    int64_t veafVer = 0;
    if (!getInt(parsed.root, "veafVersion", veafVer, err)) return {false, {}, err};
    meta.veafVersion = static_cast<uint32_t>(veafVer);

    if (!getString(parsed.root, "artifactMagic", meta.artifactMagic, err)) return {false, {}, err};
    if (!getString(parsed.root, "sourceExtension", meta.sourceExtension, err)) return {false, {}, err};
    if (!getString(parsed.root, "artifactExtension", meta.artifactExtension, err)) return {false, {}, err};

    if (parsed.hasComponents) {
        if (!getBool(parsed.components, "standardLibrary", meta.components.standardLibrary, err)) return {false, {}, err};
        if (!getBool(parsed.components, "vscodeTools", meta.components.vscodeTools, err)) return {false, {}, err};
        if (!getBool(parsed.components, "publicHeaders", meta.components.publicHeaders, err)) return {false, {}, err};
        if (!getBool(parsed.components, "cmakeIntegration", meta.components.cmakeIntegration, err)) return {false, {}, err};
        if (!getBool(parsed.components, "runtimeResources", meta.components.runtimeResources, err)) return {false, {}, err};
        if (!getBool(parsed.components, "veafResources", meta.components.veafResources, err)) return {false, {}, err};
    }

    return {true, meta, ""};
}

} // namespace thalapathy::distribution
