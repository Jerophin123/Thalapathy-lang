#include "thalapathy/ir/IR.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace thalapathy {

static void writeString(std::ostream& out, const std::string& s) {
    uint32_t len = static_cast<uint32_t>(s.length());
    out.write(reinterpret_cast<const char*>(&len), 4);
    out.write(s.data(), len);
}

static void writeStringVec(std::ostream& out, const std::vector<std::string>& v) {
    uint32_t size = static_cast<uint32_t>(v.size());
    out.write(reinterpret_cast<const char*>(&size), 4);
    for (const auto& s : v) {
        writeString(out, s);
    }
}

static std::string readString(std::istream& in) {
    uint32_t len = 0;
    if (!in.read(reinterpret_cast<char*>(&len), 4)) return "";
    if (len > 1000000) return ""; // protection
    std::string s(len, '\0');
    in.read(&s[0], len);
    return s;
}

static std::vector<std::string> readStringVec(std::istream& in) {
    uint32_t size = 0;
    if (!in.read(reinterpret_cast<char*>(&size), 4)) return {};
    if (size > 100000) return {};
    std::vector<std::string> v(size);
    for (uint32_t i = 0; i < size; ++i) {
        v[i] = readString(in);
    }
    return v;
}

bool ModuleIR::serialize(const std::string& path) const {
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) return false;

    // VEAF Magic Header: "VJAY" (0x56 4A 41 59)
    out.write("VJAY", 4);

    // VEAF Format Version: 3
    uint16_t fmtVersion = 3;
    out.write(reinterpret_cast<const char*>(&fmtVersion), 2);

    // Language Version: 0.3.0
    uint8_t major = 0;
    uint8_t minor = 3;
    uint8_t patch = 0;
    out.write(reinterpret_cast<const char*>(&major), 1);
    out.write(reinterpret_cast<const char*>(&minor), 1);
    out.write(reinterpret_cast<const char*>(&patch), 1);

    // Number of sections: 7
    uint16_t numSections = 7;
    out.write(reinterpret_cast<const char*>(&numSections), 2);

    // Helper lambda to write a section
    auto writeSection = [&](uint8_t typeId, const std::string& data) {
        uint32_t len = static_cast<uint32_t>(data.size());
        out.write(reinterpret_cast<const char*>(&typeId), 1);
        out.write(reinterpret_cast<const char*>(&len), 4);
        if (len > 0) {
            out.write(data.data(), len);
        }
    };

    // 1. Constant Pool Section
    {
        std::stringstream ss;
        uint32_t poolSize = static_cast<uint32_t>(constantPool.size());
        ss.write(reinterpret_cast<const char*>(&poolSize), 4);
        for (const auto& val : constantPool) {
            if (std::holds_alternative<long long>(val)) {
                uint8_t type = 0;
                ss.write(reinterpret_cast<const char*>(&type), 1);
                long long v = std::get<long long>(val);
                ss.write(reinterpret_cast<const char*>(&v), 8);
            } else if (std::holds_alternative<double>(val)) {
                uint8_t type = 1;
                ss.write(reinterpret_cast<const char*>(&type), 1);
                double v = std::get<double>(val);
                ss.write(reinterpret_cast<const char*>(&v), 8);
            } else {
                uint8_t type = 2;
                ss.write(reinterpret_cast<const char*>(&type), 1);
                writeString(ss, std::get<std::string>(val));
            }
        }
        writeSection(1, ss.str());
    }

    // 2. Instructions Section
    {
        std::stringstream ss;
        uint32_t instSize = static_cast<uint32_t>(instructions.size());
        ss.write(reinterpret_cast<const char*>(&instSize), 4);
        for (const auto& inst : instructions) {
            uint8_t op = static_cast<uint8_t>(inst.opcode);
            ss.write(reinterpret_cast<const char*>(&op), 1);
            uint32_t operand = inst.operand;
            ss.write(reinterpret_cast<const char*>(&operand), 4);
        }
        writeSection(2, ss.str());
    }

    // 3. Classes Section
    {
        std::stringstream ss;
        uint32_t size = static_cast<uint32_t>(classes.size());
        ss.write(reinterpret_cast<const char*>(&size), 4);
        for (const auto& c : classes) {
            writeString(ss, c.name);
            writeString(ss, c.parentName);
            writeStringVec(ss, c.implementedInterfaces);
            writeStringVec(ss, c.fields);
            writeStringVec(ss, c.methods);
            writeStringVec(ss, c.properties);
        }
        writeSection(3, ss.str());
    }

    // 4. Interfaces Section
    {
        std::stringstream ss;
        uint32_t size = static_cast<uint32_t>(interfaces.size());
        ss.write(reinterpret_cast<const char*>(&size), 4);
        for (const auto& iface : interfaces) {
            writeString(ss, iface.name);
            writeStringVec(ss, iface.methods);
        }
        writeSection(4, ss.str());
    }

    // 5. Properties Section
    {
        std::stringstream ss;
        uint32_t size = static_cast<uint32_t>(properties.size());
        ss.write(reinterpret_cast<const char*>(&size), 4);
        for (const auto& p : properties) {
            writeString(ss, p.name);
            writeString(ss, p.typeStr);
            uint8_t flags = (p.hasGetter ? 1 : 0) | (p.hasSetter ? 2 : 0);
            ss.write(reinterpret_cast<const char*>(&flags), 1);
        }
        writeSection(5, ss.str());
    }

    // 6. Methods Section
    {
        std::stringstream ss;
        uint32_t size = static_cast<uint32_t>(methods.size());
        ss.write(reinterpret_cast<const char*>(&size), 4);
        for (const auto& m : methods) {
            writeString(ss, m.name);
            writeString(ss, m.returnType);
            writeStringVec(ss, m.paramTypes);
        }
        writeSection(6, ss.str());
    }

    // 7. Types Section
    {
        std::stringstream ss;
        writeStringVec(ss, types);
        writeSection(7, ss.str());
    }

    return true;
}

