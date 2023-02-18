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

        explicit ivec2(int nv) : x{nv}, y{nv} {}
        ivec2(int nx, int ny) : x{nx}, y{ny} {}
    };

    bool operator==(const ivec2& l, const ivec2& r) noexcept {
        return l.x == r.x && l.y == r.y;
    }
}

TEST_CASE("meta/meta_utilities/uerror") {
    namespace meta = meta_hpp;

    SUBCASE("ctors") {
        {
            meta::uerror err{};
            CHECK(*err == meta::error_code::no_error);
            CHECK(err.get_error() == meta::error_code::no_error);
        }
        {
            meta::uerror err{meta::error_code::bad_argument_cast};
            CHECK(*err == meta::error_code::bad_argument_cast);
            CHECK(err.get_error() == meta::error_code::bad_argument_cast);
        }
        {
            meta::uerror err{meta::error_code::bad_instance_cast};
            CHECK(*err == meta::error_code::bad_instance_cast);
            CHECK(err.get_error() == meta::error_code::bad_instance_cast);
        }
        {
            meta::uerror err1{meta::error_code::bad_argument_cast};
            meta::uerror err2{err1};
            CHECK(*err2 == meta::error_code::bad_argument_cast);
            CHECK(err2.get_error() == meta::error_code::bad_argument_cast);
        }
        {
            meta::uerror err1{meta::error_code::bad_argument_cast};
            meta::uerror err2{std::move(err1)};
            CHECK(*err2 == meta::error_code::bad_argument_cast);
            CHECK(err2.get_error() == meta::error_code::bad_argument_cast);
        }
    }

    SUBCASE("operator=") {
        {
            meta::uerror err1{meta::error_code::bad_argument_cast};
            meta::uerror err2{meta::error_code::bad_instance_cast};
            err1 = err2;
            CHECK(err1.get_error() == meta::error_code::bad_instance_cast);
        }
        {
            meta::uerror err1{meta::error_code::bad_argument_cast};
            meta::uerror err2{meta::error_code::bad_instance_cast};
            err1 = std::move(err2);
            CHECK(err1.get_error() == meta::error_code::bad_instance_cast);
        }
        {
            meta::uerror err1{meta::error_code::bad_argument_cast};
            err1 = meta::error_code::bad_instance_cast;
            CHECK(err1.get_error() == meta::error_code::bad_instance_cast);
        }
    }

    SUBCASE("operator==") {
        meta::uerror err1{meta::error_code::bad_argument_cast};
        meta::uerror err2{meta::error_code::bad_instance_cast};
        meta::uerror err3{meta::error_code::bad_instance_cast};

        CHECK_FALSE(err1 == err2);
        CHECK(err2 == err3);
    }

    SUBCASE("hash") {
        meta::uerror err1{meta::error_code::bad_argument_cast};
        meta::uerror err2{meta::error_code::bad_instance_cast};
        meta::uerror err3{meta::error_code::bad_instance_cast};

        CHECK_FALSE(err1.get_hash() == err2.get_hash());
        CHECK(err2.get_hash() == err3.get_hash());

        CHECK_FALSE(std::hash<meta::uerror>{}(err1) == err2.get_hash());
        CHECK(std::hash<meta::uerror>{}(err2) == err3.get_hash());
    }

    SUBCASE("reset") {
        meta::uerror err{meta::error_code::bad_argument_cast};
        err.reset();
        CHECK(err.get_error() == meta::error_code::no_error);
    }

    SUBCASE("swap") {
        meta::uerror err1{meta::error_code::bad_argument_cast};
        meta::uerror err2{meta::error_code::bad_instance_cast};

        err1.swap(err2);
        CHECK(err1.get_error() == meta::error_code::bad_instance_cast);
        CHECK(err2.get_error() == meta::error_code::bad_argument_cast);

        swap(err1, err2);
        CHECK(err1.get_error() == meta::error_code::bad_argument_cast);
        CHECK(err2.get_error() == meta::error_code::bad_instance_cast);
    }

    SUBCASE("make_uerror") {
        {
            meta::uerror err = meta::make_uerror(meta::error_code::bad_argument_cast);
            CHECK(err.get_error() == meta::error_code::bad_argument_cast);
        }
        {
            meta::uerror err = meta::make_uerror(meta::error_code::bad_instance_cast);
            CHECK(err.get_error() == meta::error_code::bad_instance_cast);
        }
    }
}

