#include "thalapathy/interpreter/Interpreter.hpp"
#include <iostream>
#include <sstream>
#include <cmath>
#include <chrono>

namespace thalapathy {

// Helper builtins
static Value builtinSollu(const std::vector<Value>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        std::cout << args[i].toString();
        if (i + 1 < args.size()) std::cout << " ";
    }
    std::cout << "\n";
    return Value{std::monostate{}};
}

static Value builtinLen(const std::vector<Value>& args) {
    if (args.empty()) return Value{0LL};
    const auto& val = args[0];
    if (val.isArray()) {
        return Value{static_cast<long long>(std::get<std::shared_ptr<ArrayValue>>(val.val)->elements.size())};
    }
    if (val.isMap()) {
        return Value{static_cast<long long>(std::get<std::shared_ptr<MapValue>>(val.val)->entries.size())};
    }
    if (val.isString()) {
        return Value{static_cast<long long>(std::get<std::string>(val.val).length())};
    }
    return Value{0LL};
}

static Value builtinRange(const std::vector<Value>& args) {
    if (args.size() < 2) return Value{std::make_shared<ArrayValue>()};
    long long start = args[0].isInt() ? std::get<long long>(args[0].val) : 0LL;
    long long end = args[1].isInt() ? std::get<long long>(args[1].val) : 0LL;
    
    auto arr = std::make_shared<ArrayValue>();
    for (long long i = start; i < end; ++i) {
        arr->elements.push_back(Value{i});
    }
    return Value{arr};
}

static Value builtinType(const std::vector<Value>& args) {
    if (args.empty()) return Value{"null"};
    const auto& v = args[0];
    if (v.isNull()) return Value{"null"};
    if (v.isInt()) return Value{"int"};
    if (v.isFloat()) return Value{"float"};
    if (v.isBool()) return Value{"bool"};
    if (v.isChar()) return Value{"char"};
    if (v.isString()) return Value{"string"};
    if (v.isArray()) return Value{"array"};
    if (v.isMap()) return Value{"map"};
    if (v.isFunction()) return Value{"function"};
    if (v.isClass()) return Value{"class"};
    if (v.isInstance()) return Value{"instance"};
    return Value{"unknown"};
}

static Value builtinString(const std::vector<Value>& args) {
    if (args.empty()) return Value{""};
    return Value{args[0].toString()};
}

static Value builtinInt(const std::vector<Value>& args) {
    if (args.empty()) return Value{0LL};
    const auto& v = args[0];
    if (v.isInt()) return v;
    if (v.isFloat()) return Value{static_cast<long long>(std::get<double>(v.val))};
    if (v.isString()) {
        try {
            return Value{std::stoll(std::get<std::string>(v.val))};
        } catch (...) {
            return Value{0LL};
        }
    }
    return Value{0LL};
}

static Value builtinFloat(const std::vector<Value>& args) {
    if (args.empty()) return Value{0.0};
    const auto& v = args[0];
    if (v.isFloat()) return v;
    if (v.isInt()) return Value{static_cast<double>(std::get<long long>(v.val))};
    if (v.isString()) {
        try {
            return Value{std::stod(std::get<std::string>(v.val))};
        } catch (...) {
            return Value{0.0};
        }
    }
    return Value{0.0};
}

static Value builtinReadLine(const std::vector<Value>& args) {
    (void)args;
    std::string line;
    if (std::getline(std::cin, line)) {
        return Value{line};
    }
    return Value{""};
}

static Value builtinTimeNow(const std::vector<Value>& args) {
    (void)args;
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return Value{static_cast<long long>(millis)};
}

Interpreter::Interpreter() {
    globals = std::make_shared<Environment>();
    environment = globals;

    // Define builtins
    globals->define("sollu", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"sollu", builtinSollu})});
    globals->define("len", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"len", builtinLen})});
    globals->define("range", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"range", builtinRange})});
    globals->define("type", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"type", builtinType})});
    globals->define("string", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"string", builtinString})});
    globals->define("int", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"int", builtinInt})});
    globals->define("float", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"float", builtinFloat})});
    globals->define("__native_read_line", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_read_line", builtinReadLine})});
    globals->define("__native_time_now", Value{std::make_shared<BuiltInFunctionValue>(BuiltInFunctionValue{"__native_time_now", builtinTimeNow})});
}

void Interpreter::interpret(const std::vector<std::unique_ptr<ASTNode>>& nodes) {
    try {
        // Register classes first
        for (const auto& node : nodes) {
            if (auto classDecl = dynamic_cast<ClassDecl*>(node.get())) {
                execute(classDecl);
            }
        }

        // Run non-class and non-entry declarations (e.g. global vars/funcs)
        for (const auto& node : nodes) {
            if (!dynamic_cast<ClassDecl*>(node.get()) && !dynamic_cast<EntryBlockDecl*>(node.get())) {
                execute(node.get());
            }
        }

        // Locate and run the single entry block thalapathy aarambam
        for (const auto& node : nodes) {
            if (auto entry = dynamic_cast<EntryBlockDecl*>(node.get())) {
                execute(entry);
                break;
            }
        }
    } catch (const ReturnSignal&) {
        // Top-level return
    } catch (const ThrowSignal& s) {
        std::cerr << "Uncaught runtime exception: " << s.errorVal.toString() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Runtime error: " << e.what() << "\n";
    }
}

