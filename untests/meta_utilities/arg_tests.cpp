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

TEST_CASE("features/meta_utilities/arg/type") {
    namespace meta = meta_hpp;

    SUBCASE("ptr") {
        ivec2 v{1,2};
        ivec2* vp = &v;
        meta::detail::arg a{vp};

        CHECK(a.get_raw_type() == meta::resolve_type<ivec2*>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::ref);
    }

    SUBCASE("rptr") {
        ivec2 v{1,2};
        ivec2* vp = &v;
        meta::detail::arg a{std::move(vp)};

        CHECK(a.get_raw_type() == meta::resolve_type<ivec2*>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::rref);
    }

    SUBCASE("ptr_c") {
        ivec2 v{1,2};
        ivec2* const vp = &v;
        meta::detail::arg a{vp};

        CHECK(a.get_raw_type() == meta::resolve_type<ivec2*>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::cref);
    }

    SUBCASE("rptr_c") {
        ivec2 v{1,2};
        ivec2* const vp = &v;
        meta::detail::arg a{std::move(vp)};

        CHECK(a.get_raw_type() == meta::resolve_type<ivec2*>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::crref);
    }

    SUBCASE("cptr") {
        const ivec2 v{1,2};
        const ivec2* vp = &v;
        meta::detail::arg a{vp};

        CHECK(a.get_raw_type() == meta::resolve_type<const ivec2*>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::ref);
    }

    SUBCASE("crptr") {
        const ivec2 v{1,2};
        const ivec2* vp = &v;
        meta::detail::arg a{std::move(vp)};

        CHECK(a.get_raw_type() == meta::resolve_type<const ivec2*>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::rref);
    }

    SUBCASE("cptr_c") {
        const ivec2 v{1,2};
        const ivec2* const vp = &v;
        meta::detail::arg a{vp};

        CHECK(a.get_raw_type() == meta::resolve_type<const ivec2*>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::cref);
    }

    SUBCASE("crptr_c") {
        const ivec2 v{1,2};
        const ivec2* const vp = &v;
        meta::detail::arg a{std::move(vp)};

        CHECK(a.get_raw_type() == meta::resolve_type<const ivec2*>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::crref);
    }

    SUBCASE("ref") {
        ivec2 v{1,2};
        ivec2& vr = v;
        meta::detail::arg a{vr};

        CHECK(a.get_raw_type() == meta::resolve_type<ivec2>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::ref);
    }

    SUBCASE("cref") {
        const ivec2 v{1,2};
        const ivec2& vr = v;
        meta::detail::arg a{vr};

        CHECK(a.get_raw_type() == meta::resolve_type<ivec2>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::cref);
    }

    SUBCASE("rref") {
        ivec2 v{1,2};
        meta::detail::arg a{std::move(v)};

        CHECK(a.get_raw_type() == meta::resolve_type<ivec2>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::rref);
    }

    SUBCASE("crref") {
        const ivec2 v{1,2};
        meta::detail::arg a{std::move(v)};

        CHECK(a.get_raw_type() == meta::resolve_type<ivec2>());
        CHECK(a.get_ref_type() == meta::detail::arg::ref_types::crref);
    }
}

