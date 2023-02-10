/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct ivec2 final {
        int x{};
        int y{};

        ivec2(int nx, int ny)
        : x{nx}, y{ny} {}

        ivec2(ivec2&& other) noexcept
        : x{other.x}
        , y{other.y} {
            other.x = 0;
            other.y = 0;
        }

        ivec2(const ivec2& other)
        : x{other.x}
        , y{other.y} {}
    };

    bool operator==(const ivec2& l, const ivec2& r) noexcept {
        return l.x == r.x && l.y == r.y;
    }
}

TEST_CASE("meta/meta_utilities/value5/safe_get_as") {
    namespace meta = meta_hpp;

    SUBCASE("&/val") {
        {
            meta::uvalue v{ivec2{1,2}};

            REQUIRE(v.safe_get_as<ivec2>());
            CHECK(v.safe_get_as<ivec2>() == ivec2{1,2});

            CHECK_FALSE(v.safe_get_as<int>());
        }
        {
            const meta::uvalue v{ivec2{1,2}};

            REQUIRE(v.safe_get_as<ivec2>());
            CHECK(v.safe_get_as<ivec2>() == ivec2{1,2});

            CHECK_FALSE(v.safe_get_as<int>());
        }
    }

    SUBCASE("&&/val") {
        {
            meta::uvalue v{ivec2{1,2}};
            const ivec2 v2 = *std::move(v).safe_get_as<ivec2>();
            CHECK(v2 == ivec2{1,2});
            CHECK(v.get_as<ivec2>() == ivec2{0,0});
        }
        {
            meta::uvalue v{ivec2{1,2}};
            CHECK_FALSE(std::move(v).safe_get_as<int>());
            CHECK(v.get_as<ivec2>() == ivec2{1,2});
        }
    }

    SUBCASE("&/ptr") {
        {
            ivec2 v{1,2};
            meta::uvalue v2{&v};

            REQUIRE(v2.safe_get_as<ivec2*>());
            REQUIRE(*v2.safe_get_as<ivec2*>());
            CHECK(*v2.safe_get_as<ivec2*>() == &v);

            REQUIRE(v2.safe_get_as<const ivec2*>());
            REQUIRE(*v2.safe_get_as<const ivec2*>());
            CHECK(*v2.safe_get_as<const ivec2*>() == &v);

            CHECK_FALSE(v2.safe_get_as<ivec2>());
            CHECK_FALSE(v2.safe_get_as<int*>());
            CHECK_FALSE(v2.safe_get_as<const int*>());
        }
        {
            ivec2 v{1,2};
            const meta::uvalue v2{&v};

            REQUIRE(v2.safe_get_as<ivec2*>());
            REQUIRE(*v2.safe_get_as<ivec2*>());
            CHECK(*v2.safe_get_as<ivec2*>() == &v);

            REQUIRE(v2.safe_get_as<const ivec2*>());
            REQUIRE(*v2.safe_get_as<const ivec2*>());
            CHECK(*v2.safe_get_as<const ivec2*>() == &v);

            CHECK_FALSE(v2.safe_get_as<ivec2>());
            CHECK_FALSE(v2.safe_get_as<int*>());
            CHECK_FALSE(v2.safe_get_as<const int*>());
        }
    }

    SUBCASE("&/const_ptr") {
        {
            const ivec2 v{1,2};
            meta::uvalue v2{&v};

            CHECK_FALSE(v2.safe_get_as<ivec2*>());

            REQUIRE(v2.safe_get_as<const ivec2*>());
            REQUIRE(*v2.safe_get_as<const ivec2*>());
            CHECK(*v2.safe_get_as<const ivec2*>() == &v);

            CHECK_FALSE(v2.safe_get_as<ivec2>());
            CHECK_FALSE(v2.safe_get_as<int*>());
            CHECK_FALSE(v2.safe_get_as<const int*>());
        }
        {
            const ivec2 v{1,2};
            const meta::uvalue v2{&v};

            CHECK_FALSE(v2.safe_get_as<ivec2*>());

            REQUIRE(v2.safe_get_as<const ivec2*>());
            REQUIRE(*v2.safe_get_as<const ivec2*>());
            CHECK(*v2.safe_get_as<const ivec2*>() == &v);

            CHECK_FALSE(v2.safe_get_as<ivec2>());
            CHECK_FALSE(v2.safe_get_as<int*>());
            CHECK_FALSE(v2.safe_get_as<const int*>());
        }
    }

    SUBCASE("&&/ptr") {
        {
            ivec2 v{1,2};
            meta::uvalue v2{&v};
            REQUIRE(std::move(v2).safe_get_as<ivec2*>());
            CHECK(*std::move(v2).safe_get_as<ivec2*>() == &v);
        }
        {
            ivec2 v{1,2};
            const meta::uvalue v2{&v};
            REQUIRE(std::move(v2).safe_get_as<ivec2*>());
            CHECK(*std::move(v2).safe_get_as<ivec2*>() == &v);
        }
    }

    SUBCASE("&&/const ptr") {
        {
            const ivec2 v{1,2};
            meta::uvalue v2{&v};
            CHECK_FALSE(std::move(v2).safe_get_as<ivec2*>());
            REQUIRE(std::move(v2).safe_get_as<const ivec2*>());
            CHECK(*std::move(v2).safe_get_as<const ivec2*>() == &v);
        }
        {
            const ivec2 v{1,2};
            const meta::uvalue v2{&v};
            CHECK_FALSE(std::move(v2).safe_get_as<ivec2*>());
            REQUIRE(std::move(v2).safe_get_as<const ivec2*>());
            CHECK(*std::move(v2).safe_get_as<const ivec2*>() == &v);
        }
    }
}
