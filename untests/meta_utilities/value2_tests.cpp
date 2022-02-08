/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
    struct ivec2 final {
        int x{};
        int y{};

        ivec2() = delete;
        explicit ivec2(int v): x{v}, y{v} {}
        ivec2(int x, int y): x{x}, y{y} {}

        ivec2(ivec2&& other) noexcept
        : x{other.x}
        , y{other.y} {
            other.x = 0;
            other.y = 0;
            ++move_ctor_counter;
        }

        ivec2(const ivec2& other) noexcept
        : x{other.x}
        , y{other.y} {
            ++copy_ctor_counter;
        }

        ~ivec2() noexcept {
            ++dtor_counter;
        }

        ivec2& operator=(ivec2&& other) = delete;
        ivec2& operator=(const ivec2& other) = delete;

        static int dtor_counter;
        static int move_ctor_counter;
        static int copy_ctor_counter;
    };

    struct ivec2_big final {
        int x{};
        int y{};

        int dummy[42]{};

        ivec2_big() = delete;
        explicit ivec2_big(int v): x{v}, y{v} {}
        ivec2_big(int x, int y): x{x}, y{y} {}

        ivec2_big(ivec2_big&& other) noexcept
        : x{other.x}
        , y{other.y} {
            other.x = 0;
            other.y = 0;
            ++move_ctor_counter;
        }

        ivec2_big(const ivec2_big& other) noexcept
        : x{other.x}
        , y{other.y} {
            ++copy_ctor_counter;
        }

        ~ivec2_big() noexcept {
            ++dtor_counter;
        }

        ivec2_big& operator=(ivec2_big&& other) = delete;
        ivec2_big& operator=(const ivec2_big& other) = delete;

        static int dtor_counter;
        static int move_ctor_counter;
        static int copy_ctor_counter;
    };

    [[maybe_unused]] bool operator==(const ivec2& l, const ivec2& r) noexcept {
        return l.x == r.x && l.y == r.y;
    }

    [[maybe_unused]] bool operator==(const ivec2_big& l, const ivec2_big& r) noexcept {
        return l.x == r.x && l.y == r.y;
    }

    int ivec2::dtor_counter{0};
    int ivec2::move_ctor_counter{0};
    int ivec2::copy_ctor_counter{0};

    int ivec2_big::dtor_counter{0};
    int ivec2_big::move_ctor_counter{0};
    int ivec2_big::copy_ctor_counter{0};
}

TEST_CASE("meta/meta_utilities/value2") {
    namespace meta = meta_hpp;

    meta::class_<ivec2>()
        .ctor_<int>()
        .ctor_<int, int>()
        .ctor_<ivec2&&>()
        .ctor_<const ivec2&>()
        .member_("x", &ivec2::x)
        .member_("y", &ivec2::y);

    meta::class_<ivec2_big>()
        .ctor_<int>()
        .ctor_<int, int>()
        .ctor_<ivec2_big&&>()
        .ctor_<const ivec2_big&>()
        .member_("x", &ivec2_big::x)
        .member_("y", &ivec2_big::y);
}

TEST_CASE("meta/meta_utilities/value2/counters/small") {
    namespace meta = meta_hpp;

    ivec2::dtor_counter = 0;
    ivec2::move_ctor_counter = 0;
    ivec2::copy_ctor_counter = 0;

    SUBCASE("def ctor") {
        {
            meta::uvalue v{};
            CHECK(ivec2::dtor_counter == 0);
            CHECK(ivec2::move_ctor_counter == 0);
            CHECK(ivec2::copy_ctor_counter == 0);
        }
        CHECK(ivec2::dtor_counter == 0);
        CHECK(ivec2::move_ctor_counter == 0);
        CHECK(ivec2::copy_ctor_counter == 0);
    }

    SUBCASE("val ctor") {
        {
            meta::uvalue v{ivec2{1,2}};
            CHECK(ivec2::dtor_counter == 1);
            CHECK(ivec2::move_ctor_counter == 1);
            CHECK(ivec2::copy_ctor_counter == 0);
        }
        CHECK(ivec2::dtor_counter == 2);
        CHECK(ivec2::move_ctor_counter == 1);
        CHECK(ivec2::copy_ctor_counter == 0);
    }

    SUBCASE("move ctor") {
        {
            meta::uvalue v1{ivec2{1,2}};
            meta::uvalue v2{std::move(v1)};

            CHECK_FALSE(v1);
            CHECK(v2.cast<ivec2>().x == 1);

            CHECK(ivec2::dtor_counter == 2);
            CHECK(ivec2::move_ctor_counter == 2);
            CHECK(ivec2::copy_ctor_counter == 0);
        }
        CHECK(ivec2::dtor_counter == 3);
        CHECK(ivec2::move_ctor_counter == 2);
        CHECK(ivec2::copy_ctor_counter == 0);
    }

    SUBCASE("copy ctor") {
        {
            meta::uvalue v1{ivec2{1,2}};
            meta::uvalue v2{std::as_const(v1)};

            CHECK(v1.cast<ivec2>().x == 1);
            CHECK(v2.cast<ivec2>().y == 2);

            CHECK(ivec2::dtor_counter == 1);
            CHECK(ivec2::move_ctor_counter == 1);
            CHECK(ivec2::copy_ctor_counter == 1);
        }
        CHECK(ivec2::dtor_counter == 3);
        CHECK(ivec2::move_ctor_counter == 1);
        CHECK(ivec2::copy_ctor_counter == 1);
    }

    SUBCASE("swap") {
        {
            meta::uvalue v1{ivec2{1,2}};
            meta::uvalue v2{ivec2{3,4}};
            CHECK(ivec2::dtor_counter == 2);
            CHECK(ivec2::move_ctor_counter == 2);
            CHECK(ivec2::copy_ctor_counter == 0);

            v1.swap(v2);
            CHECK(v1.cast<ivec2>().x == 3);
            CHECK(v2.cast<ivec2>().x == 1);

            CHECK(ivec2::dtor_counter == 5);
            CHECK(ivec2::move_ctor_counter == 5);
            CHECK(ivec2::copy_ctor_counter == 0);
        }
        CHECK(ivec2::dtor_counter == 7);
        CHECK(ivec2::move_ctor_counter == 5);
        CHECK(ivec2::copy_ctor_counter == 0);
    }
}

