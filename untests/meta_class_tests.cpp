/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_class.hpp>
#include "doctest/doctest.hpp"

namespace
{
    class clazz {
    public:
        class clazz2 {
        };

        int field{1};
        const int cfield{2};

        static int func(int a) { return a; }

        int method(int a) { return a; }
        int cmethod(int a) const { return a; }

        static int variable;
        static const int cvariable;
    };

    int clazz::variable{1};
    const int clazz::cvariable{2};
}

TEST_CASE("meta/class") {
    namespace meta = meta_hpp;

    meta::class_<clazz> class_{"clazz"};
    const meta::class_<clazz>& cclass_ = class_;
    const meta::class_info& clazz_info = cclass_.info();

    CHECK_FALSE(clazz_info.get_class("clazz2"));
    CHECK_FALSE(clazz_info.get_field("field"));
    CHECK_FALSE(clazz_info.get_field("cfield"));
    CHECK_FALSE(clazz_info.get_function("func"));
    CHECK_FALSE(clazz_info.get_method("method"));
    CHECK_FALSE(clazz_info.get_method("cmethod"));
    CHECK_FALSE(clazz_info.get_variable("variable"));
    CHECK_FALSE(clazz_info.get_variable("cvariable"));

    class_(
        meta::class_<clazz::clazz2>("clazz2"),
        meta::field_<&clazz::field>("field"),
        meta::field_<&clazz::cfield>("cfield"),
        meta::function_<&clazz::func>("func"),
        meta::method_<&clazz::method>("method"),
        meta::method_<&clazz::cmethod>("cmethod"),
        meta::variable_<&clazz::variable>("variable"),
        meta::variable_<&clazz::cvariable>("cvariable"));

    CHECK(clazz_info.get_class("clazz2"));
    CHECK(clazz_info.get_field("field"));
    CHECK(clazz_info.get_field("cfield"));
    CHECK(clazz_info.get_function("func"));
    CHECK(clazz_info.get_method("method"));
    CHECK(clazz_info.get_method("cmethod"));
    CHECK(clazz_info.get_variable("variable"));
    CHECK(clazz_info.get_variable("cvariable"));

    {
        meta::class_info clazz2_info = clazz_info.get_class("clazz2").value();
        CHECK(clazz2_info.id() == "clazz2");
    }

    {
        meta::field_info field_info = clazz_info.get_field("field").value();
        CHECK(field_info.id() == "field");
    }

    {
        meta::field_info cfield_info = clazz_info.get_field("cfield").value();
        CHECK(cfield_info.id() == "cfield");
    }

    {
        meta::function_info function_info = clazz_info.get_function("func").value();
        CHECK(function_info.id() == "func");
    }

    {
        meta::method_info method_info = clazz_info.get_method("method").value();
        CHECK(method_info.id() == "method");
    }

    {
        meta::method_info cmethod_info = clazz_info.get_method("cmethod").value();
        CHECK(cmethod_info.id() == "cmethod");
    }

    {
        meta::variable_info variable_info = clazz_info.get_variable("variable").value();
        CHECK(variable_info.id() == "variable");
    }

    {
        meta::variable_info cvariable_info = clazz_info.get_variable("cvariable").value();
        CHECK(cvariable_info.id() == "cvariable");
    }
}