Value Interpreter::evaluate(ASTNode* expr) {
    expr->accept(this);
    return lastExprVal;
}

void Interpreter::execute(ASTNode* stmt) {
    stmt->accept(this);
}

void Interpreter::executeBlock(BlockStmt* block, std::shared_ptr<Environment> env) {
    auto previous = this->environment;
    try {
        this->environment = env;
        for (const auto& stmt : block->statements) {
            execute(stmt.get());
        }
        this->environment = previous;
    } catch (...) {
        this->environment = previous;
        throw; // rethrow signals
    }
}

// Visitor implementations
void Interpreter::visit(VarDecl* node) {
    Value val = Value{std::monostate{}};
    if (node->initializer) {
        val = evaluate(node->initializer.get());
    }
    environment->define(node->name, val);
}

void Interpreter::visit(FuncDecl* node) {
    auto func = std::make_shared<FunctionValue>();
    func->decl = node;
    func->closure = environment;
    environment->define(node->name, Value{func});
}

void Interpreter::visit(ClassDecl* node) {
    auto klass = std::make_shared<ClassValue>();
    klass->decl = node;

    // Find parent class value
    if (!node->parentName.empty()) {
        bool foundParent = false;
        Value pv = environment->get(node->parentName, foundParent);
        if (foundParent && pv.isClass()) {
            klass->parentClass = std::get<std::shared_ptr<ClassValue>>(pv.val);
        }
    }

    // Populate implemented interfaces list
    klass->implementedInterfaces = node->implementedInterfaces;

    // Populate static fields and initialize them
    // First, inherit parent class static fields
    if (klass->parentClass) {
        klass->staticFields = klass->parentClass->staticFields;
    }
    for (const auto& field : node->fields) {
        if (field->isStatic) {
            Value initVal = Value{std::monostate{}};
            if (field->varDecl->initializer) {
                initVal = evaluate(field->varDecl->initializer.get());
            }
            klass->staticFields[field->varDecl->name] = initVal;
        }
    }

    // Define class in environment
    environment->define(node->name, Value{klass});
}

void Interpreter::visit(EntryBlockDecl* node) {
    execute(node->body.get());
}

void Interpreter::visit(BlockStmt* node) {
    executeBlock(node, std::make_shared<Environment>(environment));
}

void Interpreter::visit(ExprStmt* node) {
    evaluate(node->expression.get());
}

void Interpreter::visit(IfStmt* node) {
    Value cond = evaluate(node->condition.get());
    bool isTrue = cond.isBool() ? std::get<bool>(cond.val) : !cond.isNull();
    if (isTrue) {
        execute(node->thenBranch.get());
    } else if (node->elseBranch) {
        execute(node->elseBranch.get());
    }
}

void Interpreter::visit(ForStmt* node) {
    auto loopEnv = std::make_shared<Environment>(environment);
    auto previous = this->environment;
    this->environment = loopEnv;

    try {
        if (node->initializer) execute(node->initializer.get());
        while (true) {
            if (node->condition) {
                Value cond = evaluate(node->condition.get());
                bool isTrue = cond.isBool() ? std::get<bool>(cond.val) : !cond.isNull();
                if (!isTrue) break;
            }

            try {
                execute(node->body.get());
            } catch (const BreakSignal&) {
                break;
            } catch (const ContinueSignal&) {
                // proceed to update
            }

            if (node->update) evaluate(node->update.get());
        }
    } catch (...) {
        this->environment = previous;
        throw;
    }
    this->environment = previous;
}

void Interpreter::visit(RangeLoopStmt* node) {
    Value startVal = evaluate(node->startExpr.get());
    Value endVal = evaluate(node->endExpr.get());

    long long start = startVal.isInt() ? std::get<long long>(startVal.val) : 0LL;
    long long end = endVal.isInt() ? std::get<long long>(endVal.val) : 0LL;

    auto loopEnv = std::make_shared<Environment>(environment);
    auto previous = this->environment;
    this->environment = loopEnv;

    try {
        for (long long i = start; i < end; ++i) {
            loopEnv->define(node->varName, Value{i});
            try {
                execute(node->body.get());
            } catch (const BreakSignal&) {
                break;
            } catch (const ContinueSignal&) {
                // proceed
            }
        }
    } catch (...) {
        this->environment = previous;
        throw;
    }
    this->environment = previous;
}

void Interpreter::visit(ReturnStmt* node) {
    Value val = Value{std::monostate{}};
    if (node->expression) {
        val = evaluate(node->expression.get());
    }
    throw ReturnSignal{val};
}

void Interpreter::visit(ThrowStmt* node) {
    Value errorVal = evaluate(node->expression.get());
    throw ThrowSignal{errorVal};
}

void Interpreter::visit(TryCatchStmt* node) {
    try {
        execute(node->tryBody.get());
    } catch (const ThrowSignal& s) {
        auto catchEnv = std::make_shared<Environment>(environment);
        catchEnv->define(node->catchVarName, s.errorVal);
        executeBlock(dynamic_cast<BlockStmt*>(node->catchBody.get()), catchEnv);
    }
}

