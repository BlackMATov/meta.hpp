/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
    struct ivec2 {
        int x{};
        int y{};

        ivec2() = delete;

        [[maybe_unused]] explicit ivec2(int nv): x{nv}, y{nv} {}
        [[maybe_unused]] ivec2(int nx, int ny): x{nx}, y{ny} {}

        [[maybe_unused]] ivec2(ivec2&& other) noexcept
        : x{other.x}
        , y{other.y} {
            other.x = 0;
            other.y = 0;
            ++move_constructor_counter;
        }

        [[maybe_unused]] ivec2(const ivec2& other) noexcept
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

        [[maybe_unused]] explicit ivec3(int nv): x{nv}, y{nv}, z{nv} {}
        [[maybe_unused]] ivec3(int nx, int ny, int nz): x{nx}, y{ny}, z{nz} {}
    };

    int ivec2::move_constructor_counter{0};
    int ivec2::copy_constructor_counter{0};

    ivec2 iadd2(ivec2 l, ivec2 r) {
        return {l.x + r.x, l.y + r.y};
    }

    [[maybe_unused]] bool operator<(const ivec2& l, const ivec2& r) noexcept {
        return (l.x < r.x) || (l.x == r.x && l.y < r.y);
    }

    [[maybe_unused]] bool operator==(const ivec2& l, const ivec2& r) noexcept {
        return l.x == r.x && l.y == r.y;
    }
}

TEST_CASE("meta/meta_utilities/value/ivec2") {
    namespace meta = meta_hpp;

    meta::class_<ivec2>()
        .constructor_<int>()
        .constructor_<int, int>()
        .constructor_<ivec2&&>()
        .constructor_<const ivec2&>()
        .member_("x", &ivec2::x)
        .member_("y", &ivec2::y);
}

