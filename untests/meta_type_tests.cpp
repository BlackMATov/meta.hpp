/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_type.hpp>

#include <meta.hpp/meta_class.hpp>
#include <meta.hpp/meta_field.hpp>
#include <meta.hpp/meta_function.hpp>
#include <meta.hpp/meta_method.hpp>
#include <meta.hpp/meta_namespace.hpp>
#include <meta.hpp/meta_variable.hpp>

#include "doctest/doctest.hpp"

namespace
{
    struct clazz {
        int field{42};
        static int variable;
        int method() const { return 42; }
        static int function() { return 42; }
    };

    int clazz::variable{42};
}

TEST_CASE("meta/type") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    meta::type class_type = meta::class_<clazz>("clazz");
    CHECK(class_type.is_class());
    CHECK(class_type.get_class()->id() == "clazz");

    meta::type field_type = meta::field_("field", &clazz::field);
    CHECK(field_type.is_field());
    CHECK(field_type.get_field()->id() == "field");

    meta::type function_type = meta::function_<&clazz::function>("function");
    CHECK(function_type.is_function());
    CHECK(function_type.get_function()->id() == "function");

    meta::type method_type = meta::method_("method", &clazz::method);
    CHECK(method_type.is_method());
    CHECK(method_type.get_method()->id() == "method");

    meta::type namespace_type = meta::namespace_("ns");
    CHECK(namespace_type.is_namespace());
    CHECK(namespace_type.get_namespace()->id() == "ns");

    meta::type variable_type = meta::variable_<&clazz::variable>("variable");
    CHECK(variable_type.is_variable());
    CHECK(variable_type.get_variable()->id() == "variable");
}

TEST_CASE("meta/type/merge") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    {
        meta::type clazz_type = meta::class_<clazz>("clazz")(
            meta::field_("field", &clazz::field)
        );

        clazz_type.merge(meta::class_<clazz>("clazz")(
            meta::function_<&clazz::function>("function")
        ));

        REQUIRE(clazz_type.is_class());
        CHECK(clazz_type.get_class()->get_field("field"));
        CHECK(clazz_type.get_class()->get_function("function"));
    }
}
