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

        ivec2& add(const ivec2& other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        int operator[](std::size_t i) const noexcept {
            return i == 0 ? x : y;
        }

        ivec2& operator=(ivec2&& other) = delete;
        ivec2& operator=(const ivec2& other) = delete;
    public:
        inline static int move_constructor_counter{};
        inline static int copy_constructor_counter{};
    };

    struct ivec3 {
        int x{};
        int y{};
        int z{};

        ivec3() = delete;

        explicit ivec3(int nv): x{nv}, y{nv}, z{nv} {}
        ivec3(int nx, int ny, int nz): x{nx}, y{ny}, z{nz} {}
    };

    struct ivec2_big {
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

        ivec2_big& add(const ivec2_big& other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        ivec2_big& operator=(ivec2_big&& other) = delete;
        ivec2_big& operator=(const ivec2_big& other) = delete;
    public:
        inline static int move_constructor_counter{};
        inline static int copy_constructor_counter{};
    };

    ivec2 iadd2(ivec2 l, ivec2 r) {
        return {l.x + r.x, l.y + r.y};
    }

    [[maybe_unused]]
    bool operator==(const ivec2& l, const ivec2& r) noexcept {
        return l.x == r.x && l.y == r.y;
    }

    [[maybe_unused]]
    bool operator==(const ivec2_big& l, const ivec2_big& r) noexcept {
        return l.x == r.x && l.y == r.y;
    }

    struct deref_custom_class {
        int operator*() const { return 42; }
    };
}

META_HPP_DECLARE_COPY_TRAITS_FOR(ivec2)
META_HPP_DECLARE_COPY_TRAITS_FOR(ivec2_big)

META_HPP_DECLARE_DEREF_TRAITS_FOR(deref_custom_class)

META_HPP_DECLARE_INDEX_TRAITS_FOR(ivec2)

TEST_CASE("meta/meta_utilities/value/_") {
    namespace meta = meta_hpp;

    meta::class_<ivec2>()
        .constructor_<int>()
        .constructor_<int, int>()
        .constructor_<ivec2&&>()
        .constructor_<const ivec2&>()
        .member_("x", &ivec2::x)
        .member_("y", &ivec2::y);

    meta::class_<ivec3>()
        .constructor_<int>()
        .constructor_<int, int, int>()
        .constructor_<ivec3&&>()
        .constructor_<const ivec3&>()
        .member_("x", &ivec3::x)
        .member_("y", &ivec3::y)
        .member_("z", &ivec3::z);
}

