#include "thalapathy/format/Formatter.hpp"
#include <vector>
#include <cctype>
#include <sstream>
#include <algorithm>

namespace thalapathy {

enum class FmtTokenType {
    KEYWORD_OR_ID,
    LITERAL,
    BINARY_OP,
    UNARY_OP,
    OPEN_BRACE,
    CLOSE_BRACE,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    COMMA,
    SEMICOLON,
    COLON,
    DOT,
    ARROW,
    COMMENT,
    NEWLINE,
    SPACE
};

struct FmtToken {
    FmtTokenType type;
    std::string text;
};

static std::vector<FmtToken> tokenizeForFormat(const std::string& src) {
    std::vector<FmtToken> tokens;
    size_t i = 0;
    size_t n = src.length();
    while (i < n) {
        char c = src[i];
        
        // Newline
        if (c == '\n') {
            tokens.push_back({FmtTokenType::NEWLINE, "\n"});
            i++;
            continue;
        }
        
        // Whitespace
        if (c == ' ' || c == '\t' || c == '\r') {
            while (i < n && (src[i] == ' ' || src[i] == '\t' || src[i] == '\r')) {
                i++;
            }
            tokens.push_back({FmtTokenType::SPACE, " "});
            continue;
        }
        
        // Single-line comment
        if (c == '/' && i + 1 < n && src[i + 1] == '/') {
            std::string text = "";
            while (i < n && src[i] != '\n') {
                text += src[i];
                i++;
            }
            tokens.push_back({FmtTokenType::COMMENT, text});
            continue;
        }
        
        // Block comment
        if (c == '/' && i + 1 < n && src[i + 1] == '*') {
            std::string text = "/*";
            i += 2;
            while (i < n) {
                if (src[i] == '*' && i + 1 < n && src[i + 1] == '/') {
                    text += "*/";
                    i += 2;
                    break;
                }
                text += src[i];
                i++;
            }
            tokens.push_back({FmtTokenType::COMMENT, text});
            continue;
        }
        
        // String literal
        if (c == '"') {
            std::string text = "\"";
            i++;
            while (i < n) {
                if (src[i] == '"') {
                    text += '"';
                    i++;
                    break;
                }
                if (src[i] == '\\' && i + 1 < n) {
                    text += '\\';
                    text += src[i + 1];
                    i += 2;
                } else {
                    text += src[i];
                    i++;
                }
            }
            tokens.push_back({FmtTokenType::LITERAL, text});
            continue;
        }
        
        // Char literal
        if (c == '\'') {
            std::string text = "'";
            i++;
            while (i < n) {
                if (src[i] == '\'') {
                    text += '\'';
                    i++;
                    break;
                }
                if (src[i] == '\\' && i + 1 < n) {
                    text += '\\';
                    text += src[i + 1];
                    i += 2;
                } else {
                    text += src[i];
                    i++;
                }
            }
            tokens.push_back({FmtTokenType::LITERAL, text});
            continue;
        }
        
        // Multi-character operators
        if (i + 1 < n) {
            std::string op2 = src.substr(i, 2);
            if (op2 == "==" || op2 == "!=" || op2 == "<=" || op2 == ">=" ||
                op2 == "&&" || op2 == "||" || op2 == "+=" || op2 == "-=" ||
                op2 == "*=" || op2 == "/=" || op2 == "%=") {
                tokens.push_back({FmtTokenType::BINARY_OP, op2});
                i += 2;
                continue;
            }
            if (op2 == "++" || op2 == "--") {
                tokens.push_back({FmtTokenType::UNARY_OP, op2});
                i += 2;
                continue;
            }
            if (op2 == "->") {
                tokens.push_back({FmtTokenType::ARROW, "->"});
                i += 2;
                continue;
            }
        }
        
        // Single-character binary operators
        if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '=' || c == '<' || c == '>') {
            tokens.push_back({FmtTokenType::BINARY_OP, std::string(1, c)});
            i++;
            continue;
        }
        
        // Unary operator
        if (c == '!') {
            tokens.push_back({FmtTokenType::UNARY_OP, "!"});
            i++;
            continue;
        }
        
        // Punctuation
        if (c == '{') {
            tokens.push_back({FmtTokenType::OPEN_BRACE, "{"});
            i++;
            continue;
        }
        if (c == '}') {
            tokens.push_back({FmtTokenType::CLOSE_BRACE, "}"});
            i++;
            continue;
        }
        if (c == '(') {
            tokens.push_back({FmtTokenType::OPEN_PAREN, "("});
            i++;
            continue;
        }
        if (c == ')') {
            tokens.push_back({FmtTokenType::CLOSE_PAREN, ")"});
            i++;
            continue;
        }
        if (c == '[') {
            tokens.push_back({FmtTokenType::OPEN_BRACKET, "["});
            i++;
            continue;
        }
        if (c == ']') {
            tokens.push_back({FmtTokenType::CLOSE_BRACKET, "]"});
            i++;
            continue;
        }
        if (c == ',') {
            tokens.push_back({FmtTokenType::COMMA, ","});
            i++;
            continue;
        }
        if (c == ';') {
            tokens.push_back({FmtTokenType::SEMICOLON, ";"});
            i++;
            continue;
        }
        if (c == ':') {
            tokens.push_back({FmtTokenType::COLON, ":"});
            i++;
            continue;
        }
        if (c == '.') {
            tokens.push_back({FmtTokenType::DOT, "."});
            i++;
            continue;
        }
        
        // Word
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
            std::string text = "";
            while (i < n && (std::isalnum(static_cast<unsigned char>(src[i])) || src[i] == '_')) {
                text += src[i];
                i++;
            }
            tokens.push_back({FmtTokenType::KEYWORD_OR_ID, text});
            continue;
        }
        