TEST_CASE("meta/meta_utilities/value2/counters/big") {
    namespace meta = meta_hpp;

    ivec2_big::dtor_counter = 0;
    ivec2_big::move_ctor_counter = 0;
    ivec2_big::copy_ctor_counter = 0;

    SUBCASE("def ctor") {
        {
            meta::uvalue v{};
            CHECK(ivec2_big::dtor_counter == 0);
            CHECK(ivec2_big::move_ctor_counter == 0);
            CHECK(ivec2_big::copy_ctor_counter == 0);
        }
        CHECK(ivec2_big::dtor_counter == 0);
        CHECK(ivec2_big::move_ctor_counter == 0);
        CHECK(ivec2_big::copy_ctor_counter == 0);
    }

    SUBCASE("val ctor") {
        {
            meta::uvalue v{ivec2_big{1,2}};
            CHECK(ivec2_big::dtor_counter == 1);
            CHECK(ivec2_big::move_ctor_counter == 1);
            CHECK(ivec2_big::copy_ctor_counter == 0);
        }
        CHECK(ivec2_big::dtor_counter == 2);
        CHECK(ivec2_big::move_ctor_counter == 1);
        CHECK(ivec2_big::copy_ctor_counter == 0);
    }

    SUBCASE("move ctor") {
        {
            meta::uvalue v1{ivec2_big{1,2}};
            meta::uvalue v2{std::move(v1)};

            CHECK_FALSE(v1);
            CHECK(v2.cast<ivec2_big>().x == 1);

            CHECK(ivec2_big::dtor_counter == 1);
            CHECK(ivec2_big::move_ctor_counter == 1);
            CHECK(ivec2_big::copy_ctor_counter == 0);
        }
        CHECK(ivec2_big::dtor_counter == 2);
        CHECK(ivec2_big::move_ctor_counter == 1);
        CHECK(ivec2_big::copy_ctor_counter == 0);
    }

    SUBCASE("copy ctor") {
        {
            meta::uvalue v1{ivec2_big{1,2}};
            meta::uvalue v2{std::as_const(v1)};

            CHECK(v1.cast<ivec2_big>().x == 1);
            CHECK(v2.cast<ivec2_big>().y == 2);

            CHECK(ivec2_big::dtor_counter == 1);
            CHECK(ivec2_big::move_ctor_counter == 1);
            CHECK(ivec2_big::copy_ctor_counter == 1);
        }
        CHECK(ivec2_big::dtor_counter == 3);
        CHECK(ivec2_big::move_ctor_counter == 1);
        CHECK(ivec2_big::copy_ctor_counter == 1);
    }

    SUBCASE("swap") {
        {
            meta::uvalue v1{ivec2_big{1,2}};
            meta::uvalue v2{ivec2_big{3,4}};
            CHECK(ivec2_big::dtor_counter == 2);
            CHECK(ivec2_big::move_ctor_counter == 2);
            CHECK(ivec2_big::copy_ctor_counter == 0);

            v1.swap(v2);
            CHECK(v1.cast<ivec2_big>().x == 3);
            CHECK(v2.cast<ivec2_big>().x == 1);

            CHECK(ivec2_big::dtor_counter == 2);
            CHECK(ivec2_big::move_ctor_counter == 2);
            CHECK(ivec2_big::copy_ctor_counter == 0);
        }
        CHECK(ivec2_big::dtor_counter == 4);
        CHECK(ivec2_big::move_ctor_counter == 2);
        CHECK(ivec2_big::copy_ctor_counter == 0);
    }
}

