#include <catch2/catch_test_macros.hpp>
#include "thalapathy/runtime/Value.hpp"

using namespace thalapathy;

TEST_CASE("Value handles primitives and references", "[runtime]") {
    Value intVal{123LL};
    CHECK(intVal.isInt());
    CHECK(intVal.toString() == "123");

    Value strVal{std::string("hello")};
    CHECK(strVal.isString());
    CHECK(strVal.toString() == "hello");

    Value nullVal{std::monostate{}};
    CHECK(nullVal.isNull());
    CHECK(nullVal.toString() == "null");
}
