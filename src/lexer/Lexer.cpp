#include "thalapathy/lexer/Lexer.hpp"
#include <cctype>
#include <unordered_map>
#include <sstream>

namespace thalapathy {

static const std::unordered_map<std::string, TokenType> KEYWORDS = {
    {"thalapathy", TokenType::THALAPATHY},
    {"aarambam", TokenType::AARAMBAM},
    {"nanba", TokenType::NANBA},
    {"nanbi", TokenType::NANBI},
    {"makkal", TokenType::MAKKAL},
    {"uruthi", TokenType::URUTHI},
    {"ghilli", TokenType::GHILLI},
    {"illana", TokenType::ILLANA},
    {"bigil", TokenType::BIGIL},
    {"vaathi", TokenType::VAATHI},
    {"mersal", TokenType::MERSAL},
    {"thiruppi", TokenType::THIRUPPI},
    {"sollu", TokenType::SOLLU},
    {"master", TokenType::MASTER},
    {"varisu", TokenType::VARISU},
    {"kaththi", TokenType::KATHTHI},
    {"theri", TokenType::THERI},
    {"pokkiri", TokenType::POKKIRI},
    {"kaavalan", TokenType::KAAVALAN},
    {"beast", TokenType::BEAST},
    {"leo", TokenType::LEO},
    {"sarkar", TokenType::SARKAR},
    {"waiting", TokenType::WAITING},
    {"goat", TokenType::GOAT},
    {"thuppakki", TokenType::THUPPAKKI},
    {"thalaivaa", TokenType::THALAIVAA},
    {"vazhakku", TokenType::VAZHAKKU},
    {"kutty", TokenType::KUTTY},
    {"nadu", TokenType::NADU},
    {"vagai", TokenType::VAGAI},
    {"kadaisi", TokenType::KADAISI},
    {"varum", TokenType::VARUM},
    {"kaathiru", TokenType::KAATHIRU},

    // Conventional
    {"int", TokenType::INT_TYPE},
    {"float", TokenType::FLOAT_TYPE},
    {"bool", TokenType::BOOL_TYPE},
    {"string", TokenType::STRING_TYPE},
    {"char", TokenType::CHAR_TYPE},
    {"void", TokenType::VOID_TYPE},
    {"null", TokenType::NULL_VAL},
    {"true", TokenType::TRUE_VAL},
    {"false", TokenType::FALSE_VAL},
    {"this", TokenType::THIS},
    {"super", TokenType::SUPER},
    {"break", TokenType::BREAK},
    {"continue", TokenType::CONTINUE},
    {"paaru", TokenType::PAARU},
    {"maathu", TokenType::MAATHU},
    {"seyal", TokenType::SEYAL},
    {"static", TokenType::STATIC},
    {"as", TokenType::AS},
    {"aaguma", TokenType::AAGUMA},
    {"operator", TokenType::OPERATOR},

    // -------- Canonical THALAPATHY syntax (alias existing grammar tokens) --------
    // These make the THALAPATHY spelling first-class; the English spellings above
    // remain as legacy compatibility aliases. Whole-word lookup keeps look-alike
    // identifiers safe: `illana`/`illavarasan`/`naanga`/`ullam` stay identifiers.
    {"aama", TokenType::TRUE_VAL},        // aama      == true
    {"illa", TokenType::FALSE_VAL},       // illa      == false
    {"onnumilla", TokenType::NULL_VAL},   // onnumilla == null
    {"naan", TokenType::THIS},            // naan      == this
    {"interval", TokenType::BREAK},       // interval  == break
    {"aduthu", TokenType::CONTINUE},      // aduthu    == continue
    {"podhu", TokenType::STATIC},         // podhu     == static
    {"maathiko", TokenType::AS},          // maathiko  == as (cast)
    {"munnadi", TokenType::SUPER},        // munnadi   == super (kootam.munnadi renamed to .mun)
    {"comeback", TokenType::COMEBACK},    // comeback  == override (method modifier)
    {"yaaru", TokenType::YAARU},          // pattern match:  yaaru <expr> { ivan <p> {..} yaarumilla {..} }
    {"ivan", TokenType::IVAN},            // match arm
    {"yaarumilla", TokenType::YAARUMILLA}, // default arm
    {"mudivu", TokenType::URUTHI}          // mudivu == final method / const (satham FATAL renamed to .saavu)
};

Lexer::Lexer(std::string filename_, std::string source_)
    : filename(std::move(filename_)), source(std::move(source_)) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        skipWhitespaceAndComments();
        if (isAtEnd()) break;
        tokens.push_back(scanToken());
    }
    // Add EOF token
    SourceSpan eofSpan = makeSpan(offset, line, col);
    tokens.push_back(Token{TokenType::EOF_TOKEN, eofSpan, ""});
    return tokens;
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[offset];
}

char Lexer::peekNext() const {
    if (offset + 1 >= source.length()) return '\0';
    return source[offset + 1];
}