void Interpreter::visit(BreakStmt*) {
    throw BreakSignal{};
}

void Interpreter::visit(ContinueStmt*) {
    throw ContinueSignal{};
}

void Interpreter::visit(LiteralExpr* node) {
    std::visit([this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            lastExprVal = Value{std::monostate{}};
        } else {
            lastExprVal = Value{arg};
        }
    }, node->value);
}

void Interpreter::visit(IdentifierExpr* node) {
    bool found = false;
    lastExprVal = environment->get(node->name, found);
    if (!found) {
        throw std::runtime_error("Undefined variable: " + node->name);
    }
}

void Interpreter::visit(BinaryExpr* node) {
    Value left = evaluate(node->left.get());
    Value right = evaluate(node->right.get());

    if (left.isInstance()) {
        auto inst = std::get<std::shared_ptr<InstanceValue>>(left.val);
        auto currKlass = inst->klass;
        OperatorDecl* matchedOp = nullptr;
        while (currKlass) {
            for (const auto& op : currKlass->decl->operators) {
                if (op->op == node->op) {
                    matchedOp = op.get();
                    break;
                }
            }
            if (matchedOp) break;
            currKlass = currKlass->parentClass;
        }

        if (matchedOp) {
            auto opEnv = std::make_shared<Environment>(globals);
            opEnv->define("this", left);
            opEnv->define(matchedOp->param.name, right);
            
            auto prevEnv = this->environment;
            this->environment = opEnv;
            try {
                executeBlock(dynamic_cast<BlockStmt*>(matchedOp->body.get()), opEnv);
                lastExprVal = Value{std::monostate{}};
            } catch (const ReturnSignal& ret) {
                lastExprVal = ret.value;
            }
            this->environment = prevEnv;
            return;
        }
    }

    switch (node->op) {
        case TokenType::PLUS:
            if (left.isInt() && right.isInt()) {
                lastExprVal = Value{std::get<long long>(left.val) + std::get<long long>(right.val)};
            } else if (left.isFloat() && right.isFloat()) {
                lastExprVal = Value{std::get<double>(left.val) + std::get<double>(right.val)};
            } else if (left.isString() && right.isString()) {
                lastExprVal = Value{std::get<std::string>(left.val) + std::get<std::string>(right.val)};
            } else {
                throw std::runtime_error("Invalid operands for +");
            }
            break;
        case TokenType::MINUS:
            if (left.isInt() && right.isInt()) {
                lastExprVal = Value{std::get<long long>(left.val) - std::get<long long>(right.val)};
            } else if (left.isFloat() && right.isFloat()) {
                lastExprVal = Value{std::get<double>(left.val) - std::get<double>(right.val)};
            } else {
                throw std::runtime_error("Invalid operands for -");
            }
            break;
        case TokenType::STAR:
            if (left.isInt() && right.isInt()) {
                lastExprVal = Value{std::get<long long>(left.val) * std::get<long long>(right.val)};
            } else if (left.isFloat() && right.isFloat()) {
                lastExprVal = Value{std::get<double>(left.val) * std::get<double>(right.val)};
            } else {
                throw std::runtime_error("Invalid operands for *");
            }
            break;
        case TokenType::SLASH:
            if (left.isInt() && right.isInt()) {
                long long divisor = std::get<long long>(right.val);
                if (divisor == 0) throw std::runtime_error("division by zero");
                lastExprVal = Value{std::get<long long>(left.val) / divisor};
            } else if (left.isFloat() && right.isFloat()) {
                double divisor = std::get<double>(right.val);
                if (divisor == 0.0) throw std::runtime_error("division by zero");
                lastExprVal = Value{std::get<double>(left.val) / divisor};
            } else {
                throw std::runtime_error("Invalid operands for /");
            }
            break;
        case TokenType::PERCENT:
            if (left.isInt() && right.isInt()) {
                long long divisor = std::get<long long>(right.val);
                if (divisor == 0) throw std::runtime_error("division by zero");
                lastExprVal = Value{std::get<long long>(left.val) % divisor};
            } else {
                throw std::runtime_error("Invalid operands for %");
            }
            break;

        // Comparisons
        case TokenType::EQ_EQ:
            if (left.isNull() && right.isNull()) lastExprVal = Value{true};
            else if (left.isInt() && right.isInt()) lastExprVal = Value{std::get<long long>(left.val) == std::get<long long>(right.val)};
            else if (left.isFloat() && right.isFloat()) lastExprVal = Value{std::get<double>(left.val) == std::get<double>(right.val)};
            else if (left.isBool() && right.isBool()) lastExprVal = Value{std::get<bool>(left.val) == std::get<bool>(right.val)};
            else if (left.isString() && right.isString()) lastExprVal = Value{std::get<std::string>(left.val) == std::get<std::string>(right.val)};
            else if (left.isInstance() && right.isInstance()) lastExprVal = Value{std::get<std::shared_ptr<InstanceValue>>(left.val) == std::get<std::shared_ptr<InstanceValue>>(right.val)};
            else lastExprVal = Value{false};
            break;
        case TokenType::BANG_EQ:
            if (left.isNull() && right.isNull()) lastExprVal = Value{false};
            else if (left.isInt() && right.isInt()) lastExprVal = Value{std::get<long long>(left.val) != std::get<long long>(right.val)};
            else if (left.isFloat() && right.isFloat()) lastExprVal = Value{std::get<double>(left.val) != std::get<double>(right.val)};
            else if (left.isBool() && right.isBool()) lastExprVal = Value{std::get<bool>(left.val) != std::get<bool>(right.val)};
            else if (left.isString() && right.isString()) lastExprVal = Value{std::get<std::string>(left.val) != std::get<std::string>(right.val)};
            else if (left.isInstance() && right.isInstance()) lastExprVal = Value{std::get<std::shared_ptr<InstanceValue>>(left.val) != std::get<std::shared_ptr<InstanceValue>>(right.val)};
            else lastExprVal = Value{true};
            break;
        case TokenType::LT:
            if (left.isInt() && right.isInt()) lastExprVal = Value{std::get<long long>(left.val) < std::get<long long>(right.val)};
            else if (left.isFloat() && right.isFloat()) lastExprVal = Value{std::get<double>(left.val) < std::get<double>(right.val)};
            else throw std::runtime_error("Invalid comparison");
            break;
        case TokenType::GT:
            if (left.isInt() && right.isInt()) lastExprVal = Value{std::get<long long>(left.val) > std::get<long long>(right.val)};
            else if (left.isFloat() && right.isFloat()) lastExprVal = Value{std::get<double>(left.val) > std::get<double>(right.val)};
            else throw std::runtime_error("Invalid comparison");
            break;
        case TokenType::LT_EQ:
            if (left.isInt() && right.isInt()) lastExprVal = Value{std::get<long long>(left.val) <= std::get<long long>(right.val)};
            else if (left.isFloat() && right.isFloat()) lastExprVal = Value{std::get<double>(left.val) <= std::get<double>(right.val)};
            else throw std::runtime_error("Invalid comparison");
            break;
        case TokenType::GT_EQ:
            if (left.isInt() && right.isInt()) lastExprVal = Value{std::get<long long>(left.val) >= std::get<long long>(right.val)};
            else if (left.isFloat() && right.isFloat()) lastExprVal = Value{std::get<double>(left.val) >= std::get<double>(right.val)};
            else throw std::runtime_error("Invalid comparison");
            break;

        // Logical
        case TokenType::AMP_AMP:
            lastExprVal = Value{std::get<bool>(left.val) && std::get<bool>(right.val)};
            break;
        case TokenType::BAR_BAR:
            lastExprVal = Value{std::get<bool>(left.val) || std::get<bool>(right.val)};
            break;
        default:
            throw std::runtime_error("Unsupported binary operator");
    }
}

