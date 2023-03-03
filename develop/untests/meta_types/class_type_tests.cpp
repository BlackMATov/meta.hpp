/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct base_clazz_1 {
        base_clazz_1(int) {}

        int base_member_1 = 1;
        int base_method_1() { return 1; }
        static int base_function_1() { return 1; }

        int base_method_1_overloaded(int i0) const { return i0; }
        int base_method_1_overloaded(float f0) const { return static_cast<int>(f0); }

        static int base_function_1_overloaded(int i0) { return i0; }
        static int base_function_1_overloaded(int i0, int i1) { return i0 + i1; }

        static int base_variable_1;
    };

    int base_clazz_1::base_variable_1 = 1;

    struct base_clazz_2 {
        base_clazz_2(float) {}

        float base_member_2 = 2.0f;
        float base_method_2() { return 2.0f; }
        static float base_function_2() { return 2.f; }

        static float base_variable_2;
    };

    float base_clazz_2::base_variable_2 = 2.0f;

    struct derived_clazz : base_clazz_1, base_clazz_2 {
        derived_clazz(int i, float f)
        : base_clazz_1{i}
        , base_clazz_2{f} {}

        double derived_member = 3.0;
        double derived_method() { return 3.0; }
        static double derived_function() { return 3.0; }

        static constexpr double derived_variable = 3.0;
    };

    struct final_derived_clazz final : derived_clazz {
        using derived_clazz::derived_clazz;
    };

    template < typename... Args >
    struct variadic_clazz {};
}

TEST_CASE("meta/meta_types/class_type/_") {
    namespace meta = meta_hpp;

    meta::class_<base_clazz_1>()
        .constructor_<int>()
        .base_<>()
        .member_("base_member_1", &base_clazz_1::base_member_1)
        .method_("base_method_1", &base_clazz_1::base_method_1)
        .function_("base_function_1", &base_clazz_1::base_function_1)
        .method_("base_method_1_overloaded", meta::select_overload<int(int) const>(&base_clazz_1::base_method_1_overloaded))
        .method_("base_method_1_overloaded", meta::select_overload<int(float) const>(&base_clazz_1::base_method_1_overloaded))
        .function_("base_function_1_overloaded", meta::select_overload<int(int)>(&base_clazz_1::base_function_1_overloaded))
        .function_("base_function_1_overloaded", meta::select_overload<int(int,int)>(&base_clazz_1::base_function_1_overloaded))
        .variable_("base_variable_1", &base_clazz_1::base_variable_1);

    meta::class_<base_clazz_2>()
        .constructor_<float>()
        .member_("base_member_2", &base_clazz_2::base_member_2)
        .method_("base_method_2", &base_clazz_2::base_method_2)
        .function_("base_function_2", &base_clazz_2::base_function_2)
        .variable_("base_variable_2", &base_clazz_2::base_variable_2);

    meta::class_<derived_clazz>()
        .constructor_<int, float>()
        .base_<base_clazz_1, base_clazz_2>()
        .member_("derived_member", &derived_clazz::derived_member)
        .method_("derived_method", &derived_clazz::derived_method)
        .function_("derived_function", &derived_clazz::derived_function)
        .variable_("derived_variable", &derived_clazz::derived_variable);

    meta::class_<final_derived_clazz>()
        .constructor_<int, float>()
        .base_<derived_clazz>();
}

