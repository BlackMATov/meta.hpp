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

    {
        const meta::class_info info = class_.make_info();
        CHECK_FALSE(info.get_class("clazz2"));
        CHECK_FALSE(info.get_field("field"));
        CHECK_FALSE(info.get_field("cfield"));
        CHECK_FALSE(info.get_function("func"));
        CHECK_FALSE(info.get_method("method"));
        CHECK_FALSE(info.get_method("cmethod"));
        CHECK_FALSE(info.get_variable("variable"));
        CHECK_FALSE(info.get_variable("cvariable"));
    }

    class_(
        meta::class_<clazz::clazz2>("clazz2"),
        meta::field_("field", &clazz::field),
        meta::field_("cfield", &clazz::cfield),
        meta::function_("func", &clazz::func),
        meta::method_("method", &clazz::method),
        meta::method_("cmethod", &clazz::cmethod),
        meta::variable_("variable", &clazz::variable),
        meta::variable_("cvariable", &clazz::cvariable));

    {
        const meta::class_info info = class_.make_info();
        CHECK(info.get_class("clazz2"));
        CHECK(info.get_field("field"));
        CHECK(info.get_field("cfield"));
        CHECK(info.get_function("func"));
        CHECK(info.get_method("method"));
        CHECK(info.get_method("cmethod"));
        CHECK(info.get_variable("variable"));
        CHECK(info.get_variable("cvariable"));

        {
            meta::class_info clazz2_info = info.get_class("clazz2").value();
            CHECK(clazz2_info.id() == "clazz2");
        }

        {
            meta::field_info field_info = info.get_field("field").value();
            CHECK(field_info.id() == "field");
        }

        {
            meta::field_info cfield_info = info.get_field("cfield").value();
            CHECK(cfield_info.id() == "cfield");
        }

        {
            meta::function_info function_info = info.get_function("func").value();
            CHECK(function_info.id() == "func");
        }

        {
            meta::method_info method_info = info.get_method("method").value();
            CHECK(method_info.id() == "method");
        }

        {
            meta::method_info cmethod_info = info.get_method("cmethod").value();
            CHECK(cmethod_info.id() == "cmethod");
        }

        {
            meta::variable_info variable_info = info.get_variable("variable").value();
            CHECK(variable_info.id() == "variable");
        }

        {
            meta::variable_info cvariable_info = info.get_variable("cvariable").value();
            CHECK(cvariable_info.id() == "cvariable");
        }
    }
}

TEST_CASE("meta/class/merge") {
    namespace meta = meta_hpp;

    meta::class_<clazz> clazz_{"clazz"};

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

        {
            const meta::class_info info = clazz_.make_info();

            CHECK(info.get_class("child"));
            CHECK(info.get_class("child")->get_field("field"));
            CHECK(info.get_class("child")->get_field("cfield"));

            {
                clazz::clazz2 instance{};
                CHECK(info.get_class("child")->get_field("field")->get(instance).to_int() == 21);
                CHECK(info.get_class("child")->get_field("cfield")->get(instance).to_int() == 22);
            }
        }
    }
}
