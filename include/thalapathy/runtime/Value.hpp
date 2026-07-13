#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <variant>
#include <sstream>
#include <functional>

#include "thalapathy/ast/AST.hpp"

namespace thalapathy {

struct Value;
class Environment;
class Interpreter;

struct ArrayValue {
    std::vector<Value> elements;
};

struct MapValue {
    std::unordered_map<std::string, Value> entries;
};

struct FunctionValue {
    FuncDecl* decl;
    std::shared_ptr<Environment> closure;
};

struct ClassValue {
    ClassDecl* decl;
    std::shared_ptr<ClassValue> parentClass;
    std::vector<std::string> implementedInterfaces;
    std::unordered_map<std::string, Value> staticFields;
};

struct InstanceValue {
    std::shared_ptr<ClassValue> klass;
    std::unordered_map<std::string, Value> fields;
};

using BuiltInFn = Value(*)(const std::vector<Value>& args);
// Context-aware builtins receive the interpreter so they can invoke user callables
// (e.g. web-server route handlers, higher-order native helpers).
using ContextBuiltInFn = std::function<Value(Interpreter&, const std::vector<Value>&)>;

struct BuiltInFunctionValue {
    std::string name;
    BuiltInFn fn = nullptr;
    ContextBuiltInFn ctxFn = nullptr;
};

struct ErrorValue {
    std::string message;
};

// A single enum member, e.g. Color.RED. Identity is (typeName, name).
struct EnumValue {
    std::string typeName;
    std::string name;
    long long ordinal = 0;
};

struct Value {
    using ValType = std::variant<
        std::monostate, // null
        long long,      // int
        double,         // float
        bool,           // bool
        char,           // char
        std::string,    // string
        std::shared_ptr<ArrayValue>,
        std::shared_ptr<MapValue>,
        std::shared_ptr<FunctionValue>,
        std::shared_ptr<ClassValue>,
        std::shared_ptr<InstanceValue>,
        std::shared_ptr<BuiltInFunctionValue>,
        std::shared_ptr<ErrorValue>,
        std::shared_ptr<EnumValue>
    >;

    ValType val;

    bool isNull() const { return std::holds_alternative<std::monostate>(val); }
    bool isInt() const { return std::holds_alternative<long long>(val); }
    bool isFloat() const { return std::holds_alternative<double>(val); }
    bool isBool() const { return std::holds_alternative<bool>(val); }
    bool isChar() const { return std::holds_alternative<char>(val); }
    bool isString() const { return std::holds_alternative<std::string>(val); }
    bool isArray() const { return std::holds_alternative<std::shared_ptr<ArrayValue>>(val); }
    bool isMap() const { return std::holds_alternative<std::shared_ptr<MapValue>>(val); }
    bool isFunction() const { return std::holds_alternative<std::shared_ptr<FunctionValue>>(val); }
    bool isClass() const { return std::holds_alternative<std::shared_ptr<ClassValue>>(val); }
    bool isInstance() const { return std::holds_alternative<std::shared_ptr<InstanceValue>>(val); }
    bool isBuiltIn() const { return std::holds_alternative<std::shared_ptr<BuiltInFunctionValue>>(val); }
    bool isError() const { return std::holds_alternative<std::shared_ptr<ErrorValue>>(val); }
    bool isEnum() const { return std::holds_alternative<std::shared_ptr<EnumValue>>(val); }

    std::string toString() const {
        if (isNull()) return "null";
        if (isInt()) return std::to_string(std::get<long long>(val));
        if (isFloat()) {
            std::stringstream ss;
            ss << std::get<double>(val);
            return ss.str();
        }
        if (isBool()) return std::get<bool>(val) ? "true" : "false";
        if (isChar()) return std::string(1, std::get<char>(val));
        if (isString()) return std::get<std::string>(val);
        if (isArray()) {
            auto arr = std::get<std::shared_ptr<ArrayValue>>(val);
            std::stringstream ss;
            ss << "[";
            for (size_t i = 0; i < arr->elements.size(); ++i) {
                ss << arr->elements[i].toString();
                if (i + 1 < arr->elements.size()) ss << ", ";
            }
            ss << "]";
            return ss.str();
        }
        if (isMap()) {
            auto map = std::get<std::shared_ptr<MapValue>>(val);
            std::stringstream ss;
            ss << "{";
            size_t idx = 0;
            for (const auto& [k, v] : map->entries) {
                ss << k << ": " << v.toString();
                if (++idx < map->entries.size()) ss << ", ";
            }
            ss << "}";
            return ss.str();
        }
        if (isFunction()) return "<function>";
        if (isClass()) return "<class " + std::get<std::shared_ptr<ClassValue>>(val)->decl->name + ">";
        if (isInstance()) return "<instance of " + std::get<std::shared_ptr<InstanceValue>>(val)->klass->decl->name + ">";
        if (isBuiltIn()) return "<builtin>";
        if (isError()) return "Error: " + std::get<std::shared_ptr<ErrorValue>>(val)->message;
        if (isEnum()) return std::get<std::shared_ptr<EnumValue>>(val)->name;
        return "unknown";
    }
};

} // namespace thalapathy
