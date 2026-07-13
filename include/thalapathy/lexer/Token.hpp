#pragma once
#include <string>
#include <string_view>

namespace thalapathy {

struct SourceSpan {
    std::string filename;
    int start_line = 1;
    int start_col = 1;
    int end_line = 1;
    int end_col = 1;
    size_t byte_offset = 0;
    size_t length = 0;
};

enum class TokenType {
    // Keywords
    THALAPATHY,
    AARAMBAM,
    NANBA,
    MAKKAL,
    URUTHI,
    GHILLI,
    ILLANA,
    BIGIL,
    VAATHI,
    MERSAL,
    THIRUPPI,
    SOLLU,
    MASTER,
    VARISU,
    KATHTHI,
    THERI,
    POKKIRI,
    KAAVALAN,
    BEAST,
    LEO,
    SARKAR,
    WAITING,
    GOAT,

    // Conventional technical keywords
    INT_TYPE,
    FLOAT_TYPE,
    BOOL_TYPE,
    STRING_TYPE,
    CHAR_TYPE,
    VOID_TYPE,
    NULL_VAL,
    TRUE_VAL,
    FALSE_VAL,
    THIS,
    SUPER,
    BREAK,
    CONTINUE,
    PAARU,
    MAATHU,
    SEYAL,
    STATIC,
    AS,
    AAGUMA,
    OPERATOR,

    // Literals & Identifiers
    IDENTIFIER,
    LITERAL_INT,
    LITERAL_FLOAT,
    LITERAL_STRING,
    LITERAL_CHAR,

    // Operators
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,
    EQ_EQ,
    BANG_EQ,
    LT,
    GT,
    LT_EQ,
    GT_EQ,
    AMP_AMP,
    BAR_BAR,
    BANG,
    EQUAL,
    PLUS_EQ,
    MINUS_EQ,
    STAR_EQ,
    SLASH_EQ,
    PLUS_PLUS,
    MINUS_MINUS,

    // Punctuation
    SEMICOLON,
    COMMA,
    DOT,
    COLON,
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    ARROW,

    // End of file / Special
    EOF_TOKEN,
    ERROR
};

struct Token {
    TokenType type;
    SourceSpan span;
    std::string value;
};

inline std::string_view tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::THALAPATHY: return "thalapathy";
        case TokenType::AARAMBAM: return "aarambam";
        case TokenType::NANBA: return "nanba";
        case TokenType::MAKKAL: return "makkal";
        case TokenType::URUTHI: return "uruthi";
        case TokenType::GHILLI: return "ghilli";
        case TokenType::ILLANA: return "illana";
        case TokenType::BIGIL: return "bigil";
        case TokenType::VAATHI: return "vaathi";
        case TokenType::MERSAL: return "mersal";
        case TokenType::THIRUPPI: return "thiruppi";
        case TokenType::SOLLU: return "sollu";
        case TokenType::MASTER: return "master";
        case TokenType::VARISU: return "varisu";
        case TokenType::KATHTHI: return "kaththi";
        case TokenType::THERI: return "theri";
        case TokenType::POKKIRI: return "pokkiri";
        case TokenType::KAAVALAN: return "kaavalan";
        case TokenType::BEAST: return "beast";
        case TokenType::LEO: return "leo";
        case TokenType::SARKAR: return "sarkar";
        case TokenType::WAITING: return "waiting";
        case TokenType::GOAT: return "goat";
        case TokenType::INT_TYPE: return "int";
        case TokenType::FLOAT_TYPE: return "float";
        case TokenType::BOOL_TYPE: return "bool";
        case TokenType::STRING_TYPE: return "string";
        case TokenType::CHAR_TYPE: return "char";
        case TokenType::VOID_TYPE: return "void";
        case TokenType::NULL_VAL: return "null";
        case TokenType::TRUE_VAL: return "true";
        case TokenType::FALSE_VAL: return "false";
        case TokenType::THIS: return "this";
        case TokenType::SUPER: return "super";
        case TokenType::BREAK: return "break";
        case TokenType::CONTINUE: return "continue";
        case TokenType::PAARU: return "paaru";
        case TokenType::MAATHU: return "maathu";
        case TokenType::SEYAL: return "seyal";
        case TokenType::STATIC: return "static";
        case TokenType::AS: return "as";
        case TokenType::AAGUMA: return "aaguma";
        case TokenType::OPERATOR: return "operator";
        case TokenType::IDENTIFIER: return "identifier";
        case TokenType::LITERAL_INT: return "integer literal";
        case TokenType::LITERAL_FLOAT: return "float literal";
        case TokenType::LITERAL_STRING: return "string literal";
        case TokenType::LITERAL_CHAR: return "char literal";
        case TokenType::PLUS: return "+";
        case TokenType::MINUS: return "-";
        case TokenType::STAR: return "*";
        case TokenType::SLASH: return "/";
        case TokenType::PERCENT: return "%";
        case TokenType::EQ_EQ: return "==";
        case TokenType::BANG_EQ: return "!=";
        case TokenType::LT: return "<";
        case TokenType::GT: return ">";
        case TokenType::LT_EQ: return "<=";
        case TokenType::GT_EQ: return ">=";
        case TokenType::AMP_AMP: return "&&";
        case TokenType::BAR_BAR: return "||";
        case TokenType::BANG: return "!";
        case TokenType::EQUAL: return "=";
        case TokenType::PLUS_EQ: return "+=";
        case TokenType::MINUS_EQ: return "-=";
        case TokenType::STAR_EQ: return "*=";
        case TokenType::SLASH_EQ: return "/=";
        case TokenType::PLUS_PLUS: return "++";
        case TokenType::MINUS_MINUS: return "--";
        case TokenType::SEMICOLON: return ";";
        case TokenType::COMMA: return ",";
        case TokenType::DOT: return ".";
        case TokenType::COLON: return ":";
        case TokenType::LEFT_PAREN: return "(";
        case TokenType::RIGHT_PAREN: return ")";
        case TokenType::LEFT_BRACE: return "{";
        case TokenType::RIGHT_BRACE: return "}";
        case TokenType::LEFT_BRACKET: return "[";
        case TokenType::RIGHT_BRACKET: return "]";
        case TokenType::ARROW: return "->";
        case TokenType::EOF_TOKEN: return "EOF";
        case TokenType::ERROR: return "ERROR";
    }
    return "UNKNOWN";
}

} // namespace thalapathy
