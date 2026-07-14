#include "thalapathy/interpreter/Interpreter.hpp"
#include "thalapathy/net/Net.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <cctype>
#include <random>
#include <thread>
#include <regex>
#include <ctime>

namespace thalapathy {

// Helper builtins
static Value builtinSollu(const std::vector<Value>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        std::cout << args[i].toString();
        if (i + 1 < args.size()) std::cout << " ";
    }
    std::cout << "\n";
    return Value{std::monostate{}};
}

static Value builtinLen(const std::vector<Value>& args) {
    if (args.empty()) return Value{0LL};
    const auto& val = args[0];
    if (val.isArray()) {
        return Value{static_cast<long long>(std::get<std::shared_ptr<ArrayValue>>(val.val)->elements.size())};
    }
    if (val.isMap()) {
        return Value{static_cast<long long>(std::get<std::shared_ptr<MapValue>>(val.val)->entries.size())};
    }
    if (val.isString()) {
        return Value{static_cast<long long>(std::get<std::string>(val.val).length())};
    }
    return Value{0LL};
}

static Value builtinRange(const std::vector<Value>& args) {
    if (args.size() < 2) return Value{std::make_shared<ArrayValue>()};
    long long start = args[0].isInt() ? std::get<long long>(args[0].val) : 0LL;
    long long end = args[1].isInt() ? std::get<long long>(args[1].val) : 0LL;
    
    auto arr = std::make_shared<ArrayValue>();
    for (long long i = start; i < end; ++i) {
        arr->elements.push_back(Value{i});
    }
    return Value{arr};
}

static Value builtinType(const std::vector<Value>& args) {
    if (args.empty()) return Value{"null"};
    const auto& v = args[0];
    if (v.isNull()) return Value{"null"};
    if (v.isInt()) return Value{"int"};
    if (v.isFloat()) return Value{"float"};
    if (v.isBool()) return Value{"bool"};
    if (v.isChar()) return Value{"char"};
    if (v.isString()) return Value{"string"};
    if (v.isArray()) return Value{"array"};
    if (v.isMap()) return Value{"map"};
    if (v.isFunction()) return Value{"function"};
    if (v.isClass()) return Value{"class"};
    if (v.isInstance()) return Value{"instance"};
    if (v.isEnum()) return Value{std::get<std::shared_ptr<EnumValue>>(v.val)->typeName};
    if (v.isBuiltIn()) return Value{"function"};
    return Value{"unknown"};
}

static Value builtinString(const std::vector<Value>& args) {
    if (args.empty()) return Value{""};
    return Value{args[0].toString()};
}

static Value builtinInt(const std::vector<Value>& args) {
    if (args.empty()) return Value{0LL};
    const auto& v = args[0];
    if (v.isInt()) return v;
    if (v.isFloat()) return Value{static_cast<long long>(std::get<double>(v.val))};
    if (v.isString()) {
        try {
            return Value{std::stoll(std::get<std::string>(v.val))};
        } catch (...) {
            return Value{0LL};
        }
    }
    return Value{0LL};
}

static Value builtinFloat(const std::vector<Value>& args) {
    if (args.empty()) return Value{0.0};
    const auto& v = args[0];
    if (v.isFloat()) return v;
    if (v.isInt()) return Value{static_cast<double>(std::get<long long>(v.val))};
    if (v.isString()) {
        try {
            return Value{std::stod(std::get<std::string>(v.val))};
        } catch (...) {
            return Value{0.0};
        }
    }
    return Value{0.0};
}

static Value builtinBool(const std::vector<Value>& args) {
    if (args.empty()) return Value{false};
    const auto& v = args[0];
    if (v.isBool()) return v;
    if (v.isInt()) return Value{std::get<long long>(v.val) != 0};
    if (v.isFloat()) return Value{std::get<double>(v.val) != 0.0};
    if (v.isString()) return Value{std::get<std::string>(v.val) == "true"};
    return Value{!v.isNull()};
}

static Value builtinChar(const std::vector<Value>& args) {
    if (args.empty()) return Value{'\0'};
    const auto& v = args[0];
    if (v.isChar()) return v;
    if (v.isInt()) return Value{static_cast<char>(std::get<long long>(v.val))};
    if (v.isString() && !std::get<std::string>(v.val).empty()) return Value{std::get<std::string>(v.val)[0]};
    return Value{'\0'};
}

static Value builtinReadLine(const std::vector<Value>& args) {
    (void)args;
    std::string line;
    if (std::getline(std::cin, line)) {
        return Value{line};
    }
    return Value{std::monostate{}};
}

static Value builtinTimeNow(const std::vector<Value>& args) {
    (void)args;
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return Value{static_cast<long long>(millis)};
}

static Value builtinReadFile(const std::vector<Value>& args) {
    if (args.empty() || !args[0].isString()) return Value{std::monostate{}};
    std::string path = std::get<std::string>(args[0].val);
    std::ifstream in(path);
    if (!in.is_open()) return Value{std::monostate{}};
    std::stringstream ss;
    ss << in.rdbuf();
    return Value{ss.str()};
}

static Value builtinWriteFile(const std::vector<Value>& args) {
    if (args.size() < 2 || !args[0].isString() || !args[1].isString()) return Value{false};
    std::string path = std::get<std::string>(args[0].val);
    std::string content = std::get<std::string>(args[1].val);
    std::ofstream out(path);
    if (!out.is_open()) return Value{false};
    out << content;
    return Value{true};
}

static Value builtinFileExists(const std::vector<Value>& args) {
    if (args.empty() || !args[0].isString()) return Value{false};
    std::string path = std::get<std::string>(args[0].val);
    return Value{std::filesystem::exists(path)};
}

static Value builtinGetEnv(const std::vector<Value>& args) {
    if (args.empty() || !args[0].isString()) return Value{std::monostate{}};
    std::string name = std::get<std::string>(args[0].val);
    const char* val = std::getenv(name.c_str());
    if (!val) return Value{std::monostate{}};
    return Value{std::string(val)};
}

static Value builtinSetEnv(const std::vector<Value>& args) {
    if (args.size() < 2 || !args[0].isString() || !args[1].isString()) return Value{false};
    std::string name = std::get<std::string>(args[0].val);
    std::string value = std::get<std::string>(args[1].val);
#ifdef _WIN32
    int res = _putenv_s(name.c_str(), value.c_str());
#else
    int res = setenv(name.c_str(), value.c_str(), 1);
#endif
    return Value{res == 0};
}

static Value builtinSysCommand(const std::vector<Value>& args) {
    if (args.empty() || !args[0].isString()) return Value{std::monostate{}};
    std::string cmd = std::get<std::string>(args[0].val);
#ifdef _WIN32
    FILE* pipe = _wpopen(std::wstring(cmd.begin(), cmd.end()).c_str(), L"r");
#else
    FILE* pipe = popen(cmd.c_str(), "r");
#endif
    if (!pipe) return Value{std::monostate{}};
    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif
    return Value{result};
}

static Value builtinMathSqrt(const std::vector<Value>& args) {
    if (args.empty()) return Value{0.0};
    double val = args[0].isFloat() ? std::get<double>(args[0].val) : (args[0].isInt() ? static_cast<double>(std::get<long long>(args[0].val)) : 0.0);
    return Value{std::sqrt(val)};
}

static Value builtinMathSin(const std::vector<Value>& args) {
    if (args.empty()) return Value{0.0};
    double val = args[0].isFloat() ? std::get<double>(args[0].val) : (args[0].isInt() ? static_cast<double>(std::get<long long>(args[0].val)) : 0.0);
    return Value{std::sin(val)};
}

static Value builtinMathCos(const std::vector<Value>& args) {
    if (args.empty()) return Value{0.0};
    double val = args[0].isFloat() ? std::get<double>(args[0].val) : (args[0].isInt() ? static_cast<double>(std::get<long long>(args[0].val)) : 0.0);
    return Value{std::cos(val)};
}

static Value builtinMathCeil(const std::vector<Value>& args) {
    if (args.empty()) return Value{0.0};
    double val = args[0].isFloat() ? std::get<double>(args[0].val) : (args[0].isInt() ? static_cast<double>(std::get<long long>(args[0].val)) : 0.0);
    return Value{std::ceil(val)};
}

static Value builtinMathFloor(const std::vector<Value>& args) {
    if (args.empty()) return Value{0.0};
    double val = args[0].isFloat() ? std::get<double>(args[0].val) : (args[0].isInt() ? static_cast<double>(std::get<long long>(args[0].val)) : 0.0);
    return Value{std::floor(val)};
}

static Value builtinStringSplit(const std::vector<Value>& args) {
    if (args.size() < 2 || !args[0].isString() || !args[1].isString()) return Value{std::make_shared<ArrayValue>()};
    std::string s = std::get<std::string>(args[0].val);
    std::string delim = std::get<std::string>(args[1].val);
    auto arr = std::make_shared<ArrayValue>();
    if (delim.empty()) {
        for (char c : s) {
            arr->elements.push_back(Value{std::string(1, c)});
        }
        return Value{arr};
    }
    size_t pos = 0;
    while ((pos = s.find(delim)) != std::string::npos) {
        arr->elements.push_back(Value{s.substr(0, pos)});
        s.erase(0, pos + delim.length());
    }
    arr->elements.push_back(Value{s});
    return Value{arr};
}

static Value builtinStringReplace(const std::vector<Value>& args) {
    if (args.size() < 3 || !args[0].isString() || !args[1].isString() || !args[2].isString()) return Value{""};
    std::string s = std::get<std::string>(args[0].val);
    std::string findStr = std::get<std::string>(args[1].val);
    std::string replaceStr = std::get<std::string>(args[2].val);
    if (findStr.empty()) return Value{s};
    size_t pos = 0;
    while ((pos = s.find(findStr, pos)) != std::string::npos) {
        s.replace(pos, findStr.length(), replaceStr);
        pos += replaceStr.length();
    }
    return Value{s};
}

// ---- Randomness ----
static std::mt19937_64& rngEngine() {
    static std::mt19937_64 engine{std::random_device{}()};
    return engine;
}

static Value builtinRandom(const std::vector<Value>& args) {
    (void)args;
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return Value{dist(rngEngine())};
}

static Value builtinRandomInt(const std::vector<Value>& args) {
    if (args.size() < 2 || !args[0].isInt() || !args[1].isInt()) return Value{0LL};
    long long lo = std::get<long long>(args[0].val);
    long long hi = std::get<long long>(args[1].val);
    if (lo > hi) std::swap(lo, hi);
    std::uniform_int_distribution<long long> dist(lo, hi);
    return Value{dist(rngEngine())};
}

static Value builtinRandomSeed(const std::vector<Value>& args) {
    if (!args.empty() && args[0].isInt()) {
        rngEngine().seed(static_cast<uint64_t>(std::get<long long>(args[0].val)));
    }
    return Value{std::monostate{}};
}

// ---- Hashing (standard SHA-256, FIPS 180-4) ----
static Value builtinSha256(const std::vector<Value>& args) {
    if (args.empty() || !args[0].isString()) return Value{std::string("")};
    const std::string& msg = std::get<std::string>(args[0].val);

    auto rotr = [](uint32_t x, uint32_t n) -> uint32_t { return (x >> n) | (x << (32 - n)); };
    static const uint32_t K[64] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2 };
    uint32_t h[8] = {0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};

    std::string data = msg;
    uint64_t bitLen = static_cast<uint64_t>(data.size()) * 8;
    data.push_back(static_cast<char>(0x80));
    while (data.size() % 64 != 56) data.push_back(0);
    for (int i = 7; i >= 0; --i) data.push_back(static_cast<char>((bitLen >> (i * 8)) & 0xff));

    for (size_t chunk = 0; chunk < data.size(); chunk += 64) {
        uint32_t w[64];
        for (int i = 0; i < 16; ++i) {
            w[i] = (static_cast<uint8_t>(data[chunk + i*4]) << 24) |
                   (static_cast<uint8_t>(data[chunk + i*4+1]) << 16) |
                   (static_cast<uint8_t>(data[chunk + i*4+2]) << 8) |
                   (static_cast<uint8_t>(data[chunk + i*4+3]));
        }
        for (int i = 16; i < 64; ++i) {
            uint32_t s0 = rotr(w[i-15],7) ^ rotr(w[i-15],18) ^ (w[i-15] >> 3);
            uint32_t s1 = rotr(w[i-2],17) ^ rotr(w[i-2],19) ^ (w[i-2] >> 10);
            w[i] = w[i-16] + s0 + w[i-7] + s1;
        }
        uint32_t a=h[0],b=h[1],c=h[2],d=h[3],e=h[4],f=h[5],g=h[6],hh=h[7];
        for (int i = 0; i < 64; ++i) {
            uint32_t S1 = rotr(e,6) ^ rotr(e,11) ^ rotr(e,25);
            uint32_t ch = (e & f) ^ ((~e) & g);
            uint32_t t1 = hh + S1 + ch + K[i] + w[i];
            uint32_t S0 = rotr(a,2) ^ rotr(a,13) ^ rotr(a,22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t t2 = S0 + maj;
            hh=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
        }
        h[0]+=a; h[1]+=b; h[2]+=c; h[3]+=d; h[4]+=e; h[5]+=f; h[6]+=g; h[7]+=hh;
    }
    static const char* hex = "0123456789abcdef";
    std::string out;
    for (int i = 0; i < 8; ++i)
        for (int b = 28; b >= 0; b -= 4)
            out.push_back(hex[(h[i] >> b) & 0xf]);
    return Value{out};
}