bool ModuleIR::deserialize(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open()) return false;

    // Check file size
    in.seekg(0, std::ios::end);
    std::streamsize size = in.tellg();
    in.seekg(0, std::ios::beg);
    if (size < 11) return false; // 4 (magic) + 2 (fmt) + 3 (lang) + 2 (sections)

    // Magic Header: Accept VJAY or legacy THLA
    char magic[4];
    in.read(magic, 4);
    bool isVJAY = (magic[0] == 'V' && magic[1] == 'J' && magic[2] == 'A' && magic[3] == 'Y');
    bool isTHLA = (magic[0] == 'T' && magic[1] == 'H' && magic[2] == 'L' && magic[3] == 'A');
    if (isTHLA) {
        std::cerr << "error[THALA-ARTIFACT-001]:\nlegacy THALA artifact format is not supported by this TDK version\n\n"
                  << "help:\nrebuild the original `.tvk` source to generate a `.vijay` artifact\n";
        return false;
    }
    if (!isVJAY) {
        return false;
    }

    // Format Version
    uint16_t fmtVersion = 0;
    in.read(reinterpret_cast<char*>(&fmtVersion), 2);
    if (fmtVersion != 3) {
        return false; // unsupported/newer/older format
    }

    // Language Version
    uint8_t major = 0, minor = 0, patch = 0;
    in.read(reinterpret_cast<char*>(&major), 1);
    in.read(reinterpret_cast<char*>(&minor), 1);
    in.read(reinterpret_cast<char*>(&patch), 1);
    if (major != 0 || minor != 3) {
        return false;
    }

    // Number of sections
    uint16_t numSections = 0;
    if (!in.read(reinterpret_cast<char*>(&numSections), 2)) return false;

    // Read sections
    for (uint16_t sIdx = 0; sIdx < numSections; ++sIdx) {
        uint8_t typeId = 0;
        uint32_t len = 0;
        if (!in.read(reinterpret_cast<char*>(&typeId), 1)) return false;
        if (!in.read(reinterpret_cast<char*>(&len), 4)) return false;

        std::string payload(len, '\0');
        if (len > 0) {
            in.read(&payload[0], len);
        }
        std::stringstream ss(payload);

        if (typeId == 1) {
            // Constant Pool
            uint32_t poolSize = 0;
            ss.read(reinterpret_cast<char*>(&poolSize), 4);
            constantPool.clear();
            for (uint32_t i = 0; i < poolSize; ++i) {
                uint8_t type = 0;
                ss.read(reinterpret_cast<char*>(&type), 1);
                if (type == 0) {
                    long long v = 0;
                    ss.read(reinterpret_cast<char*>(&v), 8);
                    constantPool.push_back(v);
                } else if (type == 1) {
                    double v = 0.0;
                    ss.read(reinterpret_cast<char*>(&v), 8);
                    constantPool.push_back(v);
                } else if (type == 2) {
                    std::string str = readString(ss);
                    constantPool.push_back(str);
                } else {
                    return false;
                }
            }
        } else if (typeId == 2) {
            // Instructions
            uint32_t instSize = 0;
            ss.read(reinterpret_cast<char*>(&instSize), 4);
            instructions.clear();
            for (uint32_t i = 0; i < instSize; ++i) {
                uint8_t op = 0;
                ss.read(reinterpret_cast<char*>(&op), 1);
                uint32_t operand = 0;
                ss.read(reinterpret_cast<char*>(&operand), 4);
                instructions.push_back(Instruction{static_cast<OpCode>(op), operand});
            }
        } else if (typeId == 3) {
            // Classes
            uint32_t numClasses = 0;
            ss.read(reinterpret_cast<char*>(&numClasses), 4);
            classes.clear();
            for (uint32_t i = 0; i < numClasses; ++i) {
                ClassMetadata c;
                c.name = readString(ss);
                c.parentName = readString(ss);
                c.implementedInterfaces = readStringVec(ss);
                c.fields = readStringVec(ss);
                c.methods = readStringVec(ss);
                c.properties = readStringVec(ss);
                classes.push_back(c);
            }
        } else if (typeId == 4) {
            // Interfaces
            uint32_t numInterfaces = 0;
            ss.read(reinterpret_cast<char*>(&numInterfaces), 4);
            interfaces.clear();
            for (uint32_t i = 0; i < numInterfaces; ++i) {
                InterfaceMetadata iface;
                iface.name = readString(ss);
                iface.methods = readStringVec(ss);
                interfaces.push_back(iface);
            }
        } else if (typeId == 5) {
            // Properties
            uint32_t numProperties = 0;
            ss.read(reinterpret_cast<char*>(&numProperties), 4);
            properties.clear();
            for (uint32_t i = 0; i < numProperties; ++i) {
                PropertyMetadata p;
                p.name = readString(ss);
                p.typeStr = readString(ss);
                uint8_t flags = 0;
                ss.read(reinterpret_cast<char*>(&flags), 1);
                p.hasGetter = (flags & 1) != 0;
                p.hasSetter = (flags & 2) != 0;
                properties.push_back(p);
            }
        } else if (typeId == 6) {
            // Methods
            uint32_t numMethods = 0;
            ss.read(reinterpret_cast<char*>(&numMethods), 4);
            methods.clear();
            for (uint32_t i = 0; i < numMethods; ++i) {
                MethodMetadata m;
                m.name = readString(ss);
                m.returnType = readString(ss);
                m.paramTypes = readStringVec(ss);
                methods.push_back(m);
            }
        } else if (typeId == 7) {
            // Types
            types = readStringVec(ss);
        } else {
            // Skip unknown section safely
        }
    }

    return true;
}