TEST_CASE("meta/meta_utilities/value") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    ivec2::move_constructor_counter = 0;
    ivec2::copy_constructor_counter = 0;
    ivec2_big::move_constructor_counter = 0;
    ivec2_big::copy_constructor_counter = 0;

    SUBCASE("cast types") {
        static_assert(std::is_same_v<
            decltype(std::declval<meta::uvalue&>().as<ivec2>()),
            ivec2&>);
        static_assert(std::is_same_v<
            decltype(std::declval<meta::uvalue&&>().as<ivec2>()),
            ivec2>);
        static_assert(std::is_same_v<
            decltype(std::declval<const meta::uvalue&>().as<ivec2>()),
            const ivec2&>);
        static_assert(std::is_same_v<
            decltype(std::declval<const meta::uvalue&&>().as<ivec2>()),
            const ivec2&&>);
    }

    SUBCASE("ivec2{}") {
        {
            meta::uvalue val{};

            CHECK(!val);
            CHECK_FALSE(val);

            CHECK_FALSE(val.has_value());
            CHECK(val.get_data() == nullptr);
            CHECK(std::as_const(val).get_data() == nullptr);
            CHECK(std::as_const(val).get_cdata() == nullptr);

            CHECK_THROWS(std::ignore = *val);
            CHECK_THROWS(std::ignore = val[0]);

            CHECK_FALSE(val.try_as<ivec2>());
            CHECK_FALSE(std::as_const(val).try_as<ivec2>());

            CHECK_THROWS(std::ignore = val.as<int>());
            CHECK_THROWS(std::ignore = std::as_const(val).as<int>());
            CHECK_THROWS(std::ignore = std::move(val).as<int>());
            CHECK_THROWS(std::ignore = std::move(std::as_const(val)).as<int>());
        }

        CHECK_FALSE(meta::uvalue{}.get_type());
    }

    SUBCASE("ivec2&") {
        ivec2 v{1,2};
        ivec2& vr = v;

        meta::uvalue val{vr};
        CHECK(ivec2::move_constructor_counter == 0);
        CHECK(ivec2::copy_constructor_counter == 1);

        CHECK(val.get_type() == meta::resolve_type<ivec2>());

        CHECK(*static_cast<const ivec2*>(val.get_data()) == vr);
        CHECK(*static_cast<const ivec2*>(val.get_cdata()) == vr);
        CHECK(*static_cast<const ivec2*>(std::as_const(val).get_data()) == vr);
        CHECK(*static_cast<const ivec2*>(std::as_const(val).get_cdata()) == vr);

        CHECK(val.as<ivec2>() == ivec2{1,2});

        CHECK(val.as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).as<ivec2>() == ivec2{1,2});

        {
            meta::uvalue val_copy{val.copy()};
            CHECK(std::move(val_copy).as<ivec2>() == ivec2{1,2});
        }
        {
            meta::uvalue val_copy{val.copy()};
            CHECK(std::move(std::as_const(val_copy)).as<ivec2>() == ivec2{1,2});
        }

        CHECK_THROWS(std::ignore = val.as<ivec3>());
        CHECK_THROWS(std::ignore = std::as_const(val).as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(val).as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(std::as_const(val)).as<ivec3>());

        CHECK(val.as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).as<ivec2>() == ivec2{1,2});
        CHECK_FALSE(val.try_as<ivec3>());
        CHECK_FALSE(std::as_const(val).try_as<ivec3>());
    }

    SUBCASE("const ivec2&") {
        const ivec2 v{1,2};
        const ivec2& vr = v;

        meta::uvalue val{vr};
        CHECK(ivec2::move_constructor_counter == 0);
        CHECK(ivec2::copy_constructor_counter == 1);

        CHECK(val.get_type() == meta::resolve_type<ivec2>());

        CHECK(*static_cast<const ivec2*>(val.get_data()) == vr);
        CHECK(*static_cast<const ivec2*>(val.get_cdata()) == vr);
        CHECK(*static_cast<const ivec2*>(std::as_const(val).get_data()) == vr);
        CHECK(*static_cast<const ivec2*>(std::as_const(val).get_cdata()) == vr);

        CHECK(val.as<ivec2>() == ivec2{1,2});

        CHECK(val.as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).as<ivec2>() == ivec2{1,2});

        {
            meta::uvalue val_copy{val.copy()};
            CHECK(std::move(val_copy).as<ivec2>() == ivec2{1,2});
        }
        {
            meta::uvalue val_copy{val.copy()};
            CHECK(std::move(std::as_const(val_copy)).as<ivec2>() == ivec2{1,2});
        }

        CHECK_THROWS(std::ignore = val.as<ivec3>());
        CHECK_THROWS(std::ignore = std::as_const(val).as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(val).as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(std::as_const(val)).as<ivec3>());

        CHECK(val.as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).as<ivec2>() == ivec2{1,2});
        CHECK_FALSE(val.try_as<ivec3>());
        CHECK_FALSE(std::as_const(val).try_as<ivec3>());
    }

    SUBCASE("ivec2&&") {
        ivec2 v{1,2};

        meta::uvalue val{std::move(v)};
        CHECK(ivec2::move_constructor_counter == 1);
        CHECK(ivec2::copy_constructor_counter == 0);

        CHECK(val.get_type() == meta::resolve_type<ivec2>());

        CHECK(val.as<ivec2>() == ivec2{1,2});

        CHECK(val.as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).as<ivec2>() == ivec2{1,2});

        {
            meta::uvalue val_copy{val.copy()};
            CHECK(std::move(val_copy).as<ivec2>() == ivec2{1,2});
        }
        {
            meta::uvalue val_copy{val.copy()};
            CHECK(std::move(std::as_const(val_copy)).as<ivec2>() == ivec2{1,2});
        }

        CHECK_THROWS(std::ignore = val.as<ivec3>());
        CHECK_THROWS(std::ignore = std::as_const(val).as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(val).as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(std::as_const(val)).as<ivec3>());

        CHECK(val.as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).as<ivec2>() == ivec2{1,2});
        CHECK_FALSE(val.try_as<ivec3>());
        CHECK_FALSE(std::as_const(val).try_as<ivec3>());
    }

    SUBCASE("const ivec2&&") {
        const ivec2 v{1,2};

        meta::uvalue val{std::move(v)};
        CHECK(ivec2::move_constructor_counter == 0);
        CHECK(ivec2::copy_constructor_counter == 1);

        CHECK(val.get_type() == meta::resolve_type<ivec2>());

        CHECK(val.as<ivec2>() == ivec2{1,2});

        CHECK(val.as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).as<ivec2>() == ivec2{1,2});

        {
            meta::uvalue val_copy{val.copy()};
            CHECK(std::move(val_copy).as<ivec2>() == ivec2{1,2});
        }
        {
            meta::uvalue val_copy{val.copy()};
            CHECK(std::move(std::as_const(val_copy)).as<ivec2>() == ivec2{1,2});
        }

        CHECK_THROWS(std::ignore = val.as<ivec3>());
        CHECK_THROWS(std::ignore = std::as_const(val).as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(val).as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(std::as_const(val)).as<ivec3>());

        CHECK(val.as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).as<ivec2>() == ivec2{1,2});
        CHECK_FALSE(val.try_as<ivec3>());
        CHECK_FALSE(std::as_const(val).try_as<ivec3>());
    }

    SUBCASE("value(value&&)") {
        ivec2 v{1,2};
        meta::uvalue val_src{std::move(v)};
        CHECK(ivec2::move_constructor_counter == 1);
        CHECK(ivec2::copy_constructor_counter == 0);

        meta::uvalue val_dst{std::move(val_src)};
        CHECK(val_dst.as<ivec2>() == ivec2{1,2});
        CHECK(ivec2::move_constructor_counter == 2);
        CHECK(ivec2::copy_constructor_counter == 0);
    }

    SUBCASE("value& operator=(T&&)") {
        meta::uvalue val{10};

        val = 20;
        CHECK(val.as<int>() == 20);

        val = "hello"s;
        CHECK(val.as<std::string>() == "hello"s);
    }

    SUBCASE("value& operator=(value&&)") {
        meta::uvalue val_src1{"world"s};
        meta::uvalue val_src2{ivec2{1,2}};
        CHECK(ivec2::move_constructor_counter == 1);
        CHECK(ivec2::copy_constructor_counter == 0);

        meta::uvalue val_dst{"hello"s};

        val_dst = std::move(val_src1);
        CHECK(val_dst.as<std::string>() == "world"s);
        CHECK(ivec2::move_constructor_counter == 1);
        CHECK(ivec2::copy_constructor_counter == 0);

        val_dst = std::move(val_src2);
        CHECK(val_dst.as<ivec2>() == ivec2{1,2});
        CHECK(ivec2::move_constructor_counter == 3);
        CHECK(ivec2::copy_constructor_counter == 0);
    }

    SUBCASE("swap/0") {
        meta::uvalue val1{"world"s};
        meta::uvalue val2{ivec2{1,2}};
        CHECK(ivec2::move_constructor_counter == 1);
        CHECK(ivec2::copy_constructor_counter == 0);

        val2.swap(val1);
        CHECK(val1.as<ivec2>() == ivec2{1,2});
        CHECK(val2.as<std::string>() == "world"s);
        CHECK(ivec2::move_constructor_counter == 2);
        CHECK(ivec2::copy_constructor_counter == 0);

        swap(val2, val1);
        CHECK(val1.as<std::string>() == "world"s);
        CHECK(val2.as<ivec2>() == ivec2{1,2});
    }

    SUBCASE("swap/1") {
        meta::uvalue val1{42};
        meta::uvalue val2{};

        swap(val1, val2);
        CHECK_FALSE(val1);
        CHECK(val2.as<int>() == 42);

        swap(val1, val2);
        CHECK(val1.as<int>() == 42);
        CHECK_FALSE(val2);
    }

    SUBCASE("swap/2") {
        meta::uvalue val1{ivec2{1,2}};
        meta::uvalue val2{};
        CHECK(ivec2::move_constructor_counter == 1);
        CHECK(ivec2::copy_constructor_counter == 0);

        swap(val1, val2);
        CHECK_FALSE(val1);
        CHECK(val2.as<ivec2>() == ivec2{1,2});
        CHECK(ivec2::move_constructor_counter == 2);
        CHECK(ivec2::copy_constructor_counter == 0);

        swap(val1, val2);
        CHECK(val1.as<ivec2>() == ivec2{1,2});
        CHECK_FALSE(val2);
        CHECK(ivec2::move_constructor_counter == 3);
        CHECK(ivec2::copy_constructor_counter == 0);
    }

    SUBCASE("swap/3") {
        meta::uvalue val1{ivec2_big{1,2}};
        meta::uvalue val2{};
        CHECK(ivec2_big::move_constructor_counter == 1);
        CHECK(ivec2_big::copy_constructor_counter == 0);

        swap(val1, val2);
        CHECK_FALSE(val1);
        CHECK(val2.as<ivec2_big>() == ivec2_big{1,2});
        CHECK(ivec2_big::move_constructor_counter == 1);
        CHECK(ivec2_big::copy_constructor_counter == 0);

        swap(val1, val2);
        CHECK(val1.as<ivec2_big>() == ivec2_big{1,2});
        CHECK_FALSE(val2);
        CHECK(ivec2_big::move_constructor_counter == 1);
        CHECK(ivec2_big::copy_constructor_counter == 0);
    }

    SUBCASE("after_move") {
        {
            meta::uvalue val1{42};

            meta::uvalue val2{std::move(val1)};
            CHECK_FALSE(val1);
            CHECK(val2.as<int>() == 42);

            val1 = std::move(val2);
            CHECK_FALSE(val2);
            CHECK(val1.as<int>() == 42);
        }
        {
            meta::uvalue val1{ivec2{1,2}};

            meta::uvalue val2{std::move(val1)};
            CHECK_FALSE(val1);
            CHECK(val2);
            CHECK(val2.as<ivec2>() == ivec2{1,2});

            val1 = std::move(val2);
            CHECK_FALSE(val2);
            CHECK(val1.as<ivec2>() == ivec2{1,2});
        }
        {
            meta::uvalue val1{ivec2_big{1,2}};

            meta::uvalue val2{std::move(val1)};
            CHECK_FALSE(val1);
            CHECK(val2);
            CHECK(val2.as<ivec2_big>() == ivec2_big{1,2});

            val1 = std::move(val2);
            CHECK_FALSE(val2);
            CHECK(val1.as<ivec2_big>() == ivec2_big{1,2});
        }
    }

    SUBCASE("copy") {
        {
            const meta::uvalue u{};
            CHECK(u.has_copy_op());

            const meta::uvalue v{u.copy()};
            CHECK_FALSE(v);
        }
        {
            const meta::uvalue u{42};
            CHECK(u.has_copy_op());

            const meta::uvalue v{u.copy()};
            CHECK(v.get_type() == meta::resolve_type<int>());
            CHECK(v.as<int>() == 42);
        }
        {
            const meta::uvalue u{std::unique_ptr<int>{}};
            CHECK_FALSE(u.has_copy_op());
            CHECK_THROWS(std::ignore = u.copy());
        }
        {
            CHECK(meta::uvalue{std::array<int, 1>{}}.has_copy_op());
            CHECK_FALSE(meta::uvalue{std::array<std::unique_ptr<int>, 1>{}}.has_copy_op());
            CHECK(meta::uvalue{std::string{}}.has_copy_op());
            CHECK(meta::uvalue{std::string_view{}}.has_copy_op());
            CHECK(meta::uvalue{std::vector{42, 21}}.has_copy_op());
            CHECK_FALSE(meta::uvalue{std::vector<std::unique_ptr<int>>{}}.has_copy_op());
            CHECK(meta::uvalue{std::shared_ptr<int>{}}.has_copy_op());
            {
                using ref_t = std::reference_wrapper<const std::unique_ptr<int>>;
                std::unique_ptr u = std::make_unique<int>(42);
                CHECK(meta::uvalue{ref_t{u}}.has_copy_op());
                meta::uvalue v = meta::uvalue{ref_t{u}}.copy();
                CHECK(v.get_type() == meta::resolve_type<ref_t>());
                CHECK(v.as<ref_t>().get().get() == u.get());
            }
            CHECK(meta::uvalue{std::make_tuple(42, std::make_shared<int>(42))}.has_copy_op());
            CHECK_FALSE(meta::uvalue{std::make_tuple(42, std::make_unique<int>(42))}.has_copy_op());
        }
    }

    SUBCASE("unmap") {
        {
            const meta::uvalue u{42};
            CHECK_FALSE(u.has_unmap_op());
            CHECK_THROWS(std::ignore = u.unmap());
        }
        {
            int i{42};
            const meta::uvalue u{std::ref(i)};
            CHECK(u.has_unmap_op());

            const meta::uvalue v{u.unmap()};
            CHECK(v.get_type() == meta::resolve_type<int*>());
            CHECK(v.as<int*>() == &i);
        }
        {
            const int i{42};
            const meta::uvalue u{std::ref(i)};
            CHECK(u.has_unmap_op());

            const meta::uvalue v{u.unmap()};
            CHECK(v.get_type() == meta::resolve_type<const int*>());
            CHECK(v.as<const int*>() == &i);
        }
        {
            const auto i = std::make_shared<ivec2>(3, 4);
            const meta::uvalue u{i};
            CHECK(u.has_unmap_op());

            const meta::uvalue v{u.unmap()};
            CHECK(v.get_type() == meta::resolve_type<ivec2*>());
            CHECK(v.as<ivec2*>() == i.get());
        }
        {
            const auto i = std::make_shared<const ivec2>(3, 4);
            const meta::uvalue u{i};
            CHECK(u.has_unmap_op());

            const meta::uvalue v{u.unmap()};
            CHECK(v.get_type() == meta::resolve_type<const ivec2*>());
            CHECK(v.as<const ivec2*>() == i.get());
        }
    }

    SUBCASE("deref") {
        {
            int i{42};
            meta::uvalue u{&i};
            CHECK(u.has_deref_op());

            const meta::uvalue v{*u};
            CHECK(v.as<int>() == i);
        }
        {
            const char i{42};
            meta::uvalue u{&i};
            CHECK(u.has_deref_op());

            const meta::uvalue v{*u};
            CHECK(v.as<char>() == i);
        }
        {
            const int i{42};
            const int* const pi = &i;
            meta::uvalue u{&pi};
            CHECK(u.has_deref_op());

            const meta::uvalue v{*u};
            CHECK(v.as<const int*>() == pi);
        }
        {
            int i{42};

            void* p1 = &i;
            const void* p2 = &i;
            void* const& p3 = &i;
            const void* const& p4 = &i;

            CHECK_FALSE(meta::uvalue(p1).has_deref_op());
            CHECK_FALSE(meta::uvalue(p2).has_deref_op());
            CHECK_FALSE(meta::uvalue(p3).has_deref_op());
            CHECK_FALSE(meta::uvalue(p4).has_deref_op());

            CHECK_THROWS(std::ignore = *meta::uvalue(p1));
            CHECK_THROWS(std::ignore = *meta::uvalue(p2));
            CHECK_THROWS(std::ignore = *meta::uvalue(p3));
            CHECK_THROWS(std::ignore = *meta::uvalue(p4));
        }
        {
            int* p1 = nullptr;
            const int* p2 = nullptr;

            CHECK(meta::uvalue{p1}.has_deref_op());
            CHECK(meta::uvalue{p2}.has_deref_op());

            CHECK_FALSE(*meta::uvalue{p1});
            CHECK_FALSE(*meta::uvalue{p2});
        }
        {
            ivec2 v{1,2};
            meta::uvalue vp{&v};
            CHECK(ivec2::move_constructor_counter == 0);
            CHECK(ivec2::copy_constructor_counter == 0);

            meta::uvalue vv1{*vp};
            CHECK(ivec2::move_constructor_counter <= 3);
            CHECK(ivec2::copy_constructor_counter == 1);

            meta::uvalue vv2{*std::move(vp)};
            CHECK(ivec2::move_constructor_counter <= 6);
            CHECK(ivec2::copy_constructor_counter == 2);

            meta::uvalue vv3{*std::as_const(vp)};
            CHECK(ivec2::move_constructor_counter <= 9);
            CHECK(ivec2::copy_constructor_counter == 3);
        }
        {
            meta::uvalue v{std::make_shared<int>(42)};
            CHECK((*v).as<int>() == 42);
        }
        {
            meta::uvalue v{std::make_unique<int>(42)};
            CHECK((*v).as<int>() == 42);
        }
        {
            CHECK(meta::uvalue{std::make_shared<std::vector<std::shared_ptr<int>>>()}.has_deref_op());
            CHECK_FALSE(meta::uvalue{std::make_shared<std::vector<std::unique_ptr<int>>>()}.has_deref_op());
        }
        {
            const meta::uvalue v{deref_custom_class{}};
            CHECK(v.has_deref_op());

            const meta::uvalue u = *v;
            CHECK(u.get_type() == meta::resolve_type<int>());
            CHECK(u.as<int>() == 42);
        }
    }

    SUBCASE("less/equal") {
        {
            meta::uvalue l{};
            meta::uvalue r{};

            CHECK(l.has_less_op());
            CHECK(r.has_less_op());
            CHECK_FALSE(l.less(l));
            CHECK_FALSE(r.less(r));
            CHECK_FALSE(l.less(r));
            CHECK_FALSE(r.less(l));

            CHECK(l.has_equals_op());
            CHECK(r.has_equals_op());
            CHECK(l.equals(l));
            CHECK(r.equals(r));
            CHECK(l.equals(r));
            CHECK(r.equals(l));
        }
        {
            meta::uvalue l{};
            meta::uvalue r{42};

            CHECK(l.has_less_op());
            CHECK(r.has_less_op());
            CHECK_FALSE(l.less(l));
            CHECK_FALSE(r.less(r));
            CHECK(l.less(r));
            CHECK_FALSE(r.less(l));

            CHECK(l.has_equals_op());
            CHECK(r.has_equals_op());
            CHECK(l.equals(l));
            CHECK(r.equals(r));
            CHECK_FALSE(l.equals(r));
            CHECK_FALSE(r.equals(l));
        }
        {
            meta::uvalue l{21};
            meta::uvalue r{42};
            meta::uvalue r2{42};

            CHECK(l.has_less_op());
            CHECK(r.has_less_op());
            CHECK_FALSE(l.less(l));
            CHECK_FALSE(r.less(r));
            CHECK(l.less(r));
            CHECK_FALSE(r.less(l));
            CHECK_FALSE(r.less(r2));

            CHECK(l.has_equals_op());
            CHECK(r.has_equals_op());
            CHECK(l.equals(l));
            CHECK(r.equals(r));
            CHECK_FALSE(l.equals(r));
            CHECK_FALSE(r.equals(l));
            CHECK(r.equals(r2));
        }
        {
            meta::uvalue l{42};
            meta::uvalue r{'a'};

            CHECK(l.has_less_op());
            CHECK(r.has_less_op());
            CHECK_FALSE(l.less(l));
            CHECK_FALSE(r.less(r));
            CHECK(l.less(r) == (l.get_type() < r.get_type()));
            CHECK(r.less(l) == (r.get_type() < l.get_type()));

            CHECK(l.has_equals_op());
            CHECK(r.has_equals_op());
            CHECK(l.equals(l));
            CHECK(r.equals(r));
            CHECK_FALSE(l.equals(r));
            CHECK_FALSE(r.equals(l));
        }
        {
            meta::uvalue l{42};
            meta::uvalue r{ivec2{1,2}};

            CHECK(l.has_less_op());
            CHECK_FALSE(r.has_less_op());
            CHECK_FALSE(l.less(l));
            CHECK_FALSE(r.less(r));
            CHECK(l.less(r) == (l.get_type() < r.get_type()));
            CHECK(r.less(l) == (r.get_type() < l.get_type()));

            CHECK(l.has_equals_op());
            CHECK(r.has_equals_op());
            CHECK(l.equals(l));
            CHECK(r.equals(r));
            CHECK_FALSE(l.equals(r));
            CHECK_FALSE(r.equals(l));
        }
        {
            meta::uvalue l{ivec2{1,2}};
            meta::uvalue r{ivec2{1,2}};

            CHECK_FALSE(l.has_less_op());
            CHECK_FALSE(r.has_less_op());
            CHECK_FALSE(l.less(l));
            CHECK_FALSE(r.less(r));
            CHECK_THROWS(std::ignore = l.less(r));
            CHECK_THROWS(std::ignore = r.less(l));

            CHECK(l.has_equals_op());
            CHECK(r.has_equals_op());
            CHECK(l.equals(l));
            CHECK(r.equals(r));
            CHECK(l.equals(r));
            CHECK(r.equals(l));
        }
        {
            meta::uvalue l{std::vector{1,2}};
            meta::uvalue r{std::vector{1,3}};

            CHECK(l.has_less_op());
            CHECK(r.has_less_op());
            CHECK_FALSE(l.less(l));
            CHECK_FALSE(r.less(r));
            CHECK(l.less(r));
            CHECK_FALSE(r.less(l));

            CHECK(l.has_equals_op());
            CHECK(r.has_equals_op());
            CHECK(l.equals(l));
            CHECK(r.equals(r));
            CHECK_FALSE(l.equals(r));
            CHECK_FALSE(r.equals(l));
        }
        {
            meta::uvalue l{std::vector{ivec2{1,2}}};
            meta::uvalue r{std::vector{ivec2{1,3}}};

            CHECK_FALSE(l.has_less_op());
            CHECK_FALSE(r.has_less_op());
            CHECK_FALSE(l.less(l));
            CHECK_FALSE(r.less(r));
            CHECK_THROWS(std::ignore = l.less(r));
            CHECK_THROWS(std::ignore = r.less(l));

            CHECK(l.has_equals_op());
            CHECK(r.has_equals_op());
            CHECK(l.equals(l));
            CHECK(r.equals(r));
            CHECK_FALSE(l.equals(r));
            CHECK_FALSE(r.equals(l));
        }
        {
            CHECK(meta::uvalue{std::array<int, 1>{42}}.has_less_op());
            CHECK(meta::uvalue{std::tuple<std::string>{""}}.has_less_op());
            CHECK(meta::uvalue{std::vector{std::string_view{""}}}.has_less_op());
            CHECK(meta::uvalue{std::make_shared<int>(42)}.has_less_op());
            CHECK(meta::uvalue{std::make_unique<int>(42)}.has_less_op());

            CHECK_FALSE(meta::uvalue{std::array<ivec2, 1>{ivec2{1,2}}}.has_less_op());
            CHECK_FALSE(meta::uvalue{std::tuple<ivec2>{ivec2{1,2}}}.has_less_op());
            CHECK_FALSE(meta::uvalue{std::vector{ivec2{1,2}}}.has_less_op());
            CHECK(meta::uvalue{std::make_shared<ivec2>(1,2)}.has_less_op());
            CHECK(meta::uvalue{std::make_unique<ivec2>(1,2)}.has_less_op());

            CHECK(meta::uvalue{std::array<int, 1>{42}}.has_equals_op());
            CHECK(meta::uvalue{std::tuple<std::string>{""}}.has_equals_op());
            CHECK(meta::uvalue{std::vector{std::string_view{""}}}.has_equals_op());
            CHECK(meta::uvalue{std::make_shared<int>(42)}.has_equals_op());
            CHECK(meta::uvalue{std::make_unique<int>(42)}.has_equals_op());

            CHECK(meta::uvalue{std::array<ivec2, 1>{ivec2{1,2}}}.has_equals_op());
            CHECK(meta::uvalue{std::tuple<ivec2>{ivec2{1,2}}}.has_equals_op());
            CHECK(meta::uvalue{std::vector{ivec2{1,2}}}.has_equals_op());
            CHECK(meta::uvalue{std::make_shared<ivec2>(1,2)}.has_equals_op());
            CHECK(meta::uvalue{std::make_unique<ivec2>(1,2)}.has_equals_op());

            {
                int v1 = 42;
                CHECK(meta::uvalue{std::ref(v1)}.has_less_op());
                CHECK(meta::uvalue{std::cref(v1)}.has_less_op());
                CHECK(meta::uvalue{std::ref(v1)}.has_equals_op());
                CHECK(meta::uvalue{std::cref(v1)}.has_equals_op());

                ivec2 v2{1,2};
                CHECK_FALSE(meta::uvalue{std::ref(v2)}.has_less_op());
                CHECK_FALSE(meta::uvalue{std::cref(v2)}.has_less_op());
                CHECK(meta::uvalue{std::ref(v2)}.has_equals_op());
                CHECK(meta::uvalue{std::cref(v2)}.has_equals_op());
            }
        }
    }
}