void Interpreter::visit(UnaryExpr* node) {
    Value operand = evaluate(node->operand.get());
    if (node->op == TokenType::BANG) {
        lastExprVal = Value{!std::get<bool>(operand.val)};
    } else if (node->op == TokenType::MINUS) {
        if (operand.isInt()) {
            lastExprVal = Value{-std::get<long long>(operand.val)};
        } else if (operand.isFloat()) {
            lastExprVal = Value{-std::get<double>(operand.val)};
        }
    } else if (node->op == TokenType::PLUS_PLUS) {
        // Prefix/postfix ++
        if (operand.isInt()) {
            lastExprVal = Value{std::get<long long>(operand.val) + 1};
            if (auto ident = dynamic_cast<IdentifierExpr*>(node->operand.get())) {
                environment->assign(ident->name, lastExprVal);
            }
        }
    } else if (node->op == TokenType::MINUS_MINUS) {
        if (operand.isInt()) {
            lastExprVal = Value{std::get<long long>(operand.val) - 1};
            if (auto ident = dynamic_cast<IdentifierExpr*>(node->operand.get())) {
                environment->assign(ident->name, lastExprVal);
            }
        }
    }
}

void Interpreter::visit(AssignExpr* node) {
    Value val = evaluate(node->value.get());

    if (auto ident = dynamic_cast<IdentifierExpr*>(node->target.get())) {
        bool assigned = false;
        if (node->op == TokenType::EQUAL) {
            assigned = environment->assign(ident->name, val);
        } else {
            bool found = false;
            Value currentVal = environment->get(ident->name, found);
            if (found) {
                if (node->op == TokenType::PLUS_EQ) {
                    if (currentVal.isInt() && val.isInt()) {
                        val = Value{std::get<long long>(currentVal.val) + std::get<long long>(val.val)};
                    } else if (currentVal.isString() && val.isString()) {
                        val = Value{std::get<std::string>(currentVal.val) + std::get<std::string>(val.val)};
                    }
                } else if (node->op == TokenType::MINUS_EQ) {
                    val = Value{std::get<long long>(currentVal.val) - std::get<long long>(val.val)};
                } else if (node->op == TokenType::STAR_EQ) {
                    val = Value{std::get<long long>(currentVal.val) * std::get<long long>(val.val)};
                } else if (node->op == TokenType::SLASH_EQ) {
                    val = Value{std::get<long long>(currentVal.val) / std::get<long long>(val.val)};
                }
                assigned = environment->assign(ident->name, val);
            }
        }
        if (!assigned) {
            throw std::runtime_error("Failed to assign to " + ident->name);
        }
        lastExprVal = val;
    } else if (auto indexExpr = dynamic_cast<IndexExpr*>(node->target.get())) {
        Value obj = evaluate(indexExpr->object.get());
        Value idx = evaluate(indexExpr->index.get());

        if (obj.isArray() && idx.isInt()) {
            auto arr = std::get<std::shared_ptr<ArrayValue>>(obj.val);
            long long i = std::get<long long>(idx.val);
            if (i < 0 || i >= static_cast<long long>(arr->elements.size())) {
                throw std::runtime_error("array index out of bounds");
            }
            arr->elements[static_cast<size_t>(i)] = val;
            lastExprVal = val;
        } else if (obj.isMap() && idx.isString()) {
            auto map = std::get<std::shared_ptr<MapValue>>(obj.val);
            map->entries[std::get<std::string>(idx.val)] = val;
            lastExprVal = val;
        }
    } else if (auto memberExpr = dynamic_cast<MemberExpr*>(node->target.get())) {
        Value obj = evaluate(memberExpr->object.get());
        if (obj.isInstance()) {
            auto inst = std::get<std::shared_ptr<InstanceValue>>(obj.val);
            
            // Check for property setter
            auto currKlass = inst->klass;
            bool setterFound = false;
            while (currKlass && !setterFound) {
                for (const auto& prop : currKlass->decl->properties) {
                    if (prop->name == memberExpr->memberName && prop->setterBody) {
                        auto setterEnv = std::make_shared<Environment>(globals);
                        setterEnv->define("this", obj);
                        std::string paramName = prop->setterParamName.empty() ? "value" : prop->setterParamName;
                        setterEnv->define(paramName, val);
                        auto prevEnv = this->environment;
                        this->environment = setterEnv;
                        try {
                            executeBlock(dynamic_cast<BlockStmt*>(prop->setterBody.get()), setterEnv);
                        } catch (const ReturnSignal&) {}
                        this->environment = prevEnv;
                        setterFound = true;
                        break;
                    }
                }
                currKlass = currKlass->parentClass;
            }
            
            if (!setterFound) {
                inst->fields[memberExpr->memberName] = val;
            }
            lastExprVal = val;
        } else if (obj.isClass()) {
            // Static field assignment
            auto klass = std::get<std::shared_ptr<ClassValue>>(obj.val);
            klass->staticFields[memberExpr->memberName] = val;
            lastExprVal = val;
        } else if (obj.isMap()) {
            auto map = std::get<std::shared_ptr<MapValue>>(obj.val);
            map->entries[memberExpr->memberName] = val;
            lastExprVal = val;
        }
    }
}

