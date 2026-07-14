#include "thalapathy/semantic/Resolver.hpp"
#include <iostream>

namespace thalapathy {

Resolver::Resolver(std::string filename_, std::string sourceCode_)
    : filename(std::move(filename_)), sourceCode(std::move(sourceCode_)) {
    // Add global built-in functions
    beginScope(); // Global scope
    
    // built-in: sollu
    Symbol solluSym;
    solluSym.name = "sollu";
    solluSym.type = Type{TypeKind::FUNCTION, ""};
    solluSym.kind = SymbolKind::FUNCTION;
    solluSym.isMutable = false;
    declare(solluSym);

    // built-in: len
    Symbol lenSym;
    lenSym.name = "len";
    lenSym.type = Type{TypeKind::FUNCTION, ""};
    lenSym.kind = SymbolKind::FUNCTION;
    lenSym.isMutable = false;
    declare(lenSym);

    // built-in: range
    Symbol rangeSym;
    rangeSym.name = "range";
    rangeSym.type = Type{TypeKind::FUNCTION, ""};
    rangeSym.kind = SymbolKind::FUNCTION;
    rangeSym.isMutable = false;
    declare(rangeSym);

    // built-in: type
    Symbol typeSym;
    typeSym.name = "type";
    typeSym.type = Type{TypeKind::FUNCTION, ""};
    typeSym.kind = SymbolKind::FUNCTION;
    typeSym.isMutable = false;
    declare(typeSym);

    // built-in conversion functions
    for (const std::string name : {
            "string", "int", "float", "bool", "char",
            "__native_read_line", "__native_time_now",
            "__native_read_file", "__native_write_file", "__native_file_exists",
            "__native_get_env", "__native_set_env", "__native_sys_command",
            "__native_math_sqrt", "__native_math_sin", "__native_math_cos",
            "__native_math_ceil", "__native_math_floor",
            "__native_string_split", "__native_string_replace",
            "__native_random", "__native_random_int", "__native_random_seed", "__native_sha256",
            "__native_os_name", "__native_os_cwd", "__native_os_exit",
            "__native_time_millis", "__native_sleep_ms",
            "__native_json_stringify", "__native_json_parse",
            "__native_http_get", "__native_http_post", "__native_http_request",
            "__native_http_serve",
            "__native_db_open", "__native_db_insert", "__native_db_all",
            "__native_db_find", "__native_db_get", "__native_db_update",
            "__native_db_remove", "__native_db_count",
            "__native_regex_match", "__native_regex_find_all", "__native_regex_replace",
            "__native_date_format", "__native_date_now_seconds", "__native_log",
            "__native_ws_upgrade", "__native_ws_send", "__native_ws_recv",
            "__native_sse_upgrade", "__native_sse_send",
            "__native_socket_is_closed", "__native_socket_close"}) {
        Symbol convSym;
        convSym.name = name;
        convSym.type = Type{TypeKind::FUNCTION, ""};
        convSym.kind = SymbolKind::FUNCTION;
        convSym.isMutable = false;
        declare(convSym);
    }
}

bool Resolver::resolve(const std::vector<std::unique_ptr<ASTNode>>& nodes, bool isEntryFile) {
    // Phase 1: Register classes, interfaces and functions
    for (const auto& node : nodes) {
        if (auto classDecl = dynamic_cast<ClassDecl*>(node.get())) {
            ClassInfo info;
            info.name = classDecl->name;
            info.parentName = classDecl->parentName;
            info.isAbstract = classDecl->isAbstract;
            info.isFinal = classDecl->isFinal;
            info.implementedInterfaces = classDecl->implementedInterfaces;
            info.genericParams = classDecl->genericParams;

            for (const auto& field : classDecl->fields) {
                info.fields[field->varDecl->name] = makePrimitiveType(field->varDecl->typeStr);
                info.fieldIsStatic[field->varDecl->name] = field->isStatic;
                info.fieldIsFinal[field->varDecl->name] = field->isFinal;
                info.fieldAccess[field->varDecl->name] = field->access;
            }
            for (const auto& method : classDecl->methods) {
                std::vector<Type> paramTypes;
                for (const auto& p : method->funcDecl->params) {
                    paramTypes.push_back(makePrimitiveType(p.typeStr));
                }
                ClassInfo::MethodSig sig;
                sig.name = method->funcDecl->name;
                sig.paramTypes = paramTypes;
                sig.returnType = makePrimitiveType(method->funcDecl->returnTypeStr);
                sig.access = method->access;
                sig.isStatic = method->isStatic;
                sig.isAbstract = method->isAbstract;
                sig.isFinal = method->isFinal;
                sig.isOverride = method->isOverride;
                info.methods[method->funcDecl->name].push_back(sig);
            }
            for (const auto& constr : classDecl->constructors) {
                std::vector<Type> paramTypes;
                for (const auto& p : constr->params) {
                    paramTypes.push_back(makePrimitiveType(p.typeStr));
                }
                ClassInfo::ConstructorSig sig;
                sig.paramTypes = paramTypes;
                info.constructors.push_back(sig);
            }
            for (const auto& prop : classDecl->properties) {
                ClassInfo::PropertyInfo pInfo;
                pInfo.name = prop->name;
                pInfo.type = makePrimitiveType(prop->typeStr);
                pInfo.access = prop->access;
                pInfo.isStatic = prop->isStatic;
                pInfo.hasGetter = (prop->getterBody != nullptr);
                pInfo.hasSetter = (prop->setterBody != nullptr);
                info.properties[prop->name] = pInfo;
            }
            for (const auto& oper : classDecl->operators) {
                ClassInfo::OperatorInfo oInfo;
                oInfo.op = oper->op;
                oInfo.paramType = makePrimitiveType(oper->param.typeStr);
                oInfo.returnType = makePrimitiveType(oper->returnTypeStr);
                info.operators.push_back(oInfo);
            }
            classTable[classDecl->name] = info;

            // Register in global symbol table
            Symbol sym;
            sym.name = classDecl->name;
            sym.type = Type{TypeKind::CLASS, classDecl->name};
            sym.kind = SymbolKind::CLASS;
            sym.isMutable = false;
            declare(sym);
        } else if (auto ifaceDecl = dynamic_cast<InterfaceDecl*>(node.get())) {
            ClassInfo info;
            info.name = ifaceDecl->name;
            info.isInterface = true;
            info.parentInterfaces = ifaceDecl->parentInterfaces;
            for (const auto& method : ifaceDecl->methodSignatures) {
                std::vector<Type> paramTypes;
                for (const auto& p : method->params) {
                    paramTypes.push_back(makePrimitiveType(p.typeStr));
                }
                ClassInfo::MethodSig sig;
                sig.name = method->name;
                sig.paramTypes = paramTypes;
                sig.returnType = makePrimitiveType(method->returnTypeStr);
                sig.access = AccessModifier::PUBLIC;
                sig.isStatic = false;
                sig.isAbstract = true;
                info.methods[method->name].push_back(sig);
            }
            classTable[ifaceDecl->name] = info;

            // Register in global symbol table
            Symbol sym;
            sym.name = ifaceDecl->name;
            sym.type = Type{TypeKind::CLASS, ifaceDecl->name};
            sym.kind = SymbolKind::CLASS;
            sym.isMutable = false;
            declare(sym);
        } else if (auto funcDecl = dynamic_cast<FuncDecl*>(node.get())) {
            Symbol sym;
            sym.name = funcDecl->name;
            sym.type = Type{TypeKind::FUNCTION, ""};
            sym.kind = SymbolKind::FUNCTION;
            sym.isMutable = false;
            declare(sym);
        } else if (auto enumDecl = dynamic_cast<EnumDecl*>(node.get())) {
            Symbol sym;
            sym.name = enumDecl->name;
            sym.type = Type{TypeKind::CLASS, enumDecl->name};
            sym.kind = SymbolKind::CLASS;
            sym.isMutable = false;
            declare(sym);
        }
    }

    // Check for inheritance cycles
    for (const auto& [name, info] : classTable) {
        std::string currentParent = info.parentName;
        std::unordered_map<std::string, bool> visited;
        visited[name] = true;
        while (!currentParent.empty()) {
            if (visited[currentParent]) {
                reportError("THALA-CLASS-001", "inheritance cycle detected", SourceSpan{filename},
                            "class '" + name + "' inherits cyclically", "break the cycle by removing inheritance");
                return false;
            }
            visited[currentParent] = true;
            auto it = classTable.find(currentParent);
            if (it == classTable.end()) {
                // Parent not defined, we'll let resolving detect it later
                break;
            }
            currentParent = it->second.parentName;
        }
    }

    // `comeback` (override) validation: an overriding method must have a
    // matching method somewhere up the parent chain.
    for (const auto& [name, info] : classTable) {
        for (const auto& [methodName, sigs] : info.methods) {
            bool anyOverride = false;
            for (const auto& s : sigs) if (s.isOverride) anyOverride = true;
            if (!anyOverride) continue;
            bool foundInParent = false;
            std::string p = info.parentName;
            std::unordered_map<std::string, bool> seen;
            while (!p.empty() && !seen[p]) {
                seen[p] = true;
                auto it = classTable.find(p);
                if (it == classTable.end()) break;
                if (it->second.methods.find(methodName) != it->second.methods.end()) {
                    foundInParent = true;
                    break;
                }
                p = it->second.parentName;
            }
            if (!foundInParent) {
                reportError("THALA-OVERRIDE-001", "invalid comeback (override)", SourceSpan{filename},
                            "method '" + methodName + "' in '" + name + "' is marked 'comeback' but no parent class declares it",
                            "remove 'comeback', or make sure a parent class has this method");
            }
        }
    }

    // Phase 2: Walk AST
    for (const auto& node : nodes) {
        node->accept(this);
    }

    // Must have exactly one aarambam entry block for the entry file
    if (isEntryFile && !hasEntryBlock) {
        reportError("THALA-FUNC-003", "missing entry block", SourceSpan{filename},
                    "expected 'thalapathy aarambam' entry block", "define 'thalapathy aarambam { ... }' in your file");
    }

    return diagnostics.empty();
}

void Resolver::beginScope() {
    scopes.push_back(ScopeMap{});
}

void Resolver::endScope() {
    scopes.pop_back();
}

bool Resolver::declare(const Symbol& sym) {
    if (scopes.empty()) return false;
    auto& currentScope = scopes.back();
    if (currentScope.find(sym.name) != currentScope.end()) {
        return false;
    }
    currentScope[sym.name] = sym;
    return true;
}

bool Resolver::lookup(const std::string& name, Symbol& outSym) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            outSym = found->second;
            return true;
        }
    }
    return false;
}

