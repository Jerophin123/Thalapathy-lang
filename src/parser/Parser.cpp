#include "thalapathy/parser/Parser.hpp"
#include <sstream>
#include <iostream>

namespace thalapathy {

Parser::Parser(std::string filename_, std::vector<Token> tokens_)
    : filename(std::move(filename_)), tokens(std::move(tokens_)) {}

std::vector<std::unique_ptr<ASTNode>> Parser::parse() {
    std::vector<std::unique_ptr<ASTNode>> declarations;
    while (!isAtEnd()) {
        try {
            auto decl = declaration();
            if (decl) {
                declarations.push_back(std::move(decl));
            }
        } catch (const std::runtime_error&) {
            synchronize();
        }
    }
    return declarations;
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EOF_TOKEN;
}

Token Parser::peek() const {
    return tokens[current];
}

Token Parser::previous() const {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    Token t = peek();
    addError(t, message);
    throw std::runtime_error(message);
}

void Parser::addError(const Token& token, const std::string& message) {
    std::stringstream ss;
    ss << "parser error: " << message << " at " << token.span.filename
       << ":" << token.span.start_line << ":" << token.span.start_col;
    errors.push_back(ss.str());
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;

        switch (peek().type) {
            case TokenType::THALAPATHY:
            case TokenType::MERSAL:
            case TokenType::MASTER:
            case TokenType::NANBA:
            case TokenType::MAKKAL:
            case TokenType::URUTHI:
            case TokenType::GHILLI:
            case TokenType::BIGIL:
            case TokenType::VAATHI:
            case TokenType::POKKIRI:
            case TokenType::THERI:
            case TokenType::THIRUPPI:
                return;
            default:
                break;
        }
        advance();
    }
}

// Declarations
std::unique_ptr<ASTNode> Parser::declaration() {
    // module import: sarkar name;
    if (match(TokenType::SARKAR)) {
        Token nameToken = consume(TokenType::IDENTIFIER, "expected module name after 'sarkar'");
        consume(TokenType::SEMICOLON, "expected ';' after module name");
        auto importNode = std::make_unique<ImportDecl>();
        importNode->span = nameToken.span;
        importNode->moduleName = nameToken.value;
        return importNode;
    }

    // entry block: thalapathy aarambam
    if (check(TokenType::THALAPATHY)) {
        return entryBlockDeclaration();
    }

    // interface: kaththi Name
    if (match(TokenType::KATHTHI)) {
        return interfaceDeclaration();
    }

    // class: master Name
    if (match(TokenType::MASTER)) {
        return classDeclaration(false, false);
    }

    // abstract class: waiting master Name
    if (match(TokenType::WAITING)) {
        consume(TokenType::MASTER, "expected 'master' after 'waiting'");
        return classDeclaration(true, false);
    }

    // function: mersal name
    if (match(TokenType::MERSAL)) {
        return functionDeclaration();
    }

    // variables: nanba, makkal, uruthi
    if (match(TokenType::NANBA)) {
        auto decl = variableDeclaration(false, false); // immutable
        consume(TokenType::SEMICOLON, "expected ';' after variable declaration");
        return decl;
    }
    if (match(TokenType::MAKKAL)) {
        auto decl = variableDeclaration(true, false); // mutable
        consume(TokenType::SEMICOLON, "expected ';' after variable declaration");
        return decl;
    }
    if (match(TokenType::URUTHI)) {
        // Can be final class: uruthi master Name
        if (match(TokenType::MASTER)) {
            return classDeclaration(false, true);
        }
        auto decl = variableDeclaration(false, true); // constant
        consume(TokenType::SEMICOLON, "expected ';' after constant declaration");
        return decl;
    }

    // Typed variable declarations (e.g. int year = 2026;)
    if (check(TokenType::INT_TYPE) || check(TokenType::FLOAT_TYPE) ||
        check(TokenType::BOOL_TYPE) || check(TokenType::STRING_TYPE) ||
        check(TokenType::CHAR_TYPE)) {
        
        Token typeToken = advance();
        Token nameToken = consume(TokenType::IDENTIFIER, "expected identifier after type name");
        auto decl = std::make_unique<VarDecl>();
        decl->span = typeToken.span;
        decl->name = nameToken.value;
        decl->typeStr = typeToken.value;
        decl->isMutable = true; // explicitly typed variables are mutable by default
        decl->isConstant = false;

        if (match(TokenType::EQUAL)) {
            decl->initializer = expression();
        }
        consume(TokenType::SEMICOLON, "expected ';' after typed variable declaration");
        return decl;
    }

    // Fallback to statement
    return statement();
}

