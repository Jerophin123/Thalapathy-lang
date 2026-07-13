#include "thalapathy/ir/IR.hpp"
#include "thalapathy/version/Version.hpp"
#include <iostream>
#include <filesystem>
#include <vector>
#include <variant>

namespace fs = std::filesystem;
using namespace thalapathy;

static const char* getOpcodeName(OpCode op) {
    switch (op) {
        case OpCode::LOAD_CONST: return "LOAD_CONST";
        case OpCode::LOAD_LOCAL: return "LOAD_LOCAL";
        case OpCode::STORE_LOCAL: return "STORE_LOCAL";
        case OpCode::LOAD_GLOBAL: return "LOAD_GLOBAL";
        case OpCode::STORE_GLOBAL: return "STORE_GLOBAL";
        case OpCode::ADD: return "ADD";
        case OpCode::SUB: return "SUB";
        case OpCode::MUL: return "MUL";
        case OpCode::DIV: return "DIV";
        case OpCode::MOD: return "MOD";
        case OpCode::EQ: return "EQ";
        case OpCode::NE: return "NE";
        case OpCode::LT: return "LT";
        case OpCode::LTE: return "LTE";
        case OpCode::GT: return "GT";
        case OpCode::GTE: return "GTE";
        case OpCode::NOT: return "NOT";
        case OpCode::JUMP: return "JUMP";
        case OpCode::JUMP_IF_FALSE: return "JUMP_IF_FALSE";
        case OpCode::CALL: return "CALL";
        case OpCode::RETURN: return "RETURN";
        case OpCode::POP: return "POP";
        case OpCode::BUILD_ARRAY: return "BUILD_ARRAY";
        case OpCode::INDEX_GET: return "INDEX_GET";
        case OpCode::INDEX_SET: return "INDEX_SET";
        case OpCode::GET_FIELD: return "GET_FIELD";
        case OpCode::SET_FIELD: return "SET_FIELD";
        case OpCode::THROW: return "THROW";
        default: return "UNKNOWN";
    }
}

static std::string escapeJsonString(const std::string& input) {
    std::string output = "";
    for (char c : input) {
        if (c == '"') output += "\\\"";
        else if (c == '\\') output += "\\\\";
        else if (c == '\n') output += "\\n";
        else if (c == '\r') output += "\\r";
        else if (c == '\t') output += "\\t";
        else output += c;
    }
    return output;
}

static std::string constantToString(const ConstantVal& val) {
    if (std::holds_alternative<long long>(val)) {
        return std::to_string(std::get<long long>(val));
    } else if (std::holds_alternative<double>(val)) {
        return std::to_string(std::get<double>(val));
    } else {
        return std::get<std::string>(val);
    }
}

static void printHeader(const ModuleIR& ir) {
    (void)ir;
    std::cout << "Magic Signature: VJAY\n"
              << "Format Version: 3\n"
              << "Language Version: 0.3.0\n";
}

static void printSections(const ModuleIR& ir) {
    std::cout << "Sections:\n"
              << "  - Section 1 (Constant Pool): size " << ir.constantPool.size() << " entries\n"
              << "  - Section 2 (Instructions): size " << ir.instructions.size() << " instructions\n"
              << "  - Section 3 (Classes): size " << ir.classes.size() << " classes\n"
              << "  - Section 4 (Interfaces): size " << ir.interfaces.size() << " interfaces\n"
              << "  - Section 5 (Properties): size " << ir.properties.size() << " properties\n"
              << "  - Section 6 (Methods): size " << ir.methods.size() << " methods\n"
              << "  - Section 7 (Types): size " << ir.types.size() << " types\n";
}

static void printConstants(const ModuleIR& ir) {
    std::cout << "Constant Pool:\n";
    for (size_t i = 0; i < ir.constantPool.size(); ++i) {
        std::cout << "  [" << i << "] ";
        const auto& val = ir.constantPool[i];
        if (std::holds_alternative<long long>(val)) {
            std::cout << "INT: " << std::get<long long>(val) << "\n";
        } else if (std::holds_alternative<double>(val)) {
            std::cout << "FLOAT: " << std::get<double>(val) << "\n";
        } else {
            std::cout << "STRING: \"" << std::get<std::string>(val) << "\"\n";
        }
    }
}