bool Resolver::isDeclaredInCurrentScope(const std::string& name) {
    if (scopes.empty()) return false;
    return scopes.back().find(name) != scopes.back().end();
}

void Resolver::reportError(const std::string& code, const std::string& title, const SourceSpan& span, const std::string& exp, const std::string& help) {
    Diagnostic diag;
    diag.code = code;
    diag.severity = DiagnosticSeverity::ERROR;
    diag.title = title;
    diag.span = span;
    diag.explanation = exp;
    diag.helpMessage = help;
    diagnostics.push_back(diag);
}

// Visitor implementations
void Resolver::visit(VarDecl* node) {
    if (isDeclaredInCurrentScope(node->name)) {
        reportError("THALA-NAME-001", "duplicate declaration", node->span,
                    "variable '" + node->name + "' is already defined in this scope",
                    "use a different name or remove this declaration");
        return;
    }

    Type initType{TypeKind::ANY, ""};
    if (node->initializer) {
        node->initializer->accept(this);
        initType = currentExprType;
    }

    Type declaredType = makePrimitiveType(node->typeStr);
    if (!node->typeStr.empty() && node->typeStr != "any") {
        if (!declaredType.isCompatible(initType)) {
            reportError("THALA-TYPE-001", "type mismatch", node->span,
                        "expected '" + typeKindToString(declaredType.kind) + "', found '" + typeKindToString(initType.kind) + "'",
                        "change the declared type or change the initializer type");
        }
    } else {
        declaredType = initType; // Infer type
    }

    Symbol sym;
    sym.name = node->name;
    sym.type = declaredType;
    sym.kind = SymbolKind::VARIABLE;
    sym.isMutable = node->isMutable;
    sym.isConstant = node->isConstant;
    sym.span = node->span;

    declare(sym);
}