TEST_CASE("meta/meta_utilities/value/ivec3") {
    namespace meta = meta_hpp;

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
            ivec2&>);
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
            CHECK(val.data() == nullptr);
            CHECK(std::as_const(val).data() == nullptr);
            CHECK(std::as_const(val).cdata() == nullptr);

            CHECK_FALSE(*val);
            CHECK_FALSE(val[0]);

            CHECK_FALSE(val.can_get_as<ivec2>());
            CHECK_FALSE(std::as_const(val).can_get_as<ivec2>());

            CHECK_THROWS(std::ignore = val.get_as<int>());
            CHECK_THROWS(std::ignore = std::as_const(val).get_as<int>());
            CHECK_THROWS(std::ignore = std::move(val).get_as<int>());
            CHECK_THROWS(std::ignore = std::move(std::as_const(val)).get_as<int>());
        }

        {
            CHECK_FALSE(meta::uvalue{1} < meta::uvalue{});
            CHECK(meta::uvalue{} < meta::uvalue{1});
            CHECK_FALSE(meta::uvalue{} < meta::uvalue{});

            CHECK_FALSE(1 < meta::uvalue{});
            CHECK(meta::uvalue{} < 1);
            CHECK_FALSE(meta::uvalue{} < meta::uvalue{});
        }

        {
            CHECK_FALSE(meta::uvalue{1} == meta::uvalue{});
            CHECK_FALSE(meta::uvalue{} == meta::uvalue{1});
            CHECK(meta::uvalue{} == meta::uvalue{});

            CHECK_FALSE(1 == meta::uvalue{});
            CHECK_FALSE(meta::uvalue{} == 1);
            CHECK(meta::uvalue{} == meta::uvalue{});
        }

        {
            CHECK(meta::uvalue{1} != meta::uvalue{});
            CHECK(meta::uvalue{} != meta::uvalue{1});
            CHECK_FALSE(meta::uvalue{} != meta::uvalue{});

            CHECK(1 != meta::uvalue{});
            CHECK(meta::uvalue{} != 1);
            CHECK_FALSE(meta::uvalue{} != meta::uvalue{});
        }

        CHECK_FALSE(meta::uvalue{} == 0);
        CHECK_FALSE(meta::uvalue{} == nullptr);
        CHECK(meta::uvalue{}.get_type() == meta::resolve_type<void>());
    }

    SUBCASE("ivec2&") {
        ivec2 v{1,2};
        ivec2& vr = v;

        meta::uvalue val{vr};
        CHECK(ivec2::move_constructor_counter == 0);
        CHECK(ivec2::copy_constructor_counter == 1);

        CHECK(val.get_type() == meta::resolve_type<ivec2>());

        CHECK(*static_cast<const ivec2*>(val.data()) == vr);
        CHECK(*static_cast<const ivec2*>(val.cdata()) == vr);
        CHECK(*static_cast<const ivec2*>(std::as_const(val).data()) == vr);
        CHECK(*static_cast<const ivec2*>(std::as_const(val).cdata()) == vr);

        CHECK(val == ivec2{1,2});
        CHECK(val == meta::uvalue{ivec2{1,2}});

        CHECK(val.get_as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).get_as<ivec2>() == ivec2{1,2});
        CHECK(std::move(val).get_as<ivec2>() == ivec2{1,2});
        CHECK(std::move(std::as_const(val)).get_as<ivec2>() == ivec2{1,2});

        CHECK_THROWS(std::ignore = val.get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::as_const(val).get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(val).get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(std::as_const(val)).get_as<ivec3>());

        CHECK(val.get_as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).get_as<ivec2>() == ivec2{1,2});
        CHECK_FALSE(val.can_get_as<ivec3>());
        CHECK_FALSE(std::as_const(val).can_get_as<ivec3>());
    }

    SUBCASE("const ivec2&") {
        const ivec2 v{1,2};
        const ivec2& vr = v;

        meta::uvalue val{vr};
        CHECK(ivec2::move_constructor_counter == 0);
        CHECK(ivec2::copy_constructor_counter == 1);

        CHECK(val.get_type() == meta::resolve_type<ivec2>());

        CHECK(*static_cast<const ivec2*>(val.data()) == vr);
        CHECK(*static_cast<const ivec2*>(val.cdata()) == vr);
        CHECK(*static_cast<const ivec2*>(std::as_const(val).data()) == vr);
        CHECK(*static_cast<const ivec2*>(std::as_const(val).cdata()) == vr);

        CHECK(val == ivec2{1,2});
        CHECK(val == meta::uvalue{ivec2{1,2}});

        CHECK(val.get_as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).get_as<ivec2>() == ivec2{1,2});
        CHECK(std::move(val).get_as<ivec2>() == ivec2{1,2});
        CHECK(std::move(std::as_const(val)).get_as<ivec2>() == ivec2{1,2});

        CHECK_THROWS(std::ignore = val.get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::as_const(val).get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(val).get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(std::as_const(val)).get_as<ivec3>());

        CHECK(val.get_as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).get_as<ivec2>() == ivec2{1,2});
        CHECK_FALSE(val.can_get_as<ivec3>());
        CHECK_FALSE(std::as_const(val).can_get_as<ivec3>());
    }

    SUBCASE("ivec2&&") {
        ivec2 v{1,2};

        meta::uvalue val{std::move(v)};
        CHECK(ivec2::move_constructor_counter == 1);
        CHECK(ivec2::copy_constructor_counter == 0);

        CHECK(val.get_type() == meta::resolve_type<ivec2>());

        CHECK(val == ivec2{1,2});
        CHECK(val == meta::uvalue{ivec2{1,2}});

        CHECK(val.get_as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).get_as<ivec2>() == ivec2{1,2});
        CHECK(std::move(val).get_as<ivec2>() == ivec2{1,2});
        CHECK(std::move(std::as_const(val)).get_as<ivec2>() == ivec2{1,2});

        CHECK_THROWS(std::ignore = val.get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::as_const(val).get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(val).get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(std::as_const(val)).get_as<ivec3>());

        CHECK(val.get_as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).get_as<ivec2>() == ivec2{1,2});
        CHECK_FALSE(val.can_get_as<ivec3>());
        CHECK_FALSE(std::as_const(val).can_get_as<ivec3>());
    }

    SUBCASE("const ivec2&&") {
        const ivec2 v{1,2};

        meta::uvalue val{std::move(v)};
        CHECK(ivec2::move_constructor_counter == 0);
        CHECK(ivec2::copy_constructor_counter == 1);

        CHECK(val.get_type() == meta::resolve_type<ivec2>());

        CHECK(val == ivec2{1,2});
        CHECK(val == meta::uvalue{ivec2{1,2}});

        CHECK(val.get_as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).get_as<ivec2>() == ivec2{1,2});
        CHECK(std::move(val).get_as<ivec2>() == ivec2{1,2});
        CHECK(std::move(std::as_const(val)).get_as<ivec2>() == ivec2{1,2});

        CHECK_THROWS(std::ignore = val.get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::as_const(val).get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(val).get_as<ivec3>());
        CHECK_THROWS(std::ignore = std::move(std::as_const(val)).get_as<ivec3>());

        CHECK(val.get_as<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).get_as<ivec2>() == ivec2{1,2});
        CHECK_FALSE(val.can_get_as<ivec3>());
        CHECK_FALSE(std::as_const(val).can_get_as<ivec3>());
    }

    SUBCASE("value(value&&)") {
        ivec2 v{1,2};
        meta::uvalue val_src{std::move(v)};
        CHECK(ivec2::move_constructor_counter == 1);
        CHECK(ivec2::copy_constructor_counter == 0);

        meta::uvalue val_dst{std::move(val_src)};
        CHECK(val_dst == ivec2{1,2});
        CHECK(ivec2::move_constructor_counter == 2);
        CHECK(ivec2::copy_constructor_counter == 0);
    }

    SUBCASE("value(const meta::value&)") {
        const ivec2 v{1,2};
        meta::uvalue val_src{v};
        CHECK(ivec2::move_constructor_counter == 0);
        CHECK(ivec2::copy_constructor_counter == 1);

        meta::uvalue val_dst{val_src};
        CHECK(val_dst == ivec2{1,2});
        CHECK(ivec2::move_constructor_counter == 0);
        CHECK(ivec2::copy_constructor_counter == 2);

        CHECK(val_src == ivec2{1,2});
        CHECK(val_src.data() != val_dst.data());
    }

    SUBCASE("value& operator=(T&&)") {
        meta::uvalue val{10};

        val = 20;
        CHECK(val == 20);

        val = "hello"s;
        CHECK(val == "hello"s);
    }

    SUBCASE("value& operator=(value&&)") {
        meta::uvalue val_src1{"world"s};
        meta::uvalue val_src2{ivec2{1,2}};
        CHECK(ivec2::move_constructor_counter == 1);
        CHECK(ivec2::copy_constructor_counter == 0);

        meta::uvalue val_dst{"hello"s};

        val_dst = std::move(val_src1);
        CHECK(val_dst == "world"s);
        CHECK(ivec2::move_constructor_counter == 1);
        CHECK(ivec2::copy_constructor_counter == 0);

        val_dst = std::move(val_src2);
        CHECK(val_dst == ivec2{1,2});
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
        CHECK(val_dst == "world"s);
        CHECK(ivec2::move_constructor_counter == 1);
        CHECK(ivec2::copy_constructor_counter == 0);

        val_dst = val_src2;
        CHECK(val_dst == ivec2{1,2});
        CHECK(ivec2::move_constructor_counter == 2);
        CHECK(ivec2::copy_constructor_counter == 1);

        CHECK(val_src2 == ivec2{1,2});
        CHECK(val_src2.data() != val_dst.data());
    }

    SUBCASE("swap") {
        meta::uvalue val1{"world"s};
        meta::uvalue val2{ivec2{1,2}};
        CHECK(ivec2::move_constructor_counter == 1);
        CHECK(ivec2::copy_constructor_counter == 0);

        val1.swap(val2);
        CHECK(val1 == ivec2{1,2});
        CHECK(val2 == "world"s);
        CHECK(ivec2::move_constructor_counter == 2);
        CHECK(ivec2::copy_constructor_counter == 0);

        swap(val1, val2);
        CHECK(val1 == "world"s);
        CHECK(val2 == ivec2{1,2});
    }

    SUBCASE("ostream") {
        std::stringstream str_stream;
        CHECK_NOTHROW(str_stream << meta::uvalue{21} << " " << meta::uvalue{42});
        CHECK_THROWS((str_stream << meta::uvalue{ivec2{1,2}}));
        REQUIRE(str_stream.str() == "21 42");
    }

    SUBCASE("istream") {
        std::stringstream str_stream{"21 42"};

        meta::uvalue v{ivec2{1,2}};
        CHECK_THROWS(str_stream >> v);

        v = meta::uvalue{0};
        CHECK_NOTHROW(str_stream >> v);
        CHECK(v == 21);
        CHECK_NOTHROW(str_stream >> v);
        CHECK(v == 42);
    }

    SUBCASE("operator<") {
        CHECK(meta::uvalue{ivec2{1,2}} < ivec2{1,3});
        CHECK_FALSE(meta::uvalue{ivec2{1,3}} < ivec2{1,2});

        CHECK(ivec2{1,2} < meta::uvalue{ivec2{1,3}});
        CHECK_FALSE(ivec2{1,3} < meta::uvalue{ivec2{1,2}});

        CHECK(meta::uvalue{ivec2{1,2}} < meta::uvalue{ivec2{1,3}});
        CHECK_FALSE(meta::uvalue{ivec2{1,3}} < meta::uvalue{ivec2{1,2}});

        {
            class empty_class1 {};
            class empty_class2 {};

            CHECK((operator<(meta::uvalue{empty_class1{}}, meta::uvalue{empty_class2{}})
                || operator<(meta::uvalue{empty_class2{}}, meta::uvalue{empty_class1{}})));
            CHECK_THROWS(std::ignore = operator<(meta::uvalue{empty_class1{}}, meta::uvalue{empty_class1{}}));
        }
    }

    SUBCASE("operator==") {
        CHECK(meta::uvalue{ivec2{1,2}} == ivec2{1,2});
        CHECK_FALSE(meta::uvalue{ivec2{1,2}} == ivec2{1,3});

        CHECK(ivec2{1,2} == meta::uvalue{ivec2{1,2}});
        CHECK_FALSE(ivec2{1,3} == meta::uvalue{ivec2{1,2}});

        CHECK(meta::uvalue{ivec2{1,2}} == meta::uvalue{ivec2{1,2}});
        CHECK_FALSE(meta::uvalue{ivec2{1,2}} == meta::uvalue{ivec2{1,3}});

        {
            class empty_class1 {};
            class empty_class2 {};

            CHECK_FALSE(operator==(meta::uvalue{empty_class1{}}, meta::uvalue{empty_class2{}}));
            CHECK_THROWS(std::ignore = operator==(meta::uvalue{empty_class1{}}, meta::uvalue{empty_class1{}}));
        }
    }

    SUBCASE("deref") {
        {
            int i{42};
            const meta::uvalue v{*meta::uvalue{&i}};
            CHECK(v.get_type() == meta::resolve_type<int>());
            CHECK(v.data() != &i);
        }
        {
            const char i{42};
            const meta::uvalue v{*meta::uvalue{&i}};
            CHECK(v.get_type() == meta::resolve_type<char>());
            CHECK(v.data() != &i);
        }
        {
            const int i{42};
            const int* const pi = &i;
            const meta::uvalue v{*meta::uvalue{&pi}};
            CHECK(v.get_type() == meta::resolve_type<const int*>() );
            CHECK(v.get_as<const int*>() == pi);
        }
        {
            int i{42};

            void* p1 = &i;
            const void* p2 = &i;
            void* const& p3 = &i;
            const void* const& p4 = &i;

            CHECK_THROWS(std::ignore = *meta::uvalue(p1));
            CHECK_THROWS(std::ignore = *meta::uvalue(p2));
            CHECK_THROWS(std::ignore = *meta::uvalue(p3));
            CHECK_THROWS(std::ignore = *meta::uvalue(p4));
        }
        {
            ivec2 v{1,2};
            meta::uvalue vp{&v};
            CHECK(ivec2::move_constructor_counter == 0);
            CHECK(ivec2::copy_constructor_counter == 0);

            [[maybe_unused]] meta::uvalue vv1{*vp};
            CHECK((ivec2::move_constructor_counter == 0 || ivec2::move_constructor_counter == 1 || ivec2::move_constructor_counter == 2));
            CHECK(ivec2::copy_constructor_counter == 1);

            [[maybe_unused]] meta::uvalue vv2{*std::move(vp)};
            CHECK((ivec2::move_constructor_counter == 0 || ivec2::move_constructor_counter == 2 || ivec2::move_constructor_counter == 4));
            CHECK(ivec2::copy_constructor_counter == 2);

            [[maybe_unused]] meta::uvalue vv3{*std::as_const(vp)};
            CHECK((ivec2::move_constructor_counter == 0 || ivec2::move_constructor_counter == 3 || ivec2::move_constructor_counter == 6));
            CHECK(ivec2::copy_constructor_counter == 3);
        }
        {
            meta::uvalue v{std::make_shared<int>(42)};
            CHECK(*v == 42);
        }
    }
}

