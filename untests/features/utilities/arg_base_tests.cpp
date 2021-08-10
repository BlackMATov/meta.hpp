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

        [[maybe_unused]] ivec2(ivec2&& other) noexcept {
            x = other.x;
            y = other.y;
            other.x = 0;
            other.y = 0;
        }

        [[maybe_unused]] ivec2(const ivec2& other) noexcept {
            x = other.x;
            y = other.y;
        }

        ivec2& operator=(ivec2&&) = delete;
        ivec2& operator=(const ivec2&) = delete;
    };

    struct ivec3 {
        int x{};
        int y{};
        int z{};

        [[maybe_unused]] ivec3() = default;
        [[maybe_unused]] explicit ivec3(int v): x{v}, y{v}, z{v} {}
        [[maybe_unused]] ivec3(int x, int y, int z): x{x}, y{y}, z{z} {}

        [[maybe_unused]] ivec3(ivec3&& other) noexcept {
            x = other.x;
            y = other.y;
            z = other.z;
            other.x = 0;
            other.y = 0;
            other.z = 0;
        }

        [[maybe_unused]] ivec3(const ivec3& other) noexcept {
            x = other.x;
            y = other.y;
            z = other.z;
        }

        ivec3& operator=(ivec3&&) = delete;
        ivec3& operator=(const ivec3&) = delete;
    };

    [[maybe_unused]] bool operator==(const ivec2& l, const ivec2& r) noexcept {
        return l.x == r.x && l.y == r.y;
    }

    [[maybe_unused]] bool operator==(const ivec3& l, const ivec3& r) noexcept {
        return l.x == r.x && l.y == r.y && l.z == r.z;
    }
}

TEST_CASE("features/utilities/arg_base/type") {
    SUBCASE("ptr") {
        ivec2 v{1,2};
        ivec2* vp = &v;
        arg_base a{typename_arg<decltype(vp)>};

        CHECK(a.raw_type().id() == type_db::get<ivec2*>().id());
        CHECK(a.ref_type() == arg_base::ref_types::ref);
    }

    SUBCASE("rptr") {
        ivec2 v{1,2};
        ivec2* vp = &v;
        arg_base a{typename_arg<decltype(std::move(vp))>};

        CHECK(a.raw_type().id() == type_db::get<ivec2*>().id());
        CHECK(a.ref_type() == arg_base::ref_types::rref);
    }

    SUBCASE("ptr_c") {
        ivec2 v{1,2};
        ivec2* const vp = &v;
        arg_base a{typename_arg<decltype(vp)>};

        CHECK(a.raw_type().id() == type_db::get<ivec2*>().id());
        CHECK(a.ref_type() == arg_base::ref_types::cref);
    }

    SUBCASE("rptr_c") {
        ivec2 v{1,2};
        ivec2* const vp = &v;
        arg_base a{typename_arg<decltype(std::move(vp))>};

        CHECK(a.raw_type().id() == type_db::get<ivec2*>().id());
        CHECK(a.ref_type() == arg_base::ref_types::crref);
    }

    SUBCASE("cptr") {
        const ivec2 v{1,2};
        const ivec2* vp = &v;
        arg_base a{typename_arg<decltype(vp)>};

        CHECK(a.raw_type().id() == type_db::get<const ivec2*>().id());
        CHECK(a.ref_type() == arg_base::ref_types::ref);
    }

    SUBCASE("crptr") {
        const ivec2 v{1,2};
        const ivec2* vp = &v;
        arg_base a{typename_arg<decltype(std::move(vp))>};

        CHECK(a.raw_type().id() == type_db::get<const ivec2*>().id());
        CHECK(a.ref_type() == arg_base::ref_types::rref);
    }

    SUBCASE("cptr_c") {
        const ivec2 v{1,2};
        const ivec2* const vp = &v;
        arg_base a{typename_arg<decltype(vp)>};

        CHECK(a.raw_type().id() == type_db::get<const ivec2*>().id());
        CHECK(a.ref_type() == arg_base::ref_types::cref);
    }

    SUBCASE("crptr_c") {
        const ivec2 v{1,2};
        const ivec2* const vp = &v;
        arg_base a{typename_arg<decltype(std::move(vp))>};

        CHECK(a.raw_type().id() == type_db::get<const ivec2*>().id());
        CHECK(a.ref_type() == arg_base::ref_types::crref);
    }

    SUBCASE("ref") {
        ivec2 v{1,2};
        ivec2& vr = v;
        arg_base a{typename_arg<decltype(vr)>};

        CHECK(a.raw_type().id() == type_db::get<ivec2>().id());
        CHECK(a.ref_type() == arg_base::ref_types::ref);
    }

    SUBCASE("cref") {
        const ivec2 v{1,2};
        const ivec2& vr = v;
        arg_base a{typename_arg<decltype(vr)>};

        CHECK(a.raw_type().id() == type_db::get<ivec2>().id());
        CHECK(a.ref_type() == arg_base::ref_types::cref);
    }

    SUBCASE("rref") {
        ivec2 v{1,2};
        arg_base a{typename_arg<decltype(std::move(v))>};

        CHECK(a.raw_type().id() == type_db::get<ivec2>().id());
        CHECK(a.ref_type() == arg_base::ref_types::rref);
    }

    SUBCASE("crref") {
        const ivec2 v{1,2};
        arg_base a{typename_arg<decltype(std::move(v))>};

        CHECK(a.raw_type().id() == type_db::get<ivec2>().id());
        CHECK(a.ref_type() == arg_base::ref_types::crref);
    }
}
