#pragma once
#include <string>
#include "thalapathy/types/Types.hpp"
#include "thalapathy/lexer/Token.hpp"

namespace thalapathy {

enum class SymbolKind {
    VARIABLE,
    FUNCTION,
    CLASS,
    PARAMETER
};

struct Symbol {
    std::string name;
    Type type;
    SymbolKind kind;
    bool isMutable = true;
    bool isConstant = false;
    SourceSpan span;
};

} // namespace thalapathy