std::unique_ptr<ASTNode> Parser::entryBlockDeclaration() {
    Token thalaToken = consume(TokenType::THALAPATHY, "expected 'thalapathy'");
    consume(TokenType::AARAMBAM, "expected 'aarambam' after 'thalapathy'");
    
    auto entry = std::make_unique<EntryBlockDecl>();
    entry->span = thalaToken.span;
    entry->body = blockStatement();
    return entry;
}

std::unique_ptr<VarDecl> Parser::variableDeclaration(bool isMutable, bool isConstant) {
    Token keyword = previous();
    Token nameToken = consume(TokenType::IDENTIFIER, "expected identifier name");
    
    auto decl = std::make_unique<VarDecl>();
    decl->span = keyword.span;
    decl->name = nameToken.value;
    decl->isMutable = isMutable;
    decl->isConstant = isConstant;

    if (match(TokenType::EQUAL)) {
        decl->initializer = expression();
    }
    return decl;
}

std::unique_ptr<FuncDecl> Parser::functionDeclaration() {
    Token mersalToken = previous();
    Token nameToken = consume(TokenType::IDENTIFIER, "expected function name");
    consume(TokenType::LEFT_PAREN, "expected '(' after function name");

    auto decl = std::make_unique<FuncDecl>();
    decl->span = mersalToken.span;
    decl->name = nameToken.value;

    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            Token typeToken = advance(); // parameter type (e.g. int, float, string)
            Token paramName = consume(TokenType::IDENTIFIER, "expected parameter name");
            decl->params.push_back(FuncDecl::Param{paramName.value, typeToken.value});
        } while (match(TokenType::COMMA));
    }
    consume(TokenType::RIGHT_PAREN, "expected ')' after parameters");

    // optional return type: -> type
    if (match(TokenType::ARROW)) {
        Token retTypeToken = advance();
        decl->returnTypeStr = retTypeToken.value;
    } else {
        decl->returnTypeStr = "void";
    }

    decl->body = blockStatement();
    return decl;
}

std::unique_ptr<ClassDecl> Parser::classDeclaration() {
    return classDeclaration(false, false);
}