char Lexer::advance() {
    char c = source[offset++];
    if (c == '\n') {
        line++;
        col = 1;
    } else {
        col++;
    }
    return c;
}

bool Lexer::isAtEnd() const {
    return offset >= source.length();
}

void Lexer::skipWhitespaceAndComments() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\r' || c == '\t' || c == '\n') {
            advance();
        } else if (c == '/' && peekNext() == '/') {
            // Single-line comment
            while (peek() != '\n' && !isAtEnd()) {
                advance();
            }
        } else if (c == '/' && peekNext() == '*') {
            // Block comment
            advance(); // '/'
            advance(); // '*'
            while (!isAtEnd() && !(peek() == '*' && peekNext() == '/')) {
                advance();
            }
            if (!isAtEnd()) {
                advance(); // '*'
                advance(); // '/'
            } else {
                // Unterminated block comment
                SourceSpan span = makeSpan(offset - 2, line, col - 2);
                addError(span, "unterminated block comment");
            }
        } else {
            break;
        }
    }
}

Token Lexer::scanToken() {
    size_t startOffset = offset;
    int startLine = line;
    int startCol = col;

    char c = advance();

    // Identifiers and Keywords
    if (std::isalpha(c) || c == '_') {
        offset--; // step back to scan full identifier
        col--;
        return scanIdentifierOrKeyword();
    }

    // Numbers
    if (std::isdigit(c)) {
        offset--;
        col--;
        return scanNumber();
    }

    switch (c) {
        // Punctuation
        case '(': return Token{TokenType::LEFT_PAREN, makeSpan(startOffset, startLine, startCol), "("};
        case ')': return Token{TokenType::RIGHT_PAREN, makeSpan(startOffset, startLine, startCol), ")"};
        case '{': return Token{TokenType::LEFT_BRACE, makeSpan(startOffset, startLine, startCol), "{"};
        case '}': return Token{TokenType::RIGHT_BRACE, makeSpan(startOffset, startLine, startCol), "}"};
        case '[': return Token{TokenType::LEFT_BRACKET, makeSpan(startOffset, startLine, startCol), "["};
        case ']': return Token{TokenType::RIGHT_BRACKET, makeSpan(startOffset, startLine, startCol), "]"};
        case ';': return Token{TokenType::SEMICOLON, makeSpan(startOffset, startLine, startCol), ";"};
        case ',': return Token{TokenType::COMMA, makeSpan(startOffset, startLine, startCol), ","};
        case ':': return Token{TokenType::COLON, makeSpan(startOffset, startLine, startCol), ":"};
        
        case '.': return Token{TokenType::DOT, makeSpan(startOffset, startLine, startCol), "."};
        case '?': return Token{TokenType::QUESTION, makeSpan(startOffset, startLine, startCol), "?"};

        // Strings
        case '"':
            offset--;
            col--;
            return scanString();

        // Characters
        case '\'':
            offset--;
            col--;
            return scanChar();

        // Operators
        case '+':
            if (peek() == '+') {
                advance();
                return Token{TokenType::PLUS_PLUS, makeSpan(startOffset, startLine, startCol), "++"};
            } else if (peek() == '=') {
                advance();
                return Token{TokenType::PLUS_EQ, makeSpan(startOffset, startLine, startCol), "+="};
            }
            return Token{TokenType::PLUS, makeSpan(startOffset, startLine, startCol), "+"};

        case '-':
            if (peek() == '-') {
                advance();
                return Token{TokenType::MINUS_MINUS, makeSpan(startOffset, startLine, startCol), "--"};
            } else if (peek() == '=') {
                advance();
                return Token{TokenType::MINUS_EQ, makeSpan(startOffset, startLine, startCol), "-="};
            } else if (peek() == '>') {
                advance();
                return Token{TokenType::ARROW, makeSpan(startOffset, startLine, startCol), "->"};
            }
            return Token{TokenType::MINUS, makeSpan(startOffset, startLine, startCol), "-"};

        case '*':
            if (peek() == '=') {
                advance();
                return Token{TokenType::STAR_EQ, makeSpan(startOffset, startLine, startCol), "*="};
            }
            return Token{TokenType::STAR, makeSpan(startOffset, startLine, startCol), "*"};

        case '/':
            if (peek() == '=') {
                advance();
                return Token{TokenType::SLASH_EQ, makeSpan(startOffset, startLine, startCol), "/="};
            }
            return Token{TokenType::SLASH, makeSpan(startOffset, startLine, startCol), "/"};

        case '%':
            return Token{TokenType::PERCENT, makeSpan(startOffset, startLine, startCol), "%"};

        case '=':
            if (peek() == '=') {
                advance();
                return Token{TokenType::EQ_EQ, makeSpan(startOffset, startLine, startCol), "=="};
            }
            return Token{TokenType::EQUAL, makeSpan(startOffset, startLine, startCol), "="};

        case '!':
            if (peek() == '=') {
                advance();
                return Token{TokenType::BANG_EQ, makeSpan(startOffset, startLine, startCol), "!="};
            }
            return Token{TokenType::BANG, makeSpan(startOffset, startLine, startCol), "!"};

        case '<':
            if (peek() == '=') {
                advance();
                return Token{TokenType::LT_EQ, makeSpan(startOffset, startLine, startCol), "<="};
            }
            return Token{TokenType::LT, makeSpan(startOffset, startLine, startCol), "<"};

        case '>':
            if (peek() == '=') {
                advance();
                return Token{TokenType::GT_EQ, makeSpan(startOffset, startLine, startCol), ">="};
            }
            return Token{TokenType::GT, makeSpan(startOffset, startLine, startCol), ">"};

        case '&':
            if (peek() == '&') {
                advance();
                return Token{TokenType::AMP_AMP, makeSpan(startOffset, startLine, startCol), "&&"};
            }
            break;

        case '|':
            if (peek() == '|') {
                advance();
                return Token{TokenType::BAR_BAR, makeSpan(startOffset, startLine, startCol), "||"};
            }
            break;
    }

    std::string msg = "unexpected character: '";
    msg += c;
    msg += "'";
    SourceSpan span = makeSpan(startOffset, startLine, startCol);
    addError(span, msg);
    return Token{TokenType::ERROR, span, std::string(1, c)};
}

