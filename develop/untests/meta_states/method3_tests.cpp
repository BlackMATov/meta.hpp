/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2025, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct ivec2 {
        int x{};
        int y{};

        ivec2(int nx, int ny): x{nx}, y{ny} {}

        ivec2(ivec2&& other) = default;
        ivec2& operator=(ivec2&& other) = default;

        ivec2(const ivec2& other) = default;
        ivec2& operator=(const ivec2& other) = default;

        ivec2& add(const ivec2& other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        ivec2 add_reverse(ivec2& other) const {
            other.x += x;
            other.y += y;
            return other;
        }

        ivec2 add_rvalue_reverse(ivec2&& other) const {
            other.x += x;
            other.y += y;
            return std::move(other);
        }
    };

    [[maybe_unused]]
    bool operator==(const ivec2& l, const ivec2& r) noexcept {
        return l.x == r.x && l.y == r.y;
    }
}

TEST_CASE("meta/meta_states/method3/_") {
    namespace meta = meta_hpp;

    meta::class_<ivec2>()
        .method_("add", &ivec2::add)
        .method_("add_reverse", &ivec2::add_reverse)
        .method_("add_rvalue_reverse", &ivec2::add_rvalue_reverse);
}

TEST_CASE("meta/meta_states/method3") {
    namespace meta = meta_hpp;

    const meta::class_type ivec2_type = meta::resolve_type<ivec2>();
    REQUIRE(ivec2_type);

    std::vector<meta::uvalue> params;
    {
        params.push_back(ivec2{3,4});
    }

    std::vector<meta::uvalue> wrong_params;
    {
        wrong_params.push_back(42);
    }

    std::vector<ivec2> typed_params;
    {
        typed_params.push_back(ivec2{3,4});
    }

    SUBCASE("add") {
        const meta::method meth = ivec2_type.get_method("add");
        REQUIRE(meth);

        {
            CHECK(meth.invoke_variadic(ivec2{1,2}, params.begin(), params.end()).as<ivec2>() == ivec2{4,6});
            CHECK(meth.invoke_variadic(ivec2{1,2}, params.cbegin(), params.cend()).as<ivec2>() == ivec2{4,6});
            CHECK(meth.invoke_variadic(ivec2{1,2}, typed_params.begin(), typed_params.end()).as<ivec2>() == ivec2{4,6});
            CHECK(meth.invoke_variadic(ivec2{1,2}, typed_params.cbegin(), typed_params.cend()).as<ivec2>() == ivec2{4,6});
        }

        {
            CHECK((*meth.try_invoke_variadic(ivec2{1,2}, params.begin(), params.end())).as<ivec2>() == ivec2{4,6});
            CHECK((*meth.try_invoke_variadic(ivec2{1,2}, params.cbegin(), params.cend())).as<ivec2>() == ivec2{4,6});
            CHECK((*meth.try_invoke_variadic(ivec2{1,2}, typed_params.begin(), typed_params.end())).as<ivec2>() == ivec2{4,6});
            CHECK((*meth.try_invoke_variadic(ivec2{1,2}, typed_params.cbegin(), typed_params.cend())).as<ivec2>() == ivec2{4,6});
            CHECK_FALSE(meth.try_invoke_variadic(ivec2{1,2}, wrong_params.begin(), wrong_params.end()));
            CHECK_FALSE(meth.try_invoke_variadic(ivec2{1,2}, wrong_params.cbegin(), wrong_params.cend()));
        }

        {
            CHECK(meth.is_variadic_invocable_with(ivec2{1,2}, params.begin(), params.end()));
            CHECK_FALSE(meth.is_variadic_invocable_with(ivec2{1,2}, wrong_params.begin(), wrong_params.end()));
        }
    }

    SUBCASE("add_reverse") {
        const meta::method meth = ivec2_type.get_method("add_reverse");
        REQUIRE(meth);

        {
            CHECK(meth.invoke_variadic(ivec2{1,2}, params.begin(), params.end()).as<ivec2>() == ivec2{4,6});
            CHECK(params[0].as<ivec2>() == ivec2{4,6});

            CHECK(meth.invoke_variadic(ivec2{1,2}, typed_params.begin(), typed_params.end()).as<ivec2>() == ivec2{4,6});
            CHECK(typed_params[0] == ivec2{4,6});
        }

        {
            CHECK((*meth.try_invoke_variadic(ivec2{1,2}, params.begin(), params.end())).as<ivec2>() == ivec2{5,8});
            CHECK_FALSE(meth.try_invoke_variadic(ivec2{1,2}, params.cbegin(), params.cend()));
            CHECK((*meth.try_invoke_variadic(ivec2{1,2}, typed_params.begin(), typed_params.end())).as<ivec2>() == ivec2{5,8});
            CHECK_FALSE(meth.try_invoke_variadic(ivec2{1,2}, typed_params.cbegin(), typed_params.cend()));
            CHECK_FALSE(meth.try_invoke_variadic(ivec2{1,2}, wrong_params.begin(), wrong_params.end()));
            CHECK_FALSE(meth.try_invoke_variadic(ivec2{1,2}, wrong_params.cbegin(), wrong_params.cend()));
        }

        {
            CHECK(meth.is_variadic_invocable_with(ivec2{1,2}, params.begin(), params.end()));
            CHECK_FALSE(meth.is_variadic_invocable_with(ivec2{1,2}, params.cbegin(), params.cend()));

            CHECK(meth.is_variadic_invocable_with(ivec2{1,2}, typed_params.begin(), typed_params.end()));
            CHECK_FALSE(meth.is_variadic_invocable_with(ivec2{1,2}, typed_params.cbegin(), typed_params.cend()));

            CHECK_FALSE(meth.is_variadic_invocable_with(ivec2{1,2}, wrong_params.begin(), wrong_params.end()));
            CHECK_FALSE(meth.is_variadic_invocable_with(ivec2{1,2}, wrong_params.cbegin(), wrong_params.cend()));
        }
    }

    SUBCASE("add_rvalue_reverse") {
        const meta::method meth = ivec2_type.get_method("add_rvalue_reverse");
        REQUIRE(meth);

        {
            CHECK_FALSE(meth.is_variadic_invocable_with(
                ivec2{1,2},
                params.begin(),
                params.end()));

            CHECK(meth.is_variadic_invocable_with(
                ivec2{1,2},
                std::make_move_iterator(params.begin()),
                std::make_move_iterator(params.end())));
        }

        {
            CHECK_FALSE(meth.try_invoke_variadic(
                ivec2{1,2},
                params.begin(),
                params.end()));

            CHECK((*meth.try_invoke_variadic(
                ivec2{1,2},
                std::make_move_iterator(params.begin()),
                std::make_move_iterator(params.end()))).as<ivec2>() == ivec2{4,6});
            CHECK(params[0].as<ivec2>() == ivec2{4,6});
        }
    }
}
