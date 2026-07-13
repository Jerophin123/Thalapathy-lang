#pragma once
#include <string>
#include <vector>
#include <memory>
#include <variant>
#include "thalapathy/lexer/Token.hpp"

namespace thalapathy {

// Forward declarations of AST nodes
struct ASTVisitor;

struct ASTNode {
    virtual ~ASTNode() = default;
    SourceSpan span;
    virtual void accept(ASTVisitor* visitor) = 0;
};

// Declarations
struct DeclNode : public ASTNode {};

struct VarDecl : public DeclNode {
    std::string name;
    std::string typeStr; // e.g. "int", "string", or empty if inferred
    std::unique_ptr<ASTNode> initializer;
    bool isMutable = true;
    bool isConstant = false;

    void accept(ASTVisitor* visitor) override;
};

struct FuncDecl : public DeclNode {
    std::string name;
    struct Param {
        std::string name;
        std::string typeStr;
        std::shared_ptr<ASTNode> defaultValue; // optional; evaluated when arg omitted
    };
    std::vector<Param> params;
    std::string returnTypeStr;
    std::unique_ptr<ASTNode> body; // usually a BlockStmt

    void accept(ASTVisitor* visitor) override;
};

enum class AccessModifier {
    PUBLIC,
    PROTECTED,
    PRIVATE
};

struct FieldDecl : public DeclNode {
    AccessModifier access = AccessModifier::PUBLIC;
    bool isStatic = false;
    bool isFinal = false;
    std::unique_ptr<VarDecl> varDecl;

    void accept(ASTVisitor* visitor) override;
};

struct MethodDecl : public DeclNode {
    AccessModifier access = AccessModifier::PUBLIC;
    bool isStatic = false;
    bool isAbstract = false;
    bool isFinal = false;
    bool isOverride = false;
    std::unique_ptr<FuncDecl> funcDecl;

    void accept(ASTVisitor* visitor) override;
};

struct ConstructorDecl : public DeclNode {
    std::vector<FuncDecl::Param> params;
    std::vector<std::unique_ptr<ASTNode>> initializerList; // super(...) or this(...)
    std::unique_ptr<ASTNode> body;

    void accept(ASTVisitor* visitor) override;
};

struct PropertyDecl : public DeclNode {
    AccessModifier access = AccessModifier::PUBLIC;
    bool isStatic = false;
    std::string name;
    std::string typeStr;
    std::unique_ptr<ASTNode> getterBody;
    std::unique_ptr<ASTNode> setterBody;
    std::string setterParamName; // "value"

    void accept(ASTVisitor* visitor) override;
};

struct OperatorDecl : public DeclNode {
    TokenType op;
    struct Param {
        std::string name;
        std::string typeStr;
    };
    Param param;
    std::string returnTypeStr;
    std::unique_ptr<ASTNode> body;

    void accept(ASTVisitor* visitor) override;
};

struct InterfaceDecl : public DeclNode {
    std::string name;
    std::vector<std::string> parentInterfaces;
    std::vector<std::unique_ptr<FuncDecl>> methodSignatures;

    void accept(ASTVisitor* visitor) override;
};

struct ClassDecl : public DeclNode {
    std::string name;
    std::string parentName; // empty if no parent
    std::vector<std::string> implementedInterfaces;
    std::vector<std::string> genericParams;
    bool isAbstract = false;
    bool isFinal = false;

    std::vector<std::unique_ptr<FieldDecl>> fields;
    std::vector<std::unique_ptr<MethodDecl>> methods;
    std::vector<std::unique_ptr<ConstructorDecl>> constructors;
    std::vector<std::unique_ptr<PropertyDecl>> properties;
    std::vector<std::unique_ptr<OperatorDecl>> operators;

    void accept(ASTVisitor* visitor) override;
};

// Statements
struct StmtNode : public ASTNode {};

struct BlockStmt : public StmtNode {
    std::vector<std::unique_ptr<ASTNode>> statements;

    void accept(ASTVisitor* visitor) override;
};

struct ExprStmt : public StmtNode {
    std::unique_ptr<ASTNode> expression;

    void accept(ASTVisitor* visitor) override;
};

struct IfStmt : public StmtNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> thenBranch;
    std::unique_ptr<ASTNode> elseBranch; // can be null

    void accept(ASTVisitor* visitor) override;
};

struct ForStmt : public StmtNode {
    std::unique_ptr<ASTNode> initializer; // VarDecl or ExprStmt or empty
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> update;
    std::unique_ptr<ASTNode> body;

    void accept(ASTVisitor* visitor) override;
};