Token Lexer::scanIdentifierOrKeyword() {
    size_t startOffset = offset;
    int startLine = line;
    int startCol = col;

    while (std::isalnum(peek()) || peek() == '_') {
        advance();
    }

    std::string value = source.substr(startOffset, offset - startOffset);
    TokenType type = TokenType::IDENTIFIER;
    auto it = KEYWORDS.find(value);
    if (it != KEYWORDS.end()) {
        type = it->second;
    }

    return Token{type, makeSpan(startOffset, startLine, startCol), value};
}

Token Lexer::scanNumber() {
    size_t startOffset = offset;
    int startLine = line;
    int startCol = col;

    while (std::isdigit(peek())) {
        advance();
    }

    bool isFloat = false;
    if (peek() == '.' && std::isdigit(peekNext())) {
        isFloat = true;
        advance(); // consume '.'
        while (std::isdigit(peek())) {
            advance();
        }
    }

    std::string value = source.substr(startOffset, offset - startOffset);
    TokenType type = isFloat ? TokenType::LITERAL_FLOAT : TokenType::LITERAL_INT;
    return Token{type, makeSpan(startOffset, startLine, startCol), value};
}

Token Lexer::scanString() {
    size_t startOffset = offset;
    int startLine = line;
    int startCol = col;

    advance(); // consume opening quote '"'
    std::string value;

    while (peek() != '"' && !isAtEnd()) {
        char c = advance();
        if (c == '\\') {
            if (isAtEnd()) break;
            char esc = advance();
            switch (esc) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                default: value += esc; break;
            }
        } else {
            value += c;
        }
    }

    if (isAtEnd()) {
        SourceSpan span = makeSpan(startOffset, startLine, startCol);
        addError(span, "unterminated string literal");
        return Token{TokenType::ERROR, span, value};
    }

    advance(); // consume closing quote '"'
    return Token{TokenType::LITERAL_STRING, makeSpan(startOffset, startLine, startCol), value};
}

Token Lexer::scanChar() {
    size_t startOffset = offset;
    int startLine = line;
    int startCol = col;

    advance(); // consume opening quote '\''
    std::string value;

    if (peek() == '\'') {
        SourceSpan span = makeSpan(startOffset, startLine, startCol);
        addError(span, "empty character literal");
        advance();
        return Token{TokenType::ERROR, span, ""};
    }

    if (!isAtEnd()) {
        char c = advance();
        if (c == '\\') {
            if (!isAtEnd()) {
                char esc = advance();
                switch (esc) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '\'': value += '\''; break;
                    default: value += esc; break;
                }
            }
        } else {
            value += c;
        }
    }

    if (peek() != '\'') {
        SourceSpan span = makeSpan(startOffset, startLine, startCol);
        addError(span, "unterminated or invalid character literal");
        return Token{TokenType::ERROR, span, value};
    }

    advance(); // consume closing quote '\''
    return Token{TokenType::LITERAL_CHAR, makeSpan(startOffset, startLine, startCol), value};
}

SourceSpan Lexer::makeSpan(size_t startOffset, int startLine, int startCol) const {
    SourceSpan span;
    span.filename = filename;
    span.start_line = startLine;
    span.start_col = startCol;
    span.end_line = line;
    span.end_col = col;
    span.byte_offset = startOffset;
    span.length = offset - startOffset;
    return span;
}

void Lexer::addError(const SourceSpan& span, const std::string& message) {
    std::stringstream ss;
    ss << "lexical error: " << message << " at " << span.filename << ":" << span.start_line << ":" << span.start_col;
    errors.push_back(ss.str());
}

} // namespace thalapathy