IRGenerator::IRGenerator() {}

ModuleIR IRGenerator::generate(const std::vector<std::unique_ptr<ASTNode>>& nodes) {
    ir.constantPool.clear();
    ir.instructions.clear();
    localVariables.clear();
    nextLocalIndex = 0;

    for (const auto& node : nodes) {
        node->accept(this);
    }
    return ir;
}

uint32_t IRGenerator::addConstant(const ConstantVal& val) {
    for (size_t i = 0; i < ir.constantPool.size(); ++i) {
        if (ir.constantPool[i] == val) return static_cast<uint32_t>(i);
    }
    ir.constantPool.push_back(val);
    return static_cast<uint32_t>(ir.constantPool.size() - 1);
}

uint32_t IRGenerator::addIdentifierConstant(const std::string& name) {
    return addConstant(ConstantVal{name});
}

// Visitor implementations for AST code generation to IR
void IRGenerator::visit(VarDecl* node) {
    if (node->initializer) {
        node->initializer->accept(this);
    } else {
        // default to load const null
        uint32_t idx = addConstant(ConstantVal{"null"});
        ir.instructions.push_back(Instruction{OpCode::LOAD_CONST, idx});
    }
    uint32_t localIdx = nextLocalIndex++;
    localVariables[node->name] = localIdx;
    ir.instructions.push_back(Instruction{OpCode::STORE_LOCAL, localIdx});
}

