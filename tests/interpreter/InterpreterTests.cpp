#include <catch2/catch_test_macros.hpp>
#include "thalapathy/lexer/Lexer.hpp"
#include "thalapathy/parser/Parser.hpp"
#include "thalapathy/interpreter/Interpreter.hpp"
#include "thalapathy/semantic/Resolver.hpp"

using namespace thalapathy;

TEST_CASE("Interpreter runs basic arithmetic", "[interpreter]") {
    std::string source = "makkal x = 0; thalapathy aarambam { x = 5 * 2 + 3; }";
    Lexer lexer("test.tvk", source);
    auto tokens = lexer.tokenize();
    Parser parser("test.tvk", tokens);
    auto nodes = parser.parse();

    Interpreter interpreter;
    interpreter.interpret(nodes);

    bool found = false;
    Value val = interpreter.getEnvironment()->get("x", found);
    REQUIRE(found);
    CHECK(std::get<long long>(val.val) == 13LL);
}

TEST_CASE("Interpreter evaluates functions and recursion", "[interpreter]") {
    std::string source = 
        "mersal fib(int n) -> int {\n"
        "    ghilli (n <= 1) { thiruppi n; }\n"
        "    thiruppi fib(n - 1) + fib(n - 2);\n"
        "}\n"
        "makkal result = 0;\n"
        "thalapathy aarambam {\n"
        "    result = fib(6);\n"
        "}\n";
    Lexer lexer("test.tvk", source);
    auto tokens = lexer.tokenize();
    Parser parser("test.tvk", tokens);
    auto nodes = parser.parse();

    Interpreter interpreter;
    interpreter.interpret(nodes);

    bool found = false;
    Value val = interpreter.getEnvironment()->get("result", found);
    REQUIRE(found);
    CHECK(std::get<long long>(val.val) == 8LL);
}