TEST_CASE("meta/meta_types/class_type") {
    namespace meta = meta_hpp;

    const meta::class_type base_clazz_1_type = meta::resolve_type<base_clazz_1>();
    REQUIRE(base_clazz_1_type);

    const meta::class_type base_clazz_2_type = meta::resolve_type<base_clazz_2>();
    REQUIRE(base_clazz_2_type);

    const meta::class_type derived_clazz_type = meta::resolve_type<const derived_clazz>();
    REQUIRE(derived_clazz_type);

    const meta::class_type final_derived_clazz_type = meta::resolve_type<const final_derived_clazz>();
    REQUIRE(final_derived_clazz_type);

    const meta::class_type variadic_clazz_int_type = meta::resolve_type<variadic_clazz<int>>();
    REQUIRE(variadic_clazz_int_type);

    const meta::class_type variadic_clazz_int_float_type = meta::resolve_type<variadic_clazz<int, float>>();
    REQUIRE(variadic_clazz_int_float_type);

    SUBCASE("get_flags") {
        CHECK(base_clazz_1_type.get_flags() == meta::class_flags{});
        CHECK(base_clazz_2_type.get_flags() == meta::class_flags{});
        CHECK(derived_clazz_type.get_flags() == meta::class_flags{});
        CHECK(final_derived_clazz_type.get_flags() == meta::class_flags::is_final);
        CHECK(variadic_clazz_int_type.get_flags() == (meta::class_flags::is_empty | meta::class_flags::is_template_instantiation));
        CHECK(variadic_clazz_int_float_type.get_flags() == (meta::class_flags::is_empty | meta::class_flags::is_template_instantiation));
    }

    SUBCASE("get_size") {
        CHECK(base_clazz_1_type.get_size() == sizeof(base_clazz_1));
        CHECK(base_clazz_2_type.get_size() == sizeof(base_clazz_2));
        CHECK(derived_clazz_type.get_size() == sizeof(derived_clazz));
        CHECK(final_derived_clazz_type.get_size() == sizeof(final_derived_clazz));
    }

    SUBCASE("get_align") {
        CHECK(base_clazz_1_type.get_align() == alignof(base_clazz_1));
        CHECK(base_clazz_2_type.get_align() == alignof(base_clazz_2));
        CHECK(derived_clazz_type.get_align() == alignof(derived_clazz));
        CHECK(final_derived_clazz_type.get_align() == alignof(final_derived_clazz));
    }

    SUBCASE("get_arity") {
        {
            const meta::class_type type = meta::resolve_type<derived_clazz>();
            REQUIRE(type);
            CHECK(type.get_arity() == 0);
        }
        {
            const meta::class_type type = meta::resolve_type<variadic_clazz<int>>();
            REQUIRE(type);
            CHECK(type.get_arity() == 1);
        }
        {
            const meta::class_type type = meta::resolve_type<variadic_clazz<int, float>>();
            REQUIRE(type);
            CHECK(type.get_arity() == 2);
        }
    }

    SUBCASE("get_argument_type") {
        {
            const meta::class_type type = meta::resolve_type<derived_clazz>();
            REQUIRE(type);
            CHECK_FALSE(type.get_argument_type(0));
        }
        {
            const meta::class_type type = meta::resolve_type<variadic_clazz<int>>();
            REQUIRE(type);
            CHECK(type.get_argument_type(0) == meta::resolve_type<int>());
            CHECK_FALSE(type.get_argument_type(1));
        }
        {
            const meta::class_type type = meta::resolve_type<variadic_clazz<int, float>>();
            REQUIRE(type);
            CHECK(type.get_argument_type(0) == meta::resolve_type<int>());
            CHECK(type.get_argument_type(1) == meta::resolve_type<float>());
            CHECK_FALSE(type.get_argument_type(2));
        }
    }

    SUBCASE("get_argument_types") {
        {
            const meta::class_type type = meta::resolve_type<derived_clazz>();
            REQUIRE(type);
            CHECK(type.get_argument_types() == meta::any_type_list{});
        }
        {
            const meta::class_type type = meta::resolve_type<variadic_clazz<int>>();
            REQUIRE(type);
            CHECK(type.get_argument_types() == meta::any_type_list{meta::resolve_type<int>()});
        }
        {
            const meta::class_type type = meta::resolve_type<variadic_clazz<int, float>>();
            REQUIRE(type);
            CHECK(type.get_argument_types() == meta::any_type_list{meta::resolve_type<int>(), meta::resolve_type<float>()});
        }
    }

    SUBCASE("get_constructors") {
        CHECK(base_clazz_1_type.get_constructors().size() == 1);
        CHECK(base_clazz_2_type.get_constructors().size() == 1);
        CHECK(derived_clazz_type.get_constructors().size() == 1);
        CHECK(final_derived_clazz_type.get_constructors().size() == 1);
    }

    SUBCASE("get_base_classes") {
        CHECK(base_clazz_1_type.get_base_classes() == meta::class_set{});
        CHECK(base_clazz_2_type.get_base_classes() == meta::class_set{});
        CHECK(derived_clazz_type.get_base_classes() == meta::class_set{base_clazz_1_type, base_clazz_2_type});
        CHECK(final_derived_clazz_type.get_base_classes() == meta::class_set{derived_clazz_type});
    }

    SUBCASE("get_derived_classes") {
        CHECK(base_clazz_1_type.get_derived_classes() == meta::class_set{derived_clazz_type});
        CHECK(base_clazz_2_type.get_derived_classes() == meta::class_set{derived_clazz_type});
        CHECK(derived_clazz_type.get_derived_classes() == meta::class_set{final_derived_clazz_type});
        CHECK(final_derived_clazz_type.get_derived_classes() == meta::class_set{});
    }

    SUBCASE("get_functions") {
        CHECK(base_clazz_1_type.get_functions().size() == 3);
        CHECK(base_clazz_2_type.get_functions().size() == 1);
        CHECK(derived_clazz_type.get_functions().size() == 1);
        CHECK(final_derived_clazz_type.get_functions().size() == 0);
    }

    SUBCASE("get_members") {
        CHECK(base_clazz_1_type.get_members().size() == 1);
        CHECK(base_clazz_2_type.get_members().size() == 1);
        CHECK(derived_clazz_type.get_members().size() == 1);
        CHECK(final_derived_clazz_type.get_members().size() == 0);
    }

    SUBCASE("get_methods") {
        CHECK(base_clazz_1_type.get_methods().size() == 3);
        CHECK(base_clazz_2_type.get_methods().size() == 1);
        CHECK(derived_clazz_type.get_methods().size() == 1);
        CHECK(final_derived_clazz_type.get_methods().size() == 0);
    }

    SUBCASE("get_variables") {
        CHECK(base_clazz_1_type.get_variables().size() == 1);
        CHECK(base_clazz_2_type.get_variables().size() == 1);
        CHECK(derived_clazz_type.get_variables().size() == 1);
        CHECK(final_derived_clazz_type.get_variables().size() == 0);
    }

    SUBCASE("is_base_of") {
        {
            CHECK_FALSE(base_clazz_1_type.is_base_of<base_clazz_1>());
            CHECK_FALSE(base_clazz_1_type.is_base_of(base_clazz_1_type));
            CHECK_FALSE(base_clazz_1_type.is_direct_base_of<base_clazz_1>());
            CHECK_FALSE(base_clazz_1_type.is_direct_base_of(base_clazz_1_type));

            CHECK_FALSE(base_clazz_1_type.is_base_of<base_clazz_2>());
            CHECK_FALSE(base_clazz_1_type.is_base_of(base_clazz_2_type));
            CHECK_FALSE(base_clazz_1_type.is_direct_base_of<base_clazz_2>());
            CHECK_FALSE(base_clazz_1_type.is_direct_base_of(base_clazz_2_type));

            CHECK(base_clazz_1_type.is_base_of<derived_clazz>());
            CHECK(base_clazz_1_type.is_base_of(derived_clazz_type));
            CHECK(base_clazz_1_type.is_direct_base_of<derived_clazz>());
            CHECK(base_clazz_1_type.is_direct_base_of(derived_clazz_type));

            CHECK(base_clazz_1_type.is_base_of<final_derived_clazz>());
            CHECK(base_clazz_1_type.is_base_of(final_derived_clazz_type));
            CHECK_FALSE(base_clazz_1_type.is_direct_base_of<final_derived_clazz>());
            CHECK_FALSE(base_clazz_1_type.is_direct_base_of(final_derived_clazz_type));
        }

        {
            CHECK_FALSE(base_clazz_2_type.is_base_of<base_clazz_1>());
            CHECK_FALSE(base_clazz_2_type.is_base_of(base_clazz_1_type));
            CHECK_FALSE(base_clazz_2_type.is_direct_base_of<base_clazz_1>());
            CHECK_FALSE(base_clazz_2_type.is_direct_base_of(base_clazz_1_type));

            CHECK_FALSE(base_clazz_2_type.is_base_of<base_clazz_2>());
            CHECK_FALSE(base_clazz_2_type.is_base_of(base_clazz_2_type));
            CHECK_FALSE(base_clazz_2_type.is_direct_base_of<base_clazz_2>());
            CHECK_FALSE(base_clazz_2_type.is_direct_base_of(base_clazz_2_type));

            CHECK(base_clazz_2_type.is_base_of<derived_clazz>());
            CHECK(base_clazz_2_type.is_base_of(derived_clazz_type));
            CHECK(base_clazz_2_type.is_direct_base_of<derived_clazz>());
            CHECK(base_clazz_2_type.is_direct_base_of(derived_clazz_type));

            CHECK(base_clazz_2_type.is_base_of<final_derived_clazz>());
            CHECK(base_clazz_2_type.is_base_of(final_derived_clazz_type));
            CHECK_FALSE(base_clazz_2_type.is_direct_base_of<final_derived_clazz>());
            CHECK_FALSE(base_clazz_2_type.is_direct_base_of(final_derived_clazz_type));
        }

        {
            CHECK_FALSE(derived_clazz_type.is_base_of<base_clazz_1>());
            CHECK_FALSE(derived_clazz_type.is_base_of(base_clazz_1_type));
            CHECK_FALSE(derived_clazz_type.is_direct_base_of<base_clazz_1>());
            CHECK_FALSE(derived_clazz_type.is_direct_base_of(base_clazz_1_type));

            CHECK_FALSE(derived_clazz_type.is_base_of<base_clazz_2>());
            CHECK_FALSE(derived_clazz_type.is_base_of(base_clazz_2_type));
            CHECK_FALSE(derived_clazz_type.is_direct_base_of<base_clazz_2>());
            CHECK_FALSE(derived_clazz_type.is_direct_base_of(base_clazz_2_type));

            CHECK_FALSE(derived_clazz_type.is_base_of<derived_clazz>());
            CHECK_FALSE(derived_clazz_type.is_base_of(derived_clazz_type));
            CHECK_FALSE(derived_clazz_type.is_direct_base_of<derived_clazz>());
            CHECK_FALSE(derived_clazz_type.is_direct_base_of(derived_clazz_type));
        }
    }

    SUBCASE("is_derived_from") {
        {
            CHECK_FALSE(base_clazz_1_type.is_derived_from<base_clazz_1>());
            CHECK_FALSE(base_clazz_1_type.is_derived_from(base_clazz_1_type));
            CHECK_FALSE(base_clazz_1_type.is_direct_derived_from<base_clazz_1>());
            CHECK_FALSE(base_clazz_1_type.is_direct_derived_from(base_clazz_1_type));

            CHECK_FALSE(base_clazz_1_type.is_derived_from<base_clazz_2>());
            CHECK_FALSE(base_clazz_1_type.is_derived_from(base_clazz_2_type));
            CHECK_FALSE(base_clazz_1_type.is_direct_derived_from<base_clazz_2>());
            CHECK_FALSE(base_clazz_1_type.is_direct_derived_from(base_clazz_2_type));

            CHECK_FALSE(base_clazz_1_type.is_derived_from<derived_clazz>());
            CHECK_FALSE(base_clazz_1_type.is_derived_from(derived_clazz_type));
            CHECK_FALSE(base_clazz_1_type.is_direct_derived_from<derived_clazz>());
            CHECK_FALSE(base_clazz_1_type.is_direct_derived_from(derived_clazz_type));
        }

        {
            CHECK_FALSE(base_clazz_2_type.is_derived_from<base_clazz_1>());
            CHECK_FALSE(base_clazz_2_type.is_derived_from(base_clazz_1_type));
            CHECK_FALSE(base_clazz_2_type.is_direct_derived_from<base_clazz_1>());
            CHECK_FALSE(base_clazz_2_type.is_direct_derived_from(base_clazz_1_type));

            CHECK_FALSE(base_clazz_2_type.is_derived_from<base_clazz_2>());
            CHECK_FALSE(base_clazz_2_type.is_derived_from(base_clazz_2_type));
            CHECK_FALSE(base_clazz_2_type.is_direct_derived_from<base_clazz_2>());
            CHECK_FALSE(base_clazz_2_type.is_direct_derived_from(base_clazz_2_type));

            CHECK_FALSE(base_clazz_2_type.is_derived_from<derived_clazz>());
            CHECK_FALSE(base_clazz_2_type.is_derived_from(derived_clazz_type));
            CHECK_FALSE(base_clazz_2_type.is_direct_derived_from<derived_clazz>());
            CHECK_FALSE(base_clazz_2_type.is_direct_derived_from(derived_clazz_type));
        }

        {
            CHECK(derived_clazz_type.is_derived_from<base_clazz_1>());
            CHECK(derived_clazz_type.is_derived_from(base_clazz_1_type));
            CHECK(derived_clazz_type.is_direct_derived_from<base_clazz_1>());
            CHECK(derived_clazz_type.is_direct_derived_from(base_clazz_1_type));

            CHECK(derived_clazz_type.is_derived_from<base_clazz_2>());
            CHECK(derived_clazz_type.is_derived_from(base_clazz_2_type));
            CHECK(derived_clazz_type.is_direct_derived_from<base_clazz_2>());
            CHECK(derived_clazz_type.is_direct_derived_from(base_clazz_2_type));

            CHECK_FALSE(derived_clazz_type.is_derived_from<derived_clazz>());
            CHECK_FALSE(derived_clazz_type.is_derived_from(derived_clazz_type));
            CHECK_FALSE(derived_clazz_type.is_direct_derived_from<derived_clazz>());
            CHECK_FALSE(derived_clazz_type.is_direct_derived_from(derived_clazz_type));
        }

        {
            CHECK(final_derived_clazz_type.is_derived_from<base_clazz_1>());
            CHECK(final_derived_clazz_type.is_derived_from(base_clazz_1_type));
            CHECK_FALSE(final_derived_clazz_type.is_direct_derived_from<base_clazz_1>());
            CHECK_FALSE(final_derived_clazz_type.is_direct_derived_from(base_clazz_1_type));

            CHECK(final_derived_clazz_type.is_derived_from<base_clazz_2>());
            CHECK(final_derived_clazz_type.is_derived_from(base_clazz_2_type));
            CHECK_FALSE(final_derived_clazz_type.is_direct_derived_from<base_clazz_2>());
            CHECK_FALSE(final_derived_clazz_type.is_direct_derived_from(base_clazz_2_type));

            CHECK(final_derived_clazz_type.is_derived_from<derived_clazz>());
            CHECK(final_derived_clazz_type.is_derived_from(derived_clazz_type));
            CHECK(final_derived_clazz_type.is_direct_derived_from<derived_clazz>());
            CHECK(final_derived_clazz_type.is_direct_derived_from(derived_clazz_type));
        }
    }

    SUBCASE("get_function") {
        CHECK(base_clazz_1_type.get_function("base_function_1"));
        CHECK_FALSE(base_clazz_1_type.get_function("base_function_2"));
        CHECK_FALSE(base_clazz_1_type.get_function("derived_function"));

        CHECK_FALSE(base_clazz_2_type.get_function("base_function_1"));
        CHECK(base_clazz_2_type.get_function("base_function_2"));
        CHECK_FALSE(base_clazz_2_type.get_function("derived_function"));

        CHECK(derived_clazz_type.get_function("base_function_1"));
        CHECK(derived_clazz_type.get_function("base_function_2"));
        CHECK(derived_clazz_type.get_function("derived_function"));
    }

    SUBCASE("get_member") {
        CHECK(base_clazz_1_type.get_member("base_member_1"));
        CHECK_FALSE(base_clazz_1_type.get_member("base_member_2"));
        CHECK_FALSE(base_clazz_1_type.get_member("derived_member"));

        CHECK_FALSE(base_clazz_2_type.get_member("base_member_1"));
        CHECK(base_clazz_2_type.get_member("base_member_2"));
        CHECK_FALSE(base_clazz_2_type.get_member("derived_member"));

        CHECK(derived_clazz_type.get_member("base_member_1"));
        CHECK(derived_clazz_type.get_member("base_member_2"));
        CHECK(derived_clazz_type.get_member("derived_member"));
    }

    SUBCASE("get_method") {
        CHECK(base_clazz_1_type.get_method("base_method_1"));
        CHECK_FALSE(base_clazz_1_type.get_method("base_method_2"));
        CHECK_FALSE(base_clazz_1_type.get_method("derived_method"));

        CHECK_FALSE(base_clazz_2_type.get_method("base_method_1"));
        CHECK(base_clazz_2_type.get_method("base_method_2"));
        CHECK_FALSE(base_clazz_2_type.get_method("derived_method"));

        CHECK(derived_clazz_type.get_method("base_method_1"));
        CHECK(derived_clazz_type.get_method("base_method_2"));
        CHECK(derived_clazz_type.get_method("derived_method"));
    }

    SUBCASE("get_variable") {
        CHECK(base_clazz_1_type.get_variable("base_variable_1"));
        CHECK_FALSE(base_clazz_1_type.get_variable("base_variable_2"));
        CHECK_FALSE(base_clazz_1_type.get_variable("derived_variable"));

        CHECK_FALSE(base_clazz_2_type.get_variable("base_variable_1"));
        CHECK(base_clazz_2_type.get_variable("base_variable_2"));
        CHECK_FALSE(base_clazz_2_type.get_variable("derived_variable"));

        CHECK(derived_clazz_type.get_variable("base_variable_1"));
        CHECK(derived_clazz_type.get_variable("base_variable_2"));
        CHECK(derived_clazz_type.get_variable("derived_variable"));
    }

    SUBCASE("get_constructor_with") {
        {
            CHECK_FALSE(base_clazz_1_type.get_constructor_with<>());
            CHECK(base_clazz_1_type.get_constructor_with<int>());
            CHECK_FALSE(base_clazz_1_type.get_constructor_with<float>());
        }
        {
            CHECK_FALSE(base_clazz_2_type.get_constructor_with<>());
            CHECK_FALSE(base_clazz_2_type.get_constructor_with<int>());
            CHECK(base_clazz_2_type.get_constructor_with<float>());
        }
        {
            CHECK_FALSE(derived_clazz_type.get_constructor_with<>());
            CHECK_FALSE(derived_clazz_type.get_constructor_with<int>());
            CHECK_FALSE(derived_clazz_type.get_constructor_with<float>());
            CHECK(derived_clazz_type.get_constructor_with<int, float>());
            CHECK_FALSE(derived_clazz_type.get_constructor_with<float, int>());
        }
    }

    SUBCASE("get_function_with") {
        CHECK(base_clazz_1_type.get_function("base_function_1_overloaded"));

        {
            CHECK_FALSE(base_clazz_1_type.get_function_with<>("base_function_1_overloaded"));
            CHECK(base_clazz_1_type.get_function_with<int>("base_function_1_overloaded"));
            CHECK_FALSE(base_clazz_1_type.get_function_with<int, float>("base_function_1_overloaded"));

            CHECK_FALSE(base_clazz_1_type.get_function_with<>("base_function_1_overloaded"));
            CHECK_FALSE(base_clazz_1_type.get_function_with<float>("base_function_1_overloaded"));
            CHECK(base_clazz_1_type.get_function_with<int, int>("base_function_1_overloaded"));
            CHECK_FALSE(base_clazz_1_type.get_function_with<float, float>("base_function_1_overloaded"));

            CHECK_FALSE(base_clazz_1_type.get_function_with<>("base_function_1_overloaded"));
            CHECK_FALSE(base_clazz_1_type.get_function_with<double>("base_function_1_overloaded"));
            CHECK_FALSE(base_clazz_1_type.get_function_with<double, double>("base_function_1_overloaded"));
        }

        {
            meta::number_type int_type = meta::resolve_type<int>();
            meta::number_type float_type = meta::resolve_type<float>();
            meta::number_type double_type = meta::resolve_type<double>();

            CHECK_FALSE(base_clazz_1_type.get_function_with<>("base_function_1_overloaded"));
            CHECK(base_clazz_1_type.get_function_with("base_function_1_overloaded", {int_type}));
            CHECK_FALSE(base_clazz_1_type.get_function_with("base_function_1_overloaded", {int_type, float_type}));

            CHECK_FALSE(base_clazz_1_type.get_function_with("base_function_1_overloaded"));
            CHECK_FALSE(base_clazz_1_type.get_function_with("base_function_1_overloaded", {float_type}));
            CHECK(base_clazz_1_type.get_function_with("base_function_1_overloaded", {int_type, int_type}));
            CHECK_FALSE(base_clazz_1_type.get_function_with("base_function_1_overloaded", {float_type, float_type}));

            CHECK_FALSE(base_clazz_1_type.get_function_with<>("base_function_1_overloaded"));
            CHECK_FALSE(base_clazz_1_type.get_function_with("base_function_1_overloaded", {double_type}));
            CHECK_FALSE(base_clazz_1_type.get_function_with("base_function_1_overloaded", {double_type, double_type}));
        }

        {
            meta::number_type int_type = meta::resolve_type<int>();
            meta::number_type float_type = meta::resolve_type<float>();
            meta::number_type double_type = meta::resolve_type<double>();

            CHECK_FALSE(base_clazz_1_type.get_function_with<>("base_function_1_overloaded"));
            CHECK(base_clazz_1_type.get_function_with("base_function_1_overloaded", meta::any_type_list{int_type}));
            CHECK_FALSE(base_clazz_1_type.get_function_with("base_function_1_overloaded", meta::any_type_list{int_type, float_type}));

            CHECK_FALSE(base_clazz_1_type.get_function_with("base_function_1_overloaded"));
            CHECK_FALSE(base_clazz_1_type.get_function_with("base_function_1_overloaded", meta::any_type_list{float_type}));
            CHECK(base_clazz_1_type.get_function_with("base_function_1_overloaded", meta::any_type_list{int_type, int_type}));
            CHECK_FALSE(base_clazz_1_type.get_function_with("base_function_1_overloaded", meta::any_type_list{float_type, float_type}));

            CHECK_FALSE(base_clazz_1_type.get_function_with<>("base_function_1_overloaded"));
            CHECK_FALSE(base_clazz_1_type.get_function_with("base_function_1_overloaded", meta::any_type_list{double_type}));
            CHECK_FALSE(base_clazz_1_type.get_function_with("base_function_1_overloaded", meta::any_type_list{double_type, double_type}));
        }
    }

    SUBCASE("get_method_with") {
        CHECK(base_clazz_1_type.get_method("base_method_1_overloaded"));

        {
            CHECK_FALSE(base_clazz_1_type.get_method_with<>("base_method_1_overloaded"));
            CHECK(base_clazz_1_type.get_method_with<int>("base_method_1_overloaded"));
            CHECK_FALSE(base_clazz_1_type.get_method_with<int, int>("base_method_1_overloaded"));

            CHECK_FALSE(base_clazz_1_type.get_method_with<>("base_method_1_overloaded"));
            CHECK(base_clazz_1_type.get_method_with<float>("base_method_1_overloaded"));
            CHECK_FALSE(base_clazz_1_type.get_method_with<float, float>("base_method_1_overloaded"));

            CHECK_FALSE(base_clazz_1_type.get_method_with<>("base_method_1_overloaded"));
            CHECK_FALSE(base_clazz_1_type.get_method_with<double>("base_method_1_overloaded"));
            CHECK_FALSE(base_clazz_1_type.get_method_with<double, double>("base_method_1_overloaded"));
        }

        {
            meta::number_type int_type = meta::resolve_type<int>();
            meta::number_type float_type = meta::resolve_type<float>();
            meta::number_type double_type = meta::resolve_type<double>();

            CHECK_FALSE(base_clazz_1_type.get_method_with<>("base_method_1_overloaded"));
            CHECK(base_clazz_1_type.get_method_with("base_method_1_overloaded", {int_type}));
            CHECK_FALSE(base_clazz_1_type.get_method_with("base_method_1_overloaded", {int_type, int_type}));

            CHECK_FALSE(base_clazz_1_type.get_method_with("base_method_1_overloaded"));
            CHECK(base_clazz_1_type.get_method_with("base_method_1_overloaded", {float_type}));
            CHECK_FALSE(base_clazz_1_type.get_method_with("base_method_1_overloaded", {float_type, float_type}));

            CHECK_FALSE(base_clazz_1_type.get_method_with<>("base_method_1_overloaded"));
            CHECK_FALSE(base_clazz_1_type.get_method_with("base_method_1_overloaded", {double_type}));
            CHECK_FALSE(base_clazz_1_type.get_method_with("base_method_1_overloaded", {double_type, double_type}));
        }

        {
            meta::number_type int_type = meta::resolve_type<int>();
            meta::number_type float_type = meta::resolve_type<float>();
            meta::number_type double_type = meta::resolve_type<double>();

            CHECK_FALSE(base_clazz_1_type.get_method_with<>("base_method_1_overloaded"));
            CHECK(base_clazz_1_type.get_method_with("base_method_1_overloaded", meta::any_type_list{int_type}));
            CHECK_FALSE(base_clazz_1_type.get_method_with("base_method_1_overloaded", meta::any_type_list{int_type, int_type}));

            CHECK_FALSE(base_clazz_1_type.get_method_with("base_method_1_overloaded"));
            CHECK(base_clazz_1_type.get_method_with("base_method_1_overloaded", meta::any_type_list{float_type}));
            CHECK_FALSE(base_clazz_1_type.get_method_with("base_method_1_overloaded", meta::any_type_list{float_type, float_type}));

            CHECK_FALSE(base_clazz_1_type.get_method_with<>("base_method_1_overloaded"));
            CHECK_FALSE(base_clazz_1_type.get_method_with("base_method_1_overloaded", meta::any_type_list{double_type}));
            CHECK_FALSE(base_clazz_1_type.get_method_with("base_method_1_overloaded", meta::any_type_list{double_type, double_type}));
        }
    }
}