// ---- OS / process ----
static Value builtinOsName(const std::vector<Value>& args) {
    (void)args;
#ifdef _WIN32
    return Value{std::string("windows")};
#elif defined(__APPLE__)
    return Value{std::string("macos")};
#else
    return Value{std::string("linux")};
#endif
}

static Value builtinOsCwd(const std::vector<Value>& args) {
    (void)args;
    try {
        return Value{std::filesystem::current_path().string()};
    } catch (...) {
        return Value{std::string("")};
    }
}

static Value builtinOsExit(const std::vector<Value>& args) {
    int code = (!args.empty() && args[0].isInt()) ? static_cast<int>(std::get<long long>(args[0].val)) : 0;
    std::exit(code);
}

static Value builtinTimeMillis(const std::vector<Value>& args) {
    (void)args;
    auto now = std::chrono::system_clock::now().time_since_epoch();
    return Value{static_cast<long long>(std::chrono::duration_cast<std::chrono::milliseconds>(now).count())};
}

static Value builtinSleepMs(const std::vector<Value>& args) {
    if (!args.empty() && args[0].isInt()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(std::get<long long>(args[0].val)));
    }
    return Value{std::monostate{}};
}

// ---- JSON ----
static void jsonEscape(std::ostream& os, const std::string& s) {
    os << '"';
    for (char c : s) {
        switch (c) {
            case '"': os << "\\\""; break;
            case '\\': os << "\\\\"; break;
            case '\n': os << "\\n"; break;
            case '\r': os << "\\r"; break;
            case '\t': os << "\\t"; break;
            default: os << c; break;
        }
    }
    os << '"';
}

static void jsonWrite(std::ostream& os, const Value& v) {
    if (v.isNull()) { os << "null"; return; }
    if (v.isBool()) { os << (std::get<bool>(v.val) ? "true" : "false"); return; }
    if (v.isInt()) { os << std::get<long long>(v.val); return; }
    if (v.isFloat()) { os << std::get<double>(v.val); return; }
    if (v.isString()) { jsonEscape(os, std::get<std::string>(v.val)); return; }
    if (v.isChar()) { jsonEscape(os, std::string(1, std::get<char>(v.val))); return; }
    if (v.isArray()) {
        auto arr = std::get<std::shared_ptr<ArrayValue>>(v.val);
        os << '[';
        for (size_t i = 0; i < arr->elements.size(); ++i) {
            if (i) os << ',';
            jsonWrite(os, arr->elements[i]);
        }
        os << ']';
        return;
    }
    if (v.isMap()) {
        auto map = std::get<std::shared_ptr<MapValue>>(v.val);
        os << '{';
        size_t i = 0;
        for (const auto& [k, val] : map->entries) {
            if (i++) os << ',';
            jsonEscape(os, k);
            os << ':';
            jsonWrite(os, val);
        }
        os << '}';
        return;
    }
    os << "null";
}

static Value builtinJsonStringify(const std::vector<Value>& args) {
    if (args.empty()) return Value{std::string("null")};
    std::ostringstream os;
    jsonWrite(os, args[0]);
    return Value{os.str()};
}

// Minimal recursive-descent JSON parser producing THALAPATHY values.
namespace {
struct JsonParser {
    const std::string& s;
    size_t i = 0;
    explicit JsonParser(const std::string& src) : s(src) {}

