/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

#include <sstream>

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

        ivec2& operator=(ivec2&& other) = delete;
        ivec2& operator=(const ivec2& other) = delete;
    public:
        static int move_constructor_counter;
        static int copy_constructor_counter;
    };

    struct ivec3 {
        int x{};
        int y{};
        int z{};

        ivec3() = delete;

        explicit ivec3(int nv): x{nv}, y{nv}, z{nv} {}
        ivec3(int nx, int ny, int nz): x{nx}, y{ny}, z{nz} {}
    };

    int ivec2::move_constructor_counter{0};
    int ivec2::copy_constructor_counter{0};

    ivec2 iadd2(ivec2 l, ivec2 r) {
        return {l.x + r.x, l.y + r.y};
    }

    bool operator==(const ivec2& l, const ivec2& r) noexcept {
        return l.x == r.x && l.y == r.y;
    }
}

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

    SUBCASE("cast types") {
        static_assert(std::is_same_v<
            decltype(std::declval<meta::uvalue&>().get_as<ivec2>()),
            ivec2&>);
        static_assert(std::is_same_v<
            decltype(std::declval<meta::uvalue&&>().get_as<ivec2>()),
            ivec2>);
        static_assert(std::is_same_v<
            decltype(std::declval<const meta::uvalue&>().get_as<ivec2>()),
            const ivec2&>);
        static_assert(std::is_same_v<
            decltype(std::declval<const meta::uvalue&&>().get_as<ivec2>()),
            const ivec2&>);
    }

    SUBCASE("ivec2{}") {
        {
            meta::uvalue val{};

            CHECK(!val);
            CHECK_FALSE(val);

            CHECK_FALSE(val.is_valid());
            CHECK(val.get_data() == nullptr);
            CHECK(std::as_const(val).get_data() == nullptr);
            CHECK(std::as_const(val).get_cdata() == nullptr);

            CHECK_FALSE(*val);
            CHECK_FALSE(val[0]);

            CHECK_FALSE(val.try_get_as<ivec2>());
            CHECK_FALSE(std::as_const(val).try_get_as<ivec2>());

            CHECK_THROWS(std::ignore = val.get_as<int>());
            CHECK_THROWS(std::ignore = std::as_const(val).get_as<int>());
            CHECK_THROWS(std::ignore = std::move(val).get_as<int>());
            CHECK_THROWS(std::ignore = std::move(std::as_const(val)).get_as<int>());
        }

        CHECK(meta::uvalue{}.get_type() == meta::resolve_type<void>());
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

        CHECK(val.get_as<ivec2>() == ivec2{1,2});

        CHECK(val.get_as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).get_as<ivec2>() == ivec2{1,2});

        {
            meta::uvalue val_copy{val};
            CHECK(std::move(val_copy).get_as<ivec2>() == ivec2{1,2});
        }
        {
            meta::uvalue val_copy{val};
            CHECK(std::move(std::as_const(val_copy)).get_as<ivec2>() == ivec2{1,2});
        }

        CHECK_THROWS(std::ignore = val.get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::as_const(val).get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(val).get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(std::as_const(val)).get_as<ivec3>());

        CHECK(val.get_as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).get_as<ivec2>() == ivec2{1,2});
        CHECK_FALSE(val.try_get_as<ivec3>());
        CHECK_FALSE(std::as_const(val).try_get_as<ivec3>());
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

        CHECK(val.get_as<ivec2>() == ivec2{1,2});

        CHECK(val.get_as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).get_as<ivec2>() == ivec2{1,2});

        {
            meta::uvalue val_copy{val};
            CHECK(std::move(val_copy).get_as<ivec2>() == ivec2{1,2});
        }
        {
            meta::uvalue val_copy{val};
            CHECK(std::move(std::as_const(val_copy)).get_as<ivec2>() == ivec2{1,2});
        }

        CHECK_THROWS(std::ignore = val.get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::as_const(val).get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(val).get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(std::as_const(val)).get_as<ivec3>());

        CHECK(val.get_as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).get_as<ivec2>() == ivec2{1,2});
        CHECK_FALSE(val.try_get_as<ivec3>());
        CHECK_FALSE(std::as_const(val).try_get_as<ivec3>());
    }

    SUBCASE("ivec2&&") {
        ivec2 v{1,2};

        meta::uvalue val{std::move(v)};
        CHECK(ivec2::move_constructor_counter == 1);
        CHECK(ivec2::copy_constructor_counter == 0);

        CHECK(val.get_type() == meta::resolve_type<ivec2>());

        CHECK(val.get_as<ivec2>() == ivec2{1,2});

        CHECK(val.get_as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).get_as<ivec2>() == ivec2{1,2});

        {
            meta::uvalue val_copy{val};
            CHECK(std::move(val_copy).get_as<ivec2>() == ivec2{1,2});
        }
        {
            meta::uvalue val_copy{val};
            CHECK(std::move(std::as_const(val_copy)).get_as<ivec2>() == ivec2{1,2});
        }

        CHECK_THROWS(std::ignore = val.get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::as_const(val).get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(val).get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(std::as_const(val)).get_as<ivec3>());

        CHECK(val.get_as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).get_as<ivec2>() == ivec2{1,2});
        CHECK_FALSE(val.try_get_as<ivec3>());
        CHECK_FALSE(std::as_const(val).try_get_as<ivec3>());
    }

    SUBCASE("const ivec2&&") {
        const ivec2 v{1,2};

        meta::uvalue val{std::move(v)};
        CHECK(ivec2::move_constructor_counter == 0);
        CHECK(ivec2::copy_constructor_counter == 1);

        CHECK(val.get_type() == meta::resolve_type<ivec2>());

        CHECK(val.get_as<ivec2>() == ivec2{1,2});

        CHECK(val.get_as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).get_as<ivec2>() == ivec2{1,2});

        {
            meta::uvalue val_copy{val};
            CHECK(std::move(val_copy).get_as<ivec2>() == ivec2{1,2});
        }
        {
            meta::uvalue val_copy{val};
            CHECK(std::move(std::as_const(val_copy)).get_as<ivec2>() == ivec2{1,2});
        }

        CHECK_THROWS(std::ignore = val.get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::as_const(val).get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(val).get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(std::as_const(val)).get_as<ivec3>());

        CHECK(val.get_as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).get_as<ivec2>() == ivec2{1,2});
        CHECK_FALSE(val.try_get_as<ivec3>());
        CHECK_FALSE(std::as_const(val).try_get_as<ivec3>());
    }

    SUBCASE("value(value&&)") {
        ivec2 v{1,2};
        meta::uvalue val_src{std::move(v)};
        CHECK(ivec2::move_constructor_counter == 1);
        CHECK(ivec2::copy_constructor_counter == 0);

        meta::uvalue val_dst{std::move(val_src)};
        CHECK(val_dst.get_as<ivec2>() == ivec2{1,2});
        CHECK(ivec2::move_constructor_counter == 2);
        CHECK(ivec2::copy_constructor_counter == 0);
    }

    SUBCASE("value(const meta::value&)") {
        const ivec2 v{1,2};
        meta::uvalue val_src{v};
        CHECK(ivec2::move_constructor_counter == 0);
        CHECK(ivec2::copy_constructor_counter == 1);

        meta::uvalue val_dst{val_src};
        CHECK(val_dst.get_as<ivec2>() == ivec2{1,2});
        CHECK(ivec2::move_constructor_counter == 0);
        CHECK(ivec2::copy_constructor_counter == 2);

        CHECK(val_src.get_as<ivec2>() == ivec2{1,2});
        CHECK(val_src.get_data() != val_dst.get_data());
    }

    SUBCASE("value& operator=(T&&)") {
        meta::uvalue val{10};

        val = 20;
        CHECK(val.get_as<int>() == 20);

        val = "hello"s;
        CHECK(val.get_as<std::string>() == "hello"s);
    }

    SUBCASE("value& operator=(value&&)") {
        meta::uvalue val_src1{"world"s};
        meta::uvalue val_src2{ivec2{1,2}};
        CHECK(ivec2::move_constructor_counter == 1);
        CHECK(ivec2::copy_constructor_counter == 0);

        meta::uvalue val_dst{"hello"s};

        val_dst = std::move(val_src1);
        CHECK(val_dst.get_as<std::string>() == "world"s);
        CHECK(ivec2::move_constructor_counter == 1);
        CHECK(ivec2::copy_constructor_counter == 0);

        val_dst = std::move(val_src2);
        CHECK(val_dst.get_as<ivec2>() == ivec2{1,2});
        CHECK(ivec2::move_constructor_counter == 3);
        CHECK(ivec2::copy_constructor_counter == 0);
    }

    SUBCASE("value& operator=(const meta::value&)") {
        meta::uvalue val_src1{"world"s};
        meta::uvalue val_src2{ivec2{1,2}};
        CHECK(ivec2::move_constructor_counter == 1);
        CHECK(ivec2::copy_constructor_counter == 0);

        meta::uvalue val_dst{"hello"s};

        val_dst = val_src1;
        CHECK(val_dst.get_as<std::string>() == "world"s);
        CHECK(ivec2::move_constructor_counter == 1);
        CHECK(ivec2::copy_constructor_counter == 0);

        val_dst = val_src2;
        CHECK(val_dst.get_as<ivec2>() == ivec2{1,2});
        CHECK(ivec2::move_constructor_counter == 2);
        CHECK(ivec2::copy_constructor_counter == 1);

        CHECK(val_src2.get_as<ivec2>() == ivec2{1,2});
        CHECK(val_src2.get_data() != val_dst.get_data());
    }

    SUBCASE("swap") {
        meta::uvalue val1{"world"s};
        meta::uvalue val2{ivec2{1,2}};
        CHECK(ivec2::move_constructor_counter == 1);
        CHECK(ivec2::copy_constructor_counter == 0);

        val1.swap(val2);
        CHECK(val1.get_as<ivec2>() == ivec2{1,2});
        CHECK(val2.get_as<std::string>() == "world"s);
        CHECK(ivec2::move_constructor_counter == 2);
        CHECK(ivec2::copy_constructor_counter == 0);

        swap(val1, val2);
        CHECK(val1.get_as<std::string>() == "world"s);
        CHECK(val2.get_as<ivec2>() == ivec2{1,2});
    }

    SUBCASE("unmap") {
        {
            const meta::uvalue u{42};
            CHECK_FALSE(u.has_unmap_op());
            CHECK_FALSE(u.unmap());
        }
        {
            int i{42};
            const meta::uvalue u{std::ref(i)};
            CHECK(u.has_unmap_op());

            const meta::uvalue v{u.unmap()};
            CHECK(v.get_type() == meta::resolve_type<int*>());
            CHECK(v.get_as<int*>() == &i);
        }
        {
            const int i{42};
            const meta::uvalue u{std::ref(i)};
            CHECK(u.has_unmap_op());

            const meta::uvalue v{u.unmap()};
            CHECK(v.get_type() == meta::resolve_type<const int*>());
            CHECK(v.get_as<const int*>() == &i);
        }
        {
            const auto i = std::make_shared<ivec2>(3, 4);
            const meta::uvalue u{i};
            CHECK(u.has_unmap_op());

            const meta::uvalue v{u.unmap()};
            CHECK(v.get_type() == meta::resolve_type<ivec2*>());
            CHECK(v.get_as<ivec2*>() == i.get());
        }
        {
            const auto i = std::make_shared<const ivec2>(3, 4);
            const meta::uvalue u{i};
            CHECK(u.has_unmap_op());

            const meta::uvalue v{u.unmap()};
            CHECK(v.get_type() == meta::resolve_type<const ivec2*>());
            CHECK(v.get_as<const ivec2*>() == i.get());
        }
    }

    SUBCASE("deref") {
        {
            int i{42};
            meta::uvalue u{&i};
            CHECK(u.has_deref_op());

            const meta::uvalue v{*u};
            CHECK(v.get_as<int>() == i);
        }
        {
            const char i{42};
            meta::uvalue u{&i};
            CHECK(u.has_deref_op());

            const meta::uvalue v{*u};
            CHECK(v.get_as<char>() == i);
        }
        {
            const int i{42};
            const int* const pi = &i;
            meta::uvalue u{&pi};
            CHECK(u.has_deref_op());

            const meta::uvalue v{*u};
            CHECK(v.get_as<const int*>() == pi);
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

            CHECK_FALSE(*meta::uvalue(p1));
            CHECK_FALSE(*meta::uvalue(p2));
            CHECK_FALSE(*meta::uvalue(p3));
            CHECK_FALSE(*meta::uvalue(p4));
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
            CHECK((ivec2::move_constructor_counter == 0 || ivec2::move_constructor_counter == 1 || ivec2::move_constructor_counter == 2));
            CHECK(ivec2::copy_constructor_counter == 1);

            meta::uvalue vv2{*std::move(vp)};
            CHECK((ivec2::move_constructor_counter == 0 || ivec2::move_constructor_counter == 2 || ivec2::move_constructor_counter == 4));
            CHECK(ivec2::copy_constructor_counter == 2);

            meta::uvalue vv3{*std::as_const(vp)};
            CHECK((ivec2::move_constructor_counter == 0 || ivec2::move_constructor_counter == 3 || ivec2::move_constructor_counter == 6));
            CHECK(ivec2::copy_constructor_counter == 3);
        }
        {
            meta::uvalue v{std::make_shared<int>(42)};
            CHECK((*v).get_as<int>() == 42);
        }
    }
}

