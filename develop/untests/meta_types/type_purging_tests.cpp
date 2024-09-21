/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct ivec2 {
        inline static int i{42};

        int x{}, y{};

        int length2() const noexcept {
            return x * x + y * y;
        }

        static int ilength2(const ivec2& v) noexcept {
            return v.x * v.x + v.y * v.y;
        }
    };

    enum class color : unsigned {
        red = 0xFF0000,
        green = 0x00FF00,
        blue = 0x0000FF,
        white = red | green | blue,
    };
}

TEST_CASE("meta/meta_types/type_purging") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    meta::class_<ivec2>(meta::metadata_()("hello", "world"s))
        .constructor_<>(meta::metadata_()("hello", "world"s))
        .destructor_(meta::metadata_()("hello", "world"s))
        .member_("x", &ivec2::x, meta::metadata_()("hello", "world"s))
        .member_("y", &ivec2::y, meta::metadata_()("hello", "world"s))
        .method_("length2", &ivec2::length2, meta::metadata_()("hello", "world"s))
        .variable_("i", &ivec2::i, meta::metadata_()("hello", "world"s))
        .function_("ilength2", &ivec2::ilength2, meta::metadata_()("hello", "world"s));

    meta::enum_<color>(meta::metadata_()("hello", "world"s))
        .evalue_("white", color::white, meta::metadata_()("hello", "world"s));

    SUBCASE("metadata purging / types") {
        meta::purge_metadata_(meta::any_type{});
        meta::purge_metadata_(meta::array_type{});
        meta::purge_metadata_(meta::class_type{});
        meta::purge_metadata_(meta::constructor_type{});
        meta::purge_metadata_(meta::destructor_type{});
        meta::purge_metadata_(meta::enum_type{});
        meta::purge_metadata_(meta::function_type{});
        meta::purge_metadata_(meta::member_type{});
        meta::purge_metadata_(meta::method_type{});
        meta::purge_metadata_(meta::nullptr_type{});
        meta::purge_metadata_(meta::number_type{});
        meta::purge_metadata_(meta::pointer_type{});
        meta::purge_metadata_(meta::reference_type{});
        meta::purge_metadata_(meta::void_type{});

        meta::bind_<int[10]>(meta::metadata_()("hello", "world"s));
        CHECK(meta::resolve_type<int[10]>().get_metadata().contains("hello"));
        meta::purge_metadata_(meta::resolve_type<int[10]>());
        CHECK_FALSE(meta::resolve_type<int[10]>().get_metadata().contains("hello"));

        meta::bind_<ivec2>(meta::metadata_()("hello", "world"s));
        CHECK(meta::resolve_type<ivec2>().get_metadata().contains("hello"));
        meta::purge_metadata_(meta::resolve_type<ivec2>());
        CHECK_FALSE(meta::resolve_type<ivec2>().get_metadata().contains("hello"));

        {
            meta::constructor ctor = meta::resolve_type<ivec2>().get_constructor_with<>();
            CHECK(ctor.get_metadata().contains("hello"));
            meta::purge_metadata_(ctor);
            CHECK_FALSE(ctor.get_metadata().contains("hello"));
        }

        {
            meta::destructor dtor = meta::resolve_type<ivec2>().get_destructor();
            CHECK(dtor.get_metadata().contains("hello"));
            meta::purge_metadata_(dtor);
            CHECK_FALSE(dtor.get_metadata().contains("hello"));
        }

        meta::bind_<color>(meta::metadata_()("hello", "world"s));
        CHECK(meta::resolve_type<color>().get_metadata().contains("hello"));
        meta::purge_metadata_(meta::resolve_type<color>());
        CHECK_FALSE(meta::resolve_type<color>().get_metadata().contains("hello"));

        meta::bind_<int(int)>(meta::metadata_()("hello", "world"s));
        CHECK(meta::resolve_type<int(int)>().get_metadata().contains("hello"));
        meta::purge_metadata_(meta::resolve_type<int(int)>());
        CHECK_FALSE(meta::resolve_type<int(int)>().get_metadata().contains("hello"));

        meta::bind_<int ivec2::*>(meta::metadata_()("hello", "world"s));
        CHECK(meta::resolve_type<int ivec2::*>().get_metadata().contains("hello"));
        meta::purge_metadata_(meta::resolve_type<int ivec2::*>());
        CHECK_FALSE(meta::resolve_type<int ivec2::*>().get_metadata().contains("hello"));

        meta::bind_<int (ivec2::*)(int)>(meta::metadata_()("hello", "world"s));
        CHECK(meta::resolve_type<int (ivec2::*)(int)>().get_metadata().contains("hello"));
        meta::purge_metadata_(meta::resolve_type<int (ivec2::*)(int)>());
        CHECK_FALSE(meta::resolve_type<int (ivec2::*)(int)>().get_metadata().contains("hello"));

        meta::bind_<std::nullptr_t>(meta::metadata_()("hello", "world"s));
        CHECK(meta::resolve_type<std::nullptr_t>().get_metadata().contains("hello"));
        meta::purge_metadata_(meta::resolve_type<std::nullptr_t>());
        CHECK_FALSE(meta::resolve_type<std::nullptr_t>().get_metadata().contains("hello"));

        meta::bind_<int*>(meta::metadata_()("hello", "world"s));
        CHECK(meta::resolve_type<int*>().get_metadata().contains("hello"));
        meta::purge_metadata_(meta::resolve_type<int*>());
        CHECK_FALSE(meta::resolve_type<int*>().get_metadata().contains("hello"));

        meta::bind_<int&>(meta::metadata_()("hello", "world"s));
        CHECK(meta::resolve_type<int&>().get_metadata().contains("hello"));
        meta::purge_metadata_(meta::resolve_type<int&>());
        CHECK_FALSE(meta::resolve_type<int&>().get_metadata().contains("hello"));

        meta::bind_<void>(meta::metadata_()("hello", "world"s));
        CHECK(meta::resolve_type<void>().get_metadata().contains("hello"));
        meta::purge_metadata_(meta::resolve_type<void>());
        CHECK_FALSE(meta::resolve_type<void>().get_metadata().contains("hello"));
    }

    SUBCASE("metadata purging / states") {
        meta::class_type ivec2_class = meta::resolve_type<ivec2>();
        REQUIRE(ivec2_class);

        meta::enum_type color_enum = meta::resolve_type<color>();
        REQUIRE(color_enum);

        {
            CHECK(ivec2_class.get_constructor_with<>().get_metadata().contains("hello"));
            meta::purge_metadata_(ivec2_class.get_constructor_with<>());
            CHECK_FALSE(ivec2_class.get_constructor_with<>().get_metadata().contains("hello"));
        }

        {
            CHECK(ivec2_class.get_destructor().get_metadata().contains("hello"));
            meta::purge_metadata_(ivec2_class.get_destructor());
            CHECK_FALSE(ivec2_class.get_destructor().get_metadata().contains("hello"));
        }

        {
            CHECK(color_enum.name_to_evalue("white").get_metadata().contains("hello"));
            meta::purge_metadata_(color_enum.name_to_evalue("white"));
            CHECK_FALSE(color_enum.name_to_evalue("white").get_metadata().contains("hello"));
        }

        {
            CHECK(ivec2_class.get_function("ilength2").get_metadata().contains("hello"));
            meta::purge_metadata_(ivec2_class.get_function("ilength2"));
            CHECK_FALSE(ivec2_class.get_function("ilength2").get_metadata().contains("hello"));
        }

        {
            CHECK(ivec2_class.get_member("x").get_metadata().contains("hello"));
            meta::purge_metadata_(ivec2_class.get_member("x"));
            CHECK_FALSE(ivec2_class.get_member("x").get_metadata().contains("hello"));
        }

        {
            CHECK(ivec2_class.get_method("length2").get_metadata().contains("hello"));
            meta::purge_metadata_(ivec2_class.get_method("length2"));
            CHECK_FALSE(ivec2_class.get_method("length2").get_metadata().contains("hello"));
        }

        {
            meta::scope scope = meta::local_scope_("", meta::metadata_()("hello", "world"s));
            CHECK(scope.get_metadata().contains("hello"));
            meta::purge_metadata_(scope);
            CHECK_FALSE(scope.get_metadata().contains("hello"));
        }

        {
            CHECK(ivec2_class.get_variable("i").get_metadata().contains("hello"));
            meta::purge_metadata_(ivec2_class.get_variable("i"));
            CHECK_FALSE(ivec2_class.get_variable("i").get_metadata().contains("hello"));
        }
    }

    SUBCASE("binds purging") {
        meta::class_type ivec2_class = meta::resolve_type<ivec2>();
        REQUIRE(ivec2_class);

        meta::enum_type color_enum = meta::resolve_type<color>();
        REQUIRE(color_enum);

        {
            CHECK(ivec2_class.get_constructors().size() > 0);
            CHECK(ivec2_class.get_destructor());
            CHECK(ivec2_class.get_functions().size() > 0);
            CHECK(ivec2_class.get_members().size() > 0);
            CHECK(ivec2_class.get_methods().size() > 0);
            CHECK(ivec2_class.get_variables().size() > 0);

            meta::purge_binds_(ivec2_class);

            CHECK(ivec2_class.get_constructors().size() == 0);
            CHECK_FALSE(ivec2_class.get_destructor());
            CHECK(ivec2_class.get_functions().size() == 0);
            CHECK(ivec2_class.get_members().size() == 0);
            CHECK(ivec2_class.get_methods().size() == 0);
            CHECK(ivec2_class.get_variables().size() == 0);
        }

        {
            CHECK(color_enum.get_evalues().size() > 0);
            meta::purge_binds_(color_enum);
            CHECK(color_enum.get_evalues().size() == 0);
        }
    }

    SUBCASE("binds and metadata purging") {
        meta::class_type ivec2_class = meta::resolve_type<ivec2>();
        REQUIRE(ivec2_class);

        meta::enum_type color_enum = meta::resolve_type<color>();
        REQUIRE(color_enum);

        {
            CHECK(ivec2_class.get_metadata().contains("hello"));
            CHECK(ivec2_class.get_constructors().size() > 0);

            meta::purge_(ivec2_class);

            CHECK_FALSE(ivec2_class.get_metadata().contains("hello"));
            CHECK(ivec2_class.get_constructors().size() == 0);
        }

        {
            CHECK(color_enum.get_metadata().contains("hello"));
            CHECK(color_enum.get_evalues().size() > 0);

            meta::purge_(color_enum);

            CHECK_FALSE(color_enum.get_metadata().contains("hello"));
            CHECK(color_enum.get_evalues().size() == 0);
        }
    }
}