std::unique_ptr<ClassDecl> Parser::classDeclaration(bool isAbstractClass, bool isFinalClass) {
    Token nameToken = consume(TokenType::IDENTIFIER, "expected class name");

    auto decl = std::make_unique<ClassDecl>();
    decl->span = nameToken.span;
    decl->name = nameToken.value;
    decl->isAbstract = isAbstractClass;
    decl->isFinal = isFinalClass;

    // Generics check: class Name<goat T, goat U>
    if (match(TokenType::LT)) {
        do {
            consume(TokenType::GOAT, "expected 'goat' before generic parameter name");
            Token genParam = consume(TokenType::IDENTIFIER, "expected generic parameter name");
            decl->genericParams.push_back(genParam.value);
        } while (match(TokenType::COMMA));
        consume(TokenType::GT, "expected '>' to close generic parameters");
    }

    // Single inheritance: varisu Parent
    if (match(TokenType::VARISU)) {
        Token parentToken = consume(TokenType::IDENTIFIER, "expected parent class name");
        decl->parentName = parentToken.value;
    }

    // Implemented interfaces: kaththi Interface1, Interface2
    if (match(TokenType::KATHTHI)) {
        do {
            Token interfaceToken = consume(TokenType::IDENTIFIER, "expected interface name");
            decl->implementedInterfaces.push_back(interfaceToken.value);
        } while (match(TokenType::COMMA));
    }

    consume(TokenType::LEFT_BRACE, "expected '{' before class body");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // Parse modifiers: beast, leo, static, uruthi, waiting
        AccessModifier access = AccessModifier::PUBLIC;
        bool isStatic = false;
        bool isFinal = false;
        bool isAbstract = false;

        while (true) {
            if (match(TokenType::BEAST)) {
                access = AccessModifier::PRIVATE;
            } else if (match(TokenType::LEO)) {
                access = AccessModifier::PROTECTED;
            } else if (match(TokenType::STATIC)) {
                isStatic = true;
            } else if (match(TokenType::URUTHI)) {
                isFinal = true;
            } else if (match(TokenType::WAITING)) {
                isAbstract = true;
            } else {
                break;
            }
        }

        // Now parse member: method, constructor, property, operator, or field
        // Is it a property? (seyal keyword)
        if (match(TokenType::SEYAL)) {
            Token seyalToken = previous();
            std::string pTypeStr = parseTypeString();
            Token pName = consume(TokenType::IDENTIFIER, "expected property name");
            consume(TokenType::LEFT_BRACE, "expected '{' before property body");

            std::unique_ptr<ASTNode> getterBody = nullptr;
            std::unique_ptr<ASTNode> setterBody = nullptr;
            std::string setterParam = "value";

            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                if (match(TokenType::PAARU)) {
                    getterBody = blockStatement();
                } else if (match(TokenType::MAATHU)) {
                    if (match(TokenType::LEFT_PAREN)) {
                        Token paramToken = consume(TokenType::IDENTIFIER, "expected setter parameter name");
                        setterParam = paramToken.value;
                        consume(TokenType::RIGHT_PAREN, "expected ')' after setter parameter name");
                    }
                    setterBody = blockStatement();
                } else {
                    Token t = peek();
                    addError(t, "expected 'paaru' or 'maathu' in property definition");
                    advance();
                }
            }
            consume(TokenType::RIGHT_BRACE, "expected '}' after property body");

            auto propDecl = std::make_unique<PropertyDecl>();
            propDecl->span = seyalToken.span;
            propDecl->access = access;
            propDecl->isStatic = isStatic;
            propDecl->name = pName.value;
            propDecl->typeStr = pTypeStr;
            propDecl->getterBody = std::move(getterBody);
            propDecl->setterBody = std::move(setterBody);
            propDecl->setterParamName = setterParam;
            decl->properties.push_back(std::move(propDecl));
        }
        // Is it a constructor/method or operator overloading?
        else if (match(TokenType::MERSAL)) {
            Token mersalToken = previous();

            // Check if it is an operator overload
            if (match(TokenType::OPERATOR)) {
                Token opToken = advance(); // operator token (+, -, *, /, %, <, >)
                if (opToken.type != TokenType::PLUS && opToken.type != TokenType::MINUS &&
                    opToken.type != TokenType::STAR && opToken.type != TokenType::SLASH &&
                    opToken.type != TokenType::PERCENT && opToken.type != TokenType::LT &&
                    opToken.type != TokenType::GT) {
                    addError(opToken, "operator overloading only supported for +, -, *, /, %, <, >");
                }
                consume(TokenType::LEFT_PAREN, "expected '(' after operator name");
                std::string pTypeStr = parseTypeString();
                Token pName = consume(TokenType::IDENTIFIER, "expected operator parameter name");
                consume(TokenType::RIGHT_PAREN, "expected ')' after operator parameter");

                std::string retType = "void";
                if (match(TokenType::ARROW)) {
                    retType = parseTypeString();
                }
                auto body = blockStatement();

                auto operDecl = std::make_unique<OperatorDecl>();
                operDecl->span = mersalToken.span;
                operDecl->op = opToken.type;
                operDecl->param = OperatorDecl::Param{pName.value, pTypeStr};
                operDecl->returnTypeStr = retType;
                operDecl->body = std::move(body);
                decl->operators.push_back(std::move(operDecl));
            }
            // Normal Method or Constructor
            else {
                Token methodName = consume(TokenType::IDENTIFIER, "expected method name");

                // Check if constructor (name is "init")
                if (methodName.value == "init") {
                    consume(TokenType::LEFT_PAREN, "expected '(' after init");
                    std::vector<FuncDecl::Param> params;
                    if (!check(TokenType::RIGHT_PAREN)) {
                        do {
                            std::string pTypeStr = parseTypeString();
                            Token pName = consume(TokenType::IDENTIFIER, "expected parameter name");
                            params.push_back(FuncDecl::Param{pName.value, pTypeStr});
                        } while (match(TokenType::COMMA));
                    }
                    consume(TokenType::RIGHT_PAREN, "expected ')' after parameters");

                    // Initializer list (optional sibling/parent routing: e.g. super(name))
                    std::vector<std::unique_ptr<ASTNode>> initList;
                    if (match(TokenType::COLON)) {
                        do {
                            if (check(TokenType::SUPER) || check(TokenType::THIS)) {
                                Token cToken = advance();
                                consume(TokenType::LEFT_PAREN, "expected '(' after call");
                                std::vector<std::unique_ptr<ASTNode>> args;
                                if (!check(TokenType::RIGHT_PAREN)) {
                                    do {
                                        args.push_back(expression());
                                    } while (match(TokenType::COMMA));
                                }
                                consume(TokenType::RIGHT_PAREN, "expected ')' after call arguments");

                                auto call = std::make_unique<CallExpr>();
                                call->span = cToken.span;
                                auto callee = std::make_unique<IdentifierExpr>();
                                callee->span = cToken.span;
                                callee->name = cToken.value;
                                call->callee = std::move(callee);
                                call->arguments = std::move(args);
                                initList.push_back(std::move(call));
                            } else {
                                Token t = peek();
                                addError(t, "expected parent or sibling constructor call");
                                advance();
                            }
                        } while (match(TokenType::COMMA));
                    }

                    auto body = blockStatement();

                    auto constrDecl = std::make_unique<ConstructorDecl>();
                    constrDecl->span = mersalToken.span;
                    constrDecl->params = params;
                    constrDecl->initializerList = std::move(initList);
                    constrDecl->body = std::move(body);
                    decl->constructors.push_back(std::move(constrDecl));
                }
                // Regular Method
                else {
                    consume(TokenType::LEFT_PAREN, "expected '(' after method name");
                    std::vector<FuncDecl::Param> params;
                    if (!check(TokenType::RIGHT_PAREN)) {
                        do {
                            std::string pTypeStr = parseTypeString();
                            Token pName = consume(TokenType::IDENTIFIER, "expected parameter name");
                            params.push_back(FuncDecl::Param{pName.value, pTypeStr});
                        } while (match(TokenType::COMMA));
                    }
                    consume(TokenType::RIGHT_PAREN, "expected ')' after parameters");

                    std::string retType = "void";
                    if (match(TokenType::ARROW)) {
                        retType = parseTypeString();
                    }

                    std::unique_ptr<ASTNode> body = nullptr;
                    if (isAbstract) {
                        consume(TokenType::SEMICOLON, "expected ';' after abstract method signature");
                    } else {
                        body = blockStatement();
                    }

                    auto funcNode = std::make_unique<FuncDecl>();
                    funcNode->span = mersalToken.span;
                    funcNode->name = methodName.value;
                    funcNode->params = params;
                    funcNode->returnTypeStr = retType;
                    funcNode->body = std::move(body);

                    auto methodDecl = std::make_unique<MethodDecl>();
                    methodDecl->span = mersalToken.span;
                    methodDecl->access = access;
                    methodDecl->isStatic = isStatic;
                    methodDecl->isAbstract = isAbstract;
                    methodDecl->isFinal = isFinal;
                    methodDecl->funcDecl = std::move(funcNode);
                    decl->methods.push_back(std::move(methodDecl));
                }
            }
        }
        // Is it a field? (e.g. string name;)
        else if (check(TokenType::INT_TYPE) || check(TokenType::FLOAT_TYPE) ||
                 check(TokenType::BOOL_TYPE) || check(TokenType::STRING_TYPE) ||
                 check(TokenType::CHAR_TYPE) || check(TokenType::IDENTIFIER)) {
            std::string typeStr = parseTypeString();
            Token fieldName = consume(TokenType::IDENTIFIER, "expected field name");
            
            std::unique_ptr<ASTNode> initializer = nullptr;
            if (match(TokenType::EQUAL)) {
                initializer = expression();
            }
            consume(TokenType::SEMICOLON, "expected ';' after field declaration");

            auto varDecl = std::make_unique<VarDecl>();
            varDecl->span = fieldName.span;
            varDecl->name = fieldName.value;
            varDecl->typeStr = typeStr;
            varDecl->initializer = std::move(initializer);
            varDecl->isMutable = !isFinal;
            varDecl->isConstant = isFinal;

            auto fieldDecl = std::make_unique<FieldDecl>();
            fieldDecl->span = fieldName.span;
            fieldDecl->access = access;
            fieldDecl->isStatic = isStatic;
            fieldDecl->isFinal = isFinal;
            fieldDecl->varDecl = std::move(varDecl);
            decl->fields.push_back(std::move(fieldDecl));
        } else {
            Token t = peek();
            addError(t, "expected class member declaration");
            advance();
        }
    }

    consume(TokenType::RIGHT_BRACE, "expected '}' after class body");
    return decl;
}

