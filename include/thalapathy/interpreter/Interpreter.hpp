#pragma once
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>
#include "thalapathy/ast/AST.hpp"
#include "thalapathy/runtime/Value.hpp"
#include "thalapathy/runtime/Environment.hpp"

namespace thalapathy {

// Control flow exception signals
struct ReturnSignal {
    Value value;
};

struct BreakSignal {};
struct ContinueSignal {};

struct ThrowSignal {
    Value errorVal;
};

class Interpreter : public ASTVisitor {
public:
    Interpreter();

    void interpret(const std::vector<std::unique_ptr<ASTNode>>& nodes);
    Value evaluate(ASTNode* expr);
    void execute(ASTNode* stmt);

    std::shared_ptr<Environment> getGlobals() const { return globals; }
    std::shared_ptr<Environment> getEnvironment() const { return environment; }

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

    // Invoke any callable value (function, lambda closure, or builtin) with args.
    Value callCallable(const Value& callee, const std::vector<Value>& args);

private:
    std::shared_ptr<Environment> globals;
    std::shared_ptr<Environment> environment;
    Value lastExprVal = Value{std::monostate{}};

    void executeBlock(BlockStmt* block, std::shared_ptr<Environment> env);

    // Built-in method dispatch on collections / strings. Returns true if handled.
    bool tryInvokeMethod(const Value& obj, const std::string& name,
                         const std::vector<Value>& args, Value& out);
};

} // namespace thalapathy
