/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_tests.hpp"

namespace
{
    struct clazz_1 {
        static int int_variable;
        static const int const_int_variable;

        static int& ref_int_variable;
        static const int& const_ref_int_variable;
    };

    int clazz_1::int_variable = 1;
    const int clazz_1::const_int_variable = 2;

    int& clazz_1::ref_int_variable = clazz_1::int_variable;
    const int& clazz_1::const_ref_int_variable = clazz_1::const_int_variable;
}

TEST_CASE("meta/meta_states/variable") {
    namespace meta = meta_hpp;

    meta::class_<clazz_1>()
        .variable_("int_variable", &clazz_1::int_variable)
        .variable_("const_int_variable", &clazz_1::const_int_variable)
        .variable_("ref_int_variable", &clazz_1::ref_int_variable)
        .variable_("const_ref_int_variable", &clazz_1::const_ref_int_variable);

    const meta::class_type clazz_1_type = meta::resolve_type<clazz_1>();
    REQUIRE(clazz_1_type);

    SUBCASE("") {
        const meta::variable variable;
        CHECK_FALSE(variable);
        CHECK_FALSE(variable.is_valid());
        CHECK(variable == clazz_1_type.get_variable("non-existent-variable"));
    }

    SUBCASE("operators") {
        meta::variable int_variable_v = clazz_1_type.get_variable("int_variable");
        meta::variable const_int_variable_v = clazz_1_type.get_variable("const_int_variable");
        CHECK(int_variable_v == int_variable_v);
        CHECK(int_variable_v != const_int_variable_v);
        CHECK((int_variable_v < const_int_variable_v || const_int_variable_v < int_variable_v));
    }

    SUBCASE("int") {
        meta::variable vm = clazz_1_type.get_variable("int_variable");
        REQUIRE(vm);

        CHECK(vm.get_type() == meta::resolve_type(&clazz_1::int_variable));
        CHECK(vm.get_name() == "int_variable");

        CHECK(vm.get() == 1);

        CHECK_NOTHROW(vm.set(10)); CHECK(vm.get() == 10);
    }

    SUBCASE("const int") {
        meta::variable vm = clazz_1_type.get_variable("const_int_variable");
        REQUIRE(vm);

        CHECK(vm.get_type() == meta::resolve_type(&clazz_1::const_int_variable));
        CHECK(vm.get_name() == "const_int_variable");

        CHECK(vm.get() == 2);

        CHECK_THROWS(vm.set(10)); CHECK(vm.get() == 2);
    }

    SUBCASE("ref int") {
        meta::variable vm = clazz_1_type.get_variable("ref_int_variable");
        REQUIRE(vm);

        CHECK(vm.get_type() == meta::resolve_type(&clazz_1::ref_int_variable));
        CHECK(vm.get_name() == "ref_int_variable");

        CHECK(vm.get() == 10);

        CHECK_NOTHROW(vm.set(20)); CHECK(vm.get() == 20);
    }

    SUBCASE("const ref int") {
        meta::variable vm = clazz_1_type.get_variable("const_ref_int_variable");
        REQUIRE(vm);

        CHECK(vm.get_type() == meta::resolve_type(&clazz_1::const_ref_int_variable));
        CHECK(vm.get_name() == "const_ref_int_variable");

        CHECK(vm.get() == 2);

        CHECK_THROWS(vm.set(10)); CHECK(vm.get() == 2);
    }
}