std::unique_ptr<InterfaceDecl> Parser::interfaceDeclaration() {
    Token kaththiToken = previous();
    Token nameToken = consume(TokenType::IDENTIFIER, "expected interface name");

    auto decl = std::make_unique<InterfaceDecl>();
    decl->span = kaththiToken.span;
    decl->name = nameToken.value;

    if (match(TokenType::VARISU)) {
        do {
            Token parentToken = consume(TokenType::IDENTIFIER, "expected parent interface name");
            decl->parentInterfaces.push_back(parentToken.value);
        } while (match(TokenType::COMMA));
    }

    consume(TokenType::LEFT_BRACE, "expected '{' before interface body");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (match(TokenType::MERSAL)) {
            Token methodToken = previous();
            Token methodName = consume(TokenType::IDENTIFIER, "expected method name");
            consume(TokenType::LEFT_PAREN, "expected '(' after method name");

            std::vector<FuncDecl::Param> params;
            if (!check(TokenType::RIGHT_PAREN)) {
                do {
                    std::string pTypeStr = parseTypeString();
                    Token pName = consume(TokenType::IDENTIFIER, "expected parameter name");
                    params.push_back(FuncDecl::Param{pName.value, pTypeStr});
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RIGHT_PAREN, "expected ')' to close parameters");

            std::string retType = "void";
            if (match(TokenType::ARROW)) {
                retType = parseTypeString();
            }
            consume(TokenType::SEMICOLON, "expected ';' after interface method signature");

            auto methodSig = std::make_unique<FuncDecl>();
            methodSig->span = methodToken.span;
            methodSig->name = methodName.value;
            methodSig->params = params;
            methodSig->returnTypeStr = retType;
            methodSig->body = nullptr;
            decl->methodSignatures.push_back(std::move(methodSig));
        } else {
            Token t = peek();
            addError(t, "expected method contract signature inside interface");
            advance();
        }
    }

    consume(TokenType::RIGHT_BRACE, "expected '}' after interface body");
    return decl;
}

