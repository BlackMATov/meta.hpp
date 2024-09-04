/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct ivec2 final {
        int x{};
        int y{};

        ivec2() = delete;

        explicit ivec2(int nv): x{nv}, y{nv} {}
        ivec2(int nx, int ny): x{nx}, y{ny} {}

        ivec2(ivec2&& other) noexcept
        : x{other.x}
        , y{other.y} {
            other.x = 0;
            other.y = 0;
            ++move_constructor_counter;
        }

        ivec2(const ivec2& other) noexcept
        : x{other.x}
        , y{other.y} {
            ++copy_constructor_counter;
        }

        ~ivec2() noexcept {
            ++destructor_counter;
        }

        ivec2& operator=(ivec2&& other) = delete;
        ivec2& operator=(const ivec2& other) = delete;

        static int destructor_counter;
        static int move_constructor_counter;
        static int copy_constructor_counter;
    };

    struct ivec2_big final {
        int x{};
        int y{};

        int dummy[42]{};

        ivec2_big() = delete;

        explicit ivec2_big(int nv): x{nv}, y{nv} {}
        ivec2_big(int nx, int ny): x{nx}, y{ny} {}

        ivec2_big(ivec2_big&& other) noexcept
        : x{other.x}
        , y{other.y} {
            other.x = 0;
            other.y = 0;
            ++move_constructor_counter;
        }

        ivec2_big(const ivec2_big& other) noexcept
        : x{other.x}
        , y{other.y} {
            ++copy_constructor_counter;
        }

        ~ivec2_big() noexcept {
            ++destructor_counter;
        }

        ivec2_big& operator=(ivec2_big&& other) = delete;
        ivec2_big& operator=(const ivec2_big& other) = delete;

        static int destructor_counter;
        static int move_constructor_counter;
        static int copy_constructor_counter;
    };

    int ivec2::destructor_counter{0};
    int ivec2::move_constructor_counter{0};
    int ivec2::copy_constructor_counter{0};

    int ivec2_big::destructor_counter{0};
    int ivec2_big::move_constructor_counter{0};
    int ivec2_big::copy_constructor_counter{0};
}

META_HPP_DECLARE_COPY_TRAITS_FOR(ivec2)
META_HPP_DECLARE_COPY_TRAITS_FOR(ivec2_big)

TEST_CASE("meta/meta_utilities/value2") {
    namespace meta = meta_hpp;

    meta::class_<ivec2>()
        .constructor_<int>()
        .constructor_<int, int>()
        .constructor_<ivec2&&>()
        .constructor_<const ivec2&>()
        .member_("x", &ivec2::x)
        .member_("y", &ivec2::y);

    meta::class_<ivec2_big>()
        .constructor_<int>()
        .constructor_<int, int>()
        .constructor_<ivec2_big&&>()
        .constructor_<const ivec2_big&>()
        .member_("x", &ivec2_big::x)
        .member_("y", &ivec2_big::y);
}

TEST_CASE("meta/meta_utilities/value2/counters/small") {
    namespace meta = meta_hpp;

    ivec2::destructor_counter = 0;
    ivec2::move_constructor_counter = 0;
    ivec2::copy_constructor_counter = 0;

    SUBCASE("def ctor") {
        {
            meta::uvalue v{};
            CHECK(ivec2::destructor_counter == 0);
            CHECK(ivec2::move_constructor_counter == 0);
            CHECK(ivec2::copy_constructor_counter == 0);
        }
        CHECK(ivec2::destructor_counter == 0);
        CHECK(ivec2::move_constructor_counter == 0);
        CHECK(ivec2::copy_constructor_counter == 0);
    }

    SUBCASE("val ctor") {
        {
            meta::uvalue v{ivec2{1,2}};
            CHECK(ivec2::destructor_counter == 1);
            CHECK(ivec2::move_constructor_counter == 1);
            CHECK(ivec2::copy_constructor_counter == 0);
        }
        CHECK(ivec2::destructor_counter == 2);
        CHECK(ivec2::move_constructor_counter == 1);
        CHECK(ivec2::copy_constructor_counter == 0);
    }

    SUBCASE("move ctor") {
        {
            meta::uvalue v1{ivec2{1,2}};
            meta::uvalue v2{std::move(v1)};

            CHECK_FALSE(v1);
            CHECK(v2.as<ivec2>().x == 1);

            CHECK(ivec2::destructor_counter == 2);
            CHECK(ivec2::move_constructor_counter == 2);
            CHECK(ivec2::copy_constructor_counter == 0);
        }
        CHECK(ivec2::destructor_counter == 3);
        CHECK(ivec2::move_constructor_counter == 2);
        CHECK(ivec2::copy_constructor_counter == 0);
    }

    SUBCASE("swap") {
        {
            meta::uvalue v1{ivec2{1,2}};
            meta::uvalue v2{ivec2{3,4}};
            CHECK(ivec2::destructor_counter == 2);
            CHECK(ivec2::move_constructor_counter == 2);
            CHECK(ivec2::copy_constructor_counter == 0);

            v1.swap(v2);
            CHECK(v1.as<ivec2>().x == 3);
            CHECK(v2.as<ivec2>().x == 1);

            CHECK((ivec2::destructor_counter == 5 || ivec2::destructor_counter == 6));
            CHECK((ivec2::move_constructor_counter == 5 || ivec2::move_constructor_counter == 6));
            CHECK(ivec2::copy_constructor_counter == 0);
        }
        CHECK((ivec2::destructor_counter == 7 || ivec2::destructor_counter == 8));
        CHECK((ivec2::move_constructor_counter == 5 || ivec2::move_constructor_counter == 6));
        CHECK(ivec2::copy_constructor_counter == 0);
    }
}

