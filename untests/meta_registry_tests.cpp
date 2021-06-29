/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_registry.hpp>
#include "doctest/doctest.hpp"

namespace
{
    class clazz {};
    int func(int a) { return a; }
    int variable{1};
    const int cvariable{2};
}

TEST_CASE("meta/registry") {
    namespace meta = meta_hpp;

    meta::registry registry;
    const meta::registry& cregistry = registry;

    CHECK_FALSE(cregistry.get_class("clazz"));
    CHECK_FALSE(cregistry.get_function("func"));
    CHECK_FALSE(cregistry.get_namespace("ns"));
    CHECK_FALSE(cregistry.get_variable("variable"));
    CHECK_FALSE(cregistry.get_variable("cvariable"));

    registry(
        meta::class_<clazz>("clazz"),
        meta::function_<&func>("func"),
        meta::namespace_("ns"),
        meta::variable_<&variable>("variable"),
        meta::variable_<&cvariable>("cvariable"));

    CHECK(cregistry.get_class("clazz"));
    CHECK(cregistry.get_function("func"));
    CHECK(cregistry.get_namespace("ns"));
    CHECK(cregistry.get_variable("variable"));
    CHECK(cregistry.get_variable("cvariable"));

    {
        meta::class_info clazz_info = cregistry.get_class("clazz").value();
        CHECK(clazz_info.id() == "clazz");
    }

    {
        meta::function_info clazz_info = cregistry.get_function("func").value();
        CHECK(clazz_info.id() == "func");
    }

    {
        meta::namespace_info namespace_info = cregistry.get_namespace("ns").value();
        CHECK(namespace_info.id() == "ns");
    }

    {
        meta::variable_info variable_info = cregistry.get_variable("variable").value();
        CHECK(variable_info.id() == "variable");
    }

    {
        meta::variable_info cvariable_info = cregistry.get_variable("cvariable").value();
        CHECK(cvariable_info.id() == "cvariable");
    }
}