struct RangeLoopStmt : public StmtNode {
    std::string varName;
    std::unique_ptr<ASTNode> startExpr;
    std::unique_ptr<ASTNode> endExpr;
    std::unique_ptr<ASTNode> body;

    void accept(ASTVisitor* visitor) override;
};

// foreach: `vaathi x in <iterable>` over arrays, strings, or map keys.
struct ForEachStmt : public StmtNode {
    std::string varName;
    std::unique_ptr<ASTNode> iterable;
    std::unique_ptr<ASTNode> body;

    void accept(ASTVisitor* visitor) override;
};

struct WhileStmt : public StmtNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;

    void accept(ASTVisitor* visitor) override;
};

struct SwitchStmt : public StmtNode {
    struct Case {
        std::unique_ptr<ASTNode> value;
        std::unique_ptr<ASTNode> body;
    };
    std::unique_ptr<ASTNode> subject;
    std::vector<Case> cases;
    std::unique_ptr<ASTNode> defaultBody; // can be null

    void accept(ASTVisitor* visitor) override;
};

struct ReturnStmt : public StmtNode {
    std::unique_ptr<ASTNode> expression; // can be null

    void accept(ASTVisitor* visitor) override;
};

struct ThrowStmt : public StmtNode {
    std::unique_ptr<ASTNode> expression;

    void accept(ASTVisitor* visitor) override;
};

struct TryCatchStmt : public StmtNode {
    std::unique_ptr<ASTNode> tryBody;
    std::string catchVarName;
    std::string catchTypeName;              // optional class/enum filter for the catch
    std::unique_ptr<ASTNode> catchBody;
    std::unique_ptr<ASTNode> finallyBody;   // optional `kadaisi { ... }` block

    void accept(ASTVisitor* visitor) override;
};

struct BreakStmt : public StmtNode {
    void accept(ASTVisitor* visitor) override;
};

struct ContinueStmt : public StmtNode {
    void accept(ASTVisitor* visitor) override;
};

// Java-style enum: `vagai Color { RED, GREEN, BLUE }`
struct EnumDecl : public DeclNode {
    std::string name;
    std::vector<std::string> members;

    void accept(ASTVisitor* visitor) override;
};

// Expressions
struct ExprNode : public ASTNode {};

struct LiteralExpr : public ExprNode {
    using ValType = std::variant<long long, double, bool, std::string, char, std::monostate>;
    ValType value;

    void accept(ASTVisitor* visitor) override;
};

struct IdentifierExpr : public ExprNode {
    std::string name;

    void accept(ASTVisitor* visitor) override;
};

struct BinaryExpr : public ExprNode {
    std::unique_ptr<ASTNode> left;
    TokenType op;
    std::unique_ptr<ASTNode> right;

    void accept(ASTVisitor* visitor) override;
};

struct UnaryExpr : public ExprNode {
    TokenType op;
    std::unique_ptr<ASTNode> operand;

    void accept(ASTVisitor* visitor) override;
};

// Ternary conditional: `cond ? whenTrue : whenFalse`
struct TernaryExpr : public ExprNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> thenExpr;
    std::unique_ptr<ASTNode> elseExpr;

    void accept(ASTVisitor* visitor) override;
};

struct AssignExpr : public ExprNode {
    std::unique_ptr<ASTNode> target; // IdentifierExpr, IndexExpr or MemberExpr
    TokenType op; // =, +=, -=, *=, /=
    std::unique_ptr<ASTNode> value;

    void accept(ASTVisitor* visitor) override;
};

struct CallExpr : public ExprNode {
    std::unique_ptr<ASTNode> callee;
    std::vector<std::unique_ptr<ASTNode>> arguments;

    void accept(ASTVisitor* visitor) override;
};

struct MemberExpr : public ExprNode {
    std::unique_ptr<ASTNode> object;
    std::string memberName;

    void accept(ASTVisitor* visitor) override;
};

struct IndexExpr : public ExprNode {
    std::unique_ptr<ASTNode> object;
    std::unique_ptr<ASTNode> index;

    void accept(ASTVisitor* visitor) override;
};

struct ArrayExpr : public ExprNode {
    std::vector<std::unique_ptr<ASTNode>> elements;

    void accept(ASTVisitor* visitor) override;
};

struct MapExpr : public ExprNode {
    std::vector<std::pair<std::string, std::unique_ptr<ASTNode>>> entries;

    void accept(ASTVisitor* visitor) override;
};

struct LambdaExpr : public ExprNode {
    std::unique_ptr<FuncDecl> fn; // anonymous function descriptor

    void accept(ASTVisitor* visitor) override;
};