TEST_CASE("meta/meta_utilities/uresult") {
    namespace meta = meta_hpp;

    SUBCASE("ctor/error") {
        {
            meta::uresult res{meta::uerror{meta::error_code::bad_argument_cast}};
            CHECK_FALSE(res);
            CHECK_FALSE(res.has_value());
            CHECK_FALSE(res.get_value());
            CHECK(res.has_error());
            CHECK(res.get_error() == meta::error_code::bad_argument_cast);
        }
        {
            meta::uresult res{meta::uresult{meta::uerror{meta::error_code::bad_instance_cast}}};
            CHECK_FALSE(res);
            CHECK_FALSE(res.has_value());
            CHECK_FALSE(res.get_value());
            CHECK(res.has_error());
            CHECK(res.get_error() == meta::error_code::bad_instance_cast);
        }
    }

    SUBCASE("ctor/value") {
        {
            meta::uresult res{ivec2{42, 21}};
            CHECK(res);
            CHECK(res.has_value());
            REQUIRE(res.get_value());
            CHECK(res.get_value().as<ivec2>() == ivec2{42, 21});
            CHECK_FALSE(res.has_error());
            CHECK(res.get_error() == meta::error_code::no_error);
        }
        {
            meta::uresult res{meta::uvalue{ivec2{42, 21}}};
            CHECK(res);
            CHECK(res.has_value());
            REQUIRE(res.get_value());
            CHECK(res.get_value().as<ivec2>() == ivec2{42, 21});
            CHECK_FALSE(res.has_error());
            CHECK(res.get_error() == meta::error_code::no_error);
        }
        {
            meta::uresult res{meta::uresult(ivec2{42, 21})};
            CHECK(res);
            CHECK(res.has_value());
            REQUIRE(res.get_value());
            CHECK(res.get_value().as<ivec2>() == ivec2{42, 21});
            CHECK_FALSE(res.has_error());
            CHECK(res.get_error() == meta::error_code::no_error);
        }
    }

    SUBCASE("ctor/value/in_place") {
        {
            meta::uresult res{std::in_place_type<ivec2>, 42, 21};
            CHECK(res);
            CHECK(res.has_value());
            REQUIRE(res.get_value());
            CHECK(res.get_value().as<ivec2>() == ivec2{42, 21});
            CHECK_FALSE(res.has_error());
            CHECK(res.get_error() == meta::error_code::no_error);
        }
        {
            meta::uresult res{meta::make_uresult<ivec2>(42, 21)};
            CHECK(res);
            CHECK(res.has_value());
            REQUIRE(res.get_value());
            CHECK(res.get_value().as<ivec2>() == ivec2{42, 21});
            CHECK_FALSE(res.has_error());
            CHECK(res.get_error() == meta::error_code::no_error);
        }
        {
            meta::uresult res{std::in_place_type<std::vector<int>>, {42, 21}};
            CHECK(res);
            CHECK(res.has_value());
            REQUIRE(res.get_value());
            CHECK(res.get_value().as<std::vector<int>>() == std::vector<int>{42, 21});
            CHECK_FALSE(res.has_error());
            CHECK(res.get_error() == meta::error_code::no_error);
        }
        {
            meta::uresult res{meta::make_uresult<std::vector<int>>({42, 21})};
            CHECK(res);
            CHECK(res.has_value());
            REQUIRE(res.get_value());
            CHECK(res.get_value().as<std::vector<int>>() == std::vector<int>{42, 21});
            CHECK_FALSE(res.has_error());
            CHECK(res.get_error() == meta::error_code::no_error);
        }
    }

    SUBCASE("operator=") {
        meta::uresult res{meta::uerror{meta::error_code::bad_argument_cast}};

        res = ivec2{42, 21};
        CHECK(res);
        CHECK(res.has_value());
        REQUIRE(res.get_value());
        CHECK(res.get_value().as<ivec2>() == ivec2{42, 21});
        CHECK_FALSE(res.has_error());
        CHECK(res.get_error() == meta::error_code::no_error);

        res = meta::uerror{meta::error_code::bad_argument_cast};
        CHECK_FALSE(res);
        CHECK_FALSE(res.has_value());
        CHECK_FALSE(res.get_value());
        CHECK(res.has_error());
        CHECK(res.get_error() == meta::error_code::bad_argument_cast);

        res = meta::uvalue{ivec2{42, 21}};
        CHECK(res);
        CHECK(res.has_value());
        REQUIRE(res.get_value());
        CHECK(res.get_value().as<ivec2>() == ivec2{42, 21});
        CHECK_FALSE(res.has_error());
        CHECK(res.get_error() == meta::error_code::no_error);
    }

    SUBCASE("emplace") {
        {
            meta::uresult res{ivec2{42, 21}};
            CHECK(res.emplace<std::vector<int>>({42, 21}) == std::vector<int>{42, 21});
            CHECK(res);
            CHECK(res.has_value());
            CHECK(res.get_value().as<std::vector<int>>() == std::vector<int>{42, 21});

            CHECK(res.emplace<ivec2>(42, 21) == ivec2{42, 21});
            CHECK(res);
            CHECK(res.has_value());
            CHECK(res.get_value().as<ivec2>() == ivec2{42, 21});
        }
        {
            meta::uresult res{meta::uerror{meta::error_code::bad_argument_cast}};
            CHECK(res.emplace<ivec2>(42, 21) == ivec2{42, 21});
            CHECK(res);
            CHECK(res.has_value());
            CHECK(res.get_value().as<ivec2>() == ivec2{42, 21});

            CHECK(res.emplace<std::vector<int>>({42, 21}) == std::vector<int>{42, 21});
            CHECK(res);
            CHECK(res.has_value());
            CHECK(res.get_value().as<std::vector<int>>() == std::vector<int>{42, 21});
        }
    }

    SUBCASE("swap") {
        {
            meta::uresult res1{ivec2{42}};
            meta::uresult res2{ivec2{21}};
            res1.swap(res2);
            CHECK(res1.get_value().as<ivec2>() == ivec2{21});
            CHECK(res2.get_value().as<ivec2>() == ivec2{42});
        }
        {
            meta::uresult res1{ivec2{42}};
            meta::uresult res2{meta::uerror{meta::error_code::bad_argument_cast}};
            swap(res1, res2);
            CHECK(res1.get_error() == meta::error_code::bad_argument_cast);
            CHECK(res2.get_value().as<ivec2>() == ivec2{42});
        }
        {
            meta::uresult res1{meta::uerror{meta::error_code::bad_argument_cast}};
            meta::uresult res2{meta::uerror{meta::error_code::bad_instance_cast}};
            swap(res1, res2);
            CHECK(res1.get_error() == meta::error_code::bad_instance_cast);
            CHECK(res2.get_error() == meta::error_code::bad_argument_cast);
        }
    }

    SUBCASE("get_value") {
        static_assert(std::is_same_v<meta::uvalue&, decltype(std::declval<meta::uresult&>().get_value())>);
        static_assert(std::is_same_v<meta::uvalue&&, decltype(std::declval<meta::uresult&&>().get_value())>);
        static_assert(std::is_same_v<const meta::uvalue&, decltype(std::declval<const meta::uresult&>().get_value())>);
        static_assert(std::is_same_v<const meta::uvalue&&, decltype(std::declval<const meta::uresult&&>().get_value())>);

        static_assert(std::is_same_v<meta::uvalue&, decltype(std::declval<meta::uresult&>().operator*())>);
        static_assert(std::is_same_v<meta::uvalue&&, decltype(std::declval<meta::uresult&&>().operator*())>);
        static_assert(std::is_same_v<const meta::uvalue&, decltype(std::declval<const meta::uresult&>().operator*())>);
        static_assert(std::is_same_v<const meta::uvalue&&, decltype(std::declval<const meta::uresult&&>().operator*())>);

        static_assert(std::is_same_v<meta::uvalue*, decltype(std::declval<meta::uresult&>().operator->())>);
        static_assert(std::is_same_v<const meta::uvalue*, decltype(std::declval<const meta::uresult&>().operator->())>);

        {
            meta::uresult res{ivec2{42, 21}};
            CHECK(res.get_value().as<ivec2>() == ivec2{42, 21});
            CHECK(std::move(res).get_value().as<ivec2>() == ivec2{42, 21});
            CHECK(std::as_const(res).get_value().as<ivec2>() == ivec2{42, 21});
            CHECK(std::move(std::as_const(res)).get_value().as<ivec2>() == ivec2{42, 21});
        }

        {
            meta::uresult res{ivec2{42, 21}};
            CHECK((*res).as<ivec2>() == ivec2{42, 21});
            CHECK((*std::move(res)).as<ivec2>() == ivec2{42, 21});
            CHECK((*std::as_const(res)).as<ivec2>() == ivec2{42, 21});
            CHECK((*std::move(std::as_const(res))).as<ivec2>() == ivec2{42, 21});
        }

        {
            meta::uresult res{ivec2{42, 21}};
            CHECK(res->as<ivec2>() == ivec2{42, 21});
            CHECK(std::as_const(res)->as<ivec2>() == ivec2{42, 21});
        }
    }
}
