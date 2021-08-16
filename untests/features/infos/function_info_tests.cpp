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
        [[maybe_unused]] ivec2(const ivec2& other) = default;
        [[maybe_unused]] explicit ivec2(int v): x{v}, y{v} {}
        [[maybe_unused]] ivec2(int x, int y): x{x}, y{y} {}
    };

    bool operator==(const ivec2& l, const ivec2& r) noexcept {
        return l.x == r.x && l.y == r.y;
    }

    ivec2 iadd(const ivec2& l, const ivec2& r) noexcept {
        return {l.x + r.x, l.y + r.y};
    }

    int ilength2(const ivec2& v) noexcept {
        return v.x * v.x + v.y * v.y;
    }
}

TEST_CASE("features/infos/function") {
    registry db;

    db(
        namespace_{"vmath"}(
            class_<ivec2>{"ivec2"},
            function_{"iadd", &iadd}(data_{"info", "iadd function"s}),
            function_{"ilength2", &ilength2}(data_{"info", "ilength2 function"s})
        )
    );

    SUBCASE("iadd") {
        const function_info fi = db.get_function_by_path("vmath::iadd");
        REQUIRE(fi);

        REQUIRE(fi.get_data_by_name("info"));
        CHECK(fi.get_data_by_name("info").value() == "iadd function"s);

        CHECK_FALSE(fi.is_invocable_with());
        CHECK_FALSE(fi.is_invocable_with<int>());
        CHECK_FALSE(fi.is_invocable_with<ivec2, int>());
        CHECK_FALSE(fi.is_invocable_with<ivec2, ivec2, int>());

        CHECK(fi.is_invocable_with<ivec2, ivec2>());
        CHECK(fi.is_invocable_with<ivec2&&, const ivec2&>());

        CHECK_THROWS(fi.invoke());
        CHECK_THROWS(fi.invoke(42));
        CHECK_THROWS(fi.invoke(ivec2{}, 42));

        CHECK(fi.invoke(ivec2{1,2},ivec2{3,4}));
        CHECK(fi.invoke(ivec2{1,2},ivec2{3,4}).value() == ivec2{4,6});
    }

    SUBCASE("ilength2") {
        const function_info fi = db.get_function_by_path("vmath::ilength2");
        REQUIRE(fi);

        REQUIRE(fi.get_data_by_name("info"));
        CHECK(fi.get_data_by_name("info").value() == "ilength2 function"s);

        CHECK_FALSE(fi.is_invocable_with());
        CHECK_FALSE(fi.is_invocable_with<int>());
        CHECK_FALSE(fi.is_invocable_with<ivec2, int>());

        CHECK(fi.is_invocable_with<ivec2>());
        CHECK(fi.is_invocable_with<const ivec2&>());

        CHECK_THROWS(fi.invoke());
        CHECK_THROWS(fi.invoke(42));
        CHECK_THROWS(fi.invoke(ivec2{}, 42));

        CHECK(fi.invoke(ivec2{2,3}));
        CHECK(fi.invoke(ivec2{2,3}).value() == 13);
    }
}