void Resolver::visit(NanbiDecl* node) {
    // Resolve the source expression once, then declare every bound name.
    if (node->initializer) {
        node->initializer->accept(this);
    }
    for (const auto& b : node->bindings) {
        if (b.kind == NanbiBinding::Kind::Ignore) continue; // `_` binds nothing
        if (isDeclaredInCurrentScope(b.name)) {
            reportError("THALA-NANBI-004", "duplicate pattern binding", node->span,
                        "binding '" + b.name + "' appears more than once in this nanbi pattern",
                        "use a different name for each binding");
            continue;
        }
        Symbol sym;
        sym.name = b.name;
        sym.type = Type{TypeKind::ANY, ""};
        sym.kind = SymbolKind::VARIABLE;
        sym.isMutable = false;   // nanbi bindings are immutable
        sym.isConstant = false;
        sym.span = node->span;
        declare(sym);
    }
}

void Resolver::visit(MatchStmt* node) {
    node->subject->accept(this);
    for (const auto& arm : node->arms) {
        if (arm.pattern) arm.pattern->accept(this);
        if (arm.body) arm.body->accept(this);
    }
    if (node->defaultBody) node->defaultBody->accept(this);
}

void Resolver::visit(FuncDecl* node) {
    Symbol sym;
    sym.name = node->name;
    sym.type = Type{TypeKind::FUNCTION, ""};
    sym.kind = SymbolKind::FUNCTION;
    sym.isMutable = false;
    declare(sym);

    FuncDecl* prevFunc = currentFunction;
    currentFunction = node;

    beginScope();
    for (const auto& p : node->params) {
        Symbol paramSym;
        paramSym.name = p.name;
        paramSym.type = makePrimitiveType(p.typeStr);
        paramSym.kind = SymbolKind::PARAMETER;
        paramSym.isMutable = true;
        declare(paramSym);
    }

    node->body->accept(this);

    endScope();
    currentFunction = prevFunc;
}