TEST_CASE("features/meta_utilities/arg/can_cast_to") {
    namespace meta = meta_hpp;

    SUBCASE("ptr") {
        ivec2 v{1,2};
        ivec2* vp = &v;
        meta::detail::arg a{vp};

        CHECK_FALSE(a.can_cast_to<ivec2>());
        CHECK_FALSE(a.can_cast_to<const ivec2>());
        CHECK_FALSE(a.can_cast_to<ivec2&>());
        CHECK_FALSE(a.can_cast_to<const ivec2&>());
        CHECK_FALSE(a.can_cast_to<ivec2&&>());
        CHECK_FALSE(a.can_cast_to<const ivec2&&>());

        {
            CHECK_FALSE(a.can_cast_to<ivec3*>());
            CHECK_FALSE(a.can_cast_to<const ivec3*>());
            CHECK_FALSE(a.can_cast_to<ivec3* const>());
            CHECK_FALSE(a.can_cast_to<const ivec3* const>());
        }

        {
            CHECK(a.can_cast_to<ivec2*>());
            CHECK(a.can_cast_to<const ivec2*>());
            CHECK(a.can_cast_to<ivec2* const>());
            CHECK(a.can_cast_to<const ivec2* const>());

            CHECK(a.can_cast_to<ivec2*&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&>());
            CHECK(a.can_cast_to<ivec2* const&>());
            CHECK(a.can_cast_to<const ivec2* const&>());

            CHECK_FALSE(a.can_cast_to<ivec2*&&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&&>());
            CHECK_FALSE(a.can_cast_to<ivec2* const&&>());
            CHECK_FALSE(a.can_cast_to<const ivec2* const&&>());

            []([[maybe_unused]] ivec2 *){}(vp);
            []([[maybe_unused]] const ivec2 *){}(vp);
            []([[maybe_unused]] ivec2 *const){}(vp);
            []([[maybe_unused]] const ivec2 *const){}(vp);

            []([[maybe_unused]] ivec2 *&){}(vp);
            //[]([[maybe_unused]] const ivec2 *&){}(vp);
            []([[maybe_unused]] ivec2 *const &){}(vp);
            []([[maybe_unused]] const ivec2 *const &){}(vp);

            //[]([[maybe_unused]] ivec2 *&&){}(vp);
            //[]([[maybe_unused]] const ivec2 *&&){}(vp);
            //[]([[maybe_unused]] ivec2 *const &&){}(vp);
            //[]([[maybe_unused]] const ivec2 *const &&){}(vp);
        }
    }

    SUBCASE("rptr") {
        ivec2 v{1,2};
        ivec2* vp = &v;
        meta::detail::arg a{std::move(vp)};

        CHECK_FALSE(a.can_cast_to<ivec2>());
        CHECK_FALSE(a.can_cast_to<const ivec2>());
        CHECK_FALSE(a.can_cast_to<ivec2&>());
        CHECK_FALSE(a.can_cast_to<const ivec2&>());
        CHECK_FALSE(a.can_cast_to<ivec2&&>());
        CHECK_FALSE(a.can_cast_to<const ivec2&&>());

        {
            CHECK_FALSE(a.can_cast_to<ivec3*>());
            CHECK_FALSE(a.can_cast_to<const ivec3*>());
            CHECK_FALSE(a.can_cast_to<ivec3* const>());
            CHECK_FALSE(a.can_cast_to<const ivec3* const>());
        }

        {
            CHECK(a.can_cast_to<ivec2*>());
            CHECK(a.can_cast_to<const ivec2*>());
            CHECK(a.can_cast_to<ivec2* const>());
            CHECK(a.can_cast_to<const ivec2* const>());

            CHECK_FALSE(a.can_cast_to<ivec2*&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&>());
            CHECK(a.can_cast_to<ivec2* const&>());
            CHECK(a.can_cast_to<const ivec2* const&>());

            CHECK(a.can_cast_to<ivec2*&&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&&>());
            CHECK(a.can_cast_to<ivec2* const&&>());
            CHECK(a.can_cast_to<const ivec2* const&&>());

            []([[maybe_unused]] ivec2 *){}(std::move(vp));
            []([[maybe_unused]] const ivec2 *){}(std::move(vp));
            []([[maybe_unused]] ivec2 *const){}(std::move(vp));
            []([[maybe_unused]] const ivec2 *const){}(std::move(vp));

            //[]([[maybe_unused]] ivec2 *&){}(std::move(vp));
            //[]([[maybe_unused]] const ivec2 *&){}(std::move(vp));
            []([[maybe_unused]] ivec2 *const &){}(std::move(vp));
            []([[maybe_unused]] const ivec2 *const &){}(std::move(vp));

            []([[maybe_unused]] ivec2 *&&){}(std::move(vp));
            //[]([[maybe_unused]] const ivec2 *&&){}(std::move(vp));
            []([[maybe_unused]] ivec2 *const &&){}(std::move(vp));
            []([[maybe_unused]] const ivec2 *const &&){}(std::move(vp));
        }
    }

    SUBCASE("ptr_c") {
        ivec2 v{1,2};
        ivec2* const vp = &v;
        meta::detail::arg a{vp};

        CHECK_FALSE(a.can_cast_to<ivec2>());
        CHECK_FALSE(a.can_cast_to<const ivec2>());
        CHECK_FALSE(a.can_cast_to<ivec2&>());
        CHECK_FALSE(a.can_cast_to<const ivec2&>());
        CHECK_FALSE(a.can_cast_to<ivec2&&>());
        CHECK_FALSE(a.can_cast_to<const ivec2&&>());

        {
            CHECK_FALSE(a.can_cast_to<ivec3*>());
            CHECK_FALSE(a.can_cast_to<const ivec3*>());
            CHECK_FALSE(a.can_cast_to<ivec3* const>());
            CHECK_FALSE(a.can_cast_to<const ivec3* const>());
        }

        {
            CHECK(a.can_cast_to<ivec2*>());
            CHECK(a.can_cast_to<const ivec2*>());
            CHECK(a.can_cast_to<ivec2* const>());
            CHECK(a.can_cast_to<const ivec2* const>());

            CHECK_FALSE(a.can_cast_to<ivec2*&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&>());
            CHECK(a.can_cast_to<ivec2* const&>());
            CHECK(a.can_cast_to<const ivec2* const&>());

            CHECK_FALSE(a.can_cast_to<ivec2*&&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&&>());
            CHECK_FALSE(a.can_cast_to<ivec2* const&&>());
            CHECK_FALSE(a.can_cast_to<const ivec2* const&&>());

            []([[maybe_unused]] ivec2 *){}(vp);
            []([[maybe_unused]] const ivec2 *){}(vp);
            []([[maybe_unused]] ivec2 *const &){}(vp);
            []([[maybe_unused]] const ivec2 *const &){}(vp);

            // []([[maybe_unused]] ivec2 *&){}(vp);
            // []([[maybe_unused]] const ivec2 *&){}(vp);
            []([[maybe_unused]] ivec2 *const &){}(vp);
            []([[maybe_unused]] const ivec2 *const &){}(vp);

            //[]([[maybe_unused]] ivec2 *&&){}(vp);
            //[]([[maybe_unused]] const ivec2 *&&){}(vp);
            //[]([[maybe_unused]] ivec2 *const &&){}(vp);
            //[]([[maybe_unused]] const ivec2 *const &&){}(vp);
        }
    }

    SUBCASE("rptr_c") {
        ivec2 v{1,2};
        ivec2* const vp = &v;
        meta::detail::arg a{std::move(vp)};

        CHECK_FALSE(a.can_cast_to<ivec2>());
        CHECK_FALSE(a.can_cast_to<const ivec2>());
        CHECK_FALSE(a.can_cast_to<ivec2&>());
        CHECK_FALSE(a.can_cast_to<const ivec2&>());
        CHECK_FALSE(a.can_cast_to<ivec2&&>());
        CHECK_FALSE(a.can_cast_to<const ivec2&&>());

        {
            CHECK_FALSE(a.can_cast_to<ivec3*>());
            CHECK_FALSE(a.can_cast_to<const ivec3*>());
            CHECK_FALSE(a.can_cast_to<ivec3* const>());
            CHECK_FALSE(a.can_cast_to<const ivec3* const>());
        }

        {
            CHECK(a.can_cast_to<ivec2*>());
            CHECK(a.can_cast_to<const ivec2*>());
            CHECK(a.can_cast_to<ivec2* const>());
            CHECK(a.can_cast_to<const ivec2* const>());

            CHECK_FALSE(a.can_cast_to<ivec2*&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&>());
            CHECK(a.can_cast_to<ivec2* const&>());
            CHECK(a.can_cast_to<const ivec2* const&>());

            CHECK_FALSE(a.can_cast_to<ivec2*&&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&&>());
            CHECK(a.can_cast_to<ivec2* const&&>());
            CHECK(a.can_cast_to<const ivec2* const&&>());

            []([[maybe_unused]] ivec2 *){}(std::move(vp));
            []([[maybe_unused]] const ivec2 *){}(std::move(vp));
            []([[maybe_unused]] ivec2 *const){}(std::move(vp));
            []([[maybe_unused]] const ivec2 *const){}(std::move(vp));

            //[]([[maybe_unused]] ivec2 *&){}(std::move(vp));
            //[]([[maybe_unused]] const ivec2 *&){}(std::move(vp));
            []([[maybe_unused]] ivec2 *const &){}(std::move(vp));
            []([[maybe_unused]] const ivec2 *const &){}(std::move(vp));

            //[]([[maybe_unused]] ivec2 *&&){}(std::move(vp));
            //[]([[maybe_unused]] const ivec2 *&&){}(std::move(vp));
            []([[maybe_unused]] ivec2 *const &&){}(std::move(vp));
            []([[maybe_unused]] const ivec2 *const &&){}(std::move(vp));
        }
    }

    SUBCASE("cptr") {
        const ivec2 v{1,2};
        const ivec2* vp = &v;
        meta::detail::arg a{vp};

        CHECK_FALSE(a.can_cast_to<ivec2>());
        CHECK_FALSE(a.can_cast_to<const ivec2>());
        CHECK_FALSE(a.can_cast_to<ivec2&>());
        CHECK_FALSE(a.can_cast_to<const ivec2&>());
        CHECK_FALSE(a.can_cast_to<ivec2&&>());
        CHECK_FALSE(a.can_cast_to<const ivec2&&>());

        {
            CHECK_FALSE(a.can_cast_to<ivec3*>());
            CHECK_FALSE(a.can_cast_to<const ivec3*>());
            CHECK_FALSE(a.can_cast_to<ivec3* const>());
            CHECK_FALSE(a.can_cast_to<const ivec3* const>());
        }

        {
            CHECK_FALSE(a.can_cast_to<ivec2*>());
            CHECK(a.can_cast_to<const ivec2*>());
            CHECK_FALSE(a.can_cast_to<ivec2* const>());
            CHECK(a.can_cast_to<const ivec2* const>());

            CHECK_FALSE(a.can_cast_to<ivec2*&>());
            CHECK(a.can_cast_to<const ivec2*&>());
            CHECK_FALSE(a.can_cast_to<ivec2* const&>());
            CHECK(a.can_cast_to<const ivec2* const&>());

            CHECK_FALSE(a.can_cast_to<ivec2*&&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&&>());
            CHECK_FALSE(a.can_cast_to<ivec2* const&&>());
            CHECK_FALSE(a.can_cast_to<const ivec2* const&&>());

            //[]([[maybe_unused]] ivec2 *){}(vp);
            []([[maybe_unused]] const ivec2 *){}(vp);
            //[]([[maybe_unused]] ivec2 *const){}(vp);
            []([[maybe_unused]] const ivec2 *const){}(vp);

            //[]([[maybe_unused]] ivec2 *&){}(vp);
            []([[maybe_unused]] const ivec2 *&){}(vp);
            //[]([[maybe_unused]] ivec2 *const &){}(vp);
            []([[maybe_unused]] const ivec2 *const &){}(vp);

            //[]([[maybe_unused]] ivec2 *&&){}(vp);
            //[]([[maybe_unused]] const ivec2 *&&){}(vp);
            //[]([[maybe_unused]] ivec2 *const &&){}(vp);
            //[]([[maybe_unused]] const ivec2 *const &&){}(vp);
        }
    }

    SUBCASE("crptr") {
        const ivec2 v{1,2};
        const ivec2* vp = &v;
        meta::detail::arg a{std::move(vp)};

        CHECK_FALSE(a.can_cast_to<ivec2>());
        CHECK_FALSE(a.can_cast_to<const ivec2>());
        CHECK_FALSE(a.can_cast_to<ivec2&>());
        CHECK_FALSE(a.can_cast_to<const ivec2&>());
        CHECK_FALSE(a.can_cast_to<ivec2&&>());
        CHECK_FALSE(a.can_cast_to<const ivec2&&>());

        {
            CHECK_FALSE(a.can_cast_to<ivec3*>());
            CHECK_FALSE(a.can_cast_to<const ivec3*>());
            CHECK_FALSE(a.can_cast_to<ivec3* const>());
            CHECK_FALSE(a.can_cast_to<const ivec3* const>());
        }

        {
            CHECK_FALSE(a.can_cast_to<ivec2*>());
            CHECK(a.can_cast_to<const ivec2*>());
            CHECK_FALSE(a.can_cast_to<ivec2* const>());
            CHECK(a.can_cast_to<const ivec2* const>());

            CHECK_FALSE(a.can_cast_to<ivec2*&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&>());
            CHECK_FALSE(a.can_cast_to<ivec2* const&>());
            CHECK(a.can_cast_to<const ivec2* const&>());

            CHECK_FALSE(a.can_cast_to<ivec2*&&>());
            CHECK(a.can_cast_to<const ivec2*&&>());
            CHECK_FALSE(a.can_cast_to<ivec2* const&&>());
            CHECK(a.can_cast_to<const ivec2* const&&>());

            //[]([[maybe_unused]] ivec2 *){}(std::move(vp));
            []([[maybe_unused]] const ivec2 *){}(std::move(vp));
            //[]([[maybe_unused]] ivec2 *const){}(std::move(vp));
            []([[maybe_unused]] const ivec2 *const){}(std::move(vp));

            //[]([[maybe_unused]] ivec2 *&){}(std::move(vp));
            //[]([[maybe_unused]] const ivec2 *&){}(std::move(vp));
            //[]([[maybe_unused]] ivec2 *const &){}(std::move(vp));
            []([[maybe_unused]] const ivec2 *const &){}(std::move(vp));

            //[]([[maybe_unused]] ivec2 *&&){}(std::move(vp));
            []([[maybe_unused]] const ivec2 *&&){}(std::move(vp));
            //[]([[maybe_unused]] ivec2 *const &&){}(std::move(vp));
            []([[maybe_unused]] const ivec2 *const &&){}(std::move(vp));
        }
    }

    SUBCASE("cptr_c") {
        const ivec2 v{1,2};
        const ivec2* const vp = &v;
        meta::detail::arg a{vp};

        CHECK_FALSE(a.can_cast_to<ivec2>());
        CHECK_FALSE(a.can_cast_to<const ivec2>());
        CHECK_FALSE(a.can_cast_to<ivec2&>());
        CHECK_FALSE(a.can_cast_to<const ivec2&>());
        CHECK_FALSE(a.can_cast_to<ivec2&&>());
        CHECK_FALSE(a.can_cast_to<const ivec2&&>());

        {
            CHECK_FALSE(a.can_cast_to<ivec3*>());
            CHECK_FALSE(a.can_cast_to<const ivec3*>());
            CHECK_FALSE(a.can_cast_to<ivec3* const>());
            CHECK_FALSE(a.can_cast_to<const ivec3* const>());
        }

        {
            CHECK_FALSE(a.can_cast_to<ivec2*>());
            CHECK(a.can_cast_to<const ivec2*>());
            CHECK_FALSE(a.can_cast_to<ivec2* const>());
            CHECK(a.can_cast_to<const ivec2* const>());

            CHECK_FALSE(a.can_cast_to<ivec2*&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&>());
            CHECK_FALSE(a.can_cast_to<ivec2* const&>());
            CHECK(a.can_cast_to<const ivec2* const&>());

            CHECK_FALSE(a.can_cast_to<ivec2*&&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&&>());
            CHECK_FALSE(a.can_cast_to<ivec2* const&&>());
            CHECK_FALSE(a.can_cast_to<const ivec2* const&&>());

            //[]([[maybe_unused]] ivec2 *){}(vp);
            []([[maybe_unused]] const ivec2 *){}(vp);
            //[]([[maybe_unused]] ivec2 *const){}(vp);
            []([[maybe_unused]] const ivec2 *const){}(vp);

            //[]([[maybe_unused]] ivec2 *&){}(vp);
            //[]([[maybe_unused]] const ivec2 *&){}(vp);
            //[]([[maybe_unused]] ivec2 *const &){}(vp);
            []([[maybe_unused]] const ivec2 *const &){}(vp);

            //[]([[maybe_unused]] ivec2 *&&){}(vp);
            //[]([[maybe_unused]] const ivec2 *&&){}(vp);
            //[]([[maybe_unused]] ivec2 *const &&){}(vp);
            //[]([[maybe_unused]] const ivec2 *const &&){}(vp);
        }
    }

    SUBCASE("crptr_c") {
        const ivec2 v{1,2};
        const ivec2* const vp = &v;
        meta::detail::arg a{std::move(vp)};

        CHECK_FALSE(a.can_cast_to<ivec2>());
        CHECK_FALSE(a.can_cast_to<const ivec2>());
        CHECK_FALSE(a.can_cast_to<ivec2&>());
        CHECK_FALSE(a.can_cast_to<const ivec2&>());
        CHECK_FALSE(a.can_cast_to<ivec2&&>());
        CHECK_FALSE(a.can_cast_to<const ivec2&&>());

        {
            CHECK_FALSE(a.can_cast_to<ivec3*>());
            CHECK_FALSE(a.can_cast_to<const ivec3*>());
            CHECK_FALSE(a.can_cast_to<ivec3* const>());
            CHECK_FALSE(a.can_cast_to<const ivec3* const>());
        }

        {
            CHECK_FALSE(a.can_cast_to<ivec2*>());
            CHECK(a.can_cast_to<const ivec2*>());
            CHECK_FALSE(a.can_cast_to<ivec2* const>());
            CHECK(a.can_cast_to<const ivec2* const>());

            CHECK_FALSE(a.can_cast_to<ivec2*&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&>());
            CHECK_FALSE(a.can_cast_to<ivec2* const&>());
            CHECK(a.can_cast_to<const ivec2* const&>());

            CHECK_FALSE(a.can_cast_to<ivec2*&&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&&>());
            CHECK_FALSE(a.can_cast_to<ivec2* const&&>());
            CHECK(a.can_cast_to<const ivec2* const&&>());

            //[]([[maybe_unused]] ivec2 *){}(std::move(vp));
            []([[maybe_unused]] const ivec2 *){}(std::move(vp));
            //[]([[maybe_unused]] ivec2 *const){}(std::move(vp));
            []([[maybe_unused]] const ivec2 *const){}(std::move(vp));

            //[]([[maybe_unused]] ivec2 *&){}(std::move(vp));
            //[]([[maybe_unused]] const ivec2 *&){}(std::move(vp));
            //[]([[maybe_unused]] ivec2 *const &){}(std::move(vp));
            []([[maybe_unused]] const ivec2 *const &){}(std::move(vp));

            //[]([[maybe_unused]] ivec2 *&&){}(std::move(vp));
            //[]([[maybe_unused]] const ivec2 *&&){}(std::move(vp));
            //[]([[maybe_unused]] ivec2 *const &&){}(std::move(vp));
            []([[maybe_unused]] const ivec2 *const &&){}(std::move(vp));
        }
    }

    SUBCASE("ref") {
        ivec2 v{1,2};
        ivec2& vr = v;
        meta::detail::arg a{vr};

        {
            CHECK(a.can_cast_to<ivec2>());
            CHECK(a.can_cast_to<const ivec2>());
            CHECK(a.can_cast_to<ivec2&>());
            CHECK(a.can_cast_to<const ivec2&>());
            CHECK_FALSE(a.can_cast_to<ivec2&&>());
            CHECK_FALSE(a.can_cast_to<const ivec2&&>());

            []([[maybe_unused]] ivec2){}(vr);
            []([[maybe_unused]] const ivec2){}(vr);
            []([[maybe_unused]] ivec2&){}(vr);
            []([[maybe_unused]] const ivec2&){}(vr);
            //[]([[maybe_unused]] ivec2&&){}(vr);
            //[]([[maybe_unused]] const ivec2&&){}(vr);
        }

        {
            CHECK_FALSE(a.can_cast_to<ivec3>());
            CHECK_FALSE(a.can_cast_to<const ivec3>());
            CHECK_FALSE(a.can_cast_to<ivec3&>());
            CHECK_FALSE(a.can_cast_to<const ivec3&>());
            CHECK_FALSE(a.can_cast_to<ivec3&&>());
            CHECK_FALSE(a.can_cast_to<const ivec3&&>());
        }

        {
            CHECK_FALSE(a.can_cast_to<ivec2*>());
            CHECK_FALSE(a.can_cast_to<const ivec2*>());
            CHECK_FALSE(a.can_cast_to<ivec2* const>());
            CHECK_FALSE(a.can_cast_to<const ivec2* const>());

            CHECK_FALSE(a.can_cast_to<ivec2*&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&>());
            CHECK_FALSE(a.can_cast_to<ivec2* const&>());
            CHECK_FALSE(a.can_cast_to<const ivec2* const&>());

            CHECK_FALSE(a.can_cast_to<ivec2*&&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&&>());
            CHECK_FALSE(a.can_cast_to<ivec2* const&&>());
            CHECK_FALSE(a.can_cast_to<const ivec2* const&&>());

            //[]([[maybe_unused]] ivec2*){}(vr);
            //[]([[maybe_unused]] const ivec2*){}(vr);
            //[]([[maybe_unused]] ivec2* const){}(vr);
            //[]([[maybe_unused]] const ivec2* const){}(vr);

            //[]([[maybe_unused]] ivec2 *&){}(vr);
            //[]([[maybe_unused]] const ivec2 *&){}(vr);
            //[]([[maybe_unused]] ivec2 *const &){}(vr);
            //[]([[maybe_unused]] const ivec2 *const &){}(vr);

            //[]([[maybe_unused]] ivec2 *&&){}(vr);
            //[]([[maybe_unused]] const ivec2 *&&){}(vr);
            //[]([[maybe_unused]] ivec2 *const &&){}(vr);
            //[]([[maybe_unused]] const ivec2 *const &&){}(vr);
        }
    }

    SUBCASE("cref") {
        const ivec2 v{1,2};
        const ivec2& vr = v;
        meta::detail::arg a{vr};

        {
            CHECK(a.can_cast_to<ivec2>());
            CHECK(a.can_cast_to<const ivec2>());
            CHECK_FALSE(a.can_cast_to<ivec2&>());
            CHECK(a.can_cast_to<const ivec2&>());
            CHECK_FALSE(a.can_cast_to<ivec2&&>());
            CHECK_FALSE(a.can_cast_to<const ivec2&&>());

            []([[maybe_unused]] ivec2){}(vr);
            []([[maybe_unused]] const ivec2){}(vr);
            //[]([[maybe_unused]] ivec2&){}(vr);
            []([[maybe_unused]] const ivec2&){}(vr);
            //[]([[maybe_unused]] ivec2&&){}(vr);
            //[]([[maybe_unused]] const ivec2&&){}(vr);
        }

        {
            CHECK_FALSE(a.can_cast_to<ivec3>());
            CHECK_FALSE(a.can_cast_to<const ivec3>());
            CHECK_FALSE(a.can_cast_to<ivec3&>());
            CHECK_FALSE(a.can_cast_to<const ivec3&>());
            CHECK_FALSE(a.can_cast_to<ivec3&&>());
            CHECK_FALSE(a.can_cast_to<const ivec3&&>());
        }

        {
            CHECK_FALSE(a.can_cast_to<ivec2*>());
            CHECK_FALSE(a.can_cast_to<const ivec2*>());
            CHECK_FALSE(a.can_cast_to<ivec2* const>());
            CHECK_FALSE(a.can_cast_to<const ivec2* const>());

            CHECK_FALSE(a.can_cast_to<ivec2*&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&>());
            CHECK_FALSE(a.can_cast_to<ivec2* const&>());
            CHECK_FALSE(a.can_cast_to<const ivec2* const&>());

            CHECK_FALSE(a.can_cast_to<ivec2*&&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&&>());
            CHECK_FALSE(a.can_cast_to<ivec2* const&&>());
            CHECK_FALSE(a.can_cast_to<const ivec2* const&&>());

            //[]([[maybe_unused]] ivec2*){}(vr);
            //[]([[maybe_unused]] const ivec2*){}(vr);
            //[]([[maybe_unused]] ivec2* const){}(vr);
            //[]([[maybe_unused]] const ivec2* const){}(vr);

            //[]([[maybe_unused]] ivec2 *&){}(vr);
            //[]([[maybe_unused]] const ivec2 *&){}(vr);
            //[]([[maybe_unused]] ivec2 *const &){}(vr);
            //[]([[maybe_unused]] const ivec2 *const &){}(vr);

            //[]([[maybe_unused]] ivec2 *&&){}(vr);
            //[]([[maybe_unused]] const ivec2 *&&){}(vr);
            //[]([[maybe_unused]] ivec2 *const &&){}(vr);
            //[]([[maybe_unused]] const ivec2 *const &&){}(vr);
        }
    }

    SUBCASE("rref") {
        ivec2 v{1,2};
        meta::detail::arg a{std::move(v)};

        {
            CHECK(a.can_cast_to<ivec2>());
            CHECK(a.can_cast_to<const ivec2>());
            CHECK_FALSE(a.can_cast_to<ivec2&>());
            CHECK(a.can_cast_to<const ivec2&>());
            CHECK(a.can_cast_to<ivec2&&>());
            CHECK(a.can_cast_to<const ivec2&&>());

            []([[maybe_unused]] ivec2){}(std::move(v));
            []([[maybe_unused]] const ivec2){}(std::move(v));
            //[]([[maybe_unused]] ivec2&){}(std::move(v));
            []([[maybe_unused]] const ivec2&){}(std::move(v));
            []([[maybe_unused]] ivec2&&){}(std::move(v));
            []([[maybe_unused]] const ivec2&&){}(std::move(v));
        }

        {
            CHECK_FALSE(a.can_cast_to<ivec3>());
            CHECK_FALSE(a.can_cast_to<const ivec3>());
            CHECK_FALSE(a.can_cast_to<ivec3&>());
            CHECK_FALSE(a.can_cast_to<const ivec3&>());
            CHECK_FALSE(a.can_cast_to<ivec3&&>());
            CHECK_FALSE(a.can_cast_to<const ivec3&&>());
        }

        {
            CHECK_FALSE(a.can_cast_to<ivec2*>());
            CHECK_FALSE(a.can_cast_to<const ivec2*>());
            CHECK_FALSE(a.can_cast_to<ivec2* const>());
            CHECK_FALSE(a.can_cast_to<const ivec2* const>());

            CHECK_FALSE(a.can_cast_to<ivec2*&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&>());
            CHECK_FALSE(a.can_cast_to<ivec2* const&>());
            CHECK_FALSE(a.can_cast_to<const ivec2* const&>());

            CHECK_FALSE(a.can_cast_to<ivec2*&&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&&>());
            CHECK_FALSE(a.can_cast_to<ivec2* const&&>());
            CHECK_FALSE(a.can_cast_to<const ivec2* const&&>());

            //[]([[maybe_unused]] ivec2*){}(std::move(v));
            //[]([[maybe_unused]] const ivec2*){}(std::move(v));
            //[]([[maybe_unused]] ivec2* const){}(std::move(v));
            //[]([[maybe_unused]] const ivec2* const){}(std::move(v));

            //[]([[maybe_unused]] ivec2 *&){}(std::move(v));
            //[]([[maybe_unused]] const ivec2 *&){}(std::move(v));
            //[]([[maybe_unused]] ivec2 *const &){}(std::move(v));
            //[]([[maybe_unused]] const ivec2 *const &){}(std::move(v));

            //[]([[maybe_unused]] ivec2 *&&){}(std::move(v));
            //[]([[maybe_unused]] const ivec2 *&&){}(std::move(v));
            //[]([[maybe_unused]] ivec2 *const &&){}(std::move(v));
            //[]([[maybe_unused]] const ivec2 *const &&){}(std::move(v));
        }
    }

    SUBCASE("crref") {
        const ivec2 v{1,2};
        meta::detail::arg a{std::move(v)};

        {
            CHECK(a.can_cast_to<ivec2>());
            CHECK(a.can_cast_to<const ivec2>());
            CHECK_FALSE(a.can_cast_to<ivec2&>());
            CHECK(a.can_cast_to<const ivec2&>());
            CHECK_FALSE(a.can_cast_to<ivec2&&>());
            CHECK(a.can_cast_to<const ivec2&&>());

            []([[maybe_unused]] ivec2){}(std::move(v));
            []([[maybe_unused]] const ivec2){}(std::move(v));
            //[]([[maybe_unused]] ivec2&){}(std::move(v));
            []([[maybe_unused]] const ivec2&){}(std::move(v));
            //[]([[maybe_unused]] ivec2&&){}(std::move(v));
            []([[maybe_unused]] const ivec2&&){}(std::move(v));
        }

        {
            CHECK_FALSE(a.can_cast_to<ivec3>());
            CHECK_FALSE(a.can_cast_to<const ivec3>());
            CHECK_FALSE(a.can_cast_to<ivec3&>());
            CHECK_FALSE(a.can_cast_to<const ivec3&>());
            CHECK_FALSE(a.can_cast_to<ivec3&&>());
            CHECK_FALSE(a.can_cast_to<const ivec3&&>());
        }

        {
            CHECK_FALSE(a.can_cast_to<ivec2*>());
            CHECK_FALSE(a.can_cast_to<const ivec2*>());
            CHECK_FALSE(a.can_cast_to<ivec2* const>());
            CHECK_FALSE(a.can_cast_to<const ivec2* const>());

            CHECK_FALSE(a.can_cast_to<ivec2*&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&>());
            CHECK_FALSE(a.can_cast_to<ivec2* const&>());
            CHECK_FALSE(a.can_cast_to<const ivec2* const&>());

            CHECK_FALSE(a.can_cast_to<ivec2*&&>());
            CHECK_FALSE(a.can_cast_to<const ivec2*&&>());
            CHECK_FALSE(a.can_cast_to<ivec2* const&&>());
            CHECK_FALSE(a.can_cast_to<const ivec2* const&&>());

            //[]([[maybe_unused]] ivec2*){}(std::move(v));
            //[]([[maybe_unused]] const ivec2*){}(std::move(v));
            //[]([[maybe_unused]] ivec2* const){}(std::move(v));
            //[]([[maybe_unused]] const ivec2* const){}(std::move(v));

            //[]([[maybe_unused]] ivec2 *&){}(std::move(v));
            //[]([[maybe_unused]] const ivec2 *&){}(std::move(v));
            //[]([[maybe_unused]] ivec2 *const &){}(std::move(v));
            //[]([[maybe_unused]] const ivec2 *const &){}(std::move(v));

            //[]([[maybe_unused]] ivec2 *&&){}(std::move(v));
            //[]([[maybe_unused]] const ivec2 *&&){}(std::move(v));
            //[]([[maybe_unused]] ivec2 *const &&){}(std::move(v));
            //[]([[maybe_unused]] const ivec2 *const &&){}(std::move(v));
        }
    }
}