void Interpreter::visit(CallExpr* node) {
    // Check if member call for arrays (push, pop)
    if (auto member = dynamic_cast<MemberExpr*>(node->callee.get())) {
        Value obj = evaluate(member->object.get());
        if (obj.isArray()) {
            auto arr = std::get<std::shared_ptr<ArrayValue>>(obj.val);
            if (member->memberName == "push") {
                if (!node->arguments.empty()) {
                    Value elem = evaluate(node->arguments[0].get());
                    arr->elements.push_back(elem);
                }
                lastExprVal = Value{std::monostate{}};
                return;
            } else if (member->memberName == "pop") {
                if (arr->elements.empty()) {
                    lastExprVal = Value{std::monostate{}};
                } else {
                    lastExprVal = arr->elements.back();
                    arr->elements.pop_back();
                }
                return;
            }
        }
    }

    Value callee = evaluate(node->callee.get());
    std::vector<Value> args;
    for (const auto& arg : node->arguments) {
        args.push_back(evaluate(arg.get()));
    }

    if (callee.isBuiltIn()) {
        auto b = std::get<std::shared_ptr<BuiltInFunctionValue>>(callee.val);
        lastExprVal = b->fn(args);
    } else if (callee.isFunction()) {
        auto f = std::get<std::shared_ptr<FunctionValue>>(callee.val);
        auto callEnv = std::make_shared<Environment>(f->closure);
        for (size_t i = 0; i < f->decl->params.size() && i < args.size(); ++i) {
            callEnv->define(f->decl->params[i].name, args[i]);
        }
        try {
            executeBlock(dynamic_cast<BlockStmt*>(f->decl->body.get()), callEnv);
            lastExprVal = Value{std::monostate{}};
        } catch (const ReturnSignal& ret) {
            lastExprVal = ret.value;
        }
    } else if (callee.isClass()) {
        // Instantiation
        auto klass = std::get<std::shared_ptr<ClassValue>>(callee.val);
        auto instance = std::make_shared<InstanceValue>();
        instance->klass = klass;

        // Walk class hierarchy from base class to derived class to collect all fields
        std::vector<std::shared_ptr<ClassValue>> hierarchy;
        auto currKlass = klass;
        while (currKlass) {
            hierarchy.push_back(currKlass);
            currKlass = currKlass->parentClass;
        }
        // Initialize fields from parent down to child
        for (auto hIt = hierarchy.rbegin(); hIt != hierarchy.rend(); ++hIt) {
            for (const auto& field : (*hIt)->decl->fields) {
                if (!field->isStatic) {
                    Value initVal = Value{std::monostate{}};
                    if (field->varDecl->initializer) {
                        initVal = evaluate(field->varDecl->initializer.get());
                    }
                    instance->fields[field->varDecl->name] = initVal;
                }
            }
        }

        // Look up constructor matching the arity (argument count)
        ConstructorDecl* matchedDecl = nullptr;
        std::shared_ptr<ClassValue> constrOwnerKlass = nullptr;
        currKlass = klass;
        while (currKlass) {
            for (const auto& constr : currKlass->decl->constructors) {
                if (constr->params.size() == args.size()) {
                    matchedDecl = constr.get();
                    constrOwnerKlass = currKlass;
                    break;
                }
            }
            if (matchedDecl) break;
            currKlass = currKlass->parentClass;
        }

        Value objVal{instance};

        if (matchedDecl) {
            // Helper lambda to execute constructor
            auto runConstructor = [&](ConstructorDecl* cDecl, std::shared_ptr<ClassValue> ownerKlass, const std::vector<Value>& cArgs, auto& selfRef) -> void {
                auto callEnv = std::make_shared<Environment>(globals);
                callEnv->define("this", objVal);
                if (ownerKlass->parentClass) {
                    callEnv->define("super", Value{ownerKlass->parentClass});
                }
                for (size_t i = 0; i < cDecl->params.size() && i < cArgs.size(); ++i) {
                    callEnv->define(cDecl->params[i].name, cArgs[i]);
                }

                // Parent initialization routing check
                bool hasExplicitSuper = false;
                for (const auto& initCall : cDecl->initializerList) {
                    if (auto callExpr = dynamic_cast<CallExpr*>(initCall.get())) {
                        if (auto ident = dynamic_cast<IdentifierExpr*>(callExpr->callee.get())) {
                            if (ident->name == "super") {
                                hasExplicitSuper = true;
                                // Evaluate constructor arguments in current constructor scope
                                auto prevEnv = this->environment;
                                this->environment = callEnv;
                                std::vector<Value> superArgs;
                                for (const auto& sArg : callExpr->arguments) {
                                    superArgs.push_back(evaluate(sArg.get()));
                                }
                                this->environment = prevEnv;

                                // Resolve and run parent constructor
                                ConstructorDecl* parentConstr = nullptr;
                                std::shared_ptr<ClassValue> parentKlass = ownerKlass->parentClass;
                                while (parentKlass) {
                                    for (const auto& pc : parentKlass->decl->constructors) {
                                        if (pc->params.size() == superArgs.size()) {
                                            parentConstr = pc.get();
                                            break;
                                        }
                                    }
                                    if (parentConstr) break;
                                    parentKlass = parentKlass->parentClass;
                                }
                                if (parentConstr) {
                                    selfRef(parentConstr, ownerKlass->parentClass, superArgs, selfRef);
                                }
                            } else if (ident->name == "this") {
                                hasExplicitSuper = true;
                                auto prevEnv = this->environment;
                                this->environment = callEnv;
                                std::vector<Value> siblingArgs;
                                for (const auto& sArg : callExpr->arguments) {
                                    siblingArgs.push_back(evaluate(sArg.get()));
                                }
                                this->environment = prevEnv;

                                ConstructorDecl* siblingConstr = nullptr;
                                for (const auto& sc : ownerKlass->decl->constructors) {
                                    if (sc->params.size() == siblingArgs.size()) {
                                        siblingConstr = sc.get();
                                        break;
                                    }
                                }
                                if (siblingConstr) {
                                    selfRef(siblingConstr, ownerKlass, siblingArgs, selfRef);
                                }
                            }
                        }
                    }
                }

                // If no explicit super, and parent class exists, call default constructor super()
                if (!hasExplicitSuper && ownerKlass->parentClass) {
                    ConstructorDecl* parentConstr = nullptr;
                    std::shared_ptr<ClassValue> parentKlass = ownerKlass->parentClass;
                    while (parentKlass) {
                        for (const auto& pc : parentKlass->decl->constructors) {
                            if (pc->params.size() == 0) {
                                parentConstr = pc.get();
                                break;
                            }
                        }
                        if (parentConstr) break;
                        parentKlass = parentKlass->parentClass;
                    }
                    if (parentConstr) {
                        selfRef(parentConstr, ownerKlass->parentClass, {}, selfRef);
                    }
                }

                // Execute constructor body
                auto prevEnv = this->environment;
                this->environment = callEnv;
                try {
                    executeBlock(dynamic_cast<BlockStmt*>(cDecl->body.get()), callEnv);
                } catch (const ReturnSignal&) {
                }
                this->environment = prevEnv;
            };

            runConstructor(matchedDecl, constrOwnerKlass, args, runConstructor);
        } else {
            // Default parent constructor invocation if class has parent but no init defined
            std::shared_ptr<ClassValue> parentKlass = klass->parentClass;
            auto runDefaultConstructors = [&](std::shared_ptr<ClassValue> pk, auto& selfRef) -> void {
                if (!pk) return;
                selfRef(pk->parentClass, selfRef);
                ConstructorDecl* pc = nullptr;
                auto currPk = pk;
                while (currPk) {
                    for (const auto& c : currPk->decl->constructors) {
                        if (c->params.size() == 0) {
                            pc = c.get();
                            break;
                        }
                    }
                    if (pc) break;
                    currPk = currPk->parentClass;
                }
                if (pc) {
                    auto callEnv = std::make_shared<Environment>(globals);
                    callEnv->define("this", objVal);
                    if (pk->parentClass) {
                        callEnv->define("super", Value{pk->parentClass});
                    }
                    auto prevEnv = this->environment;
                    this->environment = callEnv;
                    try {
                        executeBlock(dynamic_cast<BlockStmt*>(pc->body.get()), callEnv);
                    } catch (const ReturnSignal&) {}
                    this->environment = prevEnv;
                }
            };
            runDefaultConstructors(parentKlass, runDefaultConstructors);
        }

        lastExprVal = objVal;
    } else {
        throw std::runtime_error("Callee is not a callable object");
    }
}

