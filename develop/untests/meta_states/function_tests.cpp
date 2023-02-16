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

        static ivec2 iadd(const ivec2& l, const ivec2& r) noexcept {
            return {l.x + r.x, l.y + r.y};
        }

        static int ilength2(const ivec2& v) noexcept {
            return v.x * v.x + v.y * v.y;
        }

        static bool arg_nullptr(const void* ptr) { return ptr == nullptr; }
        static int arg_bounded_arr(ivec2 vs[2]) { return vs[0].x + vs[0].y + vs[1].x + vs[1].y; }
        static int arg_unbounded_arr(ivec2 vs[]) { return vs[0].x + vs[0].y + vs[1].x + vs[1].y; }
        static int arg_bounded_const_arr(const ivec2 vs[2]) { return vs[0].x + vs[0].y + vs[1].x + vs[1].y; }
        static int arg_unbounded_const_arr(const ivec2 vs[]) { return vs[0].x + vs[0].y + vs[1].x + vs[1].y; }
    };

    bool operator==(const ivec2& l, const ivec2& r) noexcept {
        return l.x == r.x && l.y == r.y;
    }
}

TEST_CASE("meta/meta_states/function") {
    namespace meta = meta_hpp;

    meta::class_<ivec2>()
        .function_("iadd", &ivec2::iadd)
        .function_("ilength2", &ivec2::ilength2)
        .function_("arg_nullptr", &ivec2::arg_nullptr)
        .function_("arg_bounded_arr", &ivec2::arg_bounded_arr)
        .function_("arg_unbounded_arr", &ivec2::arg_unbounded_arr)
        .function_("arg_bounded_const_arr", &ivec2::arg_bounded_const_arr)
        .function_("arg_unbounded_const_arr", &ivec2::arg_unbounded_const_arr);

    const meta::class_type ivec2_type = meta::resolve_type<ivec2>();
    REQUIRE(ivec2_type);

    SUBCASE("") {
        const meta::function func;
        CHECK_FALSE(func);
        CHECK(func.is_empty());
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

        CHECK(func.get_index().get_type() == func.get_type());
        CHECK(func.get_index().get_name() == "iadd");

        CHECK(func.get_type() == meta::resolve_type(&ivec2::iadd));
        CHECK(func.get_name() == "iadd");

        CHECK_FALSE(func.is_invocable_with<>());
        CHECK_FALSE(func.is_invocable_with<int>());
        CHECK_FALSE(func.is_invocable_with<ivec2, int>());
        CHECK_FALSE(func.is_invocable_with<int, ivec2>());
        CHECK_FALSE(func.is_invocable_with<ivec2, ivec2, int>());

        CHECK(func.is_invocable_with<ivec2, ivec2>());
        CHECK(func.is_invocable_with<const ivec2&, ivec2&&>());

        CHECK_FALSE(func.safe_invoke());
        CHECK_FALSE(func.safe_invoke(42));
        CHECK_FALSE(func.safe_invoke(ivec2{}, 42));
        CHECK_FALSE(func.safe_invoke(42, ivec2{}));
        CHECK_FALSE(func.safe_invoke(ivec2{}, ivec2{}, 42));

        CHECK(func.invoke(ivec2{1,2}, ivec2{3,4}));
        CHECK(func.invoke(ivec2{1,2}, ivec2{3,4}).get_as<ivec2>() == ivec2{4,6});
    }

    SUBCASE("ilength2") {
        const meta::function func = ivec2_type.get_function("ilength2");
        REQUIRE(func);

        CHECK(func.get_index().get_type() == func.get_type());
        CHECK(func.get_index().get_name() == "ilength2");

        CHECK(func.get_type() == meta::resolve_type(&ivec2::ilength2));
        CHECK(func.get_name() == "ilength2");

        CHECK_FALSE(func.is_invocable_with<>());
        CHECK_FALSE(func.is_invocable_with<int>());
        CHECK_FALSE(func.is_invocable_with<ivec2, int>());

        CHECK(func.is_invocable_with<ivec2>());
        CHECK(func.is_invocable_with<const ivec2&>());

        CHECK_FALSE(func.safe_invoke());
        CHECK_FALSE(func.safe_invoke(42));
        CHECK_FALSE(func.safe_invoke(ivec2{}, 42));

        CHECK(func.invoke(ivec2{2,3}));
        CHECK(func.invoke(ivec2{2,3}).get_as<int>() == 13);
    }

    SUBCASE("arg_null") {
        const meta::function func = ivec2_type.get_function("arg_nullptr");
        REQUIRE(func);

        CHECK(func.is_invocable_with<int*>());
        CHECK(func.is_invocable_with<const int*>());
        CHECK(func.is_invocable_with<std::nullptr_t>());

        int i{42};
        CHECK(func.invoke(&i).get_as<bool>() == false);
        CHECK(func.invoke(nullptr).get_as<bool>() == true);
    }

    SUBCASE("arg_arr") {
        ivec2 bounded_arr[2]{{1,2},{3,4}};
        ivec2* unbounded_arr = bounded_arr;

        const ivec2 bounded_const_arr[2]{{1,2},{3,4}};
        const ivec2* unbounded_const_arr = bounded_const_arr;

        {
            const meta::function func1 = ivec2_type.get_function("arg_bounded_arr");
            REQUIRE(func1);

            CHECK(func1.is_invocable_with<ivec2*>());
            CHECK(func1.is_invocable_with<ivec2* const>());
            CHECK_FALSE(func1.is_invocable_with<const ivec2*>());
            CHECK_FALSE(func1.is_invocable_with<const ivec2* const>());

            CHECK(func1.invoke(bounded_arr).get_as<int>() == 10);
            CHECK(func1.invoke(unbounded_arr).get_as<int>() == 10);
            CHECK_FALSE(func1.safe_invoke(bounded_const_arr));
            CHECK_FALSE(func1.safe_invoke(unbounded_const_arr));

            CHECK(func1.invoke(meta::uvalue{bounded_arr}).get_as<int>() == 10);
            CHECK(func1.invoke(meta::uvalue{unbounded_arr}).get_as<int>() == 10);
            CHECK_FALSE(func1.safe_invoke(meta::uvalue{bounded_const_arr}));
            CHECK_FALSE(func1.safe_invoke(meta::uvalue{unbounded_const_arr}));

            static_assert(std::is_invocable_v<decltype(&ivec2::arg_bounded_arr), decltype(bounded_arr)>);
            static_assert(std::is_invocable_v<decltype(&ivec2::arg_bounded_arr), decltype(unbounded_arr)>);
            static_assert(!std::is_invocable_v<decltype(&ivec2::arg_bounded_arr), decltype(bounded_const_arr)>);
            static_assert(!std::is_invocable_v<decltype(&ivec2::arg_bounded_arr), decltype(unbounded_const_arr)>);
        }

        {
            const meta::function func2 = ivec2_type.get_function("arg_unbounded_arr");
            REQUIRE(func2);

            CHECK(func2.is_invocable_with<ivec2*>());
            CHECK(func2.is_invocable_with<ivec2* const>());
            CHECK_FALSE(func2.is_invocable_with<const ivec2*>());
            CHECK_FALSE(func2.is_invocable_with<const ivec2* const>());

            CHECK(func2.invoke(bounded_arr).get_as<int>() == 10);
            CHECK(func2.invoke(unbounded_arr).get_as<int>() == 10);
            CHECK_FALSE(func2.safe_invoke(bounded_const_arr));
            CHECK_FALSE(func2.safe_invoke(unbounded_const_arr));

            CHECK(func2.invoke(meta::uvalue{bounded_arr}).get_as<int>() == 10);
            CHECK(func2.invoke(meta::uvalue{unbounded_arr}).get_as<int>() == 10);
            CHECK_FALSE(func2.safe_invoke(meta::uvalue{bounded_const_arr}));
            CHECK_FALSE(func2.safe_invoke(meta::uvalue{unbounded_const_arr}));

            static_assert(std::is_invocable_v<decltype(&ivec2::arg_unbounded_arr), decltype(bounded_arr)>);
            static_assert(std::is_invocable_v<decltype(&ivec2::arg_unbounded_arr), decltype(unbounded_arr)>);
            static_assert(!std::is_invocable_v<decltype(&ivec2::arg_unbounded_arr), decltype(bounded_const_arr)>);
            static_assert(!std::is_invocable_v<decltype(&ivec2::arg_unbounded_arr), decltype(unbounded_const_arr)>);
        }
    }

    SUBCASE("arg_const_arr") {
        ivec2 bounded_arr[2]{{1,2},{3,4}};
        ivec2* unbounded_arr = bounded_arr;

        const ivec2 bounded_const_arr[2]{{1,2},{3,4}};
        const ivec2* unbounded_const_arr = bounded_const_arr;

        {
            const meta::function func1 = ivec2_type.get_function("arg_bounded_const_arr");
            REQUIRE(func1);

            CHECK(func1.is_invocable_with<ivec2*>());
            CHECK(func1.is_invocable_with<ivec2* const>());
            CHECK(func1.is_invocable_with<const ivec2*>());
            CHECK(func1.is_invocable_with<const ivec2* const>());

            CHECK(func1.invoke(bounded_arr).get_as<int>() == 10);
            CHECK(func1.invoke(unbounded_arr).get_as<int>() == 10);
            CHECK(func1.invoke(bounded_const_arr).get_as<int>() == 10);
            CHECK(func1.invoke(unbounded_const_arr).get_as<int>() == 10);

            CHECK(func1.invoke(meta::uvalue{bounded_arr}).get_as<int>() == 10);
            CHECK(func1.invoke(meta::uvalue{unbounded_arr}).get_as<int>() == 10);
            CHECK(func1.invoke(meta::uvalue{bounded_const_arr}).get_as<int>() == 10);
            CHECK(func1.invoke(meta::uvalue{unbounded_const_arr}).get_as<int>() == 10);

            static_assert(std::is_invocable_v<decltype(&ivec2::arg_bounded_const_arr), decltype(bounded_arr)>);
            static_assert(std::is_invocable_v<decltype(&ivec2::arg_bounded_const_arr), decltype(unbounded_arr)>);
            static_assert(std::is_invocable_v<decltype(&ivec2::arg_bounded_const_arr), decltype(bounded_const_arr)>);
            static_assert(std::is_invocable_v<decltype(&ivec2::arg_bounded_const_arr), decltype(unbounded_const_arr)>);
        }

        {
            const meta::function func2 = ivec2_type.get_function("arg_unbounded_const_arr");
            REQUIRE(func2);

            CHECK(func2.is_invocable_with<ivec2*>());
            CHECK(func2.is_invocable_with<ivec2* const>());
            CHECK(func2.is_invocable_with<const ivec2*>());
            CHECK(func2.is_invocable_with<const ivec2* const>());

            CHECK(func2.invoke(bounded_arr).get_as<int>() == 10);
            CHECK(func2.invoke(unbounded_arr).get_as<int>() == 10);
            CHECK(func2.invoke(bounded_const_arr).get_as<int>() == 10);
            CHECK(func2.invoke(unbounded_const_arr).get_as<int>() == 10);

            CHECK(func2.invoke(meta::uvalue{bounded_arr}).get_as<int>() == 10);
            CHECK(func2.invoke(meta::uvalue{unbounded_arr}).get_as<int>() == 10);
            CHECK(func2.invoke(meta::uvalue{bounded_const_arr}).get_as<int>() == 10);
            CHECK(func2.invoke(meta::uvalue{unbounded_const_arr}).get_as<int>() == 10);

            static_assert(std::is_invocable_v<decltype(&ivec2::arg_unbounded_const_arr), decltype(bounded_arr)>);
            static_assert(std::is_invocable_v<decltype(&ivec2::arg_unbounded_const_arr), decltype(unbounded_arr)>);
            static_assert(std::is_invocable_v<decltype(&ivec2::arg_unbounded_const_arr), decltype(bounded_const_arr)>);
            static_assert(std::is_invocable_v<decltype(&ivec2::arg_unbounded_const_arr), decltype(unbounded_const_arr)>);
        }
    }
}
