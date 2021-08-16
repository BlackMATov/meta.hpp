/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "_infos_fwd.hpp"

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

        [[maybe_unused]] ivec2() = default;
        [[maybe_unused]] explicit ivec2(int v): x{v}, y{v} {}
        [[maybe_unused]] ivec2(int x, int y): x{x}, y{y} {}
    };

    struct ipnt2 {
        const int x{};
        const int y{};

        [[maybe_unused]] ipnt2() = default;
        [[maybe_unused]] explicit ipnt2(int v): x{v}, y{v} {}
        [[maybe_unused]] ipnt2(int x, int y): x{x}, y{y} {}
    };

    bool operator==(const ivec2& l, const ivec2& r) noexcept {
        return l.x == r.x && l.y == r.y;
    }

    bool operator==(const ipnt2& l, const ipnt2& r) noexcept {
        return l.x == r.x && l.y == r.y;
    }
}

TEST_CASE("features/infos/member") {
    registry db;

    db(
        namespace_{"vmath"}(
            class_<ivec2>{"ivec2"}(
                member_{"x", &ivec2::x},
                member_{"y", &ivec2::y}
            ),
            class_<ipnt2>{"ipnt2"}(
                member_{"x", &ipnt2::x},
                member_{"y", &ipnt2::y}
            )
        )
    );

    SUBCASE("non_const") {
        const member_info x_mi = db.get_member_by_path("vmath::ivec2::x");
        const member_info y_mi = db.get_member_by_path("vmath::ivec2::y");
        REQUIRE(x_mi);
        REQUIRE(y_mi);

        {
            CHECK(x_mi.name() == "x");
            CHECK(x_mi.type().class_type() == type_db::get<ivec2>());
            CHECK(x_mi.type().value_type() == type_db::get<int>());

            CHECK(y_mi.name() == "y");
            CHECK(y_mi.type().class_type() == type_db::get<ivec2>());
            CHECK(y_mi.type().value_type() == type_db::get<int>());
        }

        {
            ivec2 v{1,2};

            CHECK(x_mi.get(v) == 1);
            CHECK(x_mi.get(std::as_const(v)) == 1);
            CHECK(x_mi.get(std::move(v)) == 1);
            CHECK(x_mi.get(std::move(std::as_const(v))) == 1);

            CHECK(y_mi.get(v) == 2);
            CHECK(y_mi.get(std::as_const(v)) == 2);
            CHECK(y_mi.get(std::move(v)) == 2);
            CHECK(y_mi.get(std::move(std::as_const(v))) == 2);

            static_assert(std::is_invocable_v<decltype(&ivec2::x), ivec2&>);
            static_assert(std::is_invocable_v<decltype(&ivec2::x), const ivec2&>);
            static_assert(std::is_invocable_v<decltype(&ivec2::x), ivec2&&>);
            static_assert(std::is_invocable_v<decltype(&ivec2::x), const ivec2&&>);
        }

        {
            ivec2 v{1,2};

            CHECK_NOTHROW(x_mi.set(v, 10)); CHECK(v.x == 10);
            CHECK_THROWS(x_mi.set(std::as_const(v), 11)); CHECK(v.x == 10);
            CHECK_NOTHROW(x_mi.set(std::move(v), 12)); CHECK(v.x == 12);
            CHECK_THROWS(x_mi.set(std::move(std::as_const(v)), 13)); CHECK(v.x == 12);

            CHECK_NOTHROW(y_mi.set(v, 20)); CHECK(v.y == 20);
            CHECK_THROWS(y_mi.set(std::as_const(v), 21)); CHECK(v.y == 20);
            CHECK_NOTHROW(y_mi.set(std::move(v), 22)); CHECK(v.y == 22);
            CHECK_THROWS(y_mi.set(std::move(std::as_const(v)), 23)); CHECK(v.y == 22);

            CHECK(v == ivec2{12,22});
        }
    }

    SUBCASE("const") {
        const member_info x_mi = db.get_member_by_path("vmath::ipnt2::x");
        const member_info y_mi = db.get_member_by_path("vmath::ipnt2::y");
        REQUIRE(x_mi);
        REQUIRE(y_mi);

        {
            CHECK(x_mi.name() == "x");
            CHECK(x_mi.type().class_type() == type_db::get<ipnt2>());
            CHECK(x_mi.type().value_type() == type_db::get<const int>());

            CHECK(y_mi.name() == "y");
            CHECK(y_mi.type().class_type() == type_db::get<ipnt2>());
            CHECK(y_mi.type().value_type() == type_db::get<const int>());
        }

        {
            ipnt2 v{1,2};

            CHECK(x_mi.get(v) == 1);
            CHECK(x_mi.get(std::as_const(v)) == 1);
            CHECK(x_mi.get(std::move(v)) == 1);
            CHECK(x_mi.get(std::move(std::as_const(v))) == 1);

            CHECK(y_mi.get(v) == 2);
            CHECK(y_mi.get(std::as_const(v)) == 2);
            CHECK(y_mi.get(std::move(v)) == 2);
            CHECK(y_mi.get(std::move(std::as_const(v))) == 2);

            static_assert(std::is_invocable_v<decltype(&ipnt2::x), ipnt2&>);
            static_assert(std::is_invocable_v<decltype(&ipnt2::x), const ipnt2&>);
            static_assert(std::is_invocable_v<decltype(&ipnt2::x), ipnt2&&>);
            static_assert(std::is_invocable_v<decltype(&ipnt2::x), const ipnt2&&>);
        }

        {
            ipnt2 v{1,2};

            CHECK_THROWS(x_mi.set(v, 10)); CHECK(v.x == 1);
            CHECK_THROWS(x_mi.set(std::as_const(v), 11)); CHECK(v.x == 1);
            CHECK_THROWS(x_mi.set(std::move(v), 12)); CHECK(v.x == 1);
            CHECK_THROWS(x_mi.set(std::move(std::as_const(v)), 13)); CHECK(v.x == 1);

            CHECK_THROWS(y_mi.set(v, 20)); CHECK(v.y == 2);
            CHECK_THROWS(y_mi.set(std::as_const(v), 21)); CHECK(v.y == 2);
            CHECK_THROWS(y_mi.set(std::move(v), 22)); CHECK(v.y == 2);
            CHECK_THROWS(y_mi.set(std::move(std::as_const(v)), 23)); CHECK(v.y == 2);

            CHECK(v == ipnt2{1,2});
        }
    }
}