void Resolver::visit(ClassDecl* node) {
    auto it = classTable.find(node->name);
    ClassInfo* prevClass = currentClass;
    if (it != classTable.end()) {
        currentClass = &it->second;
    }

    beginScope();
    
    // Declare 'this' and 'super'
    Symbol thisSym;
    thisSym.name = "this";
    thisSym.type = Type{TypeKind::CLASS, node->name};
    thisSym.kind = SymbolKind::VARIABLE;
    thisSym.isMutable = false;
    declare(thisSym);

    if (!node->parentName.empty()) {
        Symbol superSym;
        superSym.name = "super";
        superSym.type = Type{TypeKind::CLASS, node->parentName};
        superSym.kind = SymbolKind::VARIABLE;
        superSym.isMutable = false;
        declare(superSym);
    }

    // Walk fields, methods, constructors, properties, and operators
    for (const auto& field : node->fields) {
        field->accept(this);
    }
    for (const auto& method : node->methods) {
        method->accept(this);
    }
    for (const auto& constr : node->constructors) {
        constr->accept(this);
    }
    for (const auto& prop : node->properties) {
        prop->accept(this);
    }
    for (const auto& oper : node->operators) {
        oper->accept(this);
    }

    endScope();
    currentClass = prevClass;
}

void Resolver::visit(EntryBlockDecl* node) {
    if (hasEntryBlock) {
        reportError("THALA-FUNC-002", "duplicate entry block", node->span,
                    "only one entry block ('thalapathy aarambam') is allowed",
                    "remove or merge this duplicate block");
        return;
    }
    hasEntryBlock = true;

    beginScope();
    node->body->accept(this);
    endScope();
}

void Resolver::visit(BlockStmt* node) {
    beginScope();
    for (const auto& stmt : node->statements) {
        stmt->accept(this);
    }
    endScope();
}

void Resolver::visit(ExprStmt* node) {
    node->expression->accept(this);
}

void Resolver::visit(IfStmt* node) {
    node->condition->accept(this);
    if (currentExprType.kind != TypeKind::BOOL && currentExprType.kind != TypeKind::ANY) {
        reportError("THALA-TYPE-001", "type mismatch", node->condition->span,
                    "if condition must be boolean, found '" + typeKindToString(currentExprType.kind) + "'",
                    "use a comparison or boolean value for the condition");
    }
    node->thenBranch->accept(this);
    if (node->elseBranch) {
        node->elseBranch->accept(this);
    }
}

void Resolver::visit(ForStmt* node) {
    loopDepth++;
    beginScope();
    if (node->initializer) node->initializer->accept(this);
    if (node->condition) {
        node->condition->accept(this);
        if (currentExprType.kind != TypeKind::BOOL && currentExprType.kind != TypeKind::ANY) {
            reportError("THALA-TYPE-001", "type mismatch", node->condition->span,
                        "loop condition must be boolean, found '" + typeKindToString(currentExprType.kind) + "'",
                        "use a comparison or boolean value");
        }
    }
    if (node->update) node->update->accept(this);
    node->body->accept(this);
    endScope();
    loopDepth--;
}

void Resolver::visit(RangeLoopStmt* node) {
    loopDepth++;
    beginScope();
    
    node->startExpr->accept(this);
    if (currentExprType.kind != TypeKind::INT && currentExprType.kind != TypeKind::ANY) {
        reportError("THALA-TYPE-001", "type mismatch", node->startExpr->span,
                    "range start must be int, found '" + typeKindToString(currentExprType.kind) + "'",
                    "use an integer for range bound");
    }
    node->endExpr->accept(this);
    if (currentExprType.kind != TypeKind::INT && currentExprType.kind != TypeKind::ANY) {
        reportError("THALA-TYPE-001", "type mismatch", node->endExpr->span,
                    "range end must be int, found '" + typeKindToString(currentExprType.kind) + "'",
                    "use an integer for range bound");
    }

    Symbol loopVar;
    loopVar.name = node->varName;
    loopVar.type = Type{TypeKind::INT, ""};
    loopVar.kind = SymbolKind::VARIABLE;
    loopVar.isMutable = false;
    declare(loopVar);

    node->body->accept(this);
    endScope();
    loopDepth--;
}