void IRGenerator::visit(FuncDecl* node) {
    // Generate inner instructions for functions
    node->body->accept(this);
}

void IRGenerator::visit(ClassDecl* node) {
    for (const auto& m : node->methods) {
        m->accept(this);
    }
}

void IRGenerator::visit(EntryBlockDecl* node) {
    node->body->accept(this);
}

void IRGenerator::visit(BlockStmt* node) {
    for (const auto& stmt : node->statements) {
        stmt->accept(this);
    }
}

void IRGenerator::visit(ExprStmt* node) {
    node->expression->accept(this);
    ir.instructions.push_back(Instruction{OpCode::POP, 0});
}

void IRGenerator::visit(IfStmt* node) {
    node->condition->accept(this);
    size_t jumpIfFalseIdx = ir.instructions.size();
    ir.instructions.push_back(Instruction{OpCode::JUMP_IF_FALSE, 0}); // target placeholder

    node->thenBranch->accept(this);

    if (node->elseBranch) {
        size_t jumpIdx = ir.instructions.size();
        ir.instructions.push_back(Instruction{OpCode::JUMP, 0}); // target placeholder
        ir.instructions[jumpIfFalseIdx].operand = static_cast<uint32_t>(ir.instructions.size());
        node->elseBranch->accept(this);
        ir.instructions[jumpIdx].operand = static_cast<uint32_t>(ir.instructions.size());
    } else {
        ir.instructions[jumpIfFalseIdx].operand = static_cast<uint32_t>(ir.instructions.size());
    }
}

void IRGenerator::visit(ForStmt* node) {
    if (node->initializer) node->initializer->accept(this);
    size_t loopStart = ir.instructions.size();
    size_t exitJumpIdx = 0;
    bool hasCond = (node->condition != nullptr);
    
    if (hasCond) {
        node->condition->accept(this);
        exitJumpIdx = ir.instructions.size();
        ir.instructions.push_back(Instruction{OpCode::JUMP_IF_FALSE, 0});
    }

    node->body->accept(this);

    if (node->update) {
        node->update->accept(this);
    }

    ir.instructions.push_back(Instruction{OpCode::JUMP, static_cast<uint32_t>(loopStart)});
    if (hasCond) {
        ir.instructions[exitJumpIdx].operand = static_cast<uint32_t>(ir.instructions.size());
    }
}