        // Fallback
        tokens.push_back({FmtTokenType::KEYWORD_OR_ID, std::string(1, c)});
        i++;
    }
    return tokens;
}

Formatter::Formatter(FormatterConfig config) : config_(std::move(config)) {}

std::string Formatter::format(const std::string& source, std::string& error) {
    (void)error;
    int indentStep = static_cast<int>(config_.indentSize);
    std::vector<FmtToken> tokens = tokenizeForFormat(source);
    
    // Filter out all original space tokens and normalize newlines
    std::vector<FmtToken> cleanTokens;
    for (size_t idx = 0; idx < tokens.size(); ++idx) {
        const auto& t = tokens[idx];
        if (t.type == FmtTokenType::SPACE) {
            continue;
        } else if (t.type == FmtTokenType::NEWLINE) {
            if (!cleanTokens.empty() && cleanTokens.back().type == FmtTokenType::NEWLINE) {
                if (cleanTokens.size() >= 2 && cleanTokens[cleanTokens.size() - 2].type == FmtTokenType::NEWLINE) {
                    continue;
                }
            }
            cleanTokens.push_back(t);
        } else {
            cleanTokens.push_back(t);
        }
    }

    std::string out = "";
    int indent = 0;
    bool startOfLine = true;
    int bigilDepth = 0;

    for (size_t idx = 0; idx < cleanTokens.size(); ++idx) {
        const auto& t = cleanTokens[idx];

        if (t.type == FmtTokenType::NEWLINE) {
            while (!out.empty() && out.back() == ' ') {
                out.pop_back();
            }
            out += "\n";
            startOfLine = true;
            continue;
        }

        if (startOfLine) {
            if (t.type == FmtTokenType::CLOSE_BRACE) {
                indent = std::max(0, indent - indentStep);
            }
            out += std::string(static_cast<size_t>(indent), ' ');
            startOfLine = false;
        }

        // Space before current token
        if (!out.empty() && out.back() != '\n' && out.back() != ' ') {
            bool needSpaceBefore = false;
            
            if (t.type == FmtTokenType::BINARY_OP) {
                needSpaceBefore = true;
            } else if (t.type == FmtTokenType::OPEN_BRACE) {
                needSpaceBefore = true;
            } else if (t.type == FmtTokenType::COLON) {
                needSpaceBefore = true;
            } else if (t.type == FmtTokenType::ARROW) {
                needSpaceBefore = true;
            } else if (t.type == FmtTokenType::COMMENT) {
                needSpaceBefore = true;
            } else if (t.type == FmtTokenType::OPEN_PAREN) {
                if (idx > 0) {
                    const auto& prev = cleanTokens[idx - 1];
                    if (prev.text == "ghilli" || prev.text == "bigil" || prev.text == "vaathi" || prev.text == "kaavalan" || prev.text == "thiruppi" || prev.text == "if" || prev.text == "for" || prev.text == "while") {
                        needSpaceBefore = true;
                    }
                }
            } else {
                if (idx > 0) {
                    const auto& prev = cleanTokens[idx - 1];
                    if (prev.type == FmtTokenType::BINARY_OP || prev.type == FmtTokenType::COMMA || prev.type == FmtTokenType::CLOSE_BRACE || prev.type == FmtTokenType::ARROW || prev.type == FmtTokenType::COLON) {
                        needSpaceBefore = true;
                    } else if ((prev.type == FmtTokenType::KEYWORD_OR_ID || prev.type == FmtTokenType::LITERAL) &&
                               (t.type == FmtTokenType::KEYWORD_OR_ID || t.type == FmtTokenType::LITERAL)) {
                        needSpaceBefore = true;
                    }
                }
            }

            if (needSpaceBefore) {
                out += " ";
            }
        }

        if (t.type == FmtTokenType::KEYWORD_OR_ID && t.text == "bigil") {
            size_t searchIdx = idx + 1;
            while (searchIdx < cleanTokens.size() && cleanTokens[searchIdx].type == FmtTokenType::SPACE) {
                searchIdx++;
            }
            if (searchIdx < cleanTokens.size() && cleanTokens[searchIdx].type == FmtTokenType::OPEN_PAREN) {
                bigilDepth = 1;
            }
        } else if (bigilDepth > 0) {
            if (t.type == FmtTokenType::OPEN_PAREN) {
                bigilDepth++;
            } else if (t.type == FmtTokenType::CLOSE_PAREN) {
                bigilDepth--;
            }
        }

        out += t.text;

        if (t.type == FmtTokenType::OPEN_BRACE) {
            indent += indentStep;
            if (idx + 1 < cleanTokens.size() && cleanTokens[idx + 1].type != FmtTokenType::NEWLINE) {
                out += "\n";
                startOfLine = true;
            }
        } else if (t.type == FmtTokenType::CLOSE_BRACE) {
            if (idx + 1 < cleanTokens.size()) {
                const auto& next = cleanTokens[idx + 1];
                if (next.type != FmtTokenType::NEWLINE && next.text != "illana" && next.type != FmtTokenType::SEMICOLON) {
                    out += "\n";
                    startOfLine = true;
                }
            }
        } else if (t.type == FmtTokenType::SEMICOLON) {
            if (bigilDepth == 0) {
                if (idx + 1 < cleanTokens.size() && 
                    cleanTokens[idx + 1].type != FmtTokenType::NEWLINE && 
                    cleanTokens[idx + 1].type != FmtTokenType::COMMENT) {
                    out += "\n";
                    startOfLine = true;
                }
            }
        }
    }

    while (!out.empty() && (out.back() == ' ' || out.back() == '\n' || out.back() == '\r')) {
        out.pop_back();
    }
    if (config_.insertFinalNewline) {
        out += "\n";
    }

    return out;
}

} // namespace thalapathy
