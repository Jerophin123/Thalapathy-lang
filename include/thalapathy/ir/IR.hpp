#pragma once
#include <string>
#include <vector>
#include <variant>
#include <memory>
#include <cstdint>
#include "thalapathy/ast/AST.hpp"
#include <unordered_map>

namespace thalapathy {

enum class OpCode : uint8_t {
    LOAD_CONST,
    LOAD_LOCAL,
    STORE_LOCAL,
    LOAD_GLOBAL,
    STORE_GLOBAL,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    EQ,
    NE,
    LT,
    LTE,
    GT,
    GTE,
    NOT,
    JUMP,
    JUMP_IF_FALSE,
    CALL,
    RETURN,
    POP,
    BUILD_ARRAY,
    INDEX_GET,
    INDEX_SET,
    GET_FIELD,
    SET_FIELD,
    THROW,
    AWAIT
};

using ConstantVal = std::variant<long long, double, std::string>;

struct Instruction {
    OpCode opcode;
    uint32_t operand = 0;
};

struct ModuleIR {
    struct ClassMetadata {
        std::string name;
        std::string parentName;
        std::vector<std::string> implementedInterfaces;
        std::vector<std::string> fields;
        std::vector<std::string> methods;
        std::vector<std::string> properties;
    };

    struct InterfaceMetadata {
        std::string name;
        std::vector<std::string> methods;
    };

    struct PropertyMetadata {
        std::string name;
        std::string typeStr;
        bool hasGetter = false;
        bool hasSetter = false;
    };

    struct MethodMetadata {
        std::string name;
        std::string returnType;
        std::vector<std::string> paramTypes;
    };

    std::vector<ConstantVal> constantPool;
    std::vector<Instruction> instructions;

    std::vector<ClassMetadata> classes;
    std::vector<InterfaceMetadata> interfaces;
    std::vector<PropertyMetadata> properties;
    std::vector<MethodMetadata> methods;
    std::vector<std::string> types;

    bool serialize(const std::string& path) const;
    bool deserialize(const std::string& path);
};

class IRGenerator : public ASTVisitor {
public:
    IRGenerator();

    ModuleIR generate(const std::vector<std::unique_ptr<ASTNode>>& nodes);

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
    uint32_t addConstant(const ConstantVal& val);
    uint32_t addIdentifierConstant(const std::string& name);

    ModuleIR ir;
    std::unordered_map<std::string, uint32_t> localVariables;
    uint32_t nextLocalIndex = 0;
};

} // namespace thalapathy