std::string Parser::parseTypeString() {
    Token typeToken = advance();
    std::string typeStr = typeToken.value;
    if (match(TokenType::LT)) {
        typeStr += "<";
        do {
            typeStr += parseTypeString();
        } while (match(TokenType::COMMA) && (typeStr += ",", true));
        consume(TokenType::GT, "expected '>' after generic type arguments");
        typeStr += ">";
    }
    return typeStr;
}

// Statements
std::unique_ptr<ASTNode> Parser::statement() {
    if (check(TokenType::LEFT_BRACE)) {
        return blockStatement();
    }
    if (match(TokenType::GHILLI)) {
        return ifStatement();
    }
    if (match(TokenType::BIGIL)) {
        return forStatement();
    }
    if (match(TokenType::VAATHI)) {
        return rangeLoopStatement();
    }
    if (match(TokenType::THIRUPPI)) {
        return returnStatement();
    }
    if (match(TokenType::THERI)) {
        return throwStatement();
    }
    if (match(TokenType::POKKIRI)) {
        return tryCatchStatement();
    }
    if (match(TokenType::BREAK)) {
        return breakStatement();
    }
    if (match(TokenType::CONTINUE)) {
        return continueStatement();
    }

    return expressionStatement();
}

std::unique_ptr<BlockStmt> Parser::blockStatement() {
    Token brace = consume(TokenType::LEFT_BRACE, "expected '{'");
    auto block = std::make_unique<BlockStmt>();
    block->span = brace.span;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        block->statements.push_back(declaration());
    }

    consume(TokenType::RIGHT_BRACE, "expected '}' to close block");
    return block;
}

std::unique_ptr<ExprStmt> Parser::expressionStatement() {
    auto expr = expression();
    consume(TokenType::SEMICOLON, "expected ';' after expression");
    
    auto stmt = std::make_unique<ExprStmt>();
    stmt->span = expr->span;
    stmt->expression = std::move(expr);
    return stmt;
}

std::unique_ptr<IfStmt> Parser::ifStatement() {
    Token ghilliToken = previous();
    consume(TokenType::LEFT_PAREN, "expected '(' before if condition");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "expected ')' after if condition");

    auto thenBranch = statement();
    std::unique_ptr<ASTNode> elseBranch = nullptr;

    if (match(TokenType::ILLANA)) {
        elseBranch = statement();
    }

    auto stmt = std::make_unique<IfStmt>();
    stmt->span = ghilliToken.span;
    stmt->condition = std::move(condition);
    stmt->thenBranch = std::move(thenBranch);
    stmt->elseBranch = std::move(elseBranch);
    return stmt;
}

