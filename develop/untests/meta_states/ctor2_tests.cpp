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
    };

    [[maybe_unused]]
    bool operator==(const ivec2& l, const ivec2& r) noexcept {
        return l.x == r.x && l.y == r.y;
    }
}

TEST_CASE("meta/meta_states/ctor2/_") {
    namespace meta = meta_hpp;

    meta::class_<ivec2>()
        .constructor_<int, int>();
}

TEST_CASE("meta/meta_states/ctor2") {
    namespace meta = meta_hpp;

    const meta::class_type ivec2_type = meta::resolve_type<ivec2>();
    REQUIRE(ivec2_type);

    alignas(ivec2) std::byte ivec2_mem[sizeof(ivec2)];

    std::vector<meta::uvalue> params;
    {
        params.push_back(1);
        params.push_back(2);
    }

    std::vector<meta::uvalue> wrong_params;
    {
        wrong_params.push_back(1);
    }

    std::vector<int> typed_params;
    {
        typed_params.push_back(1);
        typed_params.push_back(2);
    }

    const meta::constructor ctor = ivec2_type.get_constructor_with<int, int>();
    REQUIRE(ctor);

    {
        CHECK(ctor.create_variadic(params.begin(), params.end()).as<ivec2>() == ivec2{1,2});
        CHECK(ctor.create_variadic(params.cbegin(), params.cend()).as<ivec2>() == ivec2{1,2});
        CHECK(ctor.create_variadic(typed_params.begin(), typed_params.end()).as<ivec2>() == ivec2{1,2});
        CHECK(ctor.create_variadic(typed_params.cbegin(), typed_params.cend()).as<ivec2>() == ivec2{1,2});

        CHECK((*ctor.create_variadic_at(ivec2_mem, params.begin(), params.end()).as<ivec2*>()) == ivec2{1,2});
        CHECK(ivec2_type.destroy_at(ivec2_mem));
        CHECK((*ctor.create_variadic_at(ivec2_mem, typed_params.begin(), typed_params.end()).as<ivec2*>()) == ivec2{1,2});
        CHECK(ivec2_type.destroy_at(ivec2_mem));
    }

    {
        CHECK((*ctor.try_create_variadic(params.begin(), params.end())).as<ivec2>() == ivec2{1,2});
        CHECK((*ctor.try_create_variadic(params.cbegin(), params.cend())).as<ivec2>() == ivec2{1,2});
        CHECK((*ctor.try_create_variadic(typed_params.begin(), typed_params.end())).as<ivec2>() == ivec2{1,2});
        CHECK((*ctor.try_create_variadic(typed_params.cbegin(), typed_params.cend())).as<ivec2>() == ivec2{1,2});
        CHECK_FALSE(ctor.try_create_variadic(wrong_params.begin(), wrong_params.end()));
        CHECK_FALSE(ctor.try_create_variadic(wrong_params.cbegin(), wrong_params.cend()));

        CHECK((*(*ctor.try_create_variadic_at(ivec2_mem, params.begin(), params.end())).as<ivec2*>()) == ivec2{1,2});
        CHECK(ivec2_type.destroy_at(ivec2_mem));
        CHECK((*(*ctor.try_create_variadic_at(ivec2_mem, typed_params.begin(), typed_params.end())).as<ivec2*>()) == ivec2{1,2});
        CHECK(ivec2_type.destroy_at(ivec2_mem));
        CHECK_FALSE(ctor.try_create_variadic_at(ivec2_mem, wrong_params.begin(), wrong_params.end()));
        CHECK_FALSE(ctor.try_create_variadic_at(ivec2_mem, wrong_params.cbegin(), wrong_params.cend()));
    }

    {
        CHECK(ctor.is_variadic_invocable_with(params.begin(), params.end()));
        CHECK_FALSE(ctor.is_variadic_invocable_with(wrong_params.begin(), wrong_params.end()));
    }
}
