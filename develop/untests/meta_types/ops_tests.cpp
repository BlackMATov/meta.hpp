/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct ivec2 {
        int x{};
        int y{};

        ivec2& add(const ivec2& other) {
            x += other.x;
            y += other.y;
            return *this;
        }
    };
}

TEST_CASE("meta/meta_types/ops/_") {
    namespace meta = meta_hpp;

    meta::class_<ivec2>()
        .member_("x", &ivec2::x)
        .member_("y", &ivec2::y)
        .method_("add", &ivec2::add);
}

TEST_CASE("meta/meta_types/ops") {
    namespace meta = meta_hpp;

    const meta::class_type ivec2_type = meta::resolve_type<ivec2>();
    REQUIRE(ivec2_type);

    const meta::member ivec2_x = ivec2_type.get_member("x");
    REQUIRE(ivec2_x);

    const meta::member ivec2_y = ivec2_type.get_member("y");
    REQUIRE(ivec2_y);

    const meta::method ivec2_add = ivec2_type.get_method("add");
    REQUIRE(ivec2_add);

    SUBCASE("hash") {
        const meta::member_type ivec2_x_type = ivec2_x.get_type();
        const meta::method_type ivec2_add_type = ivec2_add.get_type();

        CHECK(std::hash<meta::member_type>{}(ivec2_x_type) == std::hash<meta::member_type>{}(ivec2_x_type));
        CHECK_FALSE(std::hash<meta::member_type>{}(ivec2_x_type) == std::hash<meta::member_type>{}({}));
        CHECK_FALSE(std::hash<meta::member_type>{}(ivec2_x_type) == std::hash<meta::method_type>{}(ivec2_add_type));
    }

    SUBCASE("operator<") {
        {
            CHECK(ivec2_x.get_type() == ivec2_y.get_type());
            CHECK_FALSE(ivec2_x.get_type() == ivec2_add.get_type());
            CHECK_FALSE(ivec2_x.get_type() != ivec2_y.get_type());
            CHECK(ivec2_x.get_type() != ivec2_add.get_type());

            CHECK_FALSE(ivec2_x.get_type() == meta_hpp::member_type{});
            CHECK_FALSE(meta_hpp::member_type{} == ivec2_x.get_type());
            CHECK(ivec2_x.get_type() != meta_hpp::member_type{});
            CHECK(meta_hpp::member_type{} != ivec2_x.get_type());

            CHECK(ivec2_x.get_type() < ivec2_add.get_type());
            CHECK_FALSE(ivec2_add.get_type() < ivec2_x.get_type());

            CHECK_FALSE(ivec2_x.get_type() < meta_hpp::member_type{});
            CHECK(meta_hpp::member_type{} < ivec2_x.get_type());
        }

        {
            CHECK(ivec2_x.get_type() == ivec2_x.get_type().get_id());
            CHECK_FALSE(meta_hpp::member_type{} == ivec2_x.get_type().get_id());
            CHECK(ivec2_x.get_type().get_id() == ivec2_x.get_type());
            CHECK_FALSE(ivec2_x.get_type().get_id() == meta_hpp::member_type{});

            CHECK_FALSE(ivec2_x.get_type() == ivec2_add.get_type().get_id());
            CHECK_FALSE(ivec2_add.get_type().get_id() == ivec2_x.get_type());

            CHECK(meta_hpp::member_type{} < ivec2_x.get_type());
            CHECK_FALSE(ivec2_x.get_type() < meta_hpp::member_type{});

            CHECK_FALSE(meta_hpp::member_type{} > ivec2_x.get_type());
            CHECK(ivec2_x.get_type() > meta_hpp::member_type{});
        }

        // NOLINTBEGIN(*-redundant-expression)
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::any_type>() < std::declval<meta::any_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::array_type>() < std::declval<meta::array_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::class_type>() < std::declval<meta::class_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::constructor_type>() < std::declval<meta::constructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::destructor_type>() < std::declval<meta::destructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::enum_type>() < std::declval<meta::enum_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::function_type>() < std::declval<meta::function_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::member_type>() < std::declval<meta::member_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() < std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() < std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() < std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() < std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() < std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::void_type>() < std::declval<meta::void_type>())>);

        static_assert(std::is_same_v<bool, decltype(std::declval<meta::any_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::array_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::class_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::constructor_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::destructor_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::enum_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::function_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::member_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::void_type>() < std::declval<meta::type_id>())>);

        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::any_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::array_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::class_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::constructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::destructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::enum_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::function_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::member_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::void_type>())>);
        // NOLINTEND(*-redundant-expression)
    }

    SUBCASE("operator==") {
        // NOLINTBEGIN(*-redundant-expression)
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::any_type>() == std::declval<meta::any_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::array_type>() == std::declval<meta::array_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::class_type>() == std::declval<meta::class_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::constructor_type>() == std::declval<meta::constructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::destructor_type>() == std::declval<meta::destructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::enum_type>() == std::declval<meta::enum_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::function_type>() == std::declval<meta::function_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::member_type>() == std::declval<meta::member_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() == std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() == std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() == std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() == std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() == std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() == std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() == std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() == std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() == std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() == std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::void_type>() == std::declval<meta::void_type>())>);

        static_assert(std::is_same_v<bool, decltype(std::declval<meta::any_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::array_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::class_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::constructor_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::destructor_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::enum_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::function_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::member_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::void_type>() == std::declval<meta::type_id>())>);

        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::any_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::array_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::class_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::constructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::destructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::enum_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::function_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::member_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::void_type>())>);
        // NOLINTEND(*-redundant-expression)
    }

    SUBCASE("operator!=") {
        // NOLINTBEGIN(*-redundant-expression)
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::any_type>() != std::declval<meta::any_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::array_type>() != std::declval<meta::array_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::class_type>() != std::declval<meta::class_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::constructor_type>() != std::declval<meta::constructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::destructor_type>() != std::declval<meta::destructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::enum_type>() != std::declval<meta::enum_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::function_type>() != std::declval<meta::function_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::member_type>() != std::declval<meta::member_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() != std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() != std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() != std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() != std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() != std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() != std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() != std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() != std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() != std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() != std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::void_type>() != std::declval<meta::void_type>())>);

        static_assert(std::is_same_v<bool, decltype(std::declval<meta::any_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::array_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::class_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::constructor_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::destructor_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::enum_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::function_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::member_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::void_type>() != std::declval<meta::type_id>())>);

        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::any_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::array_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::class_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::constructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::destructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::enum_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::function_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::member_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::void_type>())>);
        // NOLINTEND(*-redundant-expression)
    }
}
