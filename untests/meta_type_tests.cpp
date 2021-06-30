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

    meta::type class_type = meta::class_<clazz>("clazz").info();
    CHECK(class_type.is_class());
    CHECK(class_type.get_class_info().id() == "clazz");

    meta::type field_type = meta::field_<&clazz::field>("field").info();
    CHECK(field_type.is_field());
    CHECK(field_type.get_field_info().id() == "field");

    meta::type function_type = meta::function_<&clazz::function>("function").info();
    CHECK(function_type.is_function());
    CHECK(function_type.get_function_info().id() == "function");

    meta::type method_type = meta::method_<&clazz::method>("method").info();
    CHECK(method_type.is_method());
    CHECK(method_type.get_method_info().id() == "method");

    meta::type namespace_type = meta::namespace_("ns").info();
    CHECK(namespace_type.is_namespace());
    CHECK(namespace_type.get_namespace_info().id() == "ns");

    meta::type variable_type = meta::variable_<&clazz::variable>("variable").info();
    CHECK(variable_type.is_variable());
    CHECK(variable_type.get_variable_info().id() == "variable");
}