void Interpreter::visit(MemberExpr* node) {
    Value obj = evaluate(node->object.get());
    if (obj.isInstance()) {
        auto inst = std::get<std::shared_ptr<InstanceValue>>(obj.val);
        
        // Field access
        auto fIt = inst->fields.find(node->memberName);
        if (fIt != inst->fields.end()) {
            lastExprVal = fIt->second;
            return;
        }

        // Property getter lookup
        auto currKlass = inst->klass;
        while (currKlass) {
            for (const auto& prop : currKlass->decl->properties) {
                if (prop->name == node->memberName && prop->getterBody) {
                    auto getterEnv = std::make_shared<Environment>(globals);
                    getterEnv->define("this", obj);
                    auto prevEnv = this->environment;
                    this->environment = getterEnv;
                    try {
                        executeBlock(dynamic_cast<BlockStmt*>(prop->getterBody.get()), getterEnv);
                        lastExprVal = Value{std::monostate{}};
                    } catch (const ReturnSignal& ret) {
                        lastExprVal = ret.value;
                    }
                    this->environment = prevEnv;
                    return;
                }
            }
            currKlass = currKlass->parentClass;
        }

        // Method lookup (MethodDecl wrapping FuncDecl)
        currKlass = inst->klass;
        while (currKlass) {
            for (const auto& m : currKlass->decl->methods) {
                if (m->funcDecl && m->funcDecl->name == node->memberName) {
                    auto func = std::make_shared<FunctionValue>();
                    func->decl = m->funcDecl.get();
                    func->closure = std::make_shared<Environment>(globals);
                    func->closure->define("this", obj);
                    
                    // define super binding in method closure
                    if (currKlass->parentClass) {
                        func->closure->define("super", Value{currKlass->parentClass});
                    }
                    
                    lastExprVal = Value{func};
                    return;
                }
            }
            currKlass = currKlass->parentClass;
        }

        throw std::runtime_error("Member '" + node->memberName + "' not found on instance");
    } else if (obj.isClass()) {
        // Static field / static method access on class
        auto klass = std::get<std::shared_ptr<ClassValue>>(obj.val);
        
        // Static field lookup
        auto sfIt = klass->staticFields.find(node->memberName);
        if (sfIt != klass->staticFields.end()) {
            lastExprVal = sfIt->second;
            return;
        }

        // Static method lookup
        auto currKlass = klass;
        while (currKlass) {
            for (const auto& m : currKlass->decl->methods) {
                if (m->isStatic && m->funcDecl && m->funcDecl->name == node->memberName) {
                    auto func = std::make_shared<FunctionValue>();
                    func->decl = m->funcDecl.get();
                    func->closure = std::make_shared<Environment>(globals);
                    lastExprVal = Value{func};
                    return;
                }
            }
            currKlass = currKlass->parentClass;
        }
        throw std::runtime_error("Static member '" + node->memberName + "' not found on class '" + klass->decl->name + "'");
    } else if (obj.isMap()) {
        auto map = std::get<std::shared_ptr<MapValue>>(obj.val);
        auto it = map->entries.find(node->memberName);
        if (it != map->entries.end()) {
            lastExprVal = it->second;
        } else {
            lastExprVal = Value{std::monostate{}};
        }
    } else {
        throw std::runtime_error("Member access on non-object");
    }
}