TEST_CASE("meta/meta_utilities/value/arrays") {
    namespace meta = meta_hpp;

    SUBCASE("int") {
        meta::uvalue v{42};
        CHECK(v.get_type() == meta::resolve_type<int>());
        CHECK_FALSE(v.has_index_op());
        CHECK_THROWS(std::ignore = v[0]);
    }

    SUBCASE("void*") {
        int i{42};
        void* p{&i};
        meta::uvalue v{p};
        CHECK(v.get_type() == meta::resolve_type<void*>());
        CHECK_FALSE(v.has_index_op());
        CHECK_THROWS(std::ignore = v[0]);
    }

    SUBCASE("const void*") {
        int i{42};
        const void* p{&i};
        meta::uvalue v{p};
        CHECK(v.get_type() == meta::resolve_type<const void*>());
        CHECK_FALSE(v.has_index_op());
        CHECK_THROWS(std::ignore = v[0]);
    }

    SUBCASE("int[3]") {
        {
            int arr[3]{1,2,3};
            meta::uvalue v{arr};
            CHECK(v.get_type() == meta::resolve_type<int*>());
            CHECK(v.has_index_op());

            CHECK(v[0].as<int>() == 1);
            CHECK(v[1].as<int>() == 2);
            CHECK(v[2].as<int>() == 3);
        }
        {
            int* arr = nullptr;
            meta::uvalue v{arr};
            CHECK(v.get_type() == meta::resolve_type<int*>());
            CHECK(v.has_index_op());
            CHECK_FALSE(v[0]);
        }
    }

    SUBCASE("const int[3]") {
        {
            const int arr[3]{1,2,3};
            meta::uvalue v{arr};
            CHECK(v.get_type() == meta::resolve_type<const int*>());
            CHECK(v.has_index_op());

            CHECK(v[0].as<int>() == 1);
            CHECK(v[1].as<int>() == 2);
            CHECK(v[2].as<int>() == 3);
        }
        {
            const int* arr = nullptr;
            meta::uvalue v{arr};
            CHECK(v.get_type() == meta::resolve_type<const int*>());
            CHECK(v.has_index_op());
            CHECK_FALSE(v[0]);
        }
    }

    SUBCASE("std::array") {
        meta::uvalue v{std::array{1,2,3}};
        CHECK(v.get_type() == meta::resolve_type<std::array<int, 3>>());
        CHECK(v.has_index_op());

        CHECK(v[0].as<int>() == 1);
        CHECK(v[1].as<int>() == 2);
        CHECK(v[2].as<int>() == 3);
        CHECK_FALSE(v[3]);
    }

    SUBCASE("std::string") {
        meta::uvalue v{std::string{"hi!"}};
        CHECK(v.get_type() == meta::resolve_type<std::string>());
        CHECK(v.has_index_op());

        CHECK(v[0].as<char>() == 'h');
        CHECK(v[1].as<char>() == 'i');
        CHECK(v[2].as<char>() == '!');
        CHECK_FALSE(v[3]);
    }

    SUBCASE("std::string_view") {
        meta::uvalue v{std::string_view{"hi!"}};
        CHECK(v.get_type() == meta::resolve_type<std::string_view>());
        CHECK(v.has_index_op());

        CHECK(v[0].as<char>() == 'h');
        CHECK(v[1].as<char>() == 'i');
        CHECK(v[2].as<char>() == '!');
        CHECK_FALSE(v[3]);
    }

    SUBCASE("std::span") {
        std::vector arr{1,2,3};
        meta::uvalue v{std::span{arr}};
        CHECK(v.get_type() == meta::resolve_type<std::span<int>>());
        CHECK(v.has_index_op());

        CHECK(v[0].as<int>() == 1);
        CHECK(v[1].as<int>() == 2);
        CHECK(v[2].as<int>() == 3);
        CHECK_FALSE(v[3]);
    }

    SUBCASE("std::vector") {
        const meta::uvalue v{std::vector{1,2,3}};
        CHECK(v.get_type() == meta::resolve_type<std::vector<int>>());
        CHECK(v.has_index_op());

        CHECK(v[0].as<int>() == 1);
        CHECK(v[1].as<int>() == 2);
        CHECK(v[2].as<int>() == 3);
        CHECK_FALSE(v[3]);
    }

    SUBCASE("ivec2/ivec3") {
        {
            meta::uvalue v{ivec2{1,2}};
            CHECK(v.get_type() == meta::resolve_type<ivec2>());
            CHECK(v.has_index_op());

            CHECK(v[0].as<int>() == 1);
            CHECK(v[1].as<int>() == 2);
        }
        {
            meta::uvalue v{ivec3{1,2,3}};
            CHECK(v.get_type() == meta::resolve_type<ivec3>());
            CHECK_FALSE(v.has_index_op());
            CHECK_THROWS(std::ignore = v[0]);
        }
    }
}

