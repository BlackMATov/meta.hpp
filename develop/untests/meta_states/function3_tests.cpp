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
        int x{};
        int y{};

        ivec2(int nx, int ny): x{nx}, y{ny} {}

        ivec2(ivec2&& other) = default;
        ivec2& operator=(ivec2&& other) = default;

        ivec2(const ivec2& other) = default;
        ivec2& operator=(const ivec2& other) = default;

        static ivec2 iadd(ivec2 l, const ivec2& r) noexcept {
            l.x += r.x;
            l.y += r.y;
            return l;
        }

        static ivec2 iadd_inplace(ivec2& l, const ivec2& r) noexcept {
            l.x += r.x;
            l.y += r.y;
            return l;
        }

        static ivec2 iadd_rvalue(ivec2&& l, const ivec2& r) noexcept {
            l.x += r.x;
            l.y += r.y;
            return std::move(l);
        }
    };

    [[maybe_unused]]
    bool operator==(const ivec2& l, const ivec2& r) noexcept {
        return l.x == r.x && l.y == r.y;
    }
}

TEST_CASE("meta/meta_states/function3/_") {
    namespace meta = meta_hpp;

    meta::class_<ivec2>()
        .function_("iadd", &ivec2::iadd)
        .function_("iadd_inplace", &ivec2::iadd_inplace)
        .function_("iadd_rvalue", &ivec2::iadd_rvalue);
}

TEST_CASE("meta/meta_states/function3") {
    namespace meta = meta_hpp;

    const meta::class_type ivec2_type = meta::resolve_type<ivec2>();
    REQUIRE(ivec2_type);

    std::vector<meta::uvalue> params;
    {
        params.push_back(ivec2{1,2});
        params.push_back(ivec2{3,4});
    }

    std::vector<meta::uvalue> wrong_params;
    {
        wrong_params.push_back(ivec2{1,2});
        wrong_params.push_back(42);
    }

    std::vector<ivec2> typed_params;
    {
        typed_params.push_back(ivec2{1,2});
        typed_params.push_back(ivec2{3,4});
    }

    SUBCASE("iadd") {
        const meta::function func = ivec2_type.get_function("iadd");
        REQUIRE(func);

        {
            CHECK(func.invoke_variadic(params.begin(), params.end()).as<ivec2>() == ivec2{4,6});
            CHECK(func.invoke_variadic(params.cbegin(), params.cend()).as<ivec2>() == ivec2{4,6});
            CHECK(func.invoke_variadic(typed_params.begin(), typed_params.end()).as<ivec2>() == ivec2{4,6});
            CHECK(func.invoke_variadic(typed_params.cbegin(), typed_params.cend()).as<ivec2>() == ivec2{4,6});
        }

        {
            CHECK((*func.try_invoke_variadic(params.begin(), params.end())).as<ivec2>() == ivec2{4,6});
            CHECK((*func.try_invoke_variadic(params.cbegin(), params.cend())).as<ivec2>() == ivec2{4,6});
            CHECK((*func.try_invoke_variadic(typed_params.begin(), typed_params.end())).as<ivec2>() == ivec2{4,6});
            CHECK((*func.try_invoke_variadic(typed_params.cbegin(), typed_params.cend())).as<ivec2>() == ivec2{4,6});
            CHECK_FALSE(func.try_invoke_variadic(wrong_params.begin(), wrong_params.end()));
            CHECK_FALSE(func.try_invoke_variadic(wrong_params.cbegin(), wrong_params.cend()));
        }

        {
            CHECK(func.is_variadic_invocable_with(params.begin(), params.end()));
            CHECK(func.is_variadic_invocable_with(params.cbegin(), params.cend()));
            CHECK(func.is_variadic_invocable_with(typed_params.begin(), typed_params.end()));
            CHECK(func.is_variadic_invocable_with(typed_params.cbegin(), typed_params.cend()));
            CHECK_FALSE(func.is_variadic_invocable_with(wrong_params.begin(), wrong_params.end()));
            CHECK_FALSE(func.is_variadic_invocable_with(wrong_params.cbegin(), wrong_params.cend()));
        }

        {
            CHECK(func.is_variadic_invocable_with(params.begin(), params.end()));
            CHECK_FALSE(func.is_variadic_invocable_with(wrong_params.begin(), wrong_params.end()));
        }
    }

    SUBCASE("iadd_inplace") {
        const meta::function func = ivec2_type.get_function("iadd_inplace");
        REQUIRE(func);

        {
            CHECK(func.invoke_variadic(params.begin(), params.end()).as<ivec2>() == ivec2{4,6});
            CHECK(params[0].as<ivec2>() == ivec2{4,6});

            CHECK(func.invoke_variadic(typed_params.begin(), typed_params.end()).as<ivec2>() == ivec2{4,6});
            CHECK(typed_params[0] == ivec2{4,6});
        }

        {
            CHECK((*func.try_invoke_variadic(params.begin(), params.end())).as<ivec2>() == ivec2{7,10});
            CHECK(params[0].as<ivec2>() == ivec2{7,10});

            CHECK((*func.try_invoke_variadic(typed_params.begin(), typed_params.end())).as<ivec2>() == ivec2{7,10});
            CHECK(typed_params[0] == ivec2{7,10});

            CHECK_FALSE(func.try_invoke_variadic(params.cbegin(), params.cend()));
            CHECK_FALSE(func.try_invoke_variadic(wrong_params.cbegin(), wrong_params.cend()));
            CHECK_FALSE(func.try_invoke_variadic(typed_params.cbegin(), typed_params.cend()));
        }

        {
            CHECK(func.is_variadic_invocable_with(params.begin(), params.end()));
            CHECK_FALSE(func.is_variadic_invocable_with(params.cbegin(), params.cend()));

            CHECK(func.is_variadic_invocable_with(typed_params.begin(), typed_params.end()));
            CHECK_FALSE(func.is_variadic_invocable_with(typed_params.cbegin(), typed_params.cend()));

            CHECK_FALSE(func.is_variadic_invocable_with(wrong_params.begin(), wrong_params.end()));
            CHECK_FALSE(func.is_variadic_invocable_with(wrong_params.cbegin(), wrong_params.cend()));
        }

        {
            CHECK_FALSE(func.check_variadic_invocable_error(params.begin(), params.end()));
            CHECK(func.check_variadic_invocable_error(params.cbegin(), params.cend()));

            CHECK_FALSE(func.check_variadic_invocable_error(typed_params.begin(), typed_params.end()));
            CHECK(func.check_variadic_invocable_error(typed_params.cbegin(), typed_params.cend()));

            CHECK(func.check_variadic_invocable_error(wrong_params.begin(), wrong_params.end()));
            CHECK(func.check_variadic_invocable_error(wrong_params.cbegin(), wrong_params.cend()));
        }
    }

    SUBCASE("iadd_rvalue") {
        const meta::function func = ivec2_type.get_function("iadd_rvalue");
        REQUIRE(func);

        {
            CHECK_FALSE(func.is_variadic_invocable_with(
                params.begin(),
                params.end()));

            CHECK(func.is_variadic_invocable_with(
                std::make_move_iterator(params.begin()),
                std::make_move_iterator(params.end())));
        }

        {
            CHECK_FALSE(func.try_invoke_variadic(
                params.begin(),
                params.end()));

            CHECK((*func.try_invoke_variadic(
                std::make_move_iterator(params.begin()),
                std::make_move_iterator(params.end()))).as<ivec2>() == ivec2{4,6});
            CHECK(params[0].as<ivec2>() == ivec2{4,6});
        }
    }
}