void Resolver::visit(ForEachStmt* node) {
    node->iterable->accept(this);
    loopDepth++;
    beginScope();
    if (node->hasPattern) {
        // `vaathi nanbi [a, b] ulla ...` — declare each pattern binding.
        for (const auto& b : node->patternBindings) {
            if (b.kind == NanbiBinding::Kind::Ignore) continue;
            Symbol sym;
            sym.name = b.name;
            sym.type = Type{TypeKind::ANY, ""};
            sym.kind = SymbolKind::VARIABLE;
            sym.isMutable = false;
            declare(sym);
        }
    } else {
        Symbol loopVar;
        loopVar.name = node->varName;
        loopVar.type = Type{TypeKind::ANY, ""};
        loopVar.kind = SymbolKind::VARIABLE;
        loopVar.isMutable = false;
        declare(loopVar);
    }
    node->body->accept(this);
    endScope();
    loopDepth--;
}

void Resolver::visit(WhileStmt* node) {
    node->condition->accept(this);
    if (currentExprType.kind != TypeKind::BOOL && currentExprType.kind != TypeKind::ANY) {
        reportError("THALA-TYPE-001", "type mismatch", node->condition->span,
                    "thuppakki condition must be boolean, found '" + typeKindToString(currentExprType.kind) + "'",
                    "use a comparison or boolean value for the condition");
    }
    loopDepth++;
    node->body->accept(this);
    loopDepth--;
}

void Resolver::visit(SwitchStmt* node) {
    node->subject->accept(this);
    // A thalaivaa block allows break statements to exit early.
    loopDepth++;
    for (const auto& c : node->cases) {
        c.value->accept(this);
        c.body->accept(this);
    }
    if (node->defaultBody) {
        node->defaultBody->accept(this);
    }
    loopDepth--;
}

void Resolver::visit(ReturnStmt* node) {
    if (!currentFunction && !insideFunctionLike) {
        reportError("THALA-FUNC-001", "return outside function", node->span,
                    "thiruppi statement used outside of a mersal function block",
                    "place this statement inside a valid function body");
        return;
    }

    Type retType{TypeKind::VOID, ""};
    if (node->expression) {
        node->expression->accept(this);
        retType = currentExprType;
    }

    if (currentFunction) {
        Type expectedType = makePrimitiveType(currentFunction->returnTypeStr);
        if (!expectedType.isCompatible(retType)) {
            reportError("THALA-TYPE-001", "return type mismatch", node->span,
                        "expected return type '" + typeKindToString(expectedType.kind) + "', found '" + typeKindToString(retType.kind) + "'",
                        "ensure the returned value matches the function signature");
        }
    }
}

void Resolver::visit(ThrowStmt* node) {
    node->expression->accept(this);
}

void Resolver::visit(TryCatchStmt* node) {
    node->tryBody->accept(this);
    if (node->catchBody) {
        beginScope();
        Symbol errSym;
        errSym.name = node->catchVarName;
        errSym.type = Type{TypeKind::ANY, ""};
        errSym.kind = SymbolKind::VARIABLE;
        errSym.isMutable = false;
        declare(errSym);
        node->catchBody->accept(this);
        endScope();
    }
    if (node->finallyBody) {
        node->finallyBody->accept(this);
    }
}

void Resolver::visit(BreakStmt* node) {
    if (loopDepth == 0) {
        reportError("THALA-LOOP-001", "break outside loop", node->span,
                    "break statement must be nested inside a bigil or vaathi loop",
                    "remove this statement or wrap it inside a loop");
    }
}

void Resolver::visit(ContinueStmt* node) {
    if (loopDepth == 0) {
        reportError("THALA-LOOP-001", "continue outside loop", node->span,
                    "continue statement must be nested inside a bigil or vaathi loop",
                    "remove this statement or wrap it inside a loop");
    }
}

void Resolver::visit(LiteralExpr* node) {
    std::visit([this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, long long>) {
            currentExprType = Type{TypeKind::INT, ""};
        } else if constexpr (std::is_same_v<T, double>) {
            currentExprType = Type{TypeKind::FLOAT, ""};
        } else if constexpr (std::is_same_v<T, bool>) {
            currentExprType = Type{TypeKind::BOOL, ""};
        } else if constexpr (std::is_same_v<T, std::string>) {
            currentExprType = Type{TypeKind::STRING, ""};
        } else if constexpr (std::is_same_v<T, char>) {
            currentExprType = Type{TypeKind::CHAR, ""};
        } else {
            currentExprType = Type{TypeKind::NULL_TYPE, ""};
        }
    }, node->value);
}

void Resolver::visit(IdentifierExpr* node) {
    Symbol sym;
    if (!lookup(node->name, sym)) {
        reportError("THALA-NAME-002", "undefined variable", node->span,
                    "identifier '" + node->name + "' has not been declared in this scope",
                    "declare this variable before utilizing it");
        currentExprType = Type{TypeKind::ANY, ""};
        return;
    }
    currentExprType = sym.type;
}