TEST_CASE("meta/meta_utilities/value2/counters/big") {
    namespace meta = meta_hpp;

    ivec2_big::destructor_counter = 0;
    ivec2_big::move_constructor_counter = 0;
    ivec2_big::copy_constructor_counter = 0;

    SUBCASE("def ctor") {
        {
            meta::uvalue v{};
            CHECK(ivec2_big::destructor_counter == 0);
            CHECK(ivec2_big::move_constructor_counter == 0);
            CHECK(ivec2_big::copy_constructor_counter == 0);
        }
        CHECK(ivec2_big::destructor_counter == 0);
        CHECK(ivec2_big::move_constructor_counter == 0);
        CHECK(ivec2_big::copy_constructor_counter == 0);
    }

    SUBCASE("val ctor") {
        {
            meta::uvalue v{ivec2_big{1,2}};
            CHECK(ivec2_big::destructor_counter == 1);
            CHECK(ivec2_big::move_constructor_counter == 1);
            CHECK(ivec2_big::copy_constructor_counter == 0);
        }
        CHECK(ivec2_big::destructor_counter == 2);
        CHECK(ivec2_big::move_constructor_counter == 1);
        CHECK(ivec2_big::copy_constructor_counter == 0);
    }

    SUBCASE("move ctor") {
        {
            meta::uvalue v1{ivec2_big{1,2}};
            meta::uvalue v2{std::move(v1)};

            CHECK_FALSE(v1);
            CHECK(v2.as<ivec2_big>().x == 1);

            CHECK(ivec2_big::destructor_counter == 1);
            CHECK(ivec2_big::move_constructor_counter == 1);
            CHECK(ivec2_big::copy_constructor_counter == 0);
        }
        CHECK(ivec2_big::destructor_counter == 2);
        CHECK(ivec2_big::move_constructor_counter == 1);
        CHECK(ivec2_big::copy_constructor_counter == 0);
    }

    SUBCASE("copy ctor") {
        {
            meta::uvalue v1{ivec2_big{1,2}};
            meta::uvalue v2{std::as_const(v1).copy()};

            CHECK(v1.as<ivec2_big>().x == 1);
            CHECK(v2.as<ivec2_big>().y == 2);

            CHECK(ivec2_big::destructor_counter == 1);
            CHECK(ivec2_big::move_constructor_counter == 1);
            CHECK(ivec2_big::copy_constructor_counter == 1);
        }
        CHECK(ivec2_big::destructor_counter == 3);
        CHECK(ivec2_big::move_constructor_counter == 1);
        CHECK(ivec2_big::copy_constructor_counter == 1);
    }

    SUBCASE("swap") {
        {
            meta::uvalue v1{ivec2_big{1,2}};
            meta::uvalue v2{ivec2_big{3,4}};
            CHECK(ivec2_big::destructor_counter == 2);
            CHECK(ivec2_big::move_constructor_counter == 2);
            CHECK(ivec2_big::copy_constructor_counter == 0);

            v1.swap(v2);
            CHECK(v1.as<ivec2_big>().x == 3);
            CHECK(v2.as<ivec2_big>().x == 1);

            CHECK(ivec2_big::destructor_counter == 2);
            CHECK(ivec2_big::move_constructor_counter == 2);
            CHECK(ivec2_big::copy_constructor_counter == 0);
        }
        CHECK(ivec2_big::destructor_counter == 4);
        CHECK(ivec2_big::move_constructor_counter == 2);
        CHECK(ivec2_big::copy_constructor_counter == 0);
    }
}