std::unique_ptr<ForStmt> Parser::forStatement() {
    Token bigilToken = previous();
    consume(TokenType::LEFT_PAREN, "expected '(' after 'bigil'");

    std::unique_ptr<ASTNode> init = nullptr;
    if (!match(TokenType::SEMICOLON)) {
        if (match(TokenType::MAKKAL)) {
            init = variableDeclaration(true, false);
            consume(TokenType::SEMICOLON, "expected ';' after for-loop init variable");
        } else if (match(TokenType::NANBA)) {
            init = variableDeclaration(false, false);
            consume(TokenType::SEMICOLON, "expected ';' after for-loop init variable");
        } else {
            init = expressionStatement(); // eats its own semicolon
        }
    }

    std::unique_ptr<ASTNode> condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "expected ';' after for-loop condition");

    std::unique_ptr<ASTNode> update = nullptr;
    if (!check(TokenType::RIGHT_PAREN)) {
        update = expression();
    }
    consume(TokenType::RIGHT_PAREN, "expected ')' after for-loop clauses");

    auto body = statement();

    auto stmt = std::make_unique<ForStmt>();
    stmt->span = bigilToken.span;
    stmt->initializer = std::move(init);
    stmt->condition = std::move(condition);
    stmt->update = std::move(update);
    stmt->body = std::move(body);
    return stmt;
}

std::unique_ptr<RangeLoopStmt> Parser::rangeLoopStatement() {
    Token vaathiToken = previous();
    Token varNameToken = consume(TokenType::IDENTIFIER, "expected loop variable identifier");
    
    // Support range iteration syntax: vaathi i in range(start, end)
    consume(TokenType::IDENTIFIER, "expected 'in' keyword in loop"); // standard text 'in'
    
    Token rangeFuncToken = consume(TokenType::IDENTIFIER, "expected 'range' function");
    if (rangeFuncToken.value != "range") {
        addError(rangeFuncToken, "expected range iterator function");
    }

    consume(TokenType::LEFT_PAREN, "expected '(' after 'range'");
    auto startExpr = expression();
    consume(TokenType::COMMA, "expected ',' between range bounds");
    auto endExpr = expression();
    consume(TokenType::RIGHT_PAREN, "expected ')' after range bounds");

    auto body = statement();

    auto stmt = std::make_unique<RangeLoopStmt>();
    stmt->span = vaathiToken.span;
    stmt->varName = varNameToken.value;
    stmt->startExpr = std::move(startExpr);
    stmt->endExpr = std::move(endExpr);
    stmt->body = std::move(body);
    return stmt;
}

std::unique_ptr<ReturnStmt> Parser::returnStatement() {
    Token thiruppiToken = previous();
    std::unique_ptr<ASTNode> expr = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        expr = expression();
    }
    consume(TokenType::SEMICOLON, "expected ';' after return value");

    auto stmt = std::make_unique<ReturnStmt>();
    stmt->span = thiruppiToken.span;
    stmt->expression = std::move(expr);
    return stmt;
}

std::unique_ptr<ThrowStmt> Parser::throwStatement() {
    Token theriToken = previous();
    auto expr = expression();
    consume(TokenType::SEMICOLON, "expected ';' after throw expression");

    auto stmt = std::make_unique<ThrowStmt>();
    stmt->span = theriToken.span;
    stmt->expression = std::move(expr);
    return stmt;
}

std::unique_ptr<TryCatchStmt> Parser::tryCatchStatement() {
    Token pokkiriToken = previous();
    auto tryBody = blockStatement();
    
    consume(TokenType::KAAVALAN, "expected 'kaavalan' after try block");
    consume(TokenType::LEFT_PAREN, "expected '(' after 'kaavalan'");
    // Expected: Error type and variable name (e.g. Error e) or just variable name
    Token errorType = consume(TokenType::IDENTIFIER, "expected Exception/Error type or name");
    Token varName = errorType;
    if (check(TokenType::IDENTIFIER)) {
        varName = advance();
    }
    consume(TokenType::RIGHT_PAREN, "expected ')' after exception variable");

    auto catchBody = blockStatement();

    auto stmt = std::make_unique<TryCatchStmt>();
    stmt->span = pokkiriToken.span;
    stmt->tryBody = std::move(tryBody);
    stmt->catchVarName = varName.value;
    stmt->catchBody = std::move(catchBody);
    return stmt;
}

std::unique_ptr<BreakStmt> Parser::breakStatement() {
    Token breakToken = previous();
    consume(TokenType::SEMICOLON, "expected ';' after break");
    auto stmt = std::make_unique<BreakStmt>();
    stmt->span = breakToken.span;
    return stmt;
}

std::unique_ptr<ContinueStmt> Parser::continueStatement() {
    Token continueToken = previous();
    consume(TokenType::SEMICOLON, "expected ';' after continue");
    auto stmt = std::make_unique<ContinueStmt>();
    stmt->span = continueToken.span;
    return stmt;
}