static void printTypes(const ModuleIR& ir) {
    std::cout << "Declared Types:\n";
    for (const auto& t : ir.types) {
        std::cout << "  - " << t << "\n";
    }
}

static void printClasses(const ModuleIR& ir) {
    std::cout << "Classes:\n";
    for (const auto& c : ir.classes) {
        std::cout << "  - " << c.name;
        if (!c.parentName.empty()) {
            std::cout << " varisu " << c.parentName;
        }
        if (!c.implementedInterfaces.empty()) {
            std::cout << " kaththi ";
            for (size_t i = 0; i < c.implementedInterfaces.size(); ++i) {
                std::cout << c.implementedInterfaces[i];
                if (i + 1 < c.implementedInterfaces.size()) std::cout << ", ";
            }
        }
        std::cout << "\n";
        std::cout << "    Fields:\n";
        for (const auto& f : c.fields) std::cout << "      - " << f << "\n";
        std::cout << "    Methods:\n";
        for (const auto& m : c.methods) std::cout << "      - " << m << "\n";
        std::cout << "    Properties:\n";
        for (const auto& p : c.properties) std::cout << "      - " << p << "\n";
    }
}

static void printInterfaces(const ModuleIR& ir) {
    std::cout << "Interfaces:\n";
    for (const auto& iface : ir.interfaces) {
        std::cout << "  - " << iface.name << "\n";
        std::cout << "    Methods:\n";
        for (const auto& m : iface.methods) std::cout << "      - " << m << "\n";
    }
}

static void printProperties(const ModuleIR& ir) {
    std::cout << "Properties:\n";
    for (const auto& p : ir.properties) {
        std::cout << "  - " << p.name << " : " << p.typeStr;
        std::cout << " [";
        if (p.hasGetter) std::cout << "paaru";
        if (p.hasGetter && p.hasSetter) std::cout << ", ";
        if (p.hasSetter) std::cout << "maathu";
        std::cout << "]\n";
    }
}

static void printMethods(const ModuleIR& ir) {
    std::cout << "Methods:\n";
    for (const auto& m : ir.methods) {
        std::cout << "  - " << m.name << "(";
        for (size_t i = 0; i < m.paramTypes.size(); ++i) {
            std::cout << m.paramTypes[i];
            if (i + 1 < m.paramTypes.size()) std::cout << ", ";
        }
        std::cout << ") -> " << m.returnType << "\n";
    }
}

static void printInstructions(const ModuleIR& ir) {
    std::cout << "Instructions / Disassembly:\n";
    for (size_t i = 0; i < ir.instructions.size(); ++i) {
        const auto& inst = ir.instructions[i];
        std::cout << "  " << i << ": " << getOpcodeName(inst.opcode) << " " << inst.operand;
        // Print constant helper if applicable
        if (inst.opcode == OpCode::LOAD_CONST && inst.operand < ir.constantPool.size()) {
            std::cout << " (" << constantToString(ir.constantPool[inst.operand]) << ")";
        }
        std::cout << "\n";
    }
}