TEST_CASE("meta/meta_utilities/value2/counters/swap") {
    namespace meta = meta_hpp;

    ivec2::destructor_counter = 0;
    ivec2::move_constructor_counter = 0;
    ivec2::copy_constructor_counter = 0;

    ivec2_big::destructor_counter = 0;
    ivec2_big::move_constructor_counter = 0;
    ivec2_big::copy_constructor_counter = 0;

    SUBCASE("empty/small") {
        {
            meta::uvalue v1{};
            meta::uvalue v2{ivec2{1,2}};

            CHECK(ivec2::destructor_counter == 1);
            CHECK(ivec2::move_constructor_counter == 1);
            CHECK(ivec2::copy_constructor_counter == 0);

            v1.swap(v2);
            CHECK(v1.as<ivec2>().x == 1);
            CHECK_FALSE(v2);

            CHECK(ivec2::destructor_counter == 2);
            CHECK(ivec2::move_constructor_counter == 2);
            CHECK(ivec2::copy_constructor_counter == 0);

            v1.swap(v2);
            CHECK_FALSE(v1);
            CHECK(v2.as<ivec2>().y == 2);

            CHECK(ivec2::destructor_counter == 3);
            CHECK(ivec2::move_constructor_counter == 3);
            CHECK(ivec2::copy_constructor_counter == 0);
        }

        CHECK(ivec2::destructor_counter == 4);
        CHECK(ivec2::move_constructor_counter == 3);
        CHECK(ivec2::copy_constructor_counter == 0);
    }

    SUBCASE("empty/big") {
        {
            meta::uvalue v1{};
            meta::uvalue v2{ivec2_big{3,4}};

            CHECK(ivec2_big::destructor_counter == 1);
            CHECK(ivec2_big::move_constructor_counter == 1);
            CHECK(ivec2_big::copy_constructor_counter == 0);

            v1.swap(v2);
            CHECK(v1.as<ivec2_big>().x == 3);
            CHECK_FALSE(v2);

            CHECK(ivec2_big::destructor_counter == 1);
            CHECK(ivec2_big::move_constructor_counter == 1);
            CHECK(ivec2_big::copy_constructor_counter == 0);

            v1.swap(v2);
            CHECK_FALSE(v1);
            CHECK(v2.as<ivec2_big>().y == 4);

            CHECK(ivec2_big::destructor_counter == 1);
            CHECK(ivec2_big::move_constructor_counter == 1);
            CHECK(ivec2_big::copy_constructor_counter == 0);
        }

        CHECK(ivec2_big::destructor_counter == 2);
        CHECK(ivec2_big::move_constructor_counter == 1);
        CHECK(ivec2_big::copy_constructor_counter == 0);
    }

    SUBCASE("small/big") {
        {
            meta::uvalue v1{ivec2{1,2}};
            meta::uvalue v2{ivec2_big{3,4}};

            CHECK(ivec2::destructor_counter == 1);
            CHECK(ivec2::move_constructor_counter == 1);
            CHECK(ivec2::copy_constructor_counter == 0);

            CHECK(ivec2_big::destructor_counter == 1);
            CHECK(ivec2_big::move_constructor_counter == 1);
            CHECK(ivec2_big::copy_constructor_counter == 0);

            v1.swap(v2);
            CHECK(v1.as<ivec2_big>().x == 3);
            CHECK(v2.as<ivec2>().x == 1);

            CHECK(ivec2::destructor_counter == 2);
            CHECK(ivec2::move_constructor_counter == 2);
            CHECK(ivec2::copy_constructor_counter == 0);

            CHECK(ivec2_big::destructor_counter == 1);
            CHECK(ivec2_big::move_constructor_counter == 1);
            CHECK(ivec2_big::copy_constructor_counter == 0);

            v1.swap(v2);
            CHECK(v1.as<ivec2>().y == 2);
            CHECK(v2.as<ivec2_big>().y == 4);

            CHECK(ivec2::destructor_counter == 3);
            CHECK(ivec2::move_constructor_counter == 3);
            CHECK(ivec2::copy_constructor_counter == 0);

            CHECK(ivec2_big::destructor_counter == 1);
            CHECK(ivec2_big::move_constructor_counter == 1);
            CHECK(ivec2_big::copy_constructor_counter == 0);
        }

        CHECK(ivec2::destructor_counter == 4);
        CHECK(ivec2::move_constructor_counter == 3);
        CHECK(ivec2::copy_constructor_counter == 0);

        CHECK(ivec2_big::destructor_counter == 2);
        CHECK(ivec2_big::move_constructor_counter == 1);
        CHECK(ivec2_big::copy_constructor_counter == 0);
    }
}