    void skipWs() { while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\n' || s[i] == '\r')) ++i; }

    Value parse() { skipWs(); return parseValue(); }

    Value parseValue() {
        skipWs();
        if (i >= s.size()) return Value{std::monostate{}};
        char c = s[i];
        if (c == '{') return parseObject();
        if (c == '[') return parseArray();
        if (c == '"') return Value{parseString()};
        if (c == 't' || c == 'f') return parseBool();
        if (c == 'n') { i += 4; return Value{std::monostate{}}; }
        return parseNumber();
    }

    Value parseObject() {
        auto map = std::make_shared<MapValue>();
        ++i; // {
        skipWs();
        if (i < s.size() && s[i] == '}') { ++i; return Value{map}; }
        for (;;) {
            skipWs();
            std::string key = parseString();
            skipWs();
            if (i < s.size() && s[i] == ':') ++i;
            Value val = parseValue();
            map->entries[key] = val;
            skipWs();
            if (i < s.size() && s[i] == ',') { ++i; continue; }
            if (i < s.size() && s[i] == '}') { ++i; break; }
            break;
        }
        return Value{map};
    }

    Value parseArray() {
        auto arr = std::make_shared<ArrayValue>();
        ++i; // [
        skipWs();
        if (i < s.size() && s[i] == ']') { ++i; return Value{arr}; }
        for (;;) {
            arr->elements.push_back(parseValue());
            skipWs();
            if (i < s.size() && s[i] == ',') { ++i; continue; }
            if (i < s.size() && s[i] == ']') { ++i; break; }
            break;
        }
        return Value{arr};
    }

    std::string parseString() {
        std::string out;
        if (i < s.size() && s[i] == '"') ++i;
        while (i < s.size() && s[i] != '"') {
            char c = s[i++];
            if (c == '\\' && i < s.size()) {
                char e = s[i++];
                switch (e) {
                    case 'n': out += '\n'; break;
                    case 't': out += '\t'; break;
                    case 'r': out += '\r'; break;
                    case '"': out += '"'; break;
                    case '\\': out += '\\'; break;
                    case '/': out += '/'; break;
                    default: out += e; break;
                }
            } else {
                out += c;
            }
        }
        if (i < s.size() && s[i] == '"') ++i;
        return out;
    }

    Value parseBool() {
        if (s.compare(i, 4, "true") == 0) { i += 4; return Value{true}; }
        if (s.compare(i, 5, "false") == 0) { i += 5; return Value{false}; }
        return Value{false};
    }

    Value parseNumber() {
        size_t start = i;
        bool isFloat = false;
        while (i < s.size() && (std::isdigit(static_cast<unsigned char>(s[i])) || s[i] == '-' || s[i] == '+' ||
                                s[i] == '.' || s[i] == 'e' || s[i] == 'E')) {
            if (s[i] == '.' || s[i] == 'e' || s[i] == 'E') isFloat = true;
            ++i;
        }
        std::string num = s.substr(start, i - start);
        try {
            if (isFloat) return Value{std::stod(num)};
            return Value{static_cast<long long>(std::stoll(num))};
        } catch (...) {
            return Value{std::monostate{}};
        }
    }
};
} // namespace

static Value builtinJsonParse(const std::vector<Value>& args) {
    if (args.empty() || !args[0].isString()) return Value{std::monostate{}};
    JsonParser p(std::get<std::string>(args[0].val));
    return p.parse();
}

// ---- HTTP client ----
static Value makeHttpResultValue(const net::HttpResult& r) {
    auto map = std::make_shared<MapValue>();
    map->entries["success"] = Value{r.success};
    map->entries["status"] = Value{static_cast<long long>(r.status)};
    map->entries["body"] = Value{r.body};
    map->entries["error"] = Value{r.error};
    return Value{map};
}

static Value builtinHttpGet(const std::vector<Value>& args) {
    if (args.empty() || !args[0].isString()) return makeHttpResultValue({false, 0, "", "url required"});
    return makeHttpResultValue(net::httpRequest("GET", std::get<std::string>(args[0].val)));
}

static Value builtinHttpPost(const std::vector<Value>& args) {
    if (args.empty() || !args[0].isString()) return makeHttpResultValue({false, 0, "", "url required"});
    std::string body = (args.size() >= 2) ? args[1].toString() : "";
    std::string ct = (args.size() >= 3 && args[2].isString()) ? std::get<std::string>(args[2].val) : "application/json";
    return makeHttpResultValue(net::httpRequest("POST", std::get<std::string>(args[0].val), body, ct));
}

static Value builtinHttpRequest(const std::vector<Value>& args) {
    if (args.size() < 2 || !args[0].isString() || !args[1].isString())
        return makeHttpResultValue({false, 0, "", "verb and url required"});
    std::string body = (args.size() >= 3) ? args[2].toString() : "";
    std::string ct = (args.size() >= 4 && args[3].isString()) ? std::get<std::string>(args[3].val) : "application/json";
    return makeHttpResultValue(net::httpRequest(std::get<std::string>(args[0].val), std::get<std::string>(args[1].val), body, ct));
}

// ---- Regex (std::regex) ----
static Value builtinRegexMatch(const std::vector<Value>& args) {
    if (args.size() < 2 || !args[0].isString() || !args[1].isString()) return Value{false};
    try {
        std::regex re(std::get<std::string>(args[1].val));
        return Value{std::regex_search(std::get<std::string>(args[0].val), re)};
    } catch (...) { return Value{false}; }
}

static Value builtinRegexFindAll(const std::vector<Value>& args) {
    auto out = std::make_shared<ArrayValue>();
    if (args.size() < 2 || !args[0].isString() || !args[1].isString()) return Value{out};
    try {
        std::regex re(std::get<std::string>(args[1].val));
        const std::string& s = std::get<std::string>(args[0].val);
        auto begin = std::sregex_iterator(s.begin(), s.end(), re);
        for (auto it = begin; it != std::sregex_iterator(); ++it) {
            out->elements.push_back(Value{it->str()});
        }
    } catch (...) {}
    return Value{out};
}

static Value builtinRegexReplace(const std::vector<Value>& args) {
    if (args.size() < 3 || !args[0].isString() || !args[1].isString() || !args[2].isString())
        return args.empty() ? Value{std::string("")} : args[0];
    try {
        std::regex re(std::get<std::string>(args[1].val));
        return Value{std::regex_replace(std::get<std::string>(args[0].val), re, std::get<std::string>(args[2].val))};
    } catch (...) { return args[0]; }
}

// ---- Date / time ----
static Value builtinDateFormat(const std::vector<Value>& args) {
    // (epochSeconds, formatString) -> formatted local time
    std::time_t t = (!args.empty() && args[0].isInt()) ? static_cast<std::time_t>(std::get<long long>(args[0].val))
                                                        : std::time(nullptr);
    std::string fmt = (args.size() >= 2 && args[1].isString()) ? std::get<std::string>(args[1].val)
                                                               : "%Y-%m-%d %H:%M:%S";
    std::tm tmv{};
#ifdef _WIN32
    localtime_s(&tmv, &t);
#else
    localtime_r(&t, &tmv);
#endif
    char buf[256];
    size_t n = std::strftime(buf, sizeof(buf), fmt.c_str(), &tmv);
    return Value{std::string(buf, n)};
}

static Value builtinDateNowSeconds(const std::vector<Value>& args) {
    (void)args;
    return Value{static_cast<long long>(std::time(nullptr))};
}

// ---- Logging ----
static Value builtinLog(const std::vector<Value>& args) {
    // (level, message)
    std::string level = (!args.empty() && args[0].isString()) ? std::get<std::string>(args[0].val) : "INFO";
    std::string msg = (args.size() >= 2) ? args[1].toString() : "";
    std::time_t t = std::time(nullptr);
    std::tm tmv{};
#ifdef _WIN32
    localtime_s(&tmv, &t);
#else
    localtime_r(&t, &tmv);
#endif
    char ts[32];
    std::strftime(ts, sizeof(ts), "%H:%M:%S", &tmv);
    std::ostream& out = (level == "ERROR" || level == "WARN") ? std::cerr : std::cout;
    out << "[" << ts << "] [" << level << "] " << msg << "\n";
    return Value{std::monostate{}};
}

// ============================================================
// VijayDB — native embedded document database
// A database is a set of named collections; each collection holds documents
// (maps) with an auto-assigned integer "_id". State persists to a JSON file
// on every mutation, so data survives across runs.
// ============================================================
namespace {

struct DbCollection {
    std::vector<Value> docs;
    long long nextId = 1;
};

struct DbInstance {
    std::string path;
    std::unordered_map<std::string, DbCollection> collections;
};

std::vector<std::shared_ptr<DbInstance>>& databases() {
    static std::vector<std::shared_ptr<DbInstance>> dbs;
    return dbs;
}

DbInstance* getDb(const Value& handleVal) {
    if (!handleVal.isInt()) return nullptr;
    long long h = std::get<long long>(handleVal.val);
    if (h < 1 || h > static_cast<long long>(databases().size())) return nullptr;
    return databases()[static_cast<size_t>(h - 1)].get();
}

// Shallow copy of a document map so stored data and returned snapshots don't
// alias the caller's value (top-level fields become independent).
Value cloneDoc(const Value& doc) {
    if (!doc.isMap()) return doc;
    auto src = std::get<std::shared_ptr<MapValue>>(doc.val);
    auto copy = std::make_shared<MapValue>();
    copy->entries = src->entries;
    return Value{copy};
}

long long docId(const Value& doc) {
    if (!doc.isMap()) return -1;
    auto m = std::get<std::shared_ptr<MapValue>>(doc.val);
    auto it = m->entries.find("_id");
    if (it != m->entries.end() && it->second.isInt()) return std::get<long long>(it->second.val);
    return -1;
}

bool dbValuesEqual(const Value& a, const Value& b) {
    if (a.isInt() && b.isInt()) return std::get<long long>(a.val) == std::get<long long>(b.val);
    if (a.isFloat() && b.isFloat()) return std::get<double>(a.val) == std::get<double>(b.val);
    if (a.isBool() && b.isBool()) return std::get<bool>(a.val) == std::get<bool>(b.val);
    if (a.isString() && b.isString()) return std::get<std::string>(a.val) == std::get<std::string>(b.val);
    if (a.isNull() && b.isNull()) return true;
    return a.toString() == b.toString();
}

void dbSave(DbInstance& inst) {
    auto root = std::make_shared<MapValue>();
    for (auto& [name, coll] : inst.collections) {
        auto arr = std::make_shared<ArrayValue>();
        arr->elements = coll.docs;
        root->entries[name] = Value{arr};
    }
    std::ostringstream os;
    jsonWrite(os, Value{root});
    std::ofstream out(inst.path, std::ios::trunc);
    if (out.is_open()) out << os.str();
}

void dbLoad(DbInstance& inst) {
    std::ifstream in(inst.path);
    if (!in.is_open()) return;
    std::stringstream ss;
    ss << in.rdbuf();
    // Note: JsonParser keeps a reference to this string, so it must outlive the parse.
    std::string content = ss.str();
    JsonParser parser(content);
    Value root = parser.parse();
    if (!root.isMap()) return;
    auto m = std::get<std::shared_ptr<MapValue>>(root.val);
    for (auto& [name, val] : m->entries) {
        DbCollection coll;
        if (val.isArray()) {
            auto arr = std::get<std::shared_ptr<ArrayValue>>(val.val);
            coll.docs = arr->elements;
            long long maxId = 0;
            for (auto& d : coll.docs) {
                long long id = docId(d);
                if (id > maxId) maxId = id;
            }
            coll.nextId = maxId + 1;
        }
        inst.collections[name] = std::move(coll);
    }
}

} // namespace

static Value builtinDbOpen(const std::vector<Value>& args) {
    if (args.empty() || !args[0].isString()) return Value{-1LL};
    auto inst = std::make_shared<DbInstance>();
    inst->path = std::get<std::string>(args[0].val);
    dbLoad(*inst);
    databases().push_back(inst);
    return Value{static_cast<long long>(databases().size())}; // handle = 1-based index
}

static Value builtinDbInsert(const std::vector<Value>& args) {
    if (args.size() < 3 || !args[1].isString() || !args[2].isMap()) return Value{std::monostate{}};
    DbInstance* db = getDb(args[0]);
    if (!db) return Value{std::monostate{}};
    auto& coll = db->collections[std::get<std::string>(args[1].val)];
    Value stored = cloneDoc(args[2]);
    auto doc = std::get<std::shared_ptr<MapValue>>(stored.val);
    long long id = coll.nextId++;
    doc->entries["_id"] = Value{id};
    coll.docs.push_back(stored);
    dbSave(*db);
    return cloneDoc(stored); // caller gets an independent snapshot with _id
}

static Value builtinDbAll(const std::vector<Value>& args) {
    auto out = std::make_shared<ArrayValue>();
    if (args.size() < 2 || !args[1].isString()) return Value{out};
    DbInstance* db = getDb(args[0]);
    if (!db) return Value{out};
    auto it = db->collections.find(std::get<std::string>(args[1].val));
    if (it != db->collections.end()) {
        for (const auto& doc : it->second.docs) out->elements.push_back(cloneDoc(doc));
    }
    return Value{out};
}

static Value builtinDbFind(const std::vector<Value>& args) {
    auto out = std::make_shared<ArrayValue>();
    if (args.size() < 4 || !args[1].isString() || !args[2].isString()) return Value{out};
    DbInstance* db = getDb(args[0]);
    if (!db) return Value{out};
    auto it = db->collections.find(std::get<std::string>(args[1].val));
    if (it == db->collections.end()) return Value{out};
    const std::string& field = std::get<std::string>(args[2].val);
    for (const auto& doc : it->second.docs) {
        if (!doc.isMap()) continue;
        auto m = std::get<std::shared_ptr<MapValue>>(doc.val);
        auto fit = m->entries.find(field);
        if (fit != m->entries.end() && dbValuesEqual(fit->second, args[3])) {
            out->elements.push_back(cloneDoc(doc));
        }
    }
    return Value{out};
}

static Value builtinDbGet(const std::vector<Value>& args) {
    if (args.size() < 3 || !args[1].isString() || !args[2].isInt()) return Value{std::monostate{}};
    DbInstance* db = getDb(args[0]);
    if (!db) return Value{std::monostate{}};
    auto it = db->collections.find(std::get<std::string>(args[1].val));
    if (it == db->collections.end()) return Value{std::monostate{}};
    long long id = std::get<long long>(args[2].val);
    for (const auto& doc : it->second.docs) {
        if (docId(doc) == id) return cloneDoc(doc);
    }
    return Value{std::monostate{}};
}

static Value builtinDbUpdate(const std::vector<Value>& args) {
    if (args.size() < 4 || !args[1].isString() || !args[2].isInt() || !args[3].isMap()) return Value{false};
    DbInstance* db = getDb(args[0]);
    if (!db) return Value{false};
    auto it = db->collections.find(std::get<std::string>(args[1].val));
    if (it == db->collections.end()) return Value{false};
    long long id = std::get<long long>(args[2].val);
    auto patch = std::get<std::shared_ptr<MapValue>>(args[3].val);
    for (auto& doc : it->second.docs) {
        if (docId(doc) == id && doc.isMap()) {
            auto m = std::get<std::shared_ptr<MapValue>>(doc.val);
            for (const auto& [k, v] : patch->entries) {
                if (k != "_id") m->entries[k] = v;
            }
            dbSave(*db);
            return Value{true};
        }
    }
    return Value{false};
}

static Value builtinDbRemove(const std::vector<Value>& args) {
    if (args.size() < 3 || !args[1].isString() || !args[2].isInt()) return Value{false};
    DbInstance* db = getDb(args[0]);
    if (!db) return Value{false};
    auto it = db->collections.find(std::get<std::string>(args[1].val));
    if (it == db->collections.end()) return Value{false};
    long long id = std::get<long long>(args[2].val);
    auto& docs = it->second.docs;
    for (size_t i = 0; i < docs.size(); ++i) {
        if (docId(docs[i]) == id) {
            docs.erase(docs.begin() + static_cast<long>(i));
            dbSave(*db);
            return Value{true};
        }
    }
    return Value{false};
}

static Value builtinDbCount(const std::vector<Value>& args) {
    if (args.size() < 2 || !args[1].isString()) return Value{0LL};
    DbInstance* db = getDb(args[0]);
    if (!db) return Value{0LL};
    auto it = db->collections.find(std::get<std::string>(args[1].val));
    if (it == db->collections.end()) return Value{0LL};
    return Value{static_cast<long long>(it->second.docs.size())};
}

static Value builtinWsUpgrade(const std::vector<Value>& args) {
    if (args.size() < 2 || !args[0].isInt() || !args[1].isMap()) return Value{false};
    long long socketId = std::get<long long>(args[0].val);
    auto headersMap = std::get<std::shared_ptr<MapValue>>(args[1].val);
    std::map<std::string, std::string> headers;
    for (const auto& [k, v] : headersMap->entries) {
        headers[k] = v.toString();
    }
    bool ok = net::wsUpgrade(static_cast<net::socket_t>(socketId), headers);
    return Value{ok};
}

static Value builtinWsSend(const std::vector<Value>& args) {
    if (args.size() < 2 || !args[0].isInt() || !args[1].isString()) return Value{false};
    long long socketId = std::get<long long>(args[0].val);
    std::string msg = std::get<std::string>(args[1].val);
    bool ok = net::wsSend(static_cast<net::socket_t>(socketId), msg);
    return Value{ok};
}

static Value builtinWsRecv(const std::vector<Value>& args) {
    if (args.empty() || !args[0].isInt()) return Value{std::monostate{}};
    long long socketId = std::get<long long>(args[0].val);
    bool closed = false;
    std::string msg = net::wsRecv(static_cast<net::socket_t>(socketId), closed);
    if (closed) return Value{std::monostate{}};
    return Value{msg};
}

static Value builtinSseUpgrade(const std::vector<Value>& args) {
    if (args.empty() || !args[0].isInt()) return Value{false};
    long long socketId = std::get<long long>(args[0].val);
    bool ok = net::sseUpgrade(static_cast<net::socket_t>(socketId));
    return Value{ok};
}

static Value builtinSseSend(const std::vector<Value>& args) {
    if (args.size() < 2 || !args[0].isInt() || !args[1].isString()) return Value{false};
    long long socketId = std::get<long long>(args[0].val);
    std::string data = std::get<std::string>(args[1].val);
    bool ok = net::sseSend(static_cast<net::socket_t>(socketId), data);
    return Value{ok};
}

static Value builtinSocketIsClosed(const std::vector<Value>& args) {
    if (args.empty() || !args[0].isInt()) return Value{true};
    long long socketId = std::get<long long>(args[0].val);
    bool closed = net::socketIsClosed(static_cast<net::socket_t>(socketId));
    return Value{closed};
}

static Value builtinSocketClose(const std::vector<Value>& args) {
    if (args.empty() || !args[0].isInt()) return Value{std::monostate{}};
    long long socketId = std::get<long long>(args[0].val);
    net::socketClose(static_cast<net::socket_t>(socketId));
    return Value{std::monostate{}};
}

Interpreter::Interpreter() {
    globals = std::make_shared<Environment>();
    environment = globals;

    // Define builtins
    globals->define("sollu", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"sollu", builtinSollu})});
    globals->define("len", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"len", builtinLen})});
    globals->define("range", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"range", builtinRange})});
    globals->define("type", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"type", builtinType})});
    globals->define("string", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"string", builtinString})});
    globals->define("int", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"int", builtinInt})});
    globals->define("float", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"float", builtinFloat})});
    globals->define("__native_read_line", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_read_line", builtinReadLine})});
    globals->define("__native_time_now", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_time_now", builtinTimeNow})});
    globals->define("__native_read_file", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_read_file", builtinReadFile})});
    globals->define("__native_write_file", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_write_file", builtinWriteFile})});
    globals->define("__native_file_exists", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_file_exists", builtinFileExists})});
    globals->define("__native_get_env", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_get_env", builtinGetEnv})});
    globals->define("__native_set_env", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_set_env", builtinSetEnv})});
    globals->define("__native_sys_command", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_sys_command", builtinSysCommand})});
    globals->define("__native_math_sqrt", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_math_sqrt", builtinMathSqrt})});
    globals->define("__native_math_sin", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_math_sin", builtinMathSin})});
    globals->define("__native_math_cos", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_math_cos", builtinMathCos})});
    globals->define("__native_math_ceil", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_math_ceil", builtinMathCeil})});
    globals->define("__native_math_floor", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_math_floor", builtinMathFloor})});
    globals->define("__native_string_split", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_string_split", builtinStringSplit})});
    globals->define("__native_string_replace", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_string_replace", builtinStringReplace})});

    // Type-conversion builtins that share names with type keywords.
    globals->define("bool", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"bool", builtinBool})});
    globals->define("char", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"char", builtinChar})});

    // Randomness
    globals->define("__native_random", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_random", builtinRandom})});
    globals->define("__native_random_int", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_random_int", builtinRandomInt})});
    globals->define("__native_random_seed", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_random_seed", builtinRandomSeed})});
    globals->define("__native_sha256", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_sha256", builtinSha256})});

    // OS / process
    globals->define("__native_os_name", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_os_name", builtinOsName})});
    globals->define("__native_os_cwd", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_os_cwd", builtinOsCwd})});
    globals->define("__native_os_exit", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_os_exit", builtinOsExit})});
    globals->define("__native_time_millis", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_time_millis", builtinTimeMillis})});
    globals->define("__native_sleep_ms", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_sleep_ms", builtinSleepMs})});

    // JSON
    globals->define("__native_json_stringify", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_json_stringify", builtinJsonStringify})});
    globals->define("__native_json_parse", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_json_parse", builtinJsonParse})});

    // HTTP client
    globals->define("__native_http_get", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_http_get", builtinHttpGet})});
    globals->define("__native_http_post", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_http_post", builtinHttpPost})});
    globals->define("__native_http_request", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_http_request", builtinHttpRequest})});
    globals->define("__native_ws_upgrade", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_ws_upgrade", builtinWsUpgrade})});
    globals->define("__native_ws_send", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_ws_send", builtinWsSend})});
    globals->define("__native_ws_recv", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_ws_recv", builtinWsRecv})});
    globals->define("__native_sse_upgrade", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_sse_upgrade", builtinSseUpgrade})});
    globals->define("__native_sse_send", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_sse_send", builtinSseSend})});
    globals->define("__native_socket_is_closed", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_socket_is_closed", builtinSocketIsClosed})});
    globals->define("__native_socket_close", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_socket_close", builtinSocketClose})});

    // Regex
    globals->define("__native_regex_match", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_regex_match", builtinRegexMatch})});
    globals->define("__native_regex_find_all", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_regex_find_all", builtinRegexFindAll})});
    globals->define("__native_regex_replace", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_regex_replace", builtinRegexReplace})});

    // Date / time
    globals->define("__native_date_format", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_date_format", builtinDateFormat})});
    globals->define("__native_date_now_seconds", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_date_now_seconds", builtinDateNowSeconds})});

    // Logging
    globals->define("__native_log", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_log", builtinLog})});

    // VijayDB embedded database
    globals->define("__native_db_open", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_db_open", builtinDbOpen})});
    globals->define("__native_db_insert", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_db_insert", builtinDbInsert})});
    globals->define("__native_db_all", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_db_all", builtinDbAll})});
    globals->define("__native_db_find", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_db_find", builtinDbFind})});
    globals->define("__native_db_get", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_db_get", builtinDbGet})});
    globals->define("__native_db_update", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_db_update", builtinDbUpdate})});
    globals->define("__native_db_remove", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_db_remove", builtinDbRemove})});
    globals->define("__native_db_count", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_db_count", builtinDbCount})});

    // HTTP server (context-aware: dispatches to a THALAPATHY handler function)
    {
        auto serveFn = [](Interpreter& interp, const std::vector<Value>& args) -> Value {
            if (args.size() < 2 || !args[0].isInt()) {
                std::cerr << "Vaathi, port number-um oru handler mersal-um kudu da nanba: serve(port, handler). 😤\n";
                return Value{false};
            }
            int port = static_cast<int>(std::get<long long>(args[0].val));
            Value handler = args[1];

            net::RouteHandler cb = [&interp, handler](const net::HttpRequest& req) -> net::HttpResponse {
                // Build request map for the handler.
                auto reqMap = std::make_shared<MapValue>();
                reqMap->entries["method"] = Value{req.method};
                reqMap->entries["path"] = Value{req.path};
                reqMap->entries["query"] = Value{req.query};
                reqMap->entries["body"] = Value{req.body};
                auto headerMap = std::make_shared<MapValue>();
                for (const auto& [k, v] : req.headers) headerMap->entries[k] = Value{v};
                reqMap->entries["headers"] = Value{headerMap};
                reqMap->entries["socketId"] = Value{req.socketId};

                net::HttpResponse resp;
                Value result = interp.callCallable(handler, {Value{reqMap}});

                if (result.isString()) {
                    resp.body = std::get<std::string>(result.val);
                    resp.contentType = "text/html";
                } else if (result.isMap()) {
                    auto m = std::get<std::shared_ptr<MapValue>>(result.val);
                    if (auto it = m->entries.find("status"); it != m->entries.end() && it->second.isInt())
                        resp.status = static_cast<int>(std::get<long long>(it->second.val));
                    if (auto it = m->entries.find("body"); it != m->entries.end())
                        resp.body = it->second.toString();
                    if (auto it = m->entries.find("contentType"); it != m->entries.end() && it->second.isString())
                        resp.contentType = std::get<std::string>(it->second.val);
                    else if (auto it = m->entries.find("type"); it != m->entries.end() && it->second.isString())
                        resp.contentType = std::get<std::string>(it->second.val);
                    // Optional custom response headers (Location for redirects,
                    // security headers, ETag, Cache-Control, ...): a `headers` map.
                    if (auto it = m->entries.find("headers"); it != m->entries.end() && it->second.isMap()) {
                        auto hm = std::get<std::shared_ptr<MapValue>>(it->second.val);
                        for (const auto& [hk, hv] : hm->entries) resp.headers[hk] = hv.toString();
                    }
                } else {
                    resp.body = result.toString();
                }
                return resp;
            };

            std::cout << "Naan dhaan da leader! Sarkar server port " << port
                      << "-la vantaachu — http://localhost:" << port << " 🔥\n";
            bool ok = net::httpServe(port, cb);
            if (!ok) {
                std::cerr << "Port " << port << " already yaaro pudichuttaanga da nanba. Vaera port try pannu. 😤\n";
            }
            return Value{ok};
        };
        auto serveBuiltin = std::make_shared<BuiltInFunctionValue>();
        serveBuiltin->name = "__native_http_serve";
        serveBuiltin->ctxFn = serveFn;
        globals->define("__native_http_serve", Value{serveBuiltin});
    }
}

void Interpreter::interpret(const std::vector<std::unique_ptr<ASTNode>>& nodes) {
    try {
        // Register enums and classes first
        for (const auto& node : nodes) {
            if (auto enumDecl = dynamic_cast<EnumDecl*>(node.get())) {
                execute(enumDecl);
            }
        }
        for (const auto& node : nodes) {
            if (auto classDecl = dynamic_cast<ClassDecl*>(node.get())) {
                execute(classDecl);
            }
        }

        // Run non-class and non-entry declarations (e.g. global vars/funcs)
        for (const auto& node : nodes) {
            if (!dynamic_cast<ClassDecl*>(node.get()) && !dynamic_cast<EntryBlockDecl*>(node.get())
                && !dynamic_cast<EnumDecl*>(node.get())) {
                execute(node.get());
            }
        }

        // Locate and run the single entry block thalapathy aarambam
        for (const auto& node : nodes) {
            if (auto entry = dynamic_cast<EntryBlockDecl*>(node.get())) {
                execute(entry);
                break;
            }
        }
    } catch (const ReturnSignal&) {
        // Top-level return
    } catch (const ThrowSignal& s) {
        std::cerr << "Uncaught runtime exception: " << s.errorVal.toString() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Runtime error: " << e.what() << "\n";
    }
}

Value Interpreter::evaluate(ASTNode* expr) {
    expr->accept(this);
    return lastExprVal;
}

void Interpreter::execute(ASTNode* stmt) {
    stmt->accept(this);
}

void Interpreter::executeBlock(BlockStmt* block, std::shared_ptr<Environment> env) {
    auto previous = this->environment;
    try {
        this->environment = env;
        for (const auto& stmt : block->statements) {
            execute(stmt.get());
        }
        this->environment = previous;
    } catch (...) {
        this->environment = previous;
        throw; // rethrow signals
    }
}

// Visitor implementations
void Interpreter::visit(VarDecl* node) {
    Value val = Value{std::monostate{}};
    if (node->initializer) {
        val = evaluate(node->initializer.get());
    }
    environment->define(node->name, val);
}

void Interpreter::bindPattern(bool isObjectPattern, const std::vector<NanbiBinding>& bindings, const Value& source) {
    if (!isObjectPattern) {
        if (!source.isArray()) {
            throw std::runtime_error("[THALA-NANBI-003] array binding pattern requires a destructurable list; received a non-list value");
        }
        auto arr = std::get<std::shared_ptr<ArrayValue>>(source.val);
        size_t idx = 0;
        for (const auto& b : bindings) {
            if (b.kind == NanbiBinding::Kind::Rest) {
                auto rest = std::make_shared<ArrayValue>();
                for (size_t k = idx; k < arr->elements.size(); ++k) rest->elements.push_back(arr->elements[k]);
                idx = arr->elements.size();
                environment->define(b.name, Value{rest});
                continue;
            }
            if (idx >= arr->elements.size()) {
                throw std::runtime_error("[THALA-NANBI-002] binding pattern expects more values than the source provides");
            }
            Value elem = arr->elements[idx];
            if (b.kind == NanbiBinding::Kind::Name) environment->define(b.name, elem);
            // Ignore (`_`) consumes a slot but binds nothing.
            idx++;
        }
    } else {
        if (!source.isMap()) {
            throw std::runtime_error("[THALA-NANBI-003] object binding pattern requires a map/object source");
        }
        auto map = std::get<std::shared_ptr<MapValue>>(source.val);
        for (const auto& b : bindings) {
            auto it = map->entries.find(b.key);
            Value v = (it != map->entries.end()) ? it->second : Value{std::monostate{}};
            environment->define(b.name, v);
        }
    }
}

void Interpreter::visit(NanbiDecl* node) {
    // Evaluate the source EXACTLY ONCE, then bind the pattern against it.
    Value source = Value{std::monostate{}};
    if (node->initializer) source = evaluate(node->initializer.get());
    bindPattern(node->isObjectPattern, node->bindings, source);
}

// Equality used by `yaaru` pattern matching (numeric/string/bool/null).
static bool matchEquals(const Value& a, const Value& b) {
    if (a.isNull() || b.isNull()) return a.isNull() && b.isNull();
    if ((a.isInt() || a.isFloat()) && (b.isInt() || b.isFloat())) {
        double x = a.isFloat() ? std::get<double>(a.val) : static_cast<double>(std::get<long long>(a.val));
        double y = b.isFloat() ? std::get<double>(b.val) : static_cast<double>(std::get<long long>(b.val));
        return x == y;
    }
    if (a.isString() && b.isString()) return std::get<std::string>(a.val) == std::get<std::string>(b.val);
    if (a.isBool() && b.isBool()) return std::get<bool>(a.val) == std::get<bool>(b.val);
    return false;
}

void Interpreter::visit(MatchStmt* node) {
    // Subject evaluated exactly once; first matching `ivan` arm runs, else `yaarumilla`.
    Value subject = evaluate(node->subject.get());
    for (const auto& arm : node->arms) {
        Value pat = evaluate(arm.pattern.get());
        if (matchEquals(subject, pat)) {
            execute(arm.body.get());
            return;
        }
    }
    if (node->defaultBody) execute(node->defaultBody.get());
}

void Interpreter::visit(FuncDecl* node) {
    auto func = std::make_shared<FunctionValue>();
    func->decl = node;
    func->closure = environment;
    environment->define(node->name, Value{func});
}

void Interpreter::visit(ClassDecl* node) {
    auto klass = std::make_shared<ClassValue>();
    klass->decl = node;

    // Find parent class value
    if (!node->parentName.empty()) {
        bool foundParent = false;
        Value pv = environment->get(node->parentName, foundParent);
        if (foundParent && pv.isClass()) {
            klass->parentClass = std::get<std::shared_ptr<ClassValue>>(pv.val);
        }
    }

    // Populate implemented interfaces list
    klass->implementedInterfaces = node->implementedInterfaces;

    // Populate static fields and initialize them
    // First, inherit parent class static fields
    if (klass->parentClass) {
        klass->staticFields = klass->parentClass->staticFields;
    }
    for (const auto& field : node->fields) {
        if (field->isStatic) {
            Value initVal = Value{std::monostate{}};
            if (field->varDecl->initializer) {
                initVal = evaluate(field->varDecl->initializer.get());
            }
            klass->staticFields[field->varDecl->name] = initVal;
        }
    }

    // Define class in environment
    environment->define(node->name, Value{klass});
}

void Interpreter::visit(EntryBlockDecl* node) {
    execute(node->body.get());
}

void Interpreter::visit(BlockStmt* node) {
    executeBlock(node, std::make_shared<Environment>(environment));
}

void Interpreter::visit(ExprStmt* node) {
    evaluate(node->expression.get());
}

void Interpreter::visit(IfStmt* node) {
    Value cond = evaluate(node->condition.get());
    bool isTrue = cond.isBool() ? std::get<bool>(cond.val) : !cond.isNull();
    if (isTrue) {
        execute(node->thenBranch.get());
    } else if (node->elseBranch) {
        execute(node->elseBranch.get());
    }
}

void Interpreter::visit(ForStmt* node) {
    auto loopEnv = std::make_shared<Environment>(environment);
    auto previous = this->environment;
    this->environment = loopEnv;

    try {
        if (node->initializer) execute(node->initializer.get());
        while (true) {
            if (node->condition) {
                Value cond = evaluate(node->condition.get());
                bool isTrue = cond.isBool() ? std::get<bool>(cond.val) : !cond.isNull();
                if (!isTrue) break;
            }

            try {
                execute(node->body.get());
            } catch (const BreakSignal&) {
                break;
            } catch (const ContinueSignal&) {
                // proceed to update
            }

            if (node->update) evaluate(node->update.get());
        }
    } catch (...) {
        this->environment = previous;
        throw;
    }
    this->environment = previous;
}

void Interpreter::visit(RangeLoopStmt* node) {
    Value startVal = evaluate(node->startExpr.get());
    Value endVal = evaluate(node->endExpr.get());

    long long start = startVal.isInt() ? std::get<long long>(startVal.val) : 0LL;
    long long end = endVal.isInt() ? std::get<long long>(endVal.val) : 0LL;

    auto loopEnv = std::make_shared<Environment>(environment);
    auto previous = this->environment;
    this->environment = loopEnv;

    try {
        for (long long i = start; i < end; ++i) {
            loopEnv->define(node->varName, Value{i});
            try {
                execute(node->body.get());
            } catch (const BreakSignal&) {
                break;
            } catch (const ContinueSignal&) {
                // proceed
            }
        }
    } catch (...) {
        this->environment = previous;
        throw;
    }
    this->environment = previous;
}

void Interpreter::visit(ForEachStmt* node) {
    Value iter = evaluate(node->iterable.get());

    // Build the sequence of values to iterate over.
    std::vector<Value> items;
    if (iter.isArray()) {
        items = std::get<std::shared_ptr<ArrayValue>>(iter.val)->elements;
    } else if (iter.isString()) {
        for (char c : std::get<std::string>(iter.val)) items.push_back(Value{std::string(1, c)});
    } else if (iter.isMap()) {
        for (const auto& [k, v] : std::get<std::shared_ptr<MapValue>>(iter.val)->entries) {
            (void)v; items.push_back(Value{k}); // iterate keys
        }
    } else {
        throw std::runtime_error("vaathi ... in: value is not iterable (need array, string or map)");
    }

    auto loopEnv = std::make_shared<Environment>(environment);
    auto previous = this->environment;
    this->environment = loopEnv;
    try {
        for (const auto& item : items) {
            if (node->hasPattern) {
                bindPattern(node->patternIsObject, node->patternBindings, item);
            } else {
                loopEnv->define(node->varName, item);
            }
            try {
                execute(node->body.get());
            } catch (const BreakSignal&) {
                break;
            } catch (const ContinueSignal&) {
                // next
            }
        }
    } catch (...) {
        this->environment = previous;
        throw;
    }
    this->environment = previous;
}

void Interpreter::visit(WhileStmt* node) {
    while (true) {
        Value cond = evaluate(node->condition.get());
        bool isTrue = cond.isBool() ? std::get<bool>(cond.val) : !cond.isNull();
        if (!isTrue) break;

        try {
            execute(node->body.get());
        } catch (const BreakSignal&) {
            break;
        } catch (const ContinueSignal&) {
            // proceed to next iteration
        }
    }
}

void Interpreter::visit(SwitchStmt* node) {
    Value subject = evaluate(node->subject.get());

    auto valuesEqual = [](const Value& a, const Value& b) -> bool {
        if (a.isInt() && b.isInt()) return std::get<long long>(a.val) == std::get<long long>(b.val);
        if (a.isFloat() && b.isFloat()) return std::get<double>(a.val) == std::get<double>(b.val);
        if (a.isBool() && b.isBool()) return std::get<bool>(a.val) == std::get<bool>(b.val);
        if (a.isString() && b.isString()) return std::get<std::string>(a.val) == std::get<std::string>(b.val);
        if (a.isChar() && b.isChar()) return std::get<char>(a.val) == std::get<char>(b.val);
        if (a.isNull() && b.isNull()) return true;
        if (a.isEnum() && b.isEnum()) {
            auto ea = std::get<std::shared_ptr<EnumValue>>(a.val);
            auto eb = std::get<std::shared_ptr<EnumValue>>(b.val);
            return ea->typeName == eb->typeName && ea->name == eb->name;
        }
        return false;
    };

    try {
        for (const auto& c : node->cases) {
            Value caseVal = evaluate(c.value.get());
            if (valuesEqual(subject, caseVal)) {
                execute(c.body.get());
                return;
            }
        }
        if (node->defaultBody) {
            execute(node->defaultBody.get());
        }
    } catch (const BreakSignal&) {
        // break exits the switch
    }
}

void Interpreter::visit(ReturnStmt* node) {
    Value val = Value{std::monostate{}};
    if (node->expression) {
        val = evaluate(node->expression.get());
    }
    throw ReturnSignal{val};
}

void Interpreter::visit(ThrowStmt* node) {
    Value errorVal = evaluate(node->expression.get());
    throw ThrowSignal{errorVal};
}

void Interpreter::visit(TryCatchStmt* node) {
    // Does a thrown value match the catch's optional type filter?
    auto matchesCatchType = [&](const Value& err) -> bool {
        if (node->catchTypeName.empty()) return true; // catch-all
        if (err.isInstance()) {
            auto currKlass = std::get<std::shared_ptr<InstanceValue>>(err.val)->klass;
            while (currKlass) {
                if (currKlass->decl->name == node->catchTypeName) return true;
                for (const auto& iface : currKlass->implementedInterfaces) {
                    if (iface == node->catchTypeName) return true;
                }
                currKlass = currKlass->parentClass;
            }
            return false;
        }
        if (err.isEnum()) return std::get<std::shared_ptr<EnumValue>>(err.val)->typeName == node->catchTypeName;
        // Primitive type filters.
        const std::string& t = node->catchTypeName;
        if (t == "int") return err.isInt();
        if (t == "float") return err.isFloat();
        if (t == "string") return err.isString();
        if (t == "bool") return err.isBool();
        return false;
    };

    auto runFinally = [&]() {
        if (node->finallyBody) {
            execute(node->finallyBody.get());
        }
    };

    try {
        try {
            execute(node->tryBody.get());
        } catch (const ThrowSignal& s) {
            if (node->catchBody && matchesCatchType(s.errorVal)) {
                auto catchEnv = std::make_shared<Environment>(environment);
                catchEnv->define(node->catchVarName, s.errorVal);
                executeBlock(dynamic_cast<BlockStmt*>(node->catchBody.get()), catchEnv);
            } else {
                throw; // unmatched type or finally-only: propagate after finally
            }
        }
    } catch (...) {
        // Ensure `kadaisi` runs on any propagating signal (throw/return/break).
        runFinally();
        throw;
    }
    runFinally();
}

void Interpreter::visit(BreakStmt*) {
    throw BreakSignal{};
}

void Interpreter::visit(ContinueStmt*) {
    throw ContinueSignal{};
}

void Interpreter::visit(LiteralExpr* node) {
    std::visit([this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            lastExprVal = Value{std::monostate{}};
        } else {
            lastExprVal = Value{arg};
        }
    }, node->value);
}

void Interpreter::visit(IdentifierExpr* node) {
    bool found = false;
    lastExprVal = environment->get(node->name, found);
    if (!found) {
        throw std::runtime_error("Undefined variable: " + node->name);
    }
}

void Interpreter::visit(BinaryExpr* node) {
    Value left = evaluate(node->left.get());

    // THALA-LOGIC-001 fix: `&&` / `||` short-circuit — the right operand is
    // evaluated ONLY when the left does not already decide the result. This
    // makes the idiom `i >= 0 && arr[i] > x` safe.
    if (node->op == TokenType::AMP_AMP || node->op == TokenType::BAR_BAR) {
        bool leftBool = std::get<bool>(left.val);
        if (node->op == TokenType::AMP_AMP && !leftBool) { lastExprVal = Value{false}; return; }
        if (node->op == TokenType::BAR_BAR && leftBool)  { lastExprVal = Value{true};  return; }
        Value right = evaluate(node->right.get());
        lastExprVal = Value{std::get<bool>(right.val)};
        return;
    }

    Value right = evaluate(node->right.get());

    if (left.isInstance()) {
        auto inst = std::get<std::shared_ptr<InstanceValue>>(left.val);
        auto currKlass = inst->klass;
        OperatorDecl* matchedOp = nullptr;
        while (currKlass) {
            for (const auto& op : currKlass->decl->operators) {
                if (op->op == node->op) {
                    matchedOp = op.get();
                    break;
                }
            }
            if (matchedOp) break;
            currKlass = currKlass->parentClass;
        }

        if (matchedOp) {
            auto opEnv = std::make_shared<Environment>(globals);
            opEnv->define("this", left);
            opEnv->define(matchedOp->param.name, right);
            
            auto prevEnv = this->environment;
            this->environment = opEnv;
            try {
                executeBlock(dynamic_cast<BlockStmt*>(matchedOp->body.get()), opEnv);
                lastExprVal = Value{std::monostate{}};
            } catch (const ReturnSignal& ret) {
                lastExprVal = ret.value;
            }
            this->environment = prevEnv;
            return;
        }
    }

    switch (node->op) {
        case TokenType::PLUS:
            if (left.isInt() && right.isInt()) {
                lastExprVal = Value{std::get<long long>(left.val) + std::get<long long>(right.val)};
            } else if (left.isFloat() && right.isFloat()) {
                lastExprVal = Value{std::get<double>(left.val) + std::get<double>(right.val)};
            } else if (left.isString() && right.isString()) {
                lastExprVal = Value{std::get<std::string>(left.val) + std::get<std::string>(right.val)};
            } else {
                throw std::runtime_error("Invalid operands for +");
            }
            break;
        case TokenType::MINUS:
            if (left.isInt() && right.isInt()) {
                lastExprVal = Value{std::get<long long>(left.val) - std::get<long long>(right.val)};
            } else if (left.isFloat() && right.isFloat()) {
                lastExprVal = Value{std::get<double>(left.val) - std::get<double>(right.val)};
            } else {
                throw std::runtime_error("Invalid operands for -");
            }
            break;
        case TokenType::STAR:
            if (left.isInt() && right.isInt()) {
                lastExprVal = Value{std::get<long long>(left.val) * std::get<long long>(right.val)};
            } else if (left.isFloat() && right.isFloat()) {
                lastExprVal = Value{std::get<double>(left.val) * std::get<double>(right.val)};
            } else {
                throw std::runtime_error("Invalid operands for *");
            }
            break;
        case TokenType::SLASH:
            if (left.isInt() && right.isInt()) {
                long long divisor = std::get<long long>(right.val);
                if (divisor == 0) throw std::runtime_error("division by zero");
                lastExprVal = Value{std::get<long long>(left.val) / divisor};
            } else if (left.isFloat() && right.isFloat()) {
                double divisor = std::get<double>(right.val);
                if (divisor == 0.0) throw std::runtime_error("division by zero");
                lastExprVal = Value{std::get<double>(left.val) / divisor};
            } else {
                throw std::runtime_error("Invalid operands for /");
            }
            break;
        case TokenType::PERCENT:
            if (left.isInt() && right.isInt()) {
                long long divisor = std::get<long long>(right.val);
                if (divisor == 0) throw std::runtime_error("division by zero");
                lastExprVal = Value{std::get<long long>(left.val) % divisor};
            } else {
                throw std::runtime_error("Invalid operands for %");
            }
            break;

        // Comparisons
        case TokenType::EQ_EQ:
            if (left.isNull() && right.isNull()) lastExprVal = Value{true};
            else if (left.isInt() && right.isInt()) lastExprVal = Value{std::get<long long>(left.val) == std::get<long long>(right.val)};
            else if (left.isFloat() && right.isFloat()) lastExprVal = Value{std::get<double>(left.val) == std::get<double>(right.val)};
            else if (left.isBool() && right.isBool()) lastExprVal = Value{std::get<bool>(left.val) == std::get<bool>(right.val)};
            else if (left.isString() && right.isString()) lastExprVal = Value{std::get<std::string>(left.val) == std::get<std::string>(right.val)};
            else if (left.isInstance() && right.isInstance()) lastExprVal = Value{std::get<std::shared_ptr<InstanceValue>>(left.val) == std::get<std::shared_ptr<InstanceValue>>(right.val)};
            else if (left.isEnum() && right.isEnum()) {
                auto a = std::get<std::shared_ptr<EnumValue>>(left.val);
                auto b = std::get<std::shared_ptr<EnumValue>>(right.val);
                lastExprVal = Value{a->typeName == b->typeName && a->name == b->name};
            }
            else lastExprVal = Value{false};
            break;
        case TokenType::BANG_EQ:
            if (left.isNull() && right.isNull()) lastExprVal = Value{false};
            else if (left.isInt() && right.isInt()) lastExprVal = Value{std::get<long long>(left.val) != std::get<long long>(right.val)};
            else if (left.isFloat() && right.isFloat()) lastExprVal = Value{std::get<double>(left.val) != std::get<double>(right.val)};
            else if (left.isBool() && right.isBool()) lastExprVal = Value{std::get<bool>(left.val) != std::get<bool>(right.val)};
            else if (left.isString() && right.isString()) lastExprVal = Value{std::get<std::string>(left.val) != std::get<std::string>(right.val)};
            else if (left.isInstance() && right.isInstance()) lastExprVal = Value{std::get<std::shared_ptr<InstanceValue>>(left.val) != std::get<std::shared_ptr<InstanceValue>>(right.val)};
            else if (left.isEnum() && right.isEnum()) {
                auto a = std::get<std::shared_ptr<EnumValue>>(left.val);
                auto b = std::get<std::shared_ptr<EnumValue>>(right.val);
                lastExprVal = Value{!(a->typeName == b->typeName && a->name == b->name)};
            }
            else lastExprVal = Value{true};
            break;
        case TokenType::LT:
            if (left.isInt() && right.isInt()) lastExprVal = Value{std::get<long long>(left.val) < std::get<long long>(right.val)};
            else if (left.isFloat() && right.isFloat()) lastExprVal = Value{std::get<double>(left.val) < std::get<double>(right.val)};
            else throw std::runtime_error("Invalid comparison");
            break;
        case TokenType::GT:
            if (left.isInt() && right.isInt()) lastExprVal = Value{std::get<long long>(left.val) > std::get<long long>(right.val)};
            else if (left.isFloat() && right.isFloat()) lastExprVal = Value{std::get<double>(left.val) > std::get<double>(right.val)};
            else throw std::runtime_error("Invalid comparison");
            break;
        case TokenType::LT_EQ:
            if (left.isInt() && right.isInt()) lastExprVal = Value{std::get<long long>(left.val) <= std::get<long long>(right.val)};
            else if (left.isFloat() && right.isFloat()) lastExprVal = Value{std::get<double>(left.val) <= std::get<double>(right.val)};
            else throw std::runtime_error("Invalid comparison");
            break;
        case TokenType::GT_EQ:
            if (left.isInt() && right.isInt()) lastExprVal = Value{std::get<long long>(left.val) >= std::get<long long>(right.val)};
            else if (left.isFloat() && right.isFloat()) lastExprVal = Value{std::get<double>(left.val) >= std::get<double>(right.val)};
            else throw std::runtime_error("Invalid comparison");
            break;

        // Logical
        case TokenType::AMP_AMP:
            lastExprVal = Value{std::get<bool>(left.val) && std::get<bool>(right.val)};
            break;
        case TokenType::BAR_BAR:
            lastExprVal = Value{std::get<bool>(left.val) || std::get<bool>(right.val)};
            break;
        default:
            throw std::runtime_error("Unsupported binary operator");
    }
}

void Interpreter::visit(UnaryExpr* node) {
    if (node->op == TokenType::KAATHIRU) {
        throw std::runtime_error("error[THALA-ASYNC-001]: coroutine runtime required for 'kaathiru' (await) suspension. Staged with design.");
    }
    Value operand = evaluate(node->operand.get());
    if (node->op == TokenType::BANG) {
        lastExprVal = Value{!std::get<bool>(operand.val)};
    } else if (node->op == TokenType::MINUS) {
        if (operand.isInt()) {
            lastExprVal = Value{-std::get<long long>(operand.val)};
        } else if (operand.isFloat()) {
            lastExprVal = Value{-std::get<double>(operand.val)};
        }
    } else if (node->op == TokenType::PLUS_PLUS) {
        // Prefix/postfix ++
        if (operand.isInt()) {
            lastExprVal = Value{std::get<long long>(operand.val) + 1};
            if (auto ident = dynamic_cast<IdentifierExpr*>(node->operand.get())) {
                environment->assign(ident->name, lastExprVal);
            }
        }
    } else if (node->op == TokenType::MINUS_MINUS) {
        if (operand.isInt()) {
            lastExprVal = Value{std::get<long long>(operand.val) - 1};
            if (auto ident = dynamic_cast<IdentifierExpr*>(node->operand.get())) {
                environment->assign(ident->name, lastExprVal);
            }
        }
    }
}

void Interpreter::visit(TernaryExpr* node) {
    Value cond = evaluate(node->condition.get());
    bool isTrue = cond.isBool() ? std::get<bool>(cond.val) : !cond.isNull();
    lastExprVal = isTrue ? evaluate(node->thenExpr.get()) : evaluate(node->elseExpr.get());
}

void Interpreter::visit(AssignExpr* node) {
    Value val = evaluate(node->value.get());

    if (auto ident = dynamic_cast<IdentifierExpr*>(node->target.get())) {
        bool assigned = false;
        if (node->op == TokenType::EQUAL) {
            assigned = environment->assign(ident->name, val);
        } else {
            bool found = false;
            Value currentVal = environment->get(ident->name, found);
            if (found) {
                if (node->op == TokenType::PLUS_EQ) {
                    if (currentVal.isInt() && val.isInt()) {
                        val = Value{std::get<long long>(currentVal.val) + std::get<long long>(val.val)};
                    } else if (currentVal.isString() && val.isString()) {
                        val = Value{std::get<std::string>(currentVal.val) + std::get<std::string>(val.val)};
                    }
                } else if (node->op == TokenType::MINUS_EQ) {
                    val = Value{std::get<long long>(currentVal.val) - std::get<long long>(val.val)};
                } else if (node->op == TokenType::STAR_EQ) {
                    val = Value{std::get<long long>(currentVal.val) * std::get<long long>(val.val)};
                } else if (node->op == TokenType::SLASH_EQ) {
                    val = Value{std::get<long long>(currentVal.val) / std::get<long long>(val.val)};
                }
                assigned = environment->assign(ident->name, val);
            }
        }
        if (!assigned) {
            throw std::runtime_error("Failed to assign to " + ident->name);
        }
        lastExprVal = val;
    } else if (auto indexExpr = dynamic_cast<IndexExpr*>(node->target.get())) {
        Value obj = evaluate(indexExpr->object.get());
        Value idx = evaluate(indexExpr->index.get());

        if (obj.isArray() && idx.isInt()) {
            auto arr = std::get<std::shared_ptr<ArrayValue>>(obj.val);
            long long i = std::get<long long>(idx.val);
            if (i < 0 || i >= static_cast<long long>(arr->elements.size())) {
                throw std::runtime_error("array index out of bounds");
            }
            arr->elements[static_cast<size_t>(i)] = val;
            lastExprVal = val;
        } else if (obj.isMap() && idx.isString()) {
            auto map = std::get<std::shared_ptr<MapValue>>(obj.val);
            map->entries[std::get<std::string>(idx.val)] = val;
            lastExprVal = val;
        }
    } else if (auto memberExpr = dynamic_cast<MemberExpr*>(node->target.get())) {
        Value obj = evaluate(memberExpr->object.get());
        if (obj.isInstance()) {
            auto inst = std::get<std::shared_ptr<InstanceValue>>(obj.val);
            
            // Check for property setter
            auto currKlass = inst->klass;
            bool setterFound = false;
            while (currKlass && !setterFound) {
                for (const auto& prop : currKlass->decl->properties) {
                    if (prop->name == memberExpr->memberName && prop->setterBody) {
                        auto setterEnv = std::make_shared<Environment>(globals);
                        setterEnv->define("this", obj);
                        std::string paramName = prop->setterParamName.empty() ? "value" : prop->setterParamName;
                        setterEnv->define(paramName, val);
                        auto prevEnv = this->environment;
                        this->environment = setterEnv;
                        try {
                            executeBlock(dynamic_cast<BlockStmt*>(prop->setterBody.get()), setterEnv);
                        } catch (const ReturnSignal&) {}
                        this->environment = prevEnv;
                        setterFound = true;
                        break;
                    }
                }
                currKlass = currKlass->parentClass;
            }
            
            if (!setterFound) {
                inst->fields[memberExpr->memberName] = val;
            }
            lastExprVal = val;
        } else if (obj.isClass()) {
            // Static field assignment
            auto klass = std::get<std::shared_ptr<ClassValue>>(obj.val);
            klass->staticFields[memberExpr->memberName] = val;
            lastExprVal = val;
        } else if (obj.isMap()) {
            auto map = std::get<std::shared_ptr<MapValue>>(obj.val);
            map->entries[memberExpr->memberName] = val;
            lastExprVal = val;
        }
    }
}

Value Interpreter::callCallable(const Value& callee, const std::vector<Value>& args) {
    if (callee.isBuiltIn()) {
        auto b = std::get<std::shared_ptr<BuiltInFunctionValue>>(callee.val);
        if (b->ctxFn) return b->ctxFn(*this, args);
        return b->fn(args);
    }
    if (callee.isFunction()) {
        auto f = std::get<std::shared_ptr<FunctionValue>>(callee.val);
        auto callEnv = std::make_shared<Environment>(f->closure);
        // Bind parameters, applying default values for any omitted trailing args.
        auto prevEnv = this->environment;
        this->environment = callEnv;
        for (size_t i = 0; i < f->decl->params.size(); ++i) {
            if (i < args.size()) {
                callEnv->define(f->decl->params[i].name, args[i]);
            } else if (f->decl->params[i].defaultValue) {
                callEnv->define(f->decl->params[i].name, evaluate(f->decl->params[i].defaultValue.get()));
            } else {
                callEnv->define(f->decl->params[i].name, Value{std::monostate{}});
            }
        }
        this->environment = prevEnv;
        try {
            executeBlock(dynamic_cast<BlockStmt*>(f->decl->body.get()), callEnv);
            return Value{std::monostate{}};
        } catch (const ReturnSignal& ret) {
            return ret.value;
        }
    }
    throw std::runtime_error("value is not callable");
}

bool Interpreter::tryInvokeMethod(const Value& obj, const std::string& name,
                                  const std::vector<Value>& args, Value& out) {
    // ---- Enum member methods ----
    if (obj.isEnum()) {
        auto ev = std::get<std::shared_ptr<EnumValue>>(obj.val);
        if (name == "name") { out = Value{ev->name}; return true; }
        if (name == "ordinal") { out = Value{ev->ordinal}; return true; }
        if (name == "type") { out = Value{ev->typeName}; return true; }
        return false;
    }

    // ---- Array methods ----
    if (obj.isArray()) {
        auto arr = std::get<std::shared_ptr<ArrayValue>>(obj.val);
        if (name == "push" || name == "add") {
            for (const auto& a : args) arr->elements.push_back(a);
            out = Value{static_cast<long long>(arr->elements.size())};
            return true;
        }
        if (name == "pop") {
            if (arr->elements.empty()) { out = Value{std::monostate{}}; }
            else { out = arr->elements.back(); arr->elements.pop_back(); }
            return true;
        }
        if (name == "size" || name == "length") {
            out = Value{static_cast<long long>(arr->elements.size())};
            return true;
        }
        if (name == "isEmpty") {
            out = Value{arr->elements.empty()};
            return true;
        }
        if (name == "reverse") {
            std::reverse(arr->elements.begin(), arr->elements.end());
            out = obj;
            return true;
        }
        if (name == "get" && args.size() == 1 && args[0].isInt()) {
            long long i = std::get<long long>(args[0].val);
            if (i < 0 || i >= static_cast<long long>(arr->elements.size())) { out = Value{std::monostate{}}; }
            else { out = arr->elements[static_cast<size_t>(i)]; }
            return true;
        }
        if (name == "contains" && args.size() == 1) {
            out = Value{false};
            for (const auto& e : arr->elements) {
                if (e.toString() == args[0].toString()) { out = Value{true}; break; }
            }
            return true;
        }
        if (name == "indexOf" && args.size() == 1) {
            out = Value{-1LL};
            for (size_t i = 0; i < arr->elements.size(); ++i) {
                if (arr->elements[i].toString() == args[0].toString()) { out = Value{static_cast<long long>(i)}; break; }
            }
            return true;
        }
        if (name == "join") {
            std::string sep = (args.size() == 1 && args[0].isString()) ? std::get<std::string>(args[0].val) : "";
            std::string result;
            for (size_t i = 0; i < arr->elements.size(); ++i) {
                if (i) result += sep;
                result += arr->elements[i].toString();
            }
            out = Value{result};
            return true;
        }
        if (name == "map" && args.size() == 1) {
            auto result = std::make_shared<ArrayValue>();
            for (const auto& e : arr->elements) {
                result->elements.push_back(callCallable(args[0], {e}));
            }
            out = Value{result};
            return true;
        }
        if (name == "filter" && args.size() == 1) {
            auto result = std::make_shared<ArrayValue>();
            for (const auto& e : arr->elements) {
                Value keep = callCallable(args[0], {e});
                if (keep.isBool() ? std::get<bool>(keep.val) : !keep.isNull()) {
                    result->elements.push_back(e);
                }
            }
            out = Value{result};
            return true;
        }
        if (name == "forEach" && args.size() == 1) {
            for (const auto& e : arr->elements) callCallable(args[0], {e});
            out = Value{std::monostate{}};
            return true;
        }
        if (name == "reduce" && args.size() == 2) {
            Value acc = args[1];
            for (const auto& e : arr->elements) acc = callCallable(args[0], {acc, e});
            out = acc;
            return true;
        }
        return false;
    }

    // ---- String methods ----
    if (obj.isString()) {
        const std::string& s = std::get<std::string>(obj.val);
        if (name == "length" || name == "size") { out = Value{static_cast<long long>(s.size())}; return true; }
        if (name == "isEmpty") { out = Value{s.empty()}; return true; }
        if (name == "toUpper") {
            std::string r = s; for (auto& c : r) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
            out = Value{r}; return true;
        }
        if (name == "toLower") {
            std::string r = s; for (auto& c : r) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            out = Value{r}; return true;
        }
        if (name == "trim") {
            size_t b = s.find_first_not_of(" \t\r\n");
            size_t e = s.find_last_not_of(" \t\r\n");
            out = Value{b == std::string::npos ? std::string("") : s.substr(b, e - b + 1)};
            return true;
        }
        if (name == "charAt" && args.size() == 1 && args[0].isInt()) {
            long long i = std::get<long long>(args[0].val);
            out = (i < 0 || i >= static_cast<long long>(s.size())) ? Value{std::string("")}
                                                                    : Value{std::string(1, s[static_cast<size_t>(i)])};
            return true;
        }
        if (name == "substring" && args.size() >= 1 && args[0].isInt()) {
            long long start = std::get<long long>(args[0].val);
            if (start < 0) start = 0;
            if (start > static_cast<long long>(s.size())) start = static_cast<long long>(s.size());
            long long len = (args.size() >= 2 && args[1].isInt())
                                ? std::get<long long>(args[1].val)
                                : static_cast<long long>(s.size()) - start;
            if (len < 0) len = 0;
            out = Value{s.substr(static_cast<size_t>(start), static_cast<size_t>(len))};
            return true;
        }
        if (name == "contains" && args.size() == 1 && args[0].isString()) {
            out = Value{s.find(std::get<std::string>(args[0].val)) != std::string::npos};
            return true;
        }
        if (name == "indexOf" && args.size() >= 1 && args[0].isString()) {
            size_t start = 0;
            if (args.size() >= 2 && args[1].isInt()) {
                long long offset = std::get<long long>(args[1].val);
                if (offset < 0) start = 0;
                else if (offset > static_cast<long long>(s.size())) start = s.size();
                else start = static_cast<size_t>(offset);
            }
            size_t p = s.find(std::get<std::string>(args[0].val), start);
            out = Value{p == std::string::npos ? -1LL : static_cast<long long>(p)};
            return true;
        }
        if (name == "startsWith" && args.size() == 1 && args[0].isString()) {
            const std::string& p = std::get<std::string>(args[0].val);
            out = Value{s.size() >= p.size() && s.compare(0, p.size(), p) == 0};
            return true;
        }
        if (name == "endsWith" && args.size() == 1 && args[0].isString()) {
            const std::string& p = std::get<std::string>(args[0].val);
            out = Value{s.size() >= p.size() && s.compare(s.size() - p.size(), p.size(), p) == 0};
            return true;
        }
        if (name == "replace" && args.size() == 2 && args[0].isString() && args[1].isString()) {
            std::string r = s;
            const std::string& from = std::get<std::string>(args[0].val);
            const std::string& to = std::get<std::string>(args[1].val);
            if (!from.empty()) {
                size_t pos = 0;
                while ((pos = r.find(from, pos)) != std::string::npos) {
                    r.replace(pos, from.size(), to);
                    pos += to.size();
                }
            }
            out = Value{r};
            return true;
        }
        if (name == "split" && args.size() == 1 && args[0].isString()) {
            auto result = std::make_shared<ArrayValue>();
            const std::string& delim = std::get<std::string>(args[0].val);
            if (delim.empty()) {
                for (char c : s) result->elements.push_back(Value{std::string(1, c)});
            } else {
                size_t start = 0, pos;
                while ((pos = s.find(delim, start)) != std::string::npos) {
                    result->elements.push_back(Value{s.substr(start, pos - start)});
                    start = pos + delim.size();
                }
                result->elements.push_back(Value{s.substr(start)});
            }
            out = Value{result};
            return true;
        }
        if (name == "repeat" && args.size() == 1 && args[0].isInt()) {
            long long n = std::get<long long>(args[0].val);
            std::string r;
            for (long long i = 0; i < n; ++i) r += s;
            out = Value{r};
            return true;
        }
        return false;
    }

    // ---- Map methods ----
    if (obj.isMap()) {
        auto map = std::get<std::shared_ptr<MapValue>>(obj.val);
        if (name == "size") { out = Value{static_cast<long long>(map->entries.size())}; return true; }
        if (name == "isEmpty") { out = Value{map->entries.empty()}; return true; }
        if (name == "has" && args.size() == 1 && args[0].isString()) {
            out = Value{map->entries.count(std::get<std::string>(args[0].val)) > 0};
            return true;
        }
        if (name == "get" && args.size() == 1 && args[0].isString()) {
            auto it = map->entries.find(std::get<std::string>(args[0].val));
            out = (it != map->entries.end()) ? it->second : Value{std::monostate{}};
            return true;
        }
        if (name == "set" && args.size() == 2 && args[0].isString()) {
            map->entries[std::get<std::string>(args[0].val)] = args[1];
            out = obj;
            return true;
        }
        if (name == "remove" && args.size() == 1 && args[0].isString()) {
            out = Value{map->entries.erase(std::get<std::string>(args[0].val)) > 0};
            return true;
        }
        if (name == "keys") {
            auto result = std::make_shared<ArrayValue>();
            for (const auto& [k, v] : map->entries) { (void)v; result->elements.push_back(Value{k}); }
            out = Value{result};
            return true;
        }
        if (name == "values") {
            auto result = std::make_shared<ArrayValue>();
            for (const auto& [k, v] : map->entries) { (void)k; result->elements.push_back(v); }
            out = Value{result};
            return true;
        }
        return false;
    }

    return false;
}

void Interpreter::visit(CallExpr* node) {
    // Member call: could be a built-in collection/string method, or an instance/class method.
    if (auto member = dynamic_cast<MemberExpr*>(node->callee.get())) {
        Value obj = evaluate(member->object.get());
        std::vector<Value> args;
        for (const auto& arg : node->arguments) {
            args.push_back(evaluate(arg.get()));
        }

        // Try built-in methods for arrays, strings and maps.
        Value out{std::monostate{}};
        if (tryInvokeMethod(obj, member->memberName, args, out)) {
            lastExprVal = out;
            return;
        }

        // Instance / class / map member resolves to a callable via MemberExpr semantics.
        if (obj.isInstance() || obj.isClass() || obj.isMap()) {
            Value callee = evaluate(member);
            lastExprVal = callCallable(callee, args);
            return;
        }

        throw std::runtime_error("Method '" + member->memberName + "' not found on value");
    }

    // THALA-OOP-001 fix: `super.init(...)` / `super.aarambam(...)` invoke the
    // parent constructor on the current instance.
    if (auto sup = dynamic_cast<SuperExpr*>(node->callee.get())) {
        if (sup->method == "init" || sup->method == "aarambam") {
            bool foundSuper = false, foundThis = false;
            Value superKlassVal = environment->get("super", foundSuper);
            Value thisObj = environment->get("this", foundThis);
            if (foundSuper && superKlassVal.isClass() && foundThis) {
                std::vector<Value> cargs;
                for (const auto& a : node->arguments) cargs.push_back(evaluate(a.get()));
                auto owner = std::get<std::shared_ptr<ClassValue>>(superKlassVal.val);
                ConstructorDecl* pc = nullptr;
                while (owner) {
                    for (const auto& c : owner->decl->constructors)
                        if (c->params.size() == cargs.size()) { pc = c.get(); break; }
                    if (pc) break;
                    owner = owner->parentClass;
                }
                if (pc) runConstructorOn(thisObj, owner, pc, cargs);
                lastExprVal = Value{std::monostate{}};
                return;
            }
        }
    }

    Value callee = evaluate(node->callee.get());
    std::vector<Value> args;
    for (const auto& arg : node->arguments) {
        args.push_back(evaluate(arg.get()));
    }

    if (callee.isBuiltIn() || callee.isFunction()) {
        lastExprVal = callCallable(callee, args);
    } else if (callee.isClass()) {
        // Instantiation
        auto klass = std::get<std::shared_ptr<ClassValue>>(callee.val);
        auto instance = std::make_shared<InstanceValue>();
        instance->klass = klass;

        // Walk class hierarchy from base class to derived class to collect all fields
        std::vector<std::shared_ptr<ClassValue>> hierarchy;
        auto currKlass = klass;
        while (currKlass) {
            hierarchy.push_back(currKlass);
            currKlass = currKlass->parentClass;
        }
        // Initialize fields from parent down to child
        for (auto hIt = hierarchy.rbegin(); hIt != hierarchy.rend(); ++hIt) {
            for (const auto& field : (*hIt)->decl->fields) {
                if (!field->isStatic) {
                    Value initVal = Value{std::monostate{}};
                    if (field->varDecl->initializer) {
                        initVal = evaluate(field->varDecl->initializer.get());
                    }
                    instance->fields[field->varDecl->name] = initVal;
                }
            }
        }

        // Look up constructor matching the arity (argument count)
        ConstructorDecl* matchedDecl = nullptr;
        std::shared_ptr<ClassValue> constrOwnerKlass = nullptr;
        currKlass = klass;
        while (currKlass) {
            for (const auto& constr : currKlass->decl->constructors) {
                if (constr->params.size() == args.size()) {
                    matchedDecl = constr.get();
                    constrOwnerKlass = currKlass;
                    break;
                }
            }
            if (matchedDecl) break;
            currKlass = currKlass->parentClass;
        }

        Value objVal{instance};

        if (matchedDecl) {
            // Helper lambda to execute constructor
            auto runConstructor = [&](ConstructorDecl* cDecl, std::shared_ptr<ClassValue> ownerKlass, const std::vector<Value>& cArgs, auto& selfRef) -> void {
                auto callEnv = std::make_shared<Environment>(globals);
                callEnv->define("this", objVal);
                if (ownerKlass->parentClass) {
                    callEnv->define("super", Value{ownerKlass->parentClass});
                }
                for (size_t i = 0; i < cDecl->params.size() && i < cArgs.size(); ++i) {
                    callEnv->define(cDecl->params[i].name, cArgs[i]);
                }

                // Parent initialization routing check
                bool hasExplicitSuper = false;
                for (const auto& initCall : cDecl->initializerList) {
                    if (auto callExpr = dynamic_cast<CallExpr*>(initCall.get())) {
                        if (auto ident = dynamic_cast<IdentifierExpr*>(callExpr->callee.get())) {
                            if (ident->name == "super") {
                                hasExplicitSuper = true;
                                // Evaluate constructor arguments in current constructor scope
                                auto prevEnv = this->environment;
                                this->environment = callEnv;
                                std::vector<Value> superArgs;
                                for (const auto& sArg : callExpr->arguments) {
                                    superArgs.push_back(evaluate(sArg.get()));
                                }
                                this->environment = prevEnv;

                                // Resolve and run parent constructor
                                ConstructorDecl* parentConstr = nullptr;
                                std::shared_ptr<ClassValue> parentKlass = ownerKlass->parentClass;
                                while (parentKlass) {
                                    for (const auto& pc : parentKlass->decl->constructors) {
                                        if (pc->params.size() == superArgs.size()) {
                                            parentConstr = pc.get();
                                            break;
                                        }
                                    }
                                    if (parentConstr) break;
                                    parentKlass = parentKlass->parentClass;
                                }
                                if (parentConstr) {
                                    selfRef(parentConstr, ownerKlass->parentClass, superArgs, selfRef);
                                }
                            } else if (ident->name == "this") {
                                hasExplicitSuper = true;
                                auto prevEnv = this->environment;
                                this->environment = callEnv;
                                std::vector<Value> siblingArgs;
                                for (const auto& sArg : callExpr->arguments) {
                                    siblingArgs.push_back(evaluate(sArg.get()));
                                }
                                this->environment = prevEnv;

                                ConstructorDecl* siblingConstr = nullptr;
                                for (const auto& sc : ownerKlass->decl->constructors) {
                                    if (sc->params.size() == siblingArgs.size()) {
                                        siblingConstr = sc.get();
                                        break;
                                    }
                                }
                                if (siblingConstr) {
                                    selfRef(siblingConstr, ownerKlass, siblingArgs, selfRef);
                                }
                            }
                        }
                    }
                }

                // If no explicit super, and parent class exists, call default constructor super()
                if (!hasExplicitSuper && ownerKlass->parentClass) {
                    ConstructorDecl* parentConstr = nullptr;
                    std::shared_ptr<ClassValue> parentKlass = ownerKlass->parentClass;
                    while (parentKlass) {
                        for (const auto& pc : parentKlass->decl->constructors) {
                            if (pc->params.size() == 0) {
                                parentConstr = pc.get();
                                break;
                            }
                        }
                        if (parentConstr) break;
                        parentKlass = parentKlass->parentClass;
                    }
                    if (parentConstr) {
                        selfRef(parentConstr, ownerKlass->parentClass, {}, selfRef);
                    }
                }

                // Execute constructor body
                auto prevEnv = this->environment;
                this->environment = callEnv;
                try {
                    executeBlock(dynamic_cast<BlockStmt*>(cDecl->body.get()), callEnv);
                } catch (const ReturnSignal&) {
                }
                this->environment = prevEnv;
            };

            runConstructor(matchedDecl, constrOwnerKlass, args, runConstructor);
        } else {
            // Default parent constructor invocation if class has parent but no init defined
            std::shared_ptr<ClassValue> parentKlass = klass->parentClass;
            auto runDefaultConstructors = [&](std::shared_ptr<ClassValue> pk, auto& selfRef) -> void {
                if (!pk) return;
                selfRef(pk->parentClass, selfRef);
                ConstructorDecl* pc = nullptr;
                auto currPk = pk;
                while (currPk) {
                    for (const auto& c : currPk->decl->constructors) {
                        if (c->params.size() == 0) {
                            pc = c.get();
                            break;
                        }
                    }
                    if (pc) break;
                    currPk = currPk->parentClass;
                }
                if (pc) {
                    auto callEnv = std::make_shared<Environment>(globals);
                    callEnv->define("this", objVal);
                    if (pk->parentClass) {
                        callEnv->define("super", Value{pk->parentClass});
                    }
                    auto prevEnv = this->environment;
                    this->environment = callEnv;
                    try {
                        executeBlock(dynamic_cast<BlockStmt*>(pc->body.get()), callEnv);
                    } catch (const ReturnSignal&) {}
                    this->environment = prevEnv;
                }
            };
            runDefaultConstructors(parentKlass, runDefaultConstructors);
        }

        lastExprVal = objVal;
    } else {
        throw std::runtime_error("Callee is not a callable object: " + callee.toString());
    }
}

void Interpreter::visit(MemberExpr* node) {
    Value obj = evaluate(node->object.get());
    if (obj.isInstance()) {
        auto inst = std::get<std::shared_ptr<InstanceValue>>(obj.val);
        
        // Field access
        auto fIt = inst->fields.find(node->memberName);
        if (fIt != inst->fields.end()) {
            lastExprVal = fIt->second;
            return;
        }

        // Property getter lookup
        auto currKlass = inst->klass;
        while (currKlass) {
            for (const auto& prop : currKlass->decl->properties) {
                if (prop->name == node->memberName && prop->getterBody) {
                    auto getterEnv = std::make_shared<Environment>(globals);
                    getterEnv->define("this", obj);
                    auto prevEnv = this->environment;
                    this->environment = getterEnv;
                    try {
                        executeBlock(dynamic_cast<BlockStmt*>(prop->getterBody.get()), getterEnv);
                        lastExprVal = Value{std::monostate{}};
                    } catch (const ReturnSignal& ret) {
                        lastExprVal = ret.value;
                    }
                    this->environment = prevEnv;
                    return;
                }
            }
            currKlass = currKlass->parentClass;
        }

        // Method lookup (MethodDecl wrapping FuncDecl)
        currKlass = inst->klass;
        while (currKlass) {
            for (const auto& m : currKlass->decl->methods) {
                if (m->funcDecl && m->funcDecl->name == node->memberName) {
                    auto func = std::make_shared<FunctionValue>();
                    func->decl = m->funcDecl.get();
                    func->closure = std::make_shared<Environment>(globals);
                    func->closure->define("this", obj);
                    
                    // define super binding in method closure
                    if (currKlass->parentClass) {
                        func->closure->define("super", Value{currKlass->parentClass});
                    }
                    
                    lastExprVal = Value{func};
                    return;
                }
            }
            currKlass = currKlass->parentClass;
        }

        throw std::runtime_error("Member '" + node->memberName + "' not found on instance");
    } else if (obj.isClass()) {
        // Static field / static method access on class
        auto klass = std::get<std::shared_ptr<ClassValue>>(obj.val);
        
        // Static field lookup
        auto sfIt = klass->staticFields.find(node->memberName);
        if (sfIt != klass->staticFields.end()) {
            lastExprVal = sfIt->second;
            return;
        }

        // Static method lookup
        auto currKlass = klass;
        while (currKlass) {
            for (const auto& m : currKlass->decl->methods) {
                if (m->isStatic && m->funcDecl && m->funcDecl->name == node->memberName) {
                    auto func = std::make_shared<FunctionValue>();
                    func->decl = m->funcDecl.get();
                    func->closure = std::make_shared<Environment>(globals);
                    lastExprVal = Value{func};
                    return;
                }
            }
            currKlass = currKlass->parentClass;
        }
        throw std::runtime_error("Static member '" + node->memberName + "' not found on class '" + klass->decl->name + "'");
    } else if (obj.isMap()) {
        auto map = std::get<std::shared_ptr<MapValue>>(obj.val);
        auto it = map->entries.find(node->memberName);
        if (it != map->entries.end()) {
            lastExprVal = it->second;
        } else {
            lastExprVal = Value{std::monostate{}};
        }
    } else {
        throw std::runtime_error("Member access on non-object");
    }
}

void Interpreter::visit(IndexExpr* node) {
    Value obj = evaluate(node->object.get());
    Value idx = evaluate(node->index.get());

    if (obj.isArray() && idx.isInt()) {
        auto arr = std::get<std::shared_ptr<ArrayValue>>(obj.val);
        long long i = std::get<long long>(idx.val);
        if (i < 0 || i >= static_cast<long long>(arr->elements.size())) {
            throw std::runtime_error("array index out of bounds");
        }
        lastExprVal = arr->elements[static_cast<size_t>(i)];
    } else if (obj.isMap() && idx.isString()) {
        auto map = std::get<std::shared_ptr<MapValue>>(obj.val);
        auto it = map->entries.find(std::get<std::string>(idx.val));
        if (it != map->entries.end()) {
            lastExprVal = it->second;
        } else {
            lastExprVal = Value{std::monostate{}};
        }
    } else {
        throw std::runtime_error("Index access on unsupported type");
    }
}

void Interpreter::visit(ArrayExpr* node) {
    auto arr = std::make_shared<ArrayValue>();
    for (const auto& elem : node->elements) {
        arr->elements.push_back(evaluate(elem.get()));
    }
    lastExprVal = Value{arr};
}

void Interpreter::visit(MapExpr* node) {
    auto map = std::make_shared<MapValue>();
    for (const auto& entry : node->entries) {
        map->entries[entry.first] = evaluate(entry.second.get());
    }
    lastExprVal = Value{map};
}

void Interpreter::visit(LambdaExpr* node) {
    auto func = std::make_shared<FunctionValue>();
    func->decl = node->fn.get();
    func->closure = environment; // capture the defining environment (closure)
    lastExprVal = Value{func};
}

void Interpreter::visit(ThisExpr*) {
    bool found = false;
    lastExprVal = environment->get("this", found);
    if (!found) {
        throw std::runtime_error("this binding unresolved");
    }
}

void Interpreter::runConstructorOn(const Value& objVal,
                                   std::shared_ptr<ClassValue> ownerKlass,
                                   ConstructorDecl* cDecl,
                                   const std::vector<Value>& cArgs) {
    auto callEnv = std::make_shared<Environment>(globals);
    callEnv->define("this", objVal);
    if (ownerKlass->parentClass) {
        callEnv->define("super", Value{ownerKlass->parentClass});
    }
    for (size_t i = 0; i < cDecl->params.size() && i < cArgs.size(); ++i) {
        callEnv->define(cDecl->params[i].name, cArgs[i]);
    }

    bool hasExplicitSuper = false;
    for (const auto& initCall : cDecl->initializerList) {
        auto callExpr = dynamic_cast<CallExpr*>(initCall.get());
        if (!callExpr) continue;
        auto ident = dynamic_cast<IdentifierExpr*>(callExpr->callee.get());
        if (!ident) continue;
        if (ident->name == "super") {
            hasExplicitSuper = true;
            auto prevEnv = this->environment;
            this->environment = callEnv;
            std::vector<Value> superArgs;
            for (const auto& sArg : callExpr->arguments) superArgs.push_back(evaluate(sArg.get()));
            this->environment = prevEnv;
            ConstructorDecl* parentConstr = nullptr;
            std::shared_ptr<ClassValue> parentKlass = ownerKlass->parentClass;
            while (parentKlass) {
                for (const auto& pc : parentKlass->decl->constructors)
                    if (pc->params.size() == superArgs.size()) { parentConstr = pc.get(); break; }
                if (parentConstr) break;
                parentKlass = parentKlass->parentClass;
            }
            if (parentConstr) runConstructorOn(objVal, ownerKlass->parentClass, parentConstr, superArgs);
        } else if (ident->name == "this") {
            hasExplicitSuper = true;
            auto prevEnv = this->environment;
            this->environment = callEnv;
            std::vector<Value> siblingArgs;
            for (const auto& sArg : callExpr->arguments) siblingArgs.push_back(evaluate(sArg.get()));
            this->environment = prevEnv;
            ConstructorDecl* siblingConstr = nullptr;
            for (const auto& sc : ownerKlass->decl->constructors)
                if (sc->params.size() == siblingArgs.size()) { siblingConstr = sc.get(); break; }
            if (siblingConstr) runConstructorOn(objVal, ownerKlass, siblingConstr, siblingArgs);
        }
    }
    if (!hasExplicitSuper && ownerKlass->parentClass) {
        ConstructorDecl* parentConstr = nullptr;
        std::shared_ptr<ClassValue> parentKlass = ownerKlass->parentClass;
        while (parentKlass) {
            for (const auto& pc : parentKlass->decl->constructors)
                if (pc->params.size() == 0) { parentConstr = pc.get(); break; }
            if (parentConstr) break;
            parentKlass = parentKlass->parentClass;
        }
        if (parentConstr) runConstructorOn(objVal, ownerKlass->parentClass, parentConstr, {});
    }

    auto prevEnv = this->environment;
    this->environment = callEnv;
    try {
        executeBlock(dynamic_cast<BlockStmt*>(cDecl->body.get()), callEnv);
    } catch (const ReturnSignal&) {}
    this->environment = prevEnv;
}

void Interpreter::visit(SuperExpr* node) {
    // Resolve super method
    bool foundSuper = false;
    Value superKlassVal = environment->get("super", foundSuper);
    bool foundThis = false;
    Value thisObj = environment->get("this", foundThis);

    if (foundSuper && superKlassVal.isClass() && foundThis) {
        auto superKlass = std::get<std::shared_ptr<ClassValue>>(superKlassVal.val);
        
        auto currKlass = superKlass;
        while (currKlass) {
            for (const auto& m : currKlass->decl->methods) {
                if (m->funcDecl && m->funcDecl->name == node->method) {
                    auto func = std::make_shared<FunctionValue>();
                    func->decl = m->funcDecl.get();
                    func->closure = std::make_shared<Environment>(globals);
                    func->closure->define("this", thisObj);
                    if (currKlass->parentClass) {
                        func->closure->define("super", Value{currKlass->parentClass});
                    }
                    lastExprVal = Value{func};
                    return;
                }
            }
            currKlass = currKlass->parentClass;
        }
    }
    throw std::runtime_error("Super method '" + node->method + "' not found");
}

void Interpreter::visit(FieldDecl*) {
    // Fields are initialized during class instantiation, not visited independently
}

void Interpreter::visit(MethodDecl*) {
    // Methods are looked up during member access, not visited independently
}

void Interpreter::visit(ConstructorDecl*) {
    // Constructors are invoked during instantiation, not visited independently
}

void Interpreter::visit(PropertyDecl*) {
    // Properties are accessed through MemberExpr getter/setter dispatch
}

void Interpreter::visit(OperatorDecl*) {
    // Operators are dispatched during binary expression evaluation
}

void Interpreter::visit(InterfaceDecl*) {
    // Interfaces are metadata-only at runtime; no direct execution
}

void Interpreter::visit(CastExpr* node) {
    Value val = evaluate(node->expression.get());
    // Safe cast: returns null on failure
    if (val.isInstance()) {
        auto inst = std::get<std::shared_ptr<InstanceValue>>(val.val);
        // Walk class hierarchy to check if instance is of target type
        auto currKlass = inst->klass;
        while (currKlass) {
            if (currKlass->decl->name == node->targetType) {
                lastExprVal = val; // cast succeeds, return same object
                return;
            }
            // Check interfaces
            for (const auto& iface : currKlass->implementedInterfaces) {
                if (iface == node->targetType) {
                    lastExprVal = val;
                    return;
                }
            }
            currKlass = currKlass->parentClass;
        }
        // Cast failed, return null
        lastExprVal = Value{std::monostate{}};
    } else {
        // For non-instance types, basic type coercion
        if (node->targetType == "string") {
            lastExprVal = Value{val.toString()};
        } else if (node->targetType == "int" && val.isFloat()) {
            lastExprVal = Value{static_cast<long long>(std::get<double>(val.val))};
        } else if (node->targetType == "float" && val.isInt()) {
            lastExprVal = Value{static_cast<double>(std::get<long long>(val.val))};
        } else {
            lastExprVal = Value{std::monostate{}}; // cast failed
        }
    }
}

void Interpreter::visit(TypeTestExpr* node) {
    Value val = evaluate(node->expression.get());
    if (val.isEnum()) {
        lastExprVal = Value{std::get<std::shared_ptr<EnumValue>>(val.val)->typeName == node->targetType};
        return;
    }
    if (val.isInstance()) {
        auto inst = std::get<std::shared_ptr<InstanceValue>>(val.val);
        auto currKlass = inst->klass;
        while (currKlass) {
            if (currKlass->decl->name == node->targetType) {
                lastExprVal = Value{true};
                return;
            }
            for (const auto& iface : currKlass->implementedInterfaces) {
                if (iface == node->targetType) {
                    lastExprVal = Value{true};
                    return;
                }
            }
            currKlass = currKlass->parentClass;
        }
        lastExprVal = Value{false};
    } else {
        // Primitive type checking
        bool matches = false;
        if (node->targetType == "int") matches = val.isInt();
        else if (node->targetType == "float") matches = val.isFloat();
        else if (node->targetType == "string") matches = val.isString();
        else if (node->targetType == "bool") matches = val.isBool();
        else if (node->targetType == "char") matches = val.isChar();
        lastExprVal = Value{matches};
    }
}

void Interpreter::visit(ImportDecl*) {
    // ImportDecl is processed by the driver using topological load order.
}

void Interpreter::visit(PackageDecl*) {
    // Package declarations are organizational metadata; no runtime effect.
}

void Interpreter::visit(EnumDecl* node) {
    // An enum is a namespace map of member-name -> EnumValue, bound to its name.
    auto ns = std::make_shared<MapValue>();
    for (size_t i = 0; i < node->members.size(); ++i) {
        auto ev = std::make_shared<EnumValue>();
        ev->typeName = node->name;
        ev->name = node->members[i];
        ev->ordinal = static_cast<long long>(i);
        ns->entries[node->members[i]] = Value{ev};
    }
    environment->define(node->name, Value{ns});
}

} // namespace thalapathy