static void printJson(const ModuleIR& ir) {
    std::cout << "{\n"
              << "  \"header\": {\n"
              << "    \"magic\": \"VJAY\",\n"
              << "    \"formatVersion\": 3,\n"
              << "    \"languageVersion\": \"0.3.0\"\n"
              << "  },\n";

    // Constants
    std::cout << "  \"constants\": [\n";
    for (size_t i = 0; i < ir.constantPool.size(); ++i) {
        const auto& val = ir.constantPool[i];
        std::cout << "    {\n"
                  << "      \"index\": " << i << ",\n";
        if (std::holds_alternative<long long>(val)) {
            std::cout << "      \"type\": \"int\",\n"
                      << "      \"value\": " << std::get<long long>(val) << "\n";
        } else if (std::holds_alternative<double>(val)) {
            std::cout << "      \"type\": \"float\",\n"
                      << "      \"value\": " << std::get<double>(val) << "\n";
        } else {
            std::cout << "      \"type\": \"string\",\n"
                      << "      \"value\": \"" << escapeJsonString(std::get<std::string>(val)) << "\"\n";
        }
        std::cout << "    }" << (i + 1 < ir.constantPool.size() ? "," : "") << "\n";
    }
    std::cout << "  ],\n";

    // Types
    std::cout << "  \"types\": [\n";
    for (size_t i = 0; i < ir.types.size(); ++i) {
        std::cout << "    \"" << escapeJsonString(ir.types[i]) << "\"" << (i + 1 < ir.types.size() ? "," : "") << "\n";
    }
    std::cout << "  ],\n";

    // Classes
    std::cout << "  \"classes\": [\n";
    for (size_t i = 0; i < ir.classes.size(); ++i) {
        const auto& c = ir.classes[i];
        std::cout << "    {\n"
                  << "      \"name\": \"" << escapeJsonString(c.name) << "\",\n"
                  << "      \"parentName\": \"" << escapeJsonString(c.parentName) << "\",\n"
                  << "      \"implementedInterfaces\": [";
        for (size_t idx = 0; idx < c.implementedInterfaces.size(); ++idx) {
            std::cout << "\"" << escapeJsonString(c.implementedInterfaces[idx]) << "\"";
            if (idx + 1 < c.implementedInterfaces.size()) std::cout << ", ";
        }
        std::cout << "],\n"
                  << "      \"fields\": [";
        for (size_t idx = 0; idx < c.fields.size(); ++idx) {
            std::cout << "\"" << escapeJsonString(c.fields[idx]) << "\"";
            if (idx + 1 < c.fields.size()) std::cout << ", ";
        }
        std::cout << "],\n"
                  << "      \"methods\": [";
        for (size_t idx = 0; idx < c.methods.size(); ++idx) {
            std::cout << "\"" << escapeJsonString(c.methods[idx]) << "\"";
            if (idx + 1 < c.methods.size()) std::cout << ", ";
        }
        std::cout << "],\n"
                  << "      \"properties\": [";
        for (size_t idx = 0; idx < c.properties.size(); ++idx) {
            std::cout << "\"" << escapeJsonString(c.properties[idx]) << "\"";
            if (idx + 1 < c.properties.size()) std::cout << ", ";
        }
        std::cout << "]\n"
                  << "    }" << (i + 1 < ir.classes.size() ? "," : "") << "\n";
    }
    std::cout << "  ],\n";

    // Interfaces
    std::cout << "  \"interfaces\": [\n";
    for (size_t i = 0; i < ir.interfaces.size(); ++i) {
        const auto& iface = ir.interfaces[i];
        std::cout << "    {\n"
                  << "      \"name\": \"" << escapeJsonString(iface.name) << "\",\n"
                  << "      \"methods\": [";
        for (size_t idx = 0; idx < iface.methods.size(); ++idx) {
            std::cout << "\"" << escapeJsonString(iface.methods[idx]) << "\"";
            if (idx + 1 < iface.methods.size()) std::cout << ", ";
        }
        std::cout << "]\n"
                  << "    }" << (i + 1 < ir.interfaces.size() ? "," : "") << "\n";
    }
    std::cout << "  ],\n";

    // Properties
    std::cout << "  \"properties\": [\n";
    for (size_t i = 0; i < ir.properties.size(); ++i) {
        const auto& p = ir.properties[i];
        std::cout << "    {\n"
                  << "      \"name\": \"" << escapeJsonString(p.name) << "\",\n"
                  << "      \"type\": \"" << escapeJsonString(p.typeStr) << "\",\n"
                  << "      \"hasGetter\": " << (p.hasGetter ? "true" : "false") << ",\n"
                  << "      \"hasSetter\": " << (p.hasSetter ? "true" : "false") << "\n"
                  << "    }" << (i + 1 < ir.properties.size() ? "," : "") << "\n";
    }
    std::cout << "  ],\n";

    // Methods
    std::cout << "  \"methods\": [\n";
    for (size_t i = 0; i < ir.methods.size(); ++i) {
        const auto& m = ir.methods[i];
        std::cout << "    {\n"
                  << "      \"name\": \"" << escapeJsonString(m.name) << "\",\n"
                  << "      \"returnType\": \"" << escapeJsonString(m.returnType) << "\",\n"
                  << "      \"paramTypes\": [";
        for (size_t idx = 0; idx < m.paramTypes.size(); ++idx) {
            std::cout << "\"" << escapeJsonString(m.paramTypes[idx]) << "\"";
            if (idx + 1 < m.paramTypes.size()) std::cout << ", ";
        }
        std::cout << "]\n"
                  << "    }" << (i + 1 < ir.methods.size() ? "," : "") << "\n";
    }
    std::cout << "  ],\n";

    // Instructions
    std::cout << "  \"instructions\": [\n";
    for (size_t i = 0; i < ir.instructions.size(); ++i) {
        const auto& inst = ir.instructions[i];
        std::cout << "    {\n"
                  << "      \"index\": " << i << ",\n"
                  << "      \"opcode\": \"" << getOpcodeName(inst.opcode) << "\",\n"
                  << "      \"operand\": " << inst.operand << "\n"
                  << "    }" << (i + 1 < ir.instructions.size() ? "," : "") << "\n";
    }
    std::cout << "  ]\n";

    std::cout << "}\n";
}

