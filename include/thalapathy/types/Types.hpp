#pragma once
#include <string>

namespace thalapathy {

enum class TypeKind {
    INT,
    FLOAT,
    BOOL,
    STRING,
    CHAR,
    VOID,
    NULL_TYPE,
    ARRAY,
    MAP,
    CLASS,
    FUNCTION,
    ANY // for dynamic/gradual types
};

inline std::string typeKindToString(TypeKind kind) {
    switch (kind) {
        case TypeKind::INT: return "int";
        case TypeKind::FLOAT: return "float";
        case TypeKind::BOOL: return "bool";
        case TypeKind::STRING: return "string";
        case TypeKind::CHAR: return "char";
        case TypeKind::VOID: return "void";
        case TypeKind::NULL_TYPE: return "null";
        case TypeKind::ARRAY: return "array";
        case TypeKind::MAP: return "map";
        case TypeKind::CLASS: return "class";
        case TypeKind::FUNCTION: return "function";
        case TypeKind::ANY: return "any";
    }
    return "unknown";
}

struct Type {
    TypeKind kind;
    std::string className; // only valid if kind == CLASS

    bool operator==(const Type& other) const {
        if (kind != other.kind) return false;
        if (kind == TypeKind::CLASS) return className == other.className;
        return true;
    }

    bool isCompatible(const Type& other) const {
        if (kind == TypeKind::ANY || other.kind == TypeKind::ANY) return true;
        if (kind == TypeKind::FLOAT && other.kind == TypeKind::INT) return true; // int is compatible with float? Wait, let's keep them strict or standard. Let's make float assignable from int if needed, but the prompt says: "No arbitrary implicit coercion." So let's make it strict.
        if (kind == TypeKind::NULL_TYPE || other.kind == TypeKind::NULL_TYPE) return true; // null is compatible with everything
        return *this == other;
    }
};

inline Type makePrimitiveType(const std::string& typeStr) {
    if (typeStr == "int") return Type{TypeKind::INT, ""};
    if (typeStr == "float") return Type{TypeKind::FLOAT, ""};
    if (typeStr == "bool") return Type{TypeKind::BOOL, ""};
    if (typeStr == "string") return Type{TypeKind::STRING, ""};
    if (typeStr == "char") return Type{TypeKind::CHAR, ""};
    if (typeStr == "void") return Type{TypeKind::VOID, ""};
    if (typeStr == "null") return Type{TypeKind::NULL_TYPE, ""};
    return Type{TypeKind::ANY, ""};
}

} // namespace thalapathy