void Interpreter::visit(IndexExpr* node) {
    Value obj = evaluate(node->object.get());
    Value idx = evaluate(node->index.get());

    if (obj.isArray() && idx.isInt()) {
        auto arr = std::get<std::shared_ptr<ArrayValue>>(obj.val);
        long long i = std::get<long long>(idx.val);
        if (i < 0 || i >= static_cast<long long>(arr->elements.size())) {
            throw std::runtime_error("array index out of bounds");
        }
        lastExprVal = arr->elements[static_cast<size_t>(i)];
    } else if (obj.isMap() && idx.isString()) {
        auto map = std::get<std::shared_ptr<MapValue>>(obj.val);
        auto it = map->entries.find(std::get<std::string>(idx.val));
        if (it != map->entries.end()) {
            lastExprVal = it->second;
        } else {
            lastExprVal = Value{std::monostate{}};
        }
    } else {
        throw std::runtime_error("Index access on unsupported type");
    }
}

void Interpreter::visit(ArrayExpr* node) {
    auto arr = std::make_shared<ArrayValue>();
    for (const auto& elem : node->elements) {
        arr->elements.push_back(evaluate(elem.get()));
    }
    lastExprVal = Value{arr};
}

void Interpreter::visit(MapExpr* node) {
    auto map = std::make_shared<MapValue>();
    for (const auto& entry : node->entries) {
        map->entries[entry.first] = evaluate(entry.second.get());
    }
    lastExprVal = Value{map};
}