// Expressions
std::unique_ptr<ASTNode> Parser::expression(Precedence precedence) {
    Token token = advance();
    auto prefixExpr = parsePrefix(token);
    if (!prefixExpr) {
        throw std::runtime_error("expected expression");
    }

    while (precedence < getPrecedence(peek().type)) {
        Token nextToken = advance();
        prefixExpr = parseInfix(nextToken, std::move(prefixExpr));
    }

    return prefixExpr;
}

std::unique_ptr<ASTNode> Parser::parsePrefix(const Token& token) {
    switch (token.type) {
        case TokenType::LITERAL_INT: {
            auto expr = std::make_unique<LiteralExpr>();
            expr->span = token.span;
            expr->value = std::stoll(token.value);
            return expr;
        }
        case TokenType::LITERAL_FLOAT: {
            auto expr = std::make_unique<LiteralExpr>();
            expr->span = token.span;
            expr->value = std::stod(token.value);
            return expr;
        }
        case TokenType::LITERAL_STRING: {
            auto expr = std::make_unique<LiteralExpr>();
            expr->span = token.span;
            expr->value = token.value;
            return expr;
        }
        case TokenType::LITERAL_CHAR: {
            auto expr = std::make_unique<LiteralExpr>();
            expr->span = token.span;
            expr->value = token.value.empty() ? '\0' : token.value[0];
            return expr;
        }
        case TokenType::TRUE_VAL: {
            auto expr = std::make_unique<LiteralExpr>();
            expr->span = token.span;
            expr->value = true;
            return expr;
        }
        case TokenType::FALSE_VAL: {
            auto expr = std::make_unique<LiteralExpr>();
            expr->span = token.span;
            expr->value = false;
            return expr;
        }
        case TokenType::NULL_VAL: {
            auto expr = std::make_unique<LiteralExpr>();
            expr->span = token.span;
            expr->value = std::monostate{};
            return expr;
        }
        case TokenType::SOLLU:
        case TokenType::IDENTIFIER: {
            auto expr = std::make_unique<IdentifierExpr>();
            expr->span = token.span;
            expr->name = token.value.empty() ? "sollu" : token.value;
            return expr;
        }
        case TokenType::THIS: {
            auto expr = std::make_unique<ThisExpr>();
            expr->span = token.span;
            return expr;
        }
        case TokenType::SUPER: {
            consume(TokenType::DOT, "expected '.' after 'super'");
            Token methodToken = consume(TokenType::IDENTIFIER, "expected super method name");
            auto expr = std::make_unique<SuperExpr>();
            expr->span = token.span;
            expr->method = methodToken.value;
            return expr;
        }

        // Parentheses
        case TokenType::LEFT_PAREN: {
            auto expr = expression();
            consume(TokenType::RIGHT_PAREN, "expected ')' to close grouped expression");
            return expr;
        }

        // Array Literal
        case TokenType::LEFT_BRACKET: {
            auto expr = std::make_unique<ArrayExpr>();
            expr->span = token.span;
            if (!check(TokenType::RIGHT_BRACKET)) {
                do {
                    expr->elements.push_back(expression());
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RIGHT_BRACKET, "expected ']' to close array literal");
            return expr;
        }

        // Map Literal
        case TokenType::LEFT_BRACE: {
            auto expr = std::make_unique<MapExpr>();
            expr->span = token.span;
            if (!check(TokenType::RIGHT_BRACE)) {
                do {
                    Token keyToken = consume(TokenType::IDENTIFIER, "expected map key identifier");
                    consume(TokenType::COLON, "expected ':' after map key");
                    auto value = expression();
                    expr->entries.push_back(std::make_pair(keyToken.value, std::move(value)));
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RIGHT_BRACE, "expected '}' to close map literal");
            return expr;
        }

        // Unary Prefix
        case TokenType::BANG:
        case TokenType::MINUS:
        case TokenType::PLUS_PLUS:
        case TokenType::MINUS_MINUS: {
            auto expr = std::make_unique<UnaryExpr>();
            expr->span = token.span;
            expr->op = token.type;
            expr->operand = expression(Precedence::UNARY);
            return expr;
        }

        default:
            addError(token, "unexpected expression start");
            return nullptr;
    }
}

std::unique_ptr<ASTNode> Parser::parseInfix(const Token& token, std::unique_ptr<ASTNode> left) {
    Precedence prec = getPrecedence(token.type);

    switch (token.type) {
        // Binary operators
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::STAR:
        case TokenType::SLASH:
        case TokenType::PERCENT:
        case TokenType::EQ_EQ:
        case TokenType::BANG_EQ:
        case TokenType::LT:
        case TokenType::GT:
        case TokenType::LT_EQ:
        case TokenType::GT_EQ:
        case TokenType::AMP_AMP:
        case TokenType::BAR_BAR: {
            auto expr = std::make_unique<BinaryExpr>();
            expr->span = token.span;
            expr->left = std::move(left);
            expr->op = token.type;
            expr->right = expression(prec);
            return expr;
        }

        // Assignment operators
        case TokenType::EQUAL:
        case TokenType::PLUS_EQ:
        case TokenType::MINUS_EQ:
        case TokenType::STAR_EQ:
        case TokenType::SLASH_EQ: {
            auto expr = std::make_unique<AssignExpr>();
            expr->span = token.span;
            expr->target = std::move(left);
            expr->op = token.type;
            expr->value = expression(Precedence::ASSIGNMENT); // right-associative
            return expr;
        }

        // Postfix operators
        case TokenType::PLUS_PLUS:
        case TokenType::MINUS_MINUS: {
            auto expr = std::make_unique<UnaryExpr>();
            expr->span = token.span;
            expr->op = token.type; // is postfix here
            expr->operand = std::move(left);
            return expr;
        }

        // Dot Access (Member access)
        case TokenType::DOT: {
            Token memberToken = consume(TokenType::IDENTIFIER, "expected member identifier name");
            auto expr = std::make_unique<MemberExpr>();
            expr->span = token.span;
            expr->object = std::move(left);
            expr->memberName = memberToken.value;
            return expr;
        }

        // Index Access
        case TokenType::LEFT_BRACKET: {
            auto index = expression();
            consume(TokenType::RIGHT_BRACKET, "expected ']' after index expression");
            auto expr = std::make_unique<IndexExpr>();
            expr->span = token.span;
            expr->object = std::move(left);
            expr->index = std::move(index);
            return expr;
        }

        // Function call
        case TokenType::LEFT_PAREN: {
            auto expr = std::make_unique<CallExpr>();
            expr->span = token.span;
            expr->callee = std::move(left);
            if (!check(TokenType::RIGHT_PAREN)) {
                do {
                    expr->arguments.push_back(expression());
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RIGHT_PAREN, "expected ')' after function arguments");
            return expr;
        }

        case TokenType::AAGUMA: {
            std::string typeName = parseTypeString();
            auto expr = std::make_unique<TypeTestExpr>();
            expr->span = token.span;
            expr->expression = std::move(left);
            expr->targetType = typeName;
            return expr;
        }

        case TokenType::AS: {
            std::string typeName = parseTypeString();
            auto expr = std::make_unique<CastExpr>();
            expr->span = token.span;
            expr->expression = std::move(left);
            expr->targetType = typeName;
            return expr;
        }

        default:
            return left;
    }
}

Precedence Parser::getPrecedence(TokenType type) const {
    switch (type) {
        case TokenType::EQUAL:
        case TokenType::PLUS_EQ:
        case TokenType::MINUS_EQ:
        case TokenType::STAR_EQ:
        case TokenType::SLASH_EQ:
            return Precedence::ASSIGNMENT;
        case TokenType::BAR_BAR:
            return Precedence::OR;
        case TokenType::AMP_AMP:
            return Precedence::AND;
        case TokenType::EQ_EQ:
        case TokenType::BANG_EQ:
            return Precedence::EQUALITY;
        case TokenType::LT:
        case TokenType::GT:
        case TokenType::LT_EQ:
        case TokenType::GT_EQ:
            return Precedence::COMPARISON;
        case TokenType::PLUS:
        case TokenType::MINUS:
            return Precedence::SUM;
        case TokenType::STAR:
        case TokenType::SLASH:
        case TokenType::PERCENT:
            return Precedence::PRODUCT;
        case TokenType::PLUS_PLUS:
        case TokenType::MINUS_MINUS:
            return Precedence::UNARY; // treated as unary/postfix high precedence
        case TokenType::AAGUMA:
            return Precedence::COMPARISON;
        case TokenType::AS:
            return Precedence::AS_CAST;
        case TokenType::DOT:
        case TokenType::LEFT_BRACKET:
        case TokenType::LEFT_PAREN:
            return Precedence::CALL;
        default:
            return Precedence::NONE;
    }
}

} // namespace thalapathy