TEST_CASE("meta/meta_utilities/value/functions") {
    namespace meta = meta_hpp;

    SUBCASE("add") {
        {
            meta::uvalue v{&ivec2::add};
            CHECK(v.get_type() == meta::resolve_type<ivec2&(ivec2::*)(const ivec2&)>());
            CHECK(v.as<decltype(&ivec2::add)>() == &ivec2::add);
            CHECK((ivec2{1,2}.*(v.as<decltype(&ivec2::add)>()))(ivec2{3,4}) == ivec2(4,6));
        }
        {
            const meta::uvalue v{&ivec2::add};
            CHECK(v.get_type() == meta::resolve_type<ivec2&(ivec2::*)(const ivec2&)>());
            CHECK(v.as<decltype(&ivec2::add)>() == &ivec2::add);
            CHECK((ivec2{1,2}.*(v.as<decltype(&ivec2::add)>()))(ivec2{3,4}) == ivec2(4,6));
        }
    }

    SUBCASE("iadd2") {
        {
            meta::uvalue v{iadd2};
            CHECK(v.get_type() == meta::resolve_type<ivec2(*)(ivec2, ivec2)>());
            CHECK((v.as<decltype(&iadd2)>() == &iadd2));
            CHECK((v.as<decltype(&iadd2)>())(ivec2{1,2}, ivec2{3,4}) == ivec2{4,6});
        }
        {
            meta::uvalue v{&iadd2};
            CHECK(v.get_type() == meta::resolve_type<ivec2(*)(ivec2, ivec2)>());
            CHECK((v.as<decltype(&iadd2)>() == &iadd2));
            CHECK((v.as<decltype(&iadd2)>())(ivec2{1,2}, ivec2{3,4}) == ivec2{4,6});
        }
        {
            const meta::uvalue v{iadd2};
            CHECK(v.get_type() == meta::resolve_type<ivec2(*)(ivec2, ivec2)>());
            CHECK((v.as<decltype(&iadd2)>() == &iadd2));
            CHECK((v.as<decltype(&iadd2)>())(ivec2{1,2}, ivec2{3,4}) == ivec2{4,6});
        }
        {
            const meta::uvalue v{&iadd2};
            CHECK(v.get_type() == meta::resolve_type<ivec2(*)(ivec2, ivec2)>());
            CHECK((v.as<decltype(&iadd2)>() == &iadd2));
            CHECK((v.as<decltype(&iadd2)>())(ivec2{1,2}, ivec2{3,4}) == ivec2{4,6});
        }
    }
}