void IRGenerator::visit(RangeLoopStmt* node) {
    // Range loop generates a simplified loop structure
    node->startExpr->accept(this);
    uint32_t startLocal = nextLocalIndex++;
    ir.instructions.push_back(Instruction{OpCode::STORE_LOCAL, startLocal});

    node->endExpr->accept(this);
    uint32_t endLocal = nextLocalIndex++;
    ir.instructions.push_back(Instruction{OpCode::STORE_LOCAL, endLocal});

    size_t loopStart = ir.instructions.size();
    ir.instructions.push_back(Instruction{OpCode::LOAD_LOCAL, startLocal});
    ir.instructions.push_back(Instruction{OpCode::LOAD_LOCAL, endLocal});
    ir.instructions.push_back(Instruction{OpCode::LT, 0});

    size_t exitJump = ir.instructions.size();
    ir.instructions.push_back(Instruction{OpCode::JUMP_IF_FALSE, 0});

    node->body->accept(this);

    // increment startLocal
    ir.instructions.push_back(Instruction{OpCode::LOAD_LOCAL, startLocal});
    uint32_t oneIdx = addConstant(1LL);
    ir.instructions.push_back(Instruction{OpCode::LOAD_CONST, oneIdx});
    ir.instructions.push_back(Instruction{OpCode::ADD, 0});
    ir.instructions.push_back(Instruction{OpCode::STORE_LOCAL, startLocal});

    ir.instructions.push_back(Instruction{OpCode::JUMP, static_cast<uint32_t>(loopStart)});
    ir.instructions[exitJump].operand = static_cast<uint32_t>(ir.instructions.size());
}

void IRGenerator::visit(ReturnStmt* node) {
    if (node->expression) {
        node->expression->accept(this);
    } else {
        uint32_t nullIdx = addConstant(ConstantVal{"null"});
        ir.instructions.push_back(Instruction{OpCode::LOAD_CONST, nullIdx});
    }
    ir.instructions.push_back(Instruction{OpCode::RETURN, 0});
}

void IRGenerator::visit(ThrowStmt* node) {
    node->expression->accept(this);
    ir.instructions.push_back(Instruction{OpCode::THROW, 0});
}

void IRGenerator::visit(TryCatchStmt* node) {
    node->tryBody->accept(this);
    node->catchBody->accept(this);
}

void IRGenerator::visit(BreakStmt*) {
    ir.instructions.push_back(Instruction{OpCode::JUMP, 0}); // placeholder, resolved in VM execution
}

void IRGenerator::visit(ContinueStmt*) {
    ir.instructions.push_back(Instruction{OpCode::JUMP, 0}); // placeholder
}

void IRGenerator::visit(LiteralExpr* node) {
    uint32_t idx = 0;
    std::visit([this, &idx](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            idx = addConstant(ConstantVal{"null"});
        } else if constexpr (std::is_same_v<T, bool>) {
            idx = addConstant(ConstantVal{arg ? 1LL : 0LL});
        } else if constexpr (std::is_same_v<T, char>) {
            idx = addConstant(ConstantVal{std::string(1, arg)});
        } else {
            idx = addConstant(ConstantVal{arg});
        }
    }, node->value);
    ir.instructions.push_back(Instruction{OpCode::LOAD_CONST, idx});
}

void IRGenerator::visit(IdentifierExpr* node) {
    auto it = localVariables.find(node->name);
    if (it != localVariables.end()) {
        ir.instructions.push_back(Instruction{OpCode::LOAD_LOCAL, it->second});
    } else {
        uint32_t idx = addIdentifierConstant(node->name);
        ir.instructions.push_back(Instruction{OpCode::LOAD_GLOBAL, idx});
    }
}