void Resolver::visit(BinaryExpr* node) {
    node->left->accept(this);
    Type leftType = currentExprType;
    node->right->accept(this);
    Type rightType = currentExprType;

    // Check operator validity
    if (node->op == TokenType::PLUS) {
        if ((leftType.kind == TypeKind::STRING && rightType.kind == TypeKind::STRING) ||
            (leftType.kind == TypeKind::INT && rightType.kind == TypeKind::INT) ||
            (leftType.kind == TypeKind::FLOAT && rightType.kind == TypeKind::FLOAT)) {
            currentExprType = leftType;
        } else if (leftType.kind == TypeKind::ANY || rightType.kind == TypeKind::ANY) {
            currentExprType = Type{TypeKind::ANY, ""};
        } else {
            reportError("THALA-TYPE-001", "invalid operator '+'", node->span,
                        "operator '+' is not supported between '" + typeKindToString(leftType.kind) + "' and '" + typeKindToString(rightType.kind) + "'",
                        "convert values explicitly or match types");
            currentExprType = Type{TypeKind::ANY, ""};
        }
    } else if (node->op == TokenType::MINUS || node->op == TokenType::STAR ||
               node->op == TokenType::SLASH || node->op == TokenType::PERCENT) {
        if ((leftType.kind == TypeKind::INT && rightType.kind == TypeKind::INT) ||
            (leftType.kind == TypeKind::FLOAT && rightType.kind == TypeKind::FLOAT)) {
            currentExprType = leftType;
        } else if (leftType.kind == TypeKind::ANY || rightType.kind == TypeKind::ANY) {
            currentExprType = Type{TypeKind::ANY, ""};
        } else {
            reportError("THALA-TYPE-001", "invalid numeric operator", node->span,
                        "numeric operations require matching numeric types",
                        "make sure both sides are numeric");
            currentExprType = Type{TypeKind::ANY, ""};
        }
    } else if (node->op == TokenType::EQ_EQ || node->op == TokenType::BANG_EQ) {
        currentExprType = Type{TypeKind::BOOL, ""};
    } else if (node->op == TokenType::LT || node->op == TokenType::GT ||
               node->op == TokenType::LT_EQ || node->op == TokenType::GT_EQ) {
        if ((leftType.kind == TypeKind::INT || leftType.kind == TypeKind::FLOAT || leftType.kind == TypeKind::ANY) &&
            (rightType.kind == TypeKind::INT || rightType.kind == TypeKind::FLOAT || rightType.kind == TypeKind::ANY)) {
            currentExprType = Type{TypeKind::BOOL, ""};
        } else {
            reportError("THALA-TYPE-001", "invalid comparison", node->span,
                        "comparisons are only valid for numeric types",
                        "make sure both operands are numeric");
            currentExprType = Type{TypeKind::BOOL, ""};
        }
    } else if (node->op == TokenType::AMP_AMP || node->op == TokenType::BAR_BAR) {
        if ((leftType.kind == TypeKind::BOOL || leftType.kind == TypeKind::ANY) &&
            (rightType.kind == TypeKind::BOOL || rightType.kind == TypeKind::ANY)) {
            currentExprType = Type{TypeKind::BOOL, ""};
        } else {
            reportError("THALA-TYPE-001", "invalid logical operator", node->span,
                        "logical operations require boolean operands",
                        "make sure both operands are bool");
            currentExprType = Type{TypeKind::BOOL, ""};
        }
    }
}

void Resolver::visit(UnaryExpr* node) {
    node->operand->accept(this);
    if (node->op == TokenType::BANG) {
        if (currentExprType.kind != TypeKind::BOOL && currentExprType.kind != TypeKind::ANY) {
            reportError("THALA-TYPE-001", "invalid logical negation", node->span,
                        "logical negation is only valid for bool, found '" + typeKindToString(currentExprType.kind) + "'",
                        "provide a boolean operand");
        }
        currentExprType = Type{TypeKind::BOOL, ""};
    } else if (node->op == TokenType::MINUS || node->op == TokenType::PLUS_PLUS || node->op == TokenType::MINUS_MINUS) {
        if (currentExprType.kind != TypeKind::INT && currentExprType.kind != TypeKind::FLOAT && currentExprType.kind != TypeKind::ANY) {
            reportError("THALA-TYPE-001", "invalid numeric operator", node->span,
                        "operator is only valid for numeric types",
                        "provide a numeric operand");
        }
    } else if (node->op == TokenType::KAATHIRU) {
        currentExprType = Type{TypeKind::ANY, ""};
    }
}