TEST_CASE("Interpreter runs OOP features", "[interpreter]") {
    std::string source = 
        // 1. Base class with fields, properties, constructors, static fields, and operator overloading
        "master Base {\n"
        "    static int count = 42;\n"
        "    int id;\n"
        "    string name;\n"
        "    beast int internalVal;\n"
        "    \n"
        "    mersal init() {\n"
        "        this.id = 1;\n"
        "        this.name = \"BaseClass\";\n"
        "        this.internalVal = 100;\n"
        "    }\n"
        "    \n"
        "    mersal init(int customId) {\n"
        "        this.id = customId;\n"
        "        this.name = \"CustomBase\";\n"
        "        this.internalVal = 200;\n"
        "    }\n"
        "    \n"
        "    mersal greet() -> string {\n"
        "        thiruppi \"Hello from Base\";\n"
        "    }\n"
        "    \n"
        "    seyal int internal { \n"
        "        paaru { \n"
        "            thiruppi this.internalVal; \n"
        "        } \n"
        "        maathu(v) { \n"
        "            this.internalVal = v; \n"
        "        } \n"
        "    }\n"
        "    \n"
        "    mersal operator +(int val) -> int {\n"
        "        thiruppi this.id + val;\n"
        "    }\n"
        "}\n"
        "\n"
        // 2. Derived class with inheritance and method override
        "master Derived varisu Base {\n"
        "    int extra;\n"
        "    \n"
        "    mersal init() : super() {\n"
        "        this.extra = 999;\n"
        "    }\n"
        "    \n"
        "    mersal init(int id, int ext) : super(id) {\n"
        "        this.extra = ext;\n"
        "    }\n"
        "    \n"
        "    mersal greet() -> string {\n"
        "        thiruppi \"Hello from Derived\";\n"
        "    }\n"
        "}\n"
        "\n"
        // 3. Interface and implementing class
        "kaththi IFighter {\n"
        "    mersal fight() -> void;\n"
        "}\n"
        "master Hero varisu Base kaththi IFighter {\n"
        "    mersal init() : super() {}\n"
        "    mersal fight() -> void {}\n"
        "}\n"
        "\n"
        "makkal testBase = null;\n"
        "makkal testDerived1 = null;\n"
        "makkal testDerived2 = null;\n"
        "makkal testHero = null;\n"
        "makkal staticVal = 0;\n"
        "makkal propVal1 = 0;\n"
        "makkal propVal2 = 0;\n"
        "makkal greetBase = \"\";\n"
        "makkal greetDerived = \"\";\n"
        "makkal castSuccess = null;\n"
        "makkal castFail = null;\n"
        "makkal isHero = false;\n"
        "makkal isBase = false;\n"
        "makkal operatorRes = 0;\n"
        "\n"
        "thalapathy aarambam {\n"
        "    testBase = Base();\n"
        "    testDerived1 = Derived();\n"
        "    testDerived2 = Derived(5, 500);\n"
        "    testHero = Hero();\n"
        "    \n"
        "    staticVal = Base.count;\n"
        "    greetBase = testBase.greet();\n"
        "    greetDerived = testDerived1.greet();\n"
        "    \n"
        "    propVal1 = testBase.internal;\n"
        "    testBase.internal = 350;\n"
        "    propVal2 = testBase.internal;\n"
        "    \n"
        "    castSuccess = testDerived1 as Base;\n"
        "    castFail = testBase as Derived;\n"
        "    \n"
        "    isHero = testHero aaguma IFighter;\n"
        "    isBase = testBase aaguma Base;\n"
        "    \n"
        "    operatorRes = testBase + 10;\n"
        "}\n";

    Lexer lexer("test_oop.tvk", source);
    auto tokens = lexer.tokenize();
    Parser parser("test_oop.tvk", tokens);
    auto nodes = parser.parse();

    Resolver resolver("test_oop.tvk", source);
    bool resolved = resolver.resolve(nodes);
    REQUIRE(resolved);

    Interpreter interpreter;
    interpreter.interpret(nodes);

    bool found = false;
    auto env = interpreter.getEnvironment();

    // Check basic field init
    Value bVal = env->get("testBase", found);
    REQUIRE(found);
    REQUIRE(bVal.isInstance());
    auto bInst = std::get<std::shared_ptr<InstanceValue>>(bVal.val);
    CHECK(std::get<long long>(bInst->fields["id"].val) == 1LL);

    // Check static field
    Value sVal = env->get("staticVal", found);
    REQUIRE(found);
    CHECK(std::get<long long>(sVal.val) == 42LL);

    // Check constructor overloading & parenting
    Value d2Val = env->get("testDerived2", found);
    REQUIRE(found);
    REQUIRE(d2Val.isInstance());
    auto d2Inst = std::get<std::shared_ptr<InstanceValue>>(d2Val.val);
    CHECK(std::get<long long>(d2Inst->fields["id"].val) == 5LL);
    CHECK(std::get<long long>(d2Inst->fields["extra"].val) == 500LL);

    // Check dynamic dispatch (override)
    Value gbVal = env->get("greetBase", found);
    CHECK(std::get<std::string>(gbVal.val) == "Hello from Base");
    Value gdVal = env->get("greetDerived", found);
    CHECK(std::get<std::string>(gdVal.val) == "Hello from Derived");

    // Check property getter and setter
    Value pv1 = env->get("propVal1", found);
    CHECK(std::get<long long>(pv1.val) == 100LL);
    Value pv2 = env->get("propVal2", found);
    CHECK(std::get<long long>(pv2.val) == 350LL);

    // Check casting
    Value cSuccess = env->get("castSuccess", found);
    CHECK(cSuccess.isInstance());
    Value cFail = env->get("castFail", found);
    CHECK(cFail.isNull());

    // Check type test
    Value isH = env->get("isHero", found);
    CHECK(std::get<bool>(isH.val) == true);
    Value isB = env->get("isBase", found);
    CHECK(std::get<bool>(isB.val) == true);

    // Check operator overloading
    Value opRes = env->get("operatorRes", found);
    CHECK(std::get<long long>(opRes.val) == 11LL);
}