TEST_CASE("features/meta_utilities/arg/cast") {
    namespace meta = meta_hpp;

    SUBCASE("ptr") {
        ivec2 v{1,2};
        ivec2* vp = &v;
        meta::detail::arg a{vp};

        CHECK_THROWS(a.cast<ivec2>());
        CHECK_THROWS(a.cast<const ivec2>());
        CHECK_THROWS(a.cast<ivec2&>());
        CHECK_THROWS(a.cast<const ivec2&>());
        CHECK_THROWS(a.cast<ivec2&&>());
        CHECK_THROWS(a.cast<const ivec2&&>());

        {
            CHECK(a.cast<ivec2*>() == vp);
            CHECK(a.cast<const ivec2*>() == vp);
            CHECK(a.cast<ivec2* const>() == vp);
            CHECK(a.cast<const ivec2* const>() == vp);

            CHECK(a.cast<ivec2*&>() == vp);
            CHECK_THROWS(a.cast<const ivec2*&>());
            CHECK(a.cast<ivec2* const&>() == vp);
            CHECK(a.cast<const ivec2* const&>() == vp);

            CHECK_THROWS(a.cast<ivec2*&&>());
            CHECK_THROWS(a.cast<const ivec2*&&>());
            CHECK_THROWS(a.cast<ivec2* const&&>());
            CHECK_THROWS(a.cast<const ivec2* const&&>());
        }
    }

    SUBCASE("rptr") {
        ivec2 v{1,2};
        ivec2* vp = &v;

        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2>());
        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<const ivec2>());
        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2&>());
        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<const ivec2&>());
        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2&&>());
        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<const ivec2&&>());

        {
            CHECK(meta::detail::arg{std::move(vp)}.cast<ivec2*>() == vp);
            CHECK(meta::detail::arg{std::move(vp)}.cast<const ivec2*>() == vp);
            CHECK(meta::detail::arg{std::move(vp)}.cast<ivec2* const>() == vp);
            CHECK(meta::detail::arg{std::move(vp)}.cast<const ivec2* const>() == vp);

            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2*&>());
            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<const ivec2*&>());
            CHECK(meta::detail::arg{std::move(vp)}.cast<ivec2* const&>() == vp);
            CHECK(meta::detail::arg{std::move(vp)}.cast<const ivec2* const&>() == vp);

            CHECK(meta::detail::arg{std::move(vp)}.cast<ivec2*&&>() == vp);
            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<const ivec2*&&>());
            CHECK(meta::detail::arg{std::move(vp)}.cast<ivec2* const&&>() == vp);
            CHECK(meta::detail::arg{std::move(vp)}.cast<const ivec2* const&&>() == vp);
        }
    }

    SUBCASE("ptr_c") {
        ivec2 v{1,2};
        ivec2* const vp = &v;
        meta::detail::arg a{vp};

        CHECK_THROWS(a.cast<ivec2>());
        CHECK_THROWS(a.cast<const ivec2>());
        CHECK_THROWS(a.cast<ivec2&>());
        CHECK_THROWS(a.cast<const ivec2&>());
        CHECK_THROWS(a.cast<ivec2&&>());
        CHECK_THROWS(a.cast<const ivec2&&>());

        {
            CHECK(a.cast<ivec2*>() == vp);
            CHECK(a.cast<const ivec2*>() == vp);
            CHECK(a.cast<ivec2* const>() == vp);
            CHECK(a.cast<const ivec2* const>() == vp);

            CHECK_THROWS(a.cast<ivec2*&>());
            CHECK_THROWS(a.cast<const ivec2*&>());
            CHECK(a.cast<ivec2* const&>() == vp);
            CHECK(a.cast<const ivec2* const&>() == vp);

            CHECK_THROWS(a.cast<ivec2*&&>());
            CHECK_THROWS(a.cast<const ivec2*&&>());
            CHECK_THROWS(a.cast<ivec2* const&&>());
            CHECK_THROWS(a.cast<const ivec2* const&&>());
        }
    }

    SUBCASE("rptr_c") {
        ivec2 v{1,2};
        ivec2* const vp = &v;

        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2>());
        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<const ivec2>());
        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2&>());
        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<const ivec2&>());
        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2&&>());
        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<const ivec2&&>());

        {
            CHECK(meta::detail::arg{std::move(vp)}.cast<ivec2*>() == vp);
            CHECK(meta::detail::arg{std::move(vp)}.cast<const ivec2*>() == vp);
            CHECK(meta::detail::arg{std::move(vp)}.cast<ivec2* const>() == vp);
            CHECK(meta::detail::arg{std::move(vp)}.cast<const ivec2* const>() == vp);

            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2*&>());
            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<const ivec2*&>());
            CHECK(meta::detail::arg{std::move(vp)}.cast<ivec2* const&>() == vp);
            CHECK(meta::detail::arg{std::move(vp)}.cast<const ivec2* const&>() == vp);

            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2*&&>());
            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<const ivec2*&&>());
            CHECK(meta::detail::arg{std::move(vp)}.cast<ivec2* const&&>() == vp);
            CHECK(meta::detail::arg{std::move(vp)}.cast<const ivec2* const&&>() == vp);
        }
    }

    SUBCASE("cptr") {
        const ivec2 v{1,2};
        const ivec2* vp = &v;
        meta::detail::arg a{vp};

        CHECK_THROWS(a.cast<ivec2>());
        CHECK_THROWS(a.cast<const ivec2>());
        CHECK_THROWS(a.cast<ivec2&>());
        CHECK_THROWS(a.cast<const ivec2&>());
        CHECK_THROWS(a.cast<ivec2&&>());
        CHECK_THROWS(a.cast<const ivec2&&>());

        {
            CHECK_THROWS(a.cast<ivec2*>());
            CHECK(a.cast<const ivec2*>() == vp);
            CHECK_THROWS(a.cast<ivec2* const>());
            CHECK(a.cast<const ivec2* const>() == vp);

            CHECK_THROWS(a.cast<ivec2*&>());
            CHECK(a.cast<const ivec2*&>() == vp);
            CHECK_THROWS(a.cast<ivec2* const&>());
            CHECK(a.cast<const ivec2* const&>() == vp);

            CHECK_THROWS(a.cast<ivec2*&&>());
            CHECK_THROWS(a.cast<const ivec2*&&>());
            CHECK_THROWS(a.cast<ivec2* const&&>());
            CHECK_THROWS(a.cast<const ivec2* const&&>());
        }
    }

    SUBCASE("crptr") {
        const ivec2 v{1,2};
        const ivec2* vp = &v;

        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2>());
        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<const ivec2>());
        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2&>());
        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<const ivec2&>());
        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2&&>());
        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<const ivec2&&>());

        {
            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2*>());
            CHECK(meta::detail::arg{std::move(vp)}.cast<const ivec2*>() == vp);
            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2* const>());
            CHECK(meta::detail::arg{std::move(vp)}.cast<const ivec2* const>() == vp);

            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2*&>());
            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<const ivec2*&>());
            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2* const&>());
            CHECK(meta::detail::arg{std::move(vp)}.cast<const ivec2* const&>() == vp);

            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2*&&>());
            CHECK(meta::detail::arg{std::move(vp)}.cast<const ivec2*&&>() == vp);
            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2* const&&>());
            CHECK(meta::detail::arg{std::move(vp)}.cast<const ivec2* const&&>() == vp);
        }
    }

    SUBCASE("cptr_c") {
        const ivec2 v{1,2};
        const ivec2* const vp = &v;
        meta::detail::arg a{vp};

        CHECK_THROWS(a.cast<ivec2>());
        CHECK_THROWS(a.cast<const ivec2>());
        CHECK_THROWS(a.cast<ivec2&>());
        CHECK_THROWS(a.cast<const ivec2&>());
        CHECK_THROWS(a.cast<ivec2&&>());
        CHECK_THROWS(a.cast<const ivec2&&>());

        {
            CHECK_THROWS(a.cast<ivec2*>());
            CHECK(a.cast<const ivec2*>() == vp);
            CHECK_THROWS(a.cast<ivec2* const>());
            CHECK(a.cast<const ivec2* const>() == vp);

            CHECK_THROWS(a.cast<ivec2*&>());
            CHECK_THROWS(a.cast<const ivec2*&>());
            CHECK_THROWS(a.cast<ivec2* const&>());
            CHECK(a.cast<const ivec2* const&>() == vp);

            CHECK_THROWS(a.cast<ivec2*&&>());
            CHECK_THROWS(a.cast<const ivec2*&&>());
            CHECK_THROWS(a.cast<ivec2* const&&>());
            CHECK_THROWS(a.cast<const ivec2* const&&>());
        }
    }

    SUBCASE("crptr_c") {
        const ivec2 v{1,2};
        const ivec2* const vp = &v;

        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2>());
        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<const ivec2>());
        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2&>());
        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<const ivec2&>());
        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2&&>());
        CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<const ivec2&&>());

        {
            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2*>());
            CHECK(meta::detail::arg{std::move(vp)}.cast<const ivec2*>() == vp);
            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2* const>());
            CHECK(meta::detail::arg{std::move(vp)}.cast<const ivec2* const>() == vp);

            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2*&>());
            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<const ivec2*&>());
            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2* const&>());
            CHECK(meta::detail::arg{std::move(vp)}.cast<const ivec2* const&>() == vp);

            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2*&&>());
            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<const ivec2*&&>());
            CHECK_THROWS(meta::detail::arg{std::move(vp)}.cast<ivec2* const&&>());
            CHECK(meta::detail::arg{std::move(vp)}.cast<const ivec2* const&&>() == vp);
        }
    }

    SUBCASE("ref") {
        ivec2 v{1,2};
        ivec2& vr = v;
        meta::detail::arg a{vr};

        CHECK(a.cast<ivec2>() == ivec2{1,2});
        CHECK(a.cast<const ivec2>() == ivec2{1,2});
        CHECK(&a.cast<ivec2&>() == &v);
        CHECK(&a.cast<const ivec2&>() == &vr);
        CHECK_THROWS(a.cast<ivec2&&>());
        CHECK_THROWS(a.cast<const ivec2&&>());

        CHECK_THROWS(a.cast<ivec2*>());
        CHECK_THROWS(a.cast<const ivec2*>());
        CHECK_THROWS(a.cast<ivec2* const>());
        CHECK_THROWS(a.cast<const ivec2* const>());
    }

    SUBCASE("cref") {
        const ivec2 v{1,2};
        const ivec2& vr = v;
        meta::detail::arg a{vr};

        CHECK(a.cast<ivec2>() == v);
        CHECK(a.cast<const ivec2>() == v);
        CHECK_THROWS(a.cast<ivec2&>());
        CHECK(&a.cast<const ivec2&>() == &vr);
        CHECK_THROWS(a.cast<ivec2&&>());
        CHECK_THROWS(a.cast<const ivec2&&>());

        CHECK_THROWS(a.cast<ivec2*>());
        CHECK_THROWS(a.cast<const ivec2*>());
        CHECK_THROWS(a.cast<ivec2* const>());
        CHECK_THROWS(a.cast<const ivec2* const>());
    }

    SUBCASE("rref") {
        CHECK(meta::detail::arg{ivec2{1,2}}.cast<ivec2>() == ivec2{1,2});
        CHECK(meta::detail::arg{ivec2{1,2}}.cast<const ivec2>() == ivec2{1,2});
        CHECK_THROWS(meta::detail::arg{ivec2{1,2}}.cast<ivec2&>());
        CHECK(meta::detail::arg{ivec2{1,2}}.cast<const ivec2&>() == ivec2{1,2});
        CHECK(meta::detail::arg{ivec2{1,2}}.cast<ivec2&&>() == ivec2{1,2});
        CHECK(meta::detail::arg{ivec2{1,2}}.cast<const ivec2&&>() == ivec2{1,2});

        CHECK_THROWS(meta::detail::arg{ivec2{1,2}}.cast<ivec2*>());
        CHECK_THROWS(meta::detail::arg{ivec2{1,2}}.cast<const ivec2*>());
        CHECK_THROWS(meta::detail::arg{ivec2{1,2}}.cast<ivec2* const>());
        CHECK_THROWS(meta::detail::arg{ivec2{1,2}}.cast<const ivec2* const>());
    }

    SUBCASE("crref") {
        const ivec2 v{1,2};
        meta::detail::arg a{std::move(v)};

        CHECK(a.cast<ivec2>() == ivec2{1,2});
        CHECK(a.cast<const ivec2>() == ivec2{1,2});
        CHECK_THROWS(a.cast<ivec2&>());
        CHECK(&a.cast<const ivec2&>() == &v);
        CHECK_THROWS(a.cast<ivec2&&>());
        CHECK(a.cast<const ivec2&&>() == v);

        CHECK_THROWS(a.cast<ivec2*>());
        CHECK_THROWS(a.cast<const ivec2*>());
        CHECK_THROWS(a.cast<ivec2* const>());
        CHECK_THROWS(a.cast<const ivec2* const>());
    }
}