TEST_CASE("meta/meta_utilities/value/arrays") {
    namespace meta = meta_hpp;

    SUBCASE("int") {
        meta::uvalue v{42};
        CHECK(v.get_type() == meta::resolve_type<int>());
        CHECK_FALSE(v.has_index_op());
        CHECK_FALSE(v[0]);
    }

    SUBCASE("void*") {
        int i{42};
        void* p{&i};
        meta::uvalue v{p};
        CHECK(v.get_type() == meta::resolve_type<void*>());
        CHECK_FALSE(v.has_index_op());
        CHECK_FALSE(v[0]);
    }

    SUBCASE("const void*") {
        int i{42};
        const void* p{&i};
        meta::uvalue v{p};
        CHECK(v.get_type() == meta::resolve_type<const void*>());
        CHECK_FALSE(v.has_index_op());
        CHECK_FALSE(v[0]);
    }

    SUBCASE("int[3]") {
        {
            int arr[3]{1,2,3};
            meta::uvalue v{arr};
            CHECK(v.get_type() == meta::resolve_type<int*>());
            CHECK(v.has_index_op());

            CHECK(v[0].get_as<int>() == 1);
            CHECK(v[1].get_as<int>() == 2);
            CHECK(v[2].get_as<int>() == 3);
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

            CHECK(v[0].get_as<int>() == 1);
            CHECK(v[1].get_as<int>() == 2);
            CHECK(v[2].get_as<int>() == 3);
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

        CHECK(v[0].get_as<int>() == 1);
        CHECK(v[1].get_as<int>() == 2);
        CHECK(v[2].get_as<int>() == 3);
        CHECK_FALSE(v[3]);
    }

    SUBCASE("std::deque") {
        const meta::uvalue v{std::deque{1,2,3}};
        CHECK(v.get_type() == meta::resolve_type<std::deque<int>>());
        CHECK(v.has_index_op());

        CHECK(v[0].get_as<int>() == 1);
        CHECK(v[1].get_as<int>() == 2);
        CHECK(v[2].get_as<int>() == 3);
        CHECK_FALSE(v[3]);
    }

    SUBCASE("std::string") {
        meta::uvalue v{std::string{"hi!"}};
        CHECK(v.get_type() == meta::resolve_type<std::string>());
        CHECK(v.has_index_op());

        CHECK(v[0].get_as<char>() == 'h');
        CHECK(v[1].get_as<char>() == 'i');
        CHECK(v[2].get_as<char>() == '!');
        CHECK_FALSE(v[3]);
    }

    SUBCASE("std::span") {
        std::vector arr{1,2,3};
        meta::uvalue v{std::span{arr}};
        CHECK(v.get_type() == meta::resolve_type<std::span<int>>());
        CHECK(v.has_index_op());

        CHECK(v[0].get_as<int>() == 1);
        CHECK(v[1].get_as<int>() == 2);
        CHECK(v[2].get_as<int>() == 3);
        CHECK_FALSE(v[3]);
    }

    SUBCASE("std::vector") {
        const meta::uvalue v{std::vector{1,2,3}};
        CHECK(v.get_type() == meta::resolve_type<std::vector<int>>());
        CHECK(v.has_index_op());

        CHECK(v[0].get_as<int>() == 1);
        CHECK(v[1].get_as<int>() == 2);
        CHECK(v[2].get_as<int>() == 3);
        CHECK_FALSE(v[3]);
    }
}

TEST_CASE("meta/meta_utilities/value/functions") {
    namespace meta = meta_hpp;

    SUBCASE("add") {
        {
            meta::uvalue v{&ivec2::add};
            CHECK(v.get_type() == meta::resolve_type<ivec2&(ivec2::*)(const ivec2&)>());
            CHECK(v.get_as<decltype(&ivec2::add)>() == &ivec2::add);
            CHECK((ivec2{1,2}.*(v.get_as<decltype(&ivec2::add)>()))(ivec2{3,4}) == ivec2(4,6));
        }
        {
            const meta::uvalue v{&ivec2::add};
            CHECK(v.get_type() == meta::resolve_type<ivec2&(ivec2::*)(const ivec2&)>());
            CHECK(v.get_as<decltype(&ivec2::add)>() == &ivec2::add);
            CHECK((ivec2{1,2}.*(v.get_as<decltype(&ivec2::add)>()))(ivec2{3,4}) == ivec2(4,6));
        }
    }

    SUBCASE("iadd2") {
        {
            meta::uvalue v{iadd2};
            CHECK(v.get_type() == meta::resolve_type<ivec2(*)(ivec2, ivec2)>());
            CHECK((v.get_as<decltype(&iadd2)>() == &iadd2));
            CHECK((v.get_as<decltype(&iadd2)>())(ivec2{1,2}, ivec2{3,4}) == ivec2{4,6});
        }
        {
            meta::uvalue v{&iadd2};
            CHECK(v.get_type() == meta::resolve_type<ivec2(*)(ivec2, ivec2)>());
            CHECK((v.get_as<decltype(&iadd2)>() == &iadd2));
            CHECK((v.get_as<decltype(&iadd2)>())(ivec2{1,2}, ivec2{3,4}) == ivec2{4,6});
        }
        {
            const meta::uvalue v{iadd2};
            CHECK(v.get_type() == meta::resolve_type<ivec2(*)(ivec2, ivec2)>());
            CHECK((v.get_as<decltype(&iadd2)>() == &iadd2));
            CHECK((v.get_as<decltype(&iadd2)>())(ivec2{1,2}, ivec2{3,4}) == ivec2{4,6});
        }
        {
            const meta::uvalue v{&iadd2};
            CHECK(v.get_type() == meta::resolve_type<ivec2(*)(ivec2, ivec2)>());
            CHECK((v.get_as<decltype(&iadd2)>() == &iadd2));
            CHECK((v.get_as<decltype(&iadd2)>())(ivec2{1,2}, ivec2{3,4}) == ivec2{4,6});
        }
    }
}
