#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <stdexcept>
#include "Value.hpp"

namespace thalapathy {

class Environment : public std::enable_shared_from_this<Environment> {
public:
    Environment(std::shared_ptr<Environment> parent = nullptr)
        : enclosing(std::move(parent)) {}

    void define(const std::string& name, const Value& value) {
        values[name] = value;
    }

    bool assign(const std::string& name, const Value& value) {
        auto it = values.find(name);
        if (it != values.end()) {
            it->second = value;
            return true;
        }
        if (enclosing) {
            return enclosing->assign(name, value);
        }
        return false;
    }

    Value get(const std::string& name, bool& found) const {
        auto it = values.find(name);
        if (it != values.end()) {
            found = true;
            return it->second;
        }
        if (enclosing) {
            return enclosing->get(name, found);
        }
        found = false;
        return Value{std::monostate{}};
    }

    std::shared_ptr<Environment> getEnclosing() const {
        return enclosing;
    }

    const std::unordered_map<std::string, Value>& getValues() const {
        return values;
    }

    void clear() {
        values.clear();
    }

private:
    std::shared_ptr<Environment> enclosing;
    std::unordered_map<std::string, Value> values;
};

} // namespace thalapathy
