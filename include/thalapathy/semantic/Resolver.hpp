#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "thalapathy/ast/AST.hpp"
#include "thalapathy/semantic/Symbol.hpp"
#include "thalapathy/diagnostics/Diagnostics.hpp"

namespace thalapathy {

class Resolver : public ASTVisitor {
public:
    Resolver(std::string filename_, std::string sourceCode_);

    bool resolve(const std::vector<std::unique_ptr<ASTNode>>& nodes, bool isEntryFile = true);
    const std::vector<Diagnostic>& getDiagnostics() const { return diagnostics; }

    // Visitor methods
    void visit(VarDecl* node) override;
    void visit(FuncDecl* node) override;
    void visit(ClassDecl* node) override;
    void visit(EntryBlockDecl* node) override;

    void visit(BlockStmt* node) override;
    void visit(ExprStmt* node) override;
    void visit(IfStmt* node) override;
    void visit(ForStmt* node) override;
    void visit(RangeLoopStmt* node) override;
    void visit(ForEachStmt* node) override;
    void visit(WhileStmt* node) override;
    void visit(SwitchStmt* node) override;
    void visit(ReturnStmt* node) override;
    void visit(ThrowStmt* node) override;
    void visit(TryCatchStmt* node) override;
    void visit(BreakStmt* node) override;
    void visit(ContinueStmt* node) override;

    void visit(LiteralExpr* node) override;
    void visit(IdentifierExpr* node) override;
    void visit(BinaryExpr* node) override;
    void visit(UnaryExpr* node) override;
    void visit(TernaryExpr* node) override;
    void visit(AssignExpr* node) override;
    void visit(CallExpr* node) override;
    void visit(MemberExpr* node) override;
    void visit(IndexExpr* node) override;
    void visit(ArrayExpr* node) override;
    void visit(MapExpr* node) override;
    void visit(LambdaExpr* node) override;
    void visit(ThisExpr* node) override;
    void visit(SuperExpr* node) override;
    void visit(FieldDecl* node) override;
    void visit(MethodDecl* node) override;
    void visit(ConstructorDecl* node) override;
    void visit(PropertyDecl* node) override;
    void visit(OperatorDecl* node) override;
    void visit(InterfaceDecl* node) override;
    void visit(CastExpr* node) override;
    void visit(TypeTestExpr* node) override;
    void visit(ImportDecl* node) override;
    void visit(PackageDecl* node) override;
    void visit(EnumDecl* node) override;

private:
    // Helper types
    using ScopeMap = std::unordered_map<std::string, Symbol>;

    struct ClassInfo {
        std::string name;
        std::string parentName;
        bool isInterface = false;
        bool isAbstract = false;
        bool isFinal = false;
        std::vector<std::string> implementedInterfaces;
        std::vector<std::string> parentInterfaces;
        std::vector<std::string> genericParams;

        std::unordered_map<std::string, Type> fields;
        std::unordered_map<std::string, bool> fieldIsStatic;
        std::unordered_map<std::string, bool> fieldIsFinal;
        std::unordered_map<std::string, AccessModifier> fieldAccess;

        struct MethodSig {
            std::string name;
            std::vector<Type> paramTypes;
            Type returnType;
            AccessModifier access = AccessModifier::PUBLIC;
            bool isStatic = false;
            bool isAbstract = false;
            bool isFinal = false;
            bool isOverride = false;
        };
        std::unordered_map<std::string, std::vector<MethodSig>> methods;

        struct ConstructorSig {
            std::vector<Type> paramTypes;
        };
        std::vector<ConstructorSig> constructors;

        struct PropertyInfo {
            std::string name;
            Type type;
            AccessModifier access = AccessModifier::PUBLIC;
            bool isStatic = false;
            bool hasGetter = false;
            bool hasSetter = false;
        };
        std::unordered_map<std::string, PropertyInfo> properties;

        struct OperatorInfo {
            TokenType op;
            Type paramType;
            Type returnType;
        };
        std::vector<OperatorInfo> operators;
    };

    // Scoping helpers
    void beginScope();
    void endScope();
    bool declare(const Symbol& sym);
    bool lookup(const std::string& name, Symbol& outSym);
    bool isDeclaredInCurrentScope(const std::string& name);

    // Diagnostics
    void reportError(const std::string& code, const std::string& title, const SourceSpan& span, const std::string& exp, const std::string& help);

    // Context tracking
    std::string filename;
    std::string sourceCode;
    std::vector<ScopeMap> scopes;
    std::vector<Diagnostic> diagnostics;

    std::unordered_map<std::string, ClassInfo> classTable;
    ClassInfo* currentClass = nullptr;
    FuncDecl* currentFunction = nullptr;
    bool insideFunctionLike = false;
    int loopDepth = 0;
    bool hasEntryBlock = false;

    Type currentExprType = Type{TypeKind::ANY, ""};
};

} // namespace thalapathy
