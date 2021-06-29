/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_namespace.hpp>
#include "doctest/doctest.hpp"

namespace
{
    class clazz {};
    int func(int a) { return a; }
    int variable{1};
    const int cvariable{2};
}

TEST_CASE("meta/namespace") {
    namespace meta = meta_hpp;

    meta::namespace_ namespace_{"ns"};
    const meta::namespace_& cnamespace_ = namespace_;
    const meta::namespace_info& ns_info = cnamespace_.info();

    CHECK_FALSE(ns_info.get_class("clazz"));
    CHECK_FALSE(ns_info.get_function("func"));
    CHECK_FALSE(ns_info.get_namespace("ns"));
    CHECK_FALSE(ns_info.get_variable("variable"));
    CHECK_FALSE(ns_info.get_variable("cvariable"));

    namespace_(
        meta::class_<clazz>("clazz"),
        meta::function_<&func>("func"),
        meta::namespace_("ns2"),
        meta::variable_<&variable>("variable"),
        meta::variable_<&cvariable>("cvariable"));

    CHECK(ns_info.get_class("clazz"));
    CHECK(ns_info.get_function("func"));
    CHECK(ns_info.get_namespace("ns2"));
    CHECK(ns_info.get_variable("variable"));
    CHECK(ns_info.get_variable("cvariable"));

    {
        meta::class_info clazz_info = ns_info.get_class("clazz").value();
        CHECK(clazz_info.id() == "clazz");
    }

    {
        meta::function_info clazz_info = ns_info.get_function("func").value();
        CHECK(clazz_info.id() == "func");
    }

    {
        meta::namespace_info namespace_info = ns_info.get_namespace("ns2").value();
        CHECK(namespace_info.id() == "ns2");
    }

    {
        meta::variable_info variable_info = ns_info.get_variable("variable").value();
        CHECK(variable_info.id() == "variable");
    }

    {
        meta::variable_info cvariable_info = ns_info.get_variable("cvariable").value();
        CHECK(cvariable_info.id() == "cvariable");
    }
}