void Resolver::visit(AssignExpr* node) {
    node->value->accept(this);
    Type valType = currentExprType;

    // Resolve target
    if (auto ident = dynamic_cast<IdentifierExpr*>(node->target.get())) {
        Symbol sym;
        if (!lookup(ident->name, sym)) {
            reportError("THALA-NAME-002", "undefined variable", ident->span,
                        "identifier '" + ident->name + "' has not been declared",
                        "declare this variable before assignment");
            return;
        }
        if (!sym.isMutable) {
            reportError("THALA-MUT-001", "cannot assign to immutable binding", node->span,
                        "cannot assign to immutable binding '" + ident->name + "'",
                        "make the variable mutable with 'makkal' instead of 'nanba'");
            return;
        }
        if (sym.isConstant) {
            reportError("THALA-MUT-002", "cannot assign to constant", node->span,
                        "cannot assign to constant '" + ident->name + "'",
                        "constants declared with 'uruthi' cannot be modified");
            return;
        }
        if (!sym.type.isCompatible(valType)) {
            reportError("THALA-TYPE-001", "type mismatch", node->span,
                        "expected '" + typeKindToString(sym.type.kind) + "', found '" + typeKindToString(valType.kind) + "'",
                        "ensure types align or use type conversions");
        }
        
        // Update type in the scopes if it was previously NULL_TYPE
        if (sym.type.kind == TypeKind::NULL_TYPE) {
            for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
                auto found = it->find(ident->name);
                if (found != it->end()) {
                    found->second.type = valType;
                    sym.type = valType;
                    break;
                }
            }
        }
        
        currentExprType = sym.type;
    } else {
        node->target->accept(this);
        // Let gradual assignment pass for index/members
        currentExprType = valType;
    }
}

void Resolver::visit(CallExpr* node) {
    node->callee->accept(this);
    Type calleeType = currentExprType;

    for (const auto& arg : node->arguments) {
        arg->accept(this);
    }

    // Check function signature if identifier callee
    if (auto ident = dynamic_cast<IdentifierExpr*>(node->callee.get())) {
        if (ident->name == "range") {
            if (node->arguments.size() != 2) {
                reportError("THALA-FUNC-004", "wrong function arity", node->span,
                            "range() expects exactly 2 arguments, received " + std::to_string(node->arguments.size()),
                            "provide start and end bounds");
            }
        }
    }

    currentExprType = Type{TypeKind::ANY, ""};
}

void Resolver::visit(MemberExpr* node) {
    node->object->accept(this);
    Type objType = currentExprType;

    if (objType.kind == TypeKind::CLASS) {
        auto it = classTable.find(objType.className);
        if (it != classTable.end()) {
            auto fIt = it->second.fields.find(node->memberName);
            if (fIt != it->second.fields.end()) {
                currentExprType = fIt->second;
                return;
            }
        }
    }
    // Gradual typing lets it pass
    currentExprType = Type{TypeKind::ANY, ""};
}

void Resolver::visit(IndexExpr* node) {
    node->object->accept(this);
    node->index->accept(this);
    currentExprType = Type{TypeKind::ANY, ""};
}

void Resolver::visit(ArrayExpr* node) {
    Type elemType{TypeKind::ANY, ""};
    if (!node->elements.empty()) {
        node->elements[0]->accept(this);
        elemType = currentExprType;
        for (size_t i = 1; i < node->elements.size(); ++i) {
            node->elements[i]->accept(this);
            if (!elemType.isCompatible(currentExprType)) {
                // Heterogeneous literal => treat as a tuple of `any`. This is
                // intentional: it powers `nanbi [...]` multi-value / tuple
                // destructuring (e.g. ["Vijay", "Ghilli", 2004]).
                elemType = Type{TypeKind::ANY, ""};
            }
        }
    }
    currentExprType = Type{TypeKind::ARRAY, ""};
}

void Resolver::visit(MapExpr* node) {
    for (const auto& entry : node->entries) {
        entry.second->accept(this);
    }
    currentExprType = Type{TypeKind::MAP, ""};
}

void Resolver::visit(LambdaExpr* node) {
    FuncDecl* fn = node->fn.get();
    FuncDecl* prevFunc = currentFunction;
    bool prevFuncLike = insideFunctionLike;
    int prevLoopDepth = loopDepth;
    currentFunction = fn;
    insideFunctionLike = true;
    loopDepth = 0;

    beginScope();
    for (const auto& p : fn->params) {
        Symbol paramSym;
        paramSym.name = p.name;
        paramSym.type = makePrimitiveType(p.typeStr);
        paramSym.kind = SymbolKind::PARAMETER;
        paramSym.isMutable = true;
        declare(paramSym);
    }
    if (fn->body) {
        fn->body->accept(this);
    }
    endScope();

    loopDepth = prevLoopDepth;
    insideFunctionLike = prevFuncLike;
    currentFunction = prevFunc;
    currentExprType = Type{TypeKind::FUNCTION, ""};
}