TEST_CASE("meta/meta_utilities/value2/counters/swap") {
    namespace meta = meta_hpp;

    ivec2::dtor_counter = 0;
    ivec2::move_ctor_counter = 0;
    ivec2::copy_ctor_counter = 0;

    ivec2_big::dtor_counter = 0;
    ivec2_big::move_ctor_counter = 0;
    ivec2_big::copy_ctor_counter = 0;

    SUBCASE("empty/small") {
        {
            meta::uvalue v1{};
            meta::uvalue v2{ivec2{1,2}};

            CHECK(ivec2::dtor_counter == 1);
            CHECK(ivec2::move_ctor_counter == 1);
            CHECK(ivec2::copy_ctor_counter == 0);

            v1.swap(v2);
            CHECK(v1.cast<ivec2>().x == 1);
            CHECK_FALSE(v2);

            CHECK(ivec2::dtor_counter == 2);
            CHECK(ivec2::move_ctor_counter == 2);
            CHECK(ivec2::copy_ctor_counter == 0);

            v1.swap(v2);
            CHECK_FALSE(v1);
            CHECK(v2.cast<ivec2>().y == 2);

            CHECK(ivec2::dtor_counter == 3);
            CHECK(ivec2::move_ctor_counter == 3);
            CHECK(ivec2::copy_ctor_counter == 0);
        }

        CHECK(ivec2::dtor_counter == 4);
        CHECK(ivec2::move_ctor_counter == 3);
        CHECK(ivec2::copy_ctor_counter == 0);
    }

    SUBCASE("empty/big") {
        {
            meta::uvalue v1{};
            meta::uvalue v2{ivec2_big{3,4}};

            CHECK(ivec2_big::dtor_counter == 1);
            CHECK(ivec2_big::move_ctor_counter == 1);
            CHECK(ivec2_big::copy_ctor_counter == 0);

            v1.swap(v2);
            CHECK(v1.cast<ivec2_big>().x == 3);
            CHECK_FALSE(v2);

            CHECK(ivec2_big::dtor_counter == 1);
            CHECK(ivec2_big::move_ctor_counter == 1);
            CHECK(ivec2_big::copy_ctor_counter == 0);

            v1.swap(v2);
            CHECK_FALSE(v1);
            CHECK(v2.cast<ivec2_big>().y == 4);

            CHECK(ivec2_big::dtor_counter == 1);
            CHECK(ivec2_big::move_ctor_counter == 1);
            CHECK(ivec2_big::copy_ctor_counter == 0);
        }

        CHECK(ivec2_big::dtor_counter == 2);
        CHECK(ivec2_big::move_ctor_counter == 1);
        CHECK(ivec2_big::copy_ctor_counter == 0);
    }

    SUBCASE("small/big") {
        {
            meta::uvalue v1{ivec2{1,2}};
            meta::uvalue v2{ivec2_big{3,4}};

            CHECK(ivec2::dtor_counter == 1);
            CHECK(ivec2::move_ctor_counter == 1);
            CHECK(ivec2::copy_ctor_counter == 0);

            CHECK(ivec2_big::dtor_counter == 1);
            CHECK(ivec2_big::move_ctor_counter == 1);
            CHECK(ivec2_big::copy_ctor_counter == 0);

            v1.swap(v2);
            CHECK(v1.cast<ivec2_big>().x == 3);
            CHECK(v2.cast<ivec2>().x == 1);

            CHECK(ivec2::dtor_counter == 2);
            CHECK(ivec2::move_ctor_counter == 2);
            CHECK(ivec2::copy_ctor_counter == 0);

            CHECK(ivec2_big::dtor_counter == 1);
            CHECK(ivec2_big::move_ctor_counter == 1);
            CHECK(ivec2_big::copy_ctor_counter == 0);

            v1.swap(v2);
            CHECK(v1.cast<ivec2>().y == 2);
            CHECK(v2.cast<ivec2_big>().y == 4);

            CHECK(ivec2::dtor_counter == 3);
            CHECK(ivec2::move_ctor_counter == 3);
            CHECK(ivec2::copy_ctor_counter == 0);

            CHECK(ivec2_big::dtor_counter == 1);
            CHECK(ivec2_big::move_ctor_counter == 1);
            CHECK(ivec2_big::copy_ctor_counter == 0);
        }

        CHECK(ivec2::dtor_counter == 4);
        CHECK(ivec2::move_ctor_counter == 3);
        CHECK(ivec2::copy_ctor_counter == 0);

        CHECK(ivec2_big::dtor_counter == 2);
        CHECK(ivec2_big::move_ctor_counter == 1);
        CHECK(ivec2_big::copy_ctor_counter == 0);
    }
}