TEST_CASE("meta/meta_utilities/value/arrays") {
    namespace meta = meta_hpp;

    SUBCASE("int[3]") {
        int arr[3]{1,2,3};
        meta::uvalue v{arr};
        CHECK(v.get_type() == meta::resolve_type<int*>());
        CHECK(v[0] == 1);
        CHECK(v[1] == 2);
        CHECK(v[2] == 3);
    }

    SUBCASE("const int[3]") {
        const int arr[3]{1,2,3};
        meta::uvalue v{arr};
        CHECK(v.get_type() == meta::resolve_type<const int*>());
        CHECK(v[0] == 1);
        CHECK(v[1] == 2);
        CHECK(v[2] == 3);
    }

    SUBCASE("std::array") {
        meta::uvalue v{std::array{1,2,3}};
        CHECK(v.get_type() == meta::resolve_type<std::array<int, 3>>());
        CHECK(v[0] == 1);
        CHECK(v[1] == 2);
        CHECK(v[2] == 3);
    }

    SUBCASE("std::string") {
        meta::uvalue v{std::string{"hi!"}};
        CHECK(v.get_type() == meta::resolve_type<std::string>());
        CHECK(v[0] == 'h');
        CHECK(v[1] == 'i');
        CHECK(v[2] == '!');
    }

    SUBCASE("std::span") {
        std::vector arr{1,2,3};
        meta::uvalue v{std::span{arr}};
        CHECK(v.get_type() == meta::resolve_type<std::span<int>>());
        CHECK(v[0] == 1);
        CHECK(v[1] == 2);
        CHECK(v[2] == 3);
    }

    SUBCASE("std::vector") {
        const meta::uvalue v{std::vector{1,2,3}};
        CHECK(v.get_type() == meta::resolve_type<std::vector<int>>());
        CHECK(v[0] == 1);
        CHECK(v[1] == 2);
        CHECK(v[2] == 3);
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
            CHECK(v.get_as<decltype(&iadd2)>() == &iadd2);
            CHECK((v.get_as<decltype(&iadd2)>())(ivec2{1,2}, ivec2{3,4}) == ivec2{4,6});
        }
        {
            meta::uvalue v{&iadd2};
            CHECK(v.get_type() == meta::resolve_type<ivec2(*)(ivec2, ivec2)>());
            CHECK(v.get_as<decltype(&iadd2)>() == &iadd2);
            CHECK((v.get_as<decltype(&iadd2)>())(ivec2{1,2}, ivec2{3,4}) == ivec2{4,6});
        }
        {
            const meta::uvalue v{iadd2};
            CHECK(v.get_type() == meta::resolve_type<ivec2(*)(ivec2, ivec2)>());
            CHECK(v.get_as<decltype(&iadd2)>() == &iadd2);
            CHECK((v.get_as<decltype(&iadd2)>())(ivec2{1,2}, ivec2{3,4}) == ivec2{4,6});
        }
        {
            const meta::uvalue v{&iadd2};
            CHECK(v.get_type() == meta::resolve_type<ivec2(*)(ivec2, ivec2)>());
            CHECK(v.get_as<decltype(&iadd2)>() == &iadd2);
            CHECK((v.get_as<decltype(&iadd2)>())(ivec2{1,2}, ivec2{3,4}) == ivec2{4,6});
        }
    }
}