int main(int argc, char* argv[]) {
    bool headerFlag = false;
    bool sectionsFlag = false;
    bool constantsFlag = false;
    bool typesFlag = false;
    bool classesFlag = false;
    bool interfacesFlag = false;
    bool methodsFlag = false;
    bool propertiesFlag = false;
    bool irFlag = false;
    bool jsonFlag = false;
    std::string filepath = "";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--header") headerFlag = true;
        else if (arg == "--sections") sectionsFlag = true;
        else if (arg == "--constants") constantsFlag = true;
        else if (arg == "--types") typesFlag = true;
        else if (arg == "--classes") classesFlag = true;
        else if (arg == "--interfaces") interfacesFlag = true;
        else if (arg == "--methods") methodsFlag = true;
        else if (arg == "--properties") propertiesFlag = true;
        else if (arg == "--ir") irFlag = true;
        else if (arg == "--json") jsonFlag = true;
        else if (arg == "--debug") { /* debug mode info can be printed */ }
        else if (arg.rfind("-", 0) == 0) {
            std::cerr << "error: unknown option '" << arg << "'\n";
            return 1;
        } else {
            filepath = arg;
        }
    }

    if (filepath.empty()) {
        std::cerr << "Usage:\n"
                  << "  vijaydump <file.vijay> [options]\n\n"
                  << "Options:\n"
                  << "  --header\n"
                  << "  --sections\n"
                  << "  --constants\n"
                  << "  --types\n"
                  << "  --classes\n"
                  << "  --interfaces\n"
                  << "  --methods\n"
                  << "  --properties\n"
                  << "  --ir\n"
                  << "  --json\n";
        return 1;
    }

    if (!fs::exists(filepath)) {
        std::cerr << "error: file does not exist: " << filepath << "\n";
        return 1;
    }

    ModuleIR ir;
    if (!ir.deserialize(filepath)) {
        std::cerr << "error: failed to deserialize VEAF artifact: " << filepath << "\n";
        return 2; // technical failure code
    }

    if (jsonFlag) {
        printJson(ir);
        return 0;
    }

    bool anyFlag = headerFlag || sectionsFlag || constantsFlag || typesFlag || classesFlag || interfacesFlag || methodsFlag || propertiesFlag || irFlag;

    if (!anyFlag) {
        // Print everything in human readable
        printHeader(ir);
        std::cout << "\n";
        printSections(ir);
        std::cout << "\n";
        printConstants(ir);
        std::cout << "\n";
        printTypes(ir);
        std::cout << "\n";
        printClasses(ir);
        std::cout << "\n";
        printInterfaces(ir);
        std::cout << "\n";
        printProperties(ir);
        std::cout << "\n";
        printMethods(ir);
        std::cout << "\n";
        printInstructions(ir);
    } else {
        if (headerFlag) printHeader(ir);
        if (sectionsFlag) printSections(ir);
        if (constantsFlag) printConstants(ir);
        if (typesFlag) printTypes(ir);
        if (classesFlag) printClasses(ir);
        if (interfacesFlag) printInterfaces(ir);
        if (propertiesFlag) printProperties(ir);
        if (methodsFlag) printMethods(ir);
        if (irFlag) printInstructions(ir);
    }

    return 0;
}
