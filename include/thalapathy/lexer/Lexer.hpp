#pragma once
#include <string>
#include <vector>
#include "Token.hpp"

namespace thalapathy {

class Lexer {
public:
    Lexer(std::string filename_, std::string source_);

    std::vector<Token> tokenize();
    const std::vector<std::string>& getErrors() const { return errors; }

private:
    char peek() const;
    char peekNext() const;
    char advance();
    bool isAtEnd() const;

    void skipWhitespaceAndComments();
    Token scanToken();
    Token scanIdentifierOrKeyword();
    Token scanNumber();
    Token scanString();
    Token scanChar();
    
    SourceSpan makeSpan(size_t startOffset, int startLine, int startCol) const;
    void addError(const SourceSpan& span, const std::string& message);

    std::string filename;
    std::string source;
    size_t offset = 0;
    int line = 1;
    int col = 1;

    std::vector<std::string> errors;
};

} // namespace thalapathy