void Interpreter::visit(ThisExpr*) {
    bool found = false;
    lastExprVal = environment->get("this", found);
    if (!found) {
        throw std::runtime_error("this binding unresolved");
    }
}

void Interpreter::visit(SuperExpr* node) {
    // Resolve super method
    bool foundSuper = false;
    Value superKlassVal = environment->get("super", foundSuper);
    bool foundThis = false;
    Value thisObj = environment->get("this", foundThis);

    if (foundSuper && superKlassVal.isClass() && foundThis) {
        auto superKlass = std::get<std::shared_ptr<ClassValue>>(superKlassVal.val);
        
        auto currKlass = superKlass;
        while (currKlass) {
            for (const auto& m : currKlass->decl->methods) {
                if (m->funcDecl && m->funcDecl->name == node->method) {
                    auto func = std::make_shared<FunctionValue>();
                    func->decl = m->funcDecl.get();
                    func->closure = std::make_shared<Environment>(globals);
                    func->closure->define("this", thisObj);
                    if (currKlass->parentClass) {
                        func->closure->define("super", Value{currKlass->parentClass});
                    }
                    lastExprVal = Value{func};
                    return;
                }
            }
            currKlass = currKlass->parentClass;
        }
    }
    throw std::runtime_error("Super method '" + node->method + "' not found");
}

void Interpreter::visit(FieldDecl*) {
    // Fields are initialized during class instantiation, not visited independently
}

void Interpreter::visit(MethodDecl*) {
    // Methods are looked up during member access, not visited independently
}

void Interpreter::visit(ConstructorDecl*) {
    // Constructors are invoked during instantiation, not visited independently
}

void Interpreter::visit(PropertyDecl*) {
    // Properties are accessed through MemberExpr getter/setter dispatch
}

void Interpreter::visit(OperatorDecl*) {
    // Operators are dispatched during binary expression evaluation
}

void Interpreter::visit(InterfaceDecl*) {
    // Interfaces are metadata-only at runtime; no direct execution
}

void Interpreter::visit(CastExpr* node) {
    Value val = evaluate(node->expression.get());
    // Safe cast: returns null on failure
    if (val.isInstance()) {
        auto inst = std::get<std::shared_ptr<InstanceValue>>(val.val);
        // Walk class hierarchy to check if instance is of target type
        auto currKlass = inst->klass;
        while (currKlass) {
            if (currKlass->decl->name == node->targetType) {
                lastExprVal = val; // cast succeeds, return same object
                return;
            }
            // Check interfaces
            for (const auto& iface : currKlass->implementedInterfaces) {
                if (iface == node->targetType) {
                    lastExprVal = val;
                    return;
                }
            }
            currKlass = currKlass->parentClass;
        }
        // Cast failed, return null
        lastExprVal = Value{std::monostate{}};
    } else {
        // For non-instance types, basic type coercion
        if (node->targetType == "string") {
            lastExprVal = Value{val.toString()};
        } else if (node->targetType == "int" && val.isFloat()) {
            lastExprVal = Value{static_cast<long long>(std::get<double>(val.val))};
        } else if (node->targetType == "float" && val.isInt()) {
            lastExprVal = Value{static_cast<double>(std::get<long long>(val.val))};
        } else {
            lastExprVal = Value{std::monostate{}}; // cast failed
        }
    }
}

void Interpreter::visit(TypeTestExpr* node) {
    Value val = evaluate(node->expression.get());
    if (val.isInstance()) {
        auto inst = std::get<std::shared_ptr<InstanceValue>>(val.val);
        auto currKlass = inst->klass;
        while (currKlass) {
            if (currKlass->decl->name == node->targetType) {
                lastExprVal = Value{true};
                return;
            }
            for (const auto& iface : currKlass->implementedInterfaces) {
                if (iface == node->targetType) {
                    lastExprVal = Value{true};
                    return;
                }
            }
            currKlass = currKlass->parentClass;
        }
        lastExprVal = Value{false};
    } else {
        // Primitive type checking
        bool matches = false;
        if (node->targetType == "int") matches = val.isInt();
        else if (node->targetType == "float") matches = val.isFloat();
        else if (node->targetType == "string") matches = val.isString();
        else if (node->targetType == "bool") matches = val.isBool();
        else if (node->targetType == "char") matches = val.isChar();
        lastExprVal = Value{matches};
    }
}

void Interpreter::visit(ImportDecl*) {
    // ImportDecl is processed by the driver using topological load order.
}

} // namespace thalapathy