void IRGenerator::visit(BinaryExpr* node) {
    node->left->accept(this);
    node->right->accept(this);
    OpCode op = OpCode::ADD;
    switch (node->op) {
        case TokenType::PLUS: op = OpCode::ADD; break;
        case TokenType::MINUS: op = OpCode::SUB; break;
        case TokenType::STAR: op = OpCode::MUL; break;
        case TokenType::SLASH: op = OpCode::DIV; break;
        case TokenType::PERCENT: op = OpCode::MOD; break;
        case TokenType::EQ_EQ: op = OpCode::EQ; break;
        case TokenType::BANG_EQ: op = OpCode::NE; break;
        case TokenType::LT: op = OpCode::LT; break;
        case TokenType::GT: op = OpCode::GT; break;
        case TokenType::LT_EQ: op = OpCode::LTE; break;
        case TokenType::GT_EQ: op = OpCode::GTE; break;
        default: break;
    }
    ir.instructions.push_back(Instruction{op, 0});
}

void IRGenerator::visit(UnaryExpr* node) {
    node->operand->accept(this);
    if (node->op == TokenType::BANG) {
        ir.instructions.push_back(Instruction{OpCode::NOT, 0});
    }
}

void IRGenerator::visit(AssignExpr* node) {
    node->value->accept(this);
    if (auto ident = dynamic_cast<IdentifierExpr*>(node->target.get())) {
        auto it = localVariables.find(ident->name);
        if (it != localVariables.end()) {
            ir.instructions.push_back(Instruction{OpCode::STORE_LOCAL, it->second});
        } else {
            uint32_t idx = addIdentifierConstant(ident->name);
            ir.instructions.push_back(Instruction{OpCode::STORE_GLOBAL, idx});
        }
    }
}

void IRGenerator::visit(CallExpr* node) {
    node->callee->accept(this);
    for (const auto& arg : node->arguments) {
        arg->accept(this);
    }
    ir.instructions.push_back(Instruction{OpCode::CALL, static_cast<uint32_t>(node->arguments.size())});
}

void IRGenerator::visit(MemberExpr* node) {
    node->object->accept(this);
    uint32_t idx = addIdentifierConstant(node->memberName);
    ir.instructions.push_back(Instruction{OpCode::GET_FIELD, idx});
}

void IRGenerator::visit(IndexExpr* node) {
    node->object->accept(this);
    node->index->accept(this);
    ir.instructions.push_back(Instruction{OpCode::INDEX_GET, 0});
}

void IRGenerator::visit(ArrayExpr* node) {
    for (const auto& elem : node->elements) {
        elem->accept(this);
    }
    ir.instructions.push_back(Instruction{OpCode::BUILD_ARRAY, static_cast<uint32_t>(node->elements.size())});
}

void IRGenerator::visit(MapExpr* node) {
    // maps loaded as series of field sets
    for (const auto& entry : node->entries) {
        uint32_t idx = addIdentifierConstant(entry.first);
        entry.second->accept(this);
        ir.instructions.push_back(Instruction{OpCode::LOAD_CONST, idx});
    }
}

void IRGenerator::visit(ThisExpr*) {
    // loads current class this
}

void IRGenerator::visit(SuperExpr*) {
    // loads class super
}

void IRGenerator::visit(FieldDecl*) {
    // Field declarations handled by class-level IR
}

void IRGenerator::visit(MethodDecl* node) {
    if (node->funcDecl) {
        node->funcDecl->accept(this);
    }
}

void IRGenerator::visit(ConstructorDecl* node) {
    if (node->body) {
        node->body->accept(this);
    }
}

void IRGenerator::visit(PropertyDecl*) {
    // Property IR stubs - getter/setter codegen
}

void IRGenerator::visit(OperatorDecl* node) {
    if (node->body) {
        node->body->accept(this);
    }
}

void IRGenerator::visit(InterfaceDecl*) {
    // Interface declarations are metadata-only in IR
}

void IRGenerator::visit(CastExpr* node) {
    node->expression->accept(this);
    // Cast validation at IR level
}

void IRGenerator::visit(TypeTestExpr* node) {
    node->expression->accept(this);
    // Type test instruction
}

void IRGenerator::visit(ImportDecl*) {
    // ImportDecl is processed at the module graph driver level.
}

} // namespace thalapathy
