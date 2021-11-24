/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_tests.hpp"

namespace
{
    struct ivec2 {
        int x{};
        int y{};

        static ivec2 iadd(const ivec2& l, const ivec2& r) noexcept {
            return {l.x + r.x, l.y + r.y};
        }

        static int ilength2(const ivec2& v) noexcept {
            return v.x * v.x + v.y * v.y;
        }
    };

    bool operator==(const ivec2& l, const ivec2& r) noexcept {
        return l.x == r.x && l.y == r.y;
    }
}

TEST_CASE("meta/meta_states/function") {
    namespace meta = meta_hpp;

    meta::class_<ivec2>()
        .function_("iadd", &ivec2::iadd)
        .function_("ilength2", &ivec2::ilength2);

    const meta::class_type ivec2_type = meta::resolve_type<ivec2>();
    REQUIRE(ivec2_type);

    SUBCASE("") {
        const meta::function func;
        CHECK_FALSE(func);
        CHECK_FALSE(func.is_valid());
        CHECK(func == ivec2_type.get_function("non-existent-function"));
    }

    SUBCASE("operators") {
        const meta::function iadd_f = ivec2_type.get_function("iadd");
        const meta::function ilength2_f = ivec2_type.get_function("ilength2");
        CHECK(iadd_f == iadd_f);
        CHECK(iadd_f != ilength2_f);
        CHECK((iadd_f < ilength2_f || ilength2_f < iadd_f));
    }

    SUBCASE("iadd") {
        const meta::function func = ivec2_type.get_function("iadd");
        REQUIRE(func);

        CHECK(func.get_index().type == func.get_type());
        CHECK(func.get_index().name == "iadd");

        CHECK(func.get_type() == meta::resolve_type(&ivec2::iadd));
        CHECK(func.get_name() == "iadd");

        CHECK_FALSE(func.is_invocable_with<>());
        CHECK_FALSE(func.is_invocable_with<int>());
        CHECK_FALSE(func.is_invocable_with<ivec2, int>());
        CHECK_FALSE(func.is_invocable_with<int, ivec2>());
        CHECK_FALSE(func.is_invocable_with<ivec2, ivec2, int>());

        CHECK(func.is_invocable_with<ivec2, ivec2>());
        CHECK(func.is_invocable_with<const ivec2&, ivec2&&>());

        CHECK_THROWS(func.invoke());
        CHECK_THROWS(func.invoke(42));
        CHECK_THROWS(func.invoke(ivec2{}, 42));
        CHECK_THROWS(func.invoke(42, ivec2{}));
        CHECK_THROWS(func.invoke(ivec2{}, ivec2{}, 42));

        CHECK(func.invoke(ivec2{1,2}, ivec2{3,4}));
        CHECK(func.invoke(ivec2{1,2}, ivec2{3,4}).value() == ivec2{4,6});
    }

    SUBCASE("ilength2") {
        const meta::function func = ivec2_type.get_function("ilength2");
        REQUIRE(func);

        CHECK(func.get_index().type == func.get_type());
        CHECK(func.get_index().name == "ilength2");

        CHECK(func.get_type() == meta::resolve_type(&ivec2::ilength2));
        CHECK(func.get_name() == "ilength2");

        CHECK_FALSE(func.is_invocable_with<>());
        CHECK_FALSE(func.is_invocable_with<int>());
        CHECK_FALSE(func.is_invocable_with<ivec2, int>());

        CHECK(func.is_invocable_with<ivec2>());
        CHECK(func.is_invocable_with<const ivec2&>());

        CHECK_THROWS(func.invoke());
        CHECK_THROWS(func.invoke(42));
        CHECK_THROWS(func.invoke(ivec2{}, 42));

        CHECK(func.invoke(ivec2{2,3}));
        CHECK(func.invoke(ivec2{2,3}).value() == 13);
    }
}
