/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "_utilities_fwd.hpp"

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

    bool operator==(const ivec2& l, const ivec2& r) noexcept {
        return l.x == r.x && l.y == r.y;
    }
}

TEST_CASE("features/utilities/value") {
    ivec2::move_ctor_counter = 0;
    ivec2::copy_ctor_counter = 0;

    SUBCASE("cast types") {
        static_assert(std::is_same_v<
            decltype(std::declval<value&>().cast<ivec2>()),
            ivec2&>);
        static_assert(std::is_same_v<
            decltype(std::declval<value&&>().cast<ivec2>()),
            ivec2&&>);
        static_assert(std::is_same_v<
            decltype(std::declval<const value&>().cast<ivec2>()),
            const ivec2&>);
        static_assert(std::is_same_v<
            decltype(std::declval<const value&&>().cast<ivec2>()),
            const ivec2&&>);
    }

    SUBCASE("try_cast types") {
        static_assert(std::is_same_v<
            decltype(std::declval<value>().try_cast<ivec2>()),
            ivec2*>);
        static_assert(std::is_same_v<
            decltype(std::declval<const value>().try_cast<ivec2>()),
            const ivec2*>);
    }

    SUBCASE("ivec2&") {
        ivec2 v{1,2};
        ivec2& vr = v;

        value val{vr};
        CHECK(ivec2::move_ctor_counter == 0);
        CHECK(ivec2::copy_ctor_counter == 1);

        CHECK(val.type() == type_db::get<ivec2>());

        CHECK(!std::memcmp(val.data(), &vr, sizeof(ivec2)));
        CHECK(!std::memcmp(val.cdata(), &vr, sizeof(ivec2)));
        CHECK(!std::memcmp(std::as_const(val).data(), &vr, sizeof(ivec2)));
        CHECK(!std::memcmp(std::as_const(val).cdata(), &vr, sizeof(ivec2)));

        CHECK(val.equals(ivec2{1,2}));
        CHECK(val.equals(value{ivec2{1,2}}));

        CHECK(val.cast<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).cast<ivec2>() == ivec2{1,2});

        CHECK(*val.try_cast<ivec2>() == ivec2{1,2});
        CHECK(*std::as_const(val).try_cast<ivec2>() == ivec2{1,2});
    }

    SUBCASE("const ivec2&") {
        const ivec2 v{1,2};
        const ivec2& vr = v;

        value val{vr};
        CHECK(ivec2::move_ctor_counter == 0);
        CHECK(ivec2::copy_ctor_counter == 1);

        CHECK(val.type() == type_db::get<ivec2>());

        CHECK(!std::memcmp(val.data(), &vr, sizeof(ivec2)));
        CHECK(!std::memcmp(val.cdata(), &vr, sizeof(ivec2)));
        CHECK(!std::memcmp(std::as_const(val).data(), &vr, sizeof(ivec2)));
        CHECK(!std::memcmp(std::as_const(val).cdata(), &vr, sizeof(ivec2)));

        CHECK(val.equals(ivec2{1,2}));
        CHECK(val.equals(value{ivec2{1,2}}));

        CHECK(val.cast<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).cast<ivec2>() == ivec2{1,2});

        CHECK(*val.try_cast<ivec2>() == ivec2{1,2});
        CHECK(*std::as_const(val).try_cast<ivec2>() == ivec2{1,2});
    }

    SUBCASE("ivec2&&") {
        ivec2 v{1,2};

        value val{std::move(v)};
        CHECK(ivec2::move_ctor_counter == 1);
        CHECK(ivec2::copy_ctor_counter == 0);

        CHECK(val.type() == type_db::get<ivec2>());

        CHECK(val.equals(ivec2{1,2}));
        CHECK(val.equals(value{ivec2{1,2}}));

        CHECK(val.cast<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).cast<ivec2>() == ivec2{1,2});

        CHECK(*val.try_cast<ivec2>() == ivec2{1,2});
        CHECK(*std::as_const(val).try_cast<ivec2>() == ivec2{1,2});
    }

    SUBCASE("const ivec2&&") {
        const ivec2 v{1,2};

        value val{std::move(v)};
        CHECK(ivec2::move_ctor_counter == 0);
        CHECK(ivec2::copy_ctor_counter == 1);

        CHECK(val.type() == type_db::get<ivec2>());

        CHECK(val.equals(ivec2{1,2}));
        CHECK(val.equals(value{ivec2{1,2}}));

        CHECK(val.cast<ivec2>() == ivec2{1,2});
        CHECK(std::as_const(val).cast<ivec2>() == ivec2{1,2});

        CHECK(*val.try_cast<ivec2>() == ivec2{1,2});
        CHECK(*std::as_const(val).try_cast<ivec2>() == ivec2{1,2});
    }

    SUBCASE("value(value&&)") {
        ivec2 v{1,2};
        value val_src{std::move(v)};
        CHECK(ivec2::move_ctor_counter == 1);
        CHECK(ivec2::copy_ctor_counter == 0);

        value val_dst{std::move(val_src)};
        CHECK(val_dst.equals(ivec2{1,2}));
        CHECK(ivec2::move_ctor_counter == 2);
        CHECK(ivec2::copy_ctor_counter == 0);

        CHECK(val_src.equals(ivec2{0,0}));
        CHECK(val_src.data() != val_dst.data());
    }

    SUBCASE("value(const value&)") {
        const ivec2 v{1,2};
        value val_src{v};
        CHECK(ivec2::move_ctor_counter == 0);
        CHECK(ivec2::copy_ctor_counter == 1);

        value val_dst{val_src};
        CHECK(val_dst.equals(ivec2{1,2}));
        CHECK(ivec2::move_ctor_counter == 0);
        CHECK(ivec2::copy_ctor_counter == 2);

        CHECK(val_src.equals(ivec2{1,2}));
        CHECK(val_src.data() != val_dst.data());
    }

    SUBCASE("value& operator=(value&&)") {
        value val_src1{"world"s};
        value val_src2{ivec2{1,2}};
        CHECK(ivec2::move_ctor_counter == 1);
        CHECK(ivec2::copy_ctor_counter == 0);

        value val_dst{"hello"s};

        val_dst = std::move(val_src1);
        CHECK(val_dst.equals("world"s));
        CHECK(ivec2::move_ctor_counter == 1);
        CHECK(ivec2::copy_ctor_counter == 0);

        val_dst = std::move(val_src2);
        CHECK(val_dst.equals(ivec2{1,2}));
        CHECK(ivec2::move_ctor_counter == 2);
        CHECK(ivec2::copy_ctor_counter == 0);

        CHECK(val_src2.equals(ivec2{0,0}));
        CHECK(val_src2.data() != val_dst.data());
    }

    SUBCASE("value& operator=(const value&)") {
        value val_src1{"world"s};
        value val_src2{ivec2{1,2}};
        CHECK(ivec2::move_ctor_counter == 1);
        CHECK(ivec2::copy_ctor_counter == 0);

        value val_dst{"hello"s};

        val_dst = val_src1;
        CHECK(val_dst.equals("world"s));
        CHECK(ivec2::move_ctor_counter == 1);
        CHECK(ivec2::copy_ctor_counter == 0);

        val_dst = val_src2;
        CHECK(val_dst.equals(ivec2{1,2}));
        CHECK(ivec2::move_ctor_counter == 1);
        CHECK(ivec2::copy_ctor_counter == 1);

        CHECK(val_src2.equals(ivec2{1,2}));
        CHECK(val_src2.data() != val_dst.data());
    }

    SUBCASE("swap") {
        value val1{"world"s};
        value val2{ivec2{1,2}};
        CHECK(ivec2::move_ctor_counter == 1);
        CHECK(ivec2::copy_ctor_counter == 0);

        val1.swap(val2);
        CHECK(val1.equals(ivec2{1,2}));
        CHECK(val2.equals("world"s));
        CHECK((ivec2::move_ctor_counter == 2 || ivec2::move_ctor_counter == 3));
        CHECK(ivec2::copy_ctor_counter == 0);

        swap(val1, val2);
        CHECK(val1.equals("world"s));
        CHECK(val2.equals(ivec2{1,2}));
    }
}
