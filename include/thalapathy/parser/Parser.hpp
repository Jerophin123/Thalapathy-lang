#pragma once
#include <vector>
#include <memory>
#include <string>
#include "thalapathy/lexer/Token.hpp"
#include "thalapathy/ast/AST.hpp"

namespace thalapathy {

enum class Precedence {
    NONE,
    ASSIGNMENT,  // = += -= *= /=
    TERNARY,     // ?:
    OR,          // ||
    AND,         // &&
    EQUALITY,    // == !=
    COMPARISON,  // < > <= >=
    SUM,         // + -
    PRODUCT,     // * / %
    UNARY,       // ! - ++ -- (prefix)
    AS_CAST,     // as
    CALL,        // . (member), [ (index), ( (call)
};

class Parser {
public:
    Parser(std::string filename_, std::vector<Token> tokens_);

    std::vector<std::unique_ptr<ASTNode>> parse();
    const std::vector<std::string>& getErrors() const { return errors; }

private:
    // Parsing helpers
    bool check(TokenType type) const;
    bool isAtEnd() const;
    Token peek() const;
    Token previous() const;
    Token advance();
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& message);
    std::string consumeNameLike(const std::string& message);
    void addError(const Token& token, const std::string& message);
    void synchronize();

    // Declarations
    std::unique_ptr<ASTNode> declaration();
    std::unique_ptr<ASTNode> entryBlockDeclaration();
    std::unique_ptr<VarDecl> variableDeclaration(bool isImmutable, bool isConstant);
    std::unique_ptr<NanbiDecl> nanbiDeclaration();
    void parseNanbiPattern(bool& isObjectPattern, std::vector<NanbiBinding>& out);
    std::unique_ptr<ASTNode> matchStatement();
    std::unique_ptr<ConstructorDecl> parseConstructorRest(const SourceSpan& span);
    std::unique_ptr<FuncDecl> functionDeclaration();
    std::unique_ptr<ClassDecl> classDeclaration();
    std::unique_ptr<ClassDecl> classDeclaration(bool isAbstractClass, bool isFinalClass);
    std::unique_ptr<InterfaceDecl> interfaceDeclaration();
    std::unique_ptr<EnumDecl> enumDeclaration();
    std::string parseTypeString();
    std::string parseDottedName(const std::string& what);

    // Statements
    std::unique_ptr<ASTNode> statement();
    std::unique_ptr<BlockStmt> blockStatement();
    std::unique_ptr<ExprStmt> expressionStatement();
    std::unique_ptr<IfStmt> ifStatement();
    std::unique_ptr<ForStmt> forStatement();
    std::unique_ptr<ASTNode> rangeLoopStatement();
    std::unique_ptr<WhileStmt> whileStatement();
    std::unique_ptr<SwitchStmt> switchStatement();
    std::unique_ptr<ReturnStmt> returnStatement();
    std::unique_ptr<ThrowStmt> throwStatement();
    std::unique_ptr<TryCatchStmt> tryCatchStatement();
    std::unique_ptr<BreakStmt> breakStatement();
    std::unique_ptr<ContinueStmt> continueStatement();

    // Expressions (Pratt Parsing)
    std::unique_ptr<ASTNode> expression(Precedence precedence = Precedence::NONE);
    std::unique_ptr<ASTNode> parsePrefix(const Token& token);
    std::unique_ptr<ASTNode> lambdaExpression(const Token& kuttyToken, bool isAsync = false);
    std::unique_ptr<ASTNode> parseInfix(const Token& token, std::unique_ptr<ASTNode> left);
    Precedence getPrecedence(TokenType type) const;

    std::string filename;
    std::vector<Token> tokens;
    size_t current = 0;
    std::vector<std::string> errors;
};

} // namespace thalapathy