struct ThisExpr : public ExprNode {
    void accept(ASTVisitor* visitor) override;
};

struct SuperExpr : public ExprNode {
    std::string method;

    void accept(ASTVisitor* visitor) override;
};

struct CastExpr : public ExprNode {
    std::unique_ptr<ASTNode> expression;
    std::string targetType;

    void accept(ASTVisitor* visitor) override;
};

struct TypeTestExpr : public ExprNode {
    std::unique_ptr<ASTNode> expression;
    std::string targetType;

    void accept(ASTVisitor* visitor) override;
};

struct EntryBlockDecl : public DeclNode {
    std::unique_ptr<ASTNode> body; // BlockStmt

    void accept(ASTVisitor* visitor) override;
};

struct ImportDecl : public DeclNode {
    std::string moduleName;

    void accept(ASTVisitor* visitor) override;
};

// Java-style package declaration: `nadu com.tvk.models;`
struct PackageDecl : public DeclNode {
    std::string packageName; // dotted, e.g. "com.tvk.models"

    void accept(ASTVisitor* visitor) override;
};

// Visitor Interface
struct ASTVisitor {
    virtual ~ASTVisitor() = default;
    
    virtual void visit(VarDecl* node) = 0;
    virtual void visit(FuncDecl* node) = 0;
    virtual void visit(ClassDecl* node) = 0;
    virtual void visit(EntryBlockDecl* node) = 0;
    virtual void visit(ImportDecl* node) = 0;
    virtual void visit(PackageDecl* node) = 0;
    virtual void visit(EnumDecl* node) = 0;
    virtual void visit(FieldDecl* node) = 0;
    virtual void visit(MethodDecl* node) = 0;
    virtual void visit(ConstructorDecl* node) = 0;
    virtual void visit(PropertyDecl* node) = 0;
    virtual void visit(OperatorDecl* node) = 0;
    virtual void visit(InterfaceDecl* node) = 0;
    virtual void visit(CastExpr* node) = 0;
    virtual void visit(TypeTestExpr* node) = 0;

    virtual void visit(BlockStmt* node) = 0;
    virtual void visit(ExprStmt* node) = 0;
    virtual void visit(IfStmt* node) = 0;
    virtual void visit(ForStmt* node) = 0;
    virtual void visit(RangeLoopStmt* node) = 0;
    virtual void visit(ForEachStmt* node) = 0;
    virtual void visit(WhileStmt* node) = 0;
    virtual void visit(SwitchStmt* node) = 0;
    virtual void visit(ReturnStmt* node) = 0;
    virtual void visit(ThrowStmt* node) = 0;
    virtual void visit(TryCatchStmt* node) = 0;
    virtual void visit(BreakStmt* node) = 0;
    virtual void visit(ContinueStmt* node) = 0;

    virtual void visit(LiteralExpr* node) = 0;
    virtual void visit(IdentifierExpr* node) = 0;
    virtual void visit(BinaryExpr* node) = 0;
    virtual void visit(UnaryExpr* node) = 0;
    virtual void visit(TernaryExpr* node) = 0;
    virtual void visit(AssignExpr* node) = 0;
    virtual void visit(CallExpr* node) = 0;
    virtual void visit(MemberExpr* node) = 0;
    virtual void visit(IndexExpr* node) = 0;
    virtual void visit(ArrayExpr* node) = 0;
    virtual void visit(MapExpr* node) = 0;
    virtual void visit(LambdaExpr* node) = 0;
    virtual void visit(ThisExpr* node) = 0;
    virtual void visit(SuperExpr* node) = 0;
};

// Out-of-line implementations of accept to avoid circular dependencies
inline void VarDecl::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void FuncDecl::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void ClassDecl::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void EntryBlockDecl::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void ImportDecl::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void PackageDecl::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void EnumDecl::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void FieldDecl::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void MethodDecl::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void ConstructorDecl::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void PropertyDecl::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void OperatorDecl::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void InterfaceDecl::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void CastExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void TypeTestExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }

inline void BlockStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void ExprStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void IfStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void ForStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void RangeLoopStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void ForEachStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void WhileStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void SwitchStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void ReturnStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void ThrowStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void TryCatchStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void BreakStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void ContinueStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }

inline void LiteralExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void IdentifierExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void BinaryExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void UnaryExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void TernaryExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void AssignExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void CallExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void MemberExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void IndexExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void ArrayExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void MapExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void LambdaExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void ThisExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
inline void SuperExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }

} // namespace thalapathy
