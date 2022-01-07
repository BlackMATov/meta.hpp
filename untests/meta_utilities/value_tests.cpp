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

        [[maybe_unused]] ivec2() = default;
        [[maybe_unused]] explicit ivec2(int v): x{v}, y{v} {}
        [[maybe_unused]] ivec2(int x, int y): x{x}, y{y} {}

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

        ivec2& operator=(ivec2&& other) = delete;
        ivec2& operator=(const ivec2& other) = delete;
    public:
        static int move_ctor_counter;
        static int copy_ctor_counter;
    };

    int ivec2::move_ctor_counter{0};
    int ivec2::copy_ctor_counter{0};

    [[maybe_unused]] bool operator<(const ivec2& l, const ivec2& r) noexcept {
        return (l.x < r.x) || (l.x == r.x && l.y < r.y);
    }

    [[maybe_unused]] bool operator==(const ivec2& l, const ivec2& r) noexcept {
        return l.x == r.x && l.y == r.y;
    }
}

TEST_CASE("meta/meta_utilities/value") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    ivec2::move_ctor_counter = 0;
    ivec2::copy_ctor_counter = 0;

    SUBCASE("cast types") {
        static_assert(std::is_same_v<
            decltype(std::declval<meta::value&>().cast<ivec2>()),
            ivec2&>);
        static_assert(std::is_same_v<
            decltype(std::declval<meta::value&&>().cast<ivec2>()),
            ivec2&&>);
        static_assert(std::is_same_v<
            decltype(std::declval<const meta::value&>().cast<ivec2>()),
            const ivec2&>);
        static_assert(std::is_same_v<
            decltype(std::declval<const meta::value&&>().cast<ivec2>()),
            const ivec2&&>);
    }

    SUBCASE("try_cast types") {
        static_assert(std::is_same_v<
            decltype(std::declval<meta::value>().try_cast<ivec2>()),
            ivec2*>);
        static_assert(std::is_same_v<
            decltype(std::declval<const meta::value>().try_cast<ivec2>()),
            const ivec2*>);
    }

    SUBCASE("ivec2&") {
        ivec2 v{1,2};
        ivec2& vr = v;

        meta::value val{vr};
        CHECK(ivec2::move_ctor_counter == 0);
        CHECK(ivec2::copy_ctor_counter == 1);

        CHECK(val.get_type() == meta::resolve_type<ivec2>());

        CHECK(!std::memcmp(val.data(), &vr, sizeof(ivec2)));
        CHECK(!std::memcmp(val.cdata(), &vr, sizeof(ivec2)));
        CHECK(!std::memcmp(std::as_const(val).data(), &vr, sizeof(ivec2)));
        CHECK(!std::memcmp(std::as_const(val).cdata(), &vr, sizeof(ivec2)));

        CHECK(val == ivec2{1,2});
        CHECK(val == meta::value{ivec2{1,2}});

        CHECK(val.cast<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).cast<ivec2>() == ivec2{1,2});

        CHECK(*val.try_cast<ivec2>() == ivec2{1,2});
        CHECK(*std::as_const(val).try_cast<ivec2>() == ivec2{1,2});
    }

    SUBCASE("const ivec2&") {
        const ivec2 v{1,2};
        const ivec2& vr = v;

        meta::value val{vr};
        CHECK(ivec2::move_ctor_counter == 0);
        CHECK(ivec2::copy_ctor_counter == 1);

        CHECK(val.get_type() == meta::resolve_type<ivec2>());

        CHECK(!std::memcmp(val.data(), &vr, sizeof(ivec2)));
        CHECK(!std::memcmp(val.cdata(), &vr, sizeof(ivec2)));
        CHECK(!std::memcmp(std::as_const(val).data(), &vr, sizeof(ivec2)));
        CHECK(!std::memcmp(std::as_const(val).cdata(), &vr, sizeof(ivec2)));

        CHECK(val == ivec2{1,2});
        CHECK(val == meta::value{ivec2{1,2}});

        CHECK(val.cast<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).cast<ivec2>() == ivec2{1,2});

        CHECK(*val.try_cast<ivec2>() == ivec2{1,2});
        CHECK(*std::as_const(val).try_cast<ivec2>() == ivec2{1,2});
    }

    SUBCASE("ivec2&&") {
        ivec2 v{1,2};

        meta::value val{std::move(v)};
        CHECK(ivec2::move_ctor_counter == 1);
        CHECK(ivec2::copy_ctor_counter == 0);

        CHECK(val.get_type() == meta::resolve_type<ivec2>());

        CHECK(val == ivec2{1,2});
        CHECK(val == meta::value{ivec2{1,2}});

        CHECK(val.cast<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).cast<ivec2>() == ivec2{1,2});

        CHECK(*val.try_cast<ivec2>() == ivec2{1,2});
        CHECK(*std::as_const(val).try_cast<ivec2>() == ivec2{1,2});
    }

    SUBCASE("const ivec2&&") {
        const ivec2 v{1,2};

        meta::value val{std::move(v)};
        CHECK(ivec2::move_ctor_counter == 0);
        CHECK(ivec2::copy_ctor_counter == 1);

        CHECK(val.get_type() == meta::resolve_type<ivec2>());

        CHECK(val == ivec2{1,2});
        CHECK(val == meta::value{ivec2{1,2}});

        CHECK(val.cast<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).cast<ivec2>() == ivec2{1,2});

        CHECK(*val.try_cast<ivec2>() == ivec2{1,2});
        CHECK(*std::as_const(val).try_cast<ivec2>() == ivec2{1,2});
    }

    SUBCASE("value(value&&)") {
        ivec2 v{1,2};
        meta::value val_src{std::move(v)};
        CHECK(ivec2::move_ctor_counter == 1);
        CHECK(ivec2::copy_ctor_counter == 0);

        meta::value val_dst{std::move(val_src)};
        CHECK(val_dst == ivec2{1,2});
        CHECK(ivec2::move_ctor_counter == 2);
        CHECK(ivec2::copy_ctor_counter == 0);

        CHECK(val_src == ivec2{0,0});
        CHECK(val_src.data() != val_dst.data());
    }

    SUBCASE("value(const meta::value&)") {
        const ivec2 v{1,2};
        meta::value val_src{v};
        CHECK(ivec2::move_ctor_counter == 0);
        CHECK(ivec2::copy_ctor_counter == 1);

        meta::value val_dst{val_src};
        CHECK(val_dst == ivec2{1,2});
        CHECK(ivec2::move_ctor_counter == 0);
        CHECK(ivec2::copy_ctor_counter == 2);

        CHECK(val_src == ivec2{1,2});
        CHECK(val_src.data() != val_dst.data());
    }

    SUBCASE("value& operator=(T&&)") {
        meta::value val{10};

        val = 20;
        CHECK(val == 20);

        val = "hello"s;
        CHECK(val == "hello"s);
    }

    SUBCASE("value& operator=(value&&)") {
        meta::value val_src1{"world"s};
        meta::value val_src2{ivec2{1,2}};
        CHECK(ivec2::move_ctor_counter == 1);
        CHECK(ivec2::copy_ctor_counter == 0);

        meta::value val_dst{"hello"s};

        val_dst = std::move(val_src1);
        CHECK(val_dst == "world"s);
        CHECK(ivec2::move_ctor_counter == 1);
        CHECK(ivec2::copy_ctor_counter == 0);

        val_dst = std::move(val_src2);
        CHECK(val_dst == ivec2{1,2});
        CHECK(ivec2::move_ctor_counter == 2);
        CHECK(ivec2::copy_ctor_counter == 0);

        CHECK(val_src2 == ivec2{0,0});
        CHECK(val_src2.data() != val_dst.data());
    }

    SUBCASE("value& operator=(const meta::value&)") {
        meta::value val_src1{"world"s};
        meta::value val_src2{ivec2{1,2}};
        CHECK(ivec2::move_ctor_counter == 1);
        CHECK(ivec2::copy_ctor_counter == 0);

        meta::value val_dst{"hello"s};

        val_dst = val_src1;
        CHECK(val_dst == "world"s);
        CHECK(ivec2::move_ctor_counter == 1);
        CHECK(ivec2::copy_ctor_counter == 0);

        val_dst = val_src2;
        CHECK(val_dst == ivec2{1,2});
        CHECK(ivec2::move_ctor_counter == 1);
        CHECK(ivec2::copy_ctor_counter == 1);

        CHECK(val_src2 == ivec2{1,2});
        CHECK(val_src2.data() != val_dst.data());
    }

    SUBCASE("swap") {
        meta::value val1{"world"s};
        meta::value val2{ivec2{1,2}};
        CHECK(ivec2::move_ctor_counter == 1);
        CHECK(ivec2::copy_ctor_counter == 0);

        val1.swap(val2);
        CHECK(val1 == ivec2{1,2});
        CHECK(val2 == "world"s);
        CHECK((ivec2::move_ctor_counter == 2 || ivec2::move_ctor_counter == 3));
        CHECK(ivec2::copy_ctor_counter == 0);

        swap(val1, val2);
        CHECK(val1 == "world"s);
        CHECK(val2 == ivec2{1,2});
    }

    SUBCASE("ostream") {
        std::stringstream str_stream;
        CHECK_NOTHROW(str_stream << meta::value{21} << " " << meta::value{42});
        CHECK_THROWS((str_stream << meta::value{ivec2{1,2}}));
        REQUIRE(str_stream.str() == "21 42");
    }

    SUBCASE("istream") {
        std::stringstream str_stream{"21 42"};

        meta::value v{ivec2{1,2}};
        CHECK_THROWS(str_stream >> v);

        v = meta::value{0};
        CHECK_NOTHROW(str_stream >> v);
        CHECK(v == 21);
        CHECK_NOTHROW(str_stream >> v);
        CHECK(v == 42);
    }

    SUBCASE("operator<") {
        CHECK(meta::value{ivec2{1,2}} < ivec2{1,3});
        CHECK_FALSE(meta::value{ivec2{1,3}} < ivec2{1,2});

        CHECK(ivec2{1,2} < meta::value{ivec2{1,3}});
        CHECK_FALSE(ivec2{1,3} < meta::value{ivec2{1,2}});

        CHECK(meta::value{ivec2{1,2}} < meta::value{ivec2{1,3}});
        CHECK_FALSE(meta::value{ivec2{1,3}} < meta::value{ivec2{1,2}});

        {
            class empty_class1 {};
            class empty_class2 {};

            CHECK((operator<(meta::value{empty_class1{}}, meta::value{empty_class2{}}) || operator<(meta::value{empty_class2{}}, meta::value{empty_class1{}})));
            CHECK_THROWS(std::ignore = operator<(meta::value{empty_class1{}}, meta::value{empty_class1{}}));
        }
    }

    SUBCASE("operator==") {
        CHECK(meta::value{ivec2{1,2}} == ivec2{1,2});
        CHECK_FALSE(meta::value{ivec2{1,2}} == ivec2{1,3});

        CHECK(ivec2{1,2} == meta::value{ivec2{1,2}});
        CHECK_FALSE(ivec2{1,3} == meta::value{ivec2{1,2}});

        CHECK(meta::value{ivec2{1,2}} == meta::value{ivec2{1,2}});
        CHECK_FALSE(meta::value{ivec2{1,2}} == meta::value{ivec2{1,3}});

        {
            class empty_class1 {};
            class empty_class2 {};

            CHECK_FALSE(operator==(meta::value{empty_class1{}}, meta::value{empty_class2{}}));
            CHECK_THROWS(std::ignore = operator==(meta::value{empty_class1{}}, meta::value{empty_class1{}}));
        }
    }
}
