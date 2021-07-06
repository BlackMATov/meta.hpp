/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_class.hpp>
#include <meta.hpp/meta_field.hpp>
#include <meta.hpp/meta_function.hpp>
#include <meta.hpp/meta_method.hpp>
#include <meta.hpp/meta_variable.hpp>

#include "doctest/doctest.hpp"

namespace
{
    class clazz {
    public:
        class clazz2 {
        public:
            int field{21};
            const int cfield{22};
        };

        class clazz3 {
        public:
            int field{31};
            const int cfield{32};
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
    const meta::class_info& clazz_info = class_;

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
        meta::field_("field", &clazz::field),
        meta::field_("cfield", &clazz::cfield),
        meta::function_("func", &clazz::func),
        meta::method_("method", &clazz::method),
        meta::method_("cmethod", &clazz::cmethod),
        meta::variable_("variable", &clazz::variable),
        meta::variable_("cvariable", &clazz::cvariable));

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

TEST_CASE("meta/class/merge") {
    namespace meta = meta_hpp;

    meta::class_<clazz> clazz_{"clazz"};
    const meta::class_info& clazz_info = clazz_;

    SUBCASE("merge") {
        CHECK_NOTHROW(clazz_(
            meta::class_<clazz::clazz2>("child")(
                meta::field_("field", &clazz::clazz2::field)
            )
        ));

        CHECK_NOTHROW(clazz_(
            meta::class_<clazz::clazz2>("child")(
                meta::field_("cfield", &clazz::clazz2::cfield)
            )
        ));

        CHECK_THROWS_AS(clazz_(
            meta::class_<clazz::clazz3>("child")(
                meta::field_("field", &clazz::clazz3::field)
            )
        ), std::logic_error);

        CHECK(clazz_info.get_class("child"));
        CHECK(clazz_info.get_class("child")->get_field("field"));
        CHECK(clazz_info.get_class("child")->get_field("cfield"));

        {
            clazz::clazz2 instance{};
            CHECK(clazz_info.get_class("child")->get_field("field")->get(instance).to_int() == 21);
            CHECK(clazz_info.get_class("child")->get_field("cfield")->get(instance).to_int() == 22);
        }
    }
}
