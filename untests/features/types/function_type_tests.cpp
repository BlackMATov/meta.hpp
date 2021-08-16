/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "_types_fwd.hpp"

namespace
{
    using namespace meta_hpp;
    using namespace std::string_literals;
}

namespace
{
    struct ivec2 {
        int x{};
        int y{};
    };

    [[maybe_unused]] void arg_copy(ivec2) {}
    [[maybe_unused]] void arg_ref_noexcept(ivec2&) noexcept {}
    [[maybe_unused]] void arg_cref_noexcept(const ivec2&) noexcept {}
}

TEST_CASE("features/types/function") {
    SUBCASE("arg_copy") {
        using type = decltype(&arg_copy);

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<function_type>());

        const function_type ft = type_db::get<type>().as<function_type>();
        CHECK(ft.return_type() == type_db::get<void>());
        CHECK(ft.argument_types().size() == 1);
        CHECK(ft.arity() == 1);

        CHECK(ft.flags() == (function_flags{}));
        CHECK_FALSE(ft.is_noexcept());

        {
            REQUIRE(ft.argument_type(0));
            const any_type arg0 = ft.argument_type(0);
            CHECK(arg0 == type_db::get<ivec2>());
        }

        {
            CHECK_FALSE(ft.argument_type(1));
        }
    }

    SUBCASE("arg_ref_noexcept") {
        using type = decltype(&arg_ref_noexcept);

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<function_type>());

        const function_type ft = type_db::get<type>().as<function_type>();
        CHECK(ft.return_type() == type_db::get<void>());
        CHECK(ft.argument_types().size() == 1);
        CHECK(ft.arity() == 1);

        CHECK(ft.flags() == (function_flags::is_noexcept));
        CHECK(ft.is_noexcept());

        {
            REQUIRE(ft.argument_type(0));
            const any_type arg0 = ft.argument_type(0);
            CHECK(arg0 == type_db::get<ivec2&>());
        }

        {
            CHECK_FALSE(ft.argument_type(1));
        }
    }

    SUBCASE("arg_cref_noexcept") {
        using type = decltype(&arg_cref_noexcept);

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<function_type>());

        const function_type ft = type_db::get<type>().as<function_type>();
        CHECK(ft.return_type() == type_db::get<void>());
        CHECK(ft.argument_types().size() == 1);
        CHECK(ft.arity() == 1);

        CHECK(ft.flags() == (function_flags::is_noexcept));
        CHECK(ft.is_noexcept());

        {
            REQUIRE(ft.argument_type(0));
            const any_type arg0 = ft.argument_type(0);
            CHECK(arg0 == type_db::get<const ivec2&>());
        }

        {
            CHECK_FALSE(ft.argument_type(1));
        }
    }
}