void Resolver::visit(ThisExpr* node) {
    if (!currentClass) {
        reportError("THALA-CLASS-003", "invalid 'this' usage", node->span,
                    "keyword 'this' can only be used inside class methods",
                    "remove 'this' or wrap it inside a class scope");
        currentExprType = Type{TypeKind::ANY, ""};
        return;
    }
    currentExprType = Type{TypeKind::CLASS, currentClass->name};
}

void Resolver::visit(SuperExpr* node) {
    if (!currentClass) {
        reportError("THALA-CLASS-004", "invalid 'super' usage", node->span,
                    "keyword 'super' can only be used inside class methods",
                    "remove 'super'");
        currentExprType = Type{TypeKind::ANY, ""};
        return;
    }
    if (currentClass->parentName.empty()) {
        reportError("THALA-CLASS-004", "invalid 'super' usage", node->span,
                    "cannot use 'super' in a class that does not inherit from any parent",
                    "define a parent class using 'varisu' first");
        currentExprType = Type{TypeKind::ANY, ""};
        return;
    }
    currentExprType = Type{TypeKind::ANY, ""};
}

void Resolver::visit(FieldDecl* node) {
    node->varDecl->accept(this);
}

void Resolver::visit(MethodDecl* node) {
    if (node->isAbstract) {
        return;
    }
    FuncDecl* func = node->funcDecl.get();
    FuncDecl* prevFunc = currentFunction;
    currentFunction = func;

    beginScope();
    for (const auto& p : func->params) {
        Symbol paramSym;
        paramSym.name = p.name;
        paramSym.type = makePrimitiveType(p.typeStr);
        paramSym.kind = SymbolKind::PARAMETER;
        paramSym.isMutable = true;
        declare(paramSym);
    }
    if (func->body) {
        func->body->accept(this);
    }
    endScope();
    currentFunction = prevFunc;
}

void Resolver::visit(ConstructorDecl* node) {
    bool prevFuncLike = insideFunctionLike;
    insideFunctionLike = true;
    beginScope();
    for (const auto& p : node->params) {
        Symbol paramSym;
        paramSym.name = p.name;
        paramSym.type = makePrimitiveType(p.typeStr);
        paramSym.kind = SymbolKind::PARAMETER;
        paramSym.isMutable = true;
        declare(paramSym);
    }
    for (const auto& initCall : node->initializerList) {
        initCall->accept(this);
    }
    if (node->body) {
        node->body->accept(this);
    }
    endScope();
    insideFunctionLike = prevFuncLike;
}

void Resolver::visit(PropertyDecl* node) {
    bool prevFuncLike = insideFunctionLike;
    insideFunctionLike = true;
    if (node->getterBody) {
        beginScope();
        node->getterBody->accept(this);
        endScope();
    }
    if (node->setterBody) {
        beginScope();
        Symbol valSym;
        valSym.name = node->setterParamName;
        valSym.type = makePrimitiveType(node->typeStr);
        valSym.kind = SymbolKind::PARAMETER;
        valSym.isMutable = false;
        declare(valSym);
        node->setterBody->accept(this);
        endScope();
    }
    insideFunctionLike = prevFuncLike;
}

void Resolver::visit(OperatorDecl* node) {
    bool prevFuncLike = insideFunctionLike;
    insideFunctionLike = true;
    beginScope();
    Symbol paramSym;
    paramSym.name = node->param.name;
    paramSym.type = makePrimitiveType(node->param.typeStr);
    paramSym.kind = SymbolKind::PARAMETER;
    paramSym.isMutable = false;
    declare(paramSym);
    if (node->body) {
        node->body->accept(this);
    }
    endScope();
    insideFunctionLike = prevFuncLike;
}

void Resolver::visit(InterfaceDecl*) {
}

void Resolver::visit(CastExpr* node) {
    node->expression->accept(this);
    currentExprType = makePrimitiveType(node->targetType);
}

void Resolver::visit(TypeTestExpr* node) {
    node->expression->accept(this);
    currentExprType = Type{TypeKind::BOOL, ""};
}

void Resolver::visit(ImportDecl*) {
    // ImportDecl is processed by the driver using topological load order.
}

void Resolver::visit(PackageDecl*) {
    // Package declarations are organizational metadata; no semantic checks.
}

void Resolver::visit(EnumDecl*) {
    // Enum names are registered in phase 1; members resolve dynamically.
}

void Resolver::visit(TernaryExpr* node) {
    node->condition->accept(this);
    node->thenExpr->accept(this);
    Type thenType = currentExprType;
    node->elseExpr->accept(this);
    Type elseType = currentExprType;
    // Result type unifies to a common type, else ANY.
    currentExprType = (thenType.kind == elseType.kind) ? thenType : Type{TypeKind::ANY, ""};
}

} // namespace thalapathy
