/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    template < auto v0 >
    struct v {};
    template < typename t0 >
    struct t {};

    template < auto v1, auto v0 >
    struct vv {};
    template < auto v0, typename t0 >
    struct vt {};
    template < typename t0, auto v0 >
    struct tv {};
    template < typename t1, typename t0 >
    struct tt {};

    template < auto v2, auto v1, auto v0 >
    struct vvv {};
    template < auto v1, auto v0, typename t0 >
    struct vvt {};
    template < auto v1, typename t0, auto v0 >
    struct vtv {};
    template < auto v0, typename t1, typename t0 >
    struct vtt {};
    template < typename t0, auto v1, auto v0 >
    struct tvv {};
    template < typename t1, auto v0, typename t0 >
    struct tvt {};
    template < typename t1, typename t0, auto v0 >
    struct ttv {};
    template < typename t2, typename t1, typename t0 >
    struct ttt {};

    template < auto v2, auto v1, auto v0, typename t0, typename t1 >
    struct vvvtt {};
    template < typename t2, typename t1, typename t0, auto v0, auto v1 >
    struct tttvv {};
}

TEST_CASE("meta/meta_issues/random/6") {
    namespace meta = meta_hpp;

    {
        meta::class_type type = meta::resolve_type<v<42>>();
        CHECK(type.get_argument_types() == meta::any_type_list{
            meta::resolve_type<int>()});
        CHECK(type.get_argument_value(0).is<int>());
    }
    {
        meta::class_type type = meta::resolve_type<t<int>>();
        CHECK(type.get_argument_types() == meta::any_type_list{
            meta::resolve_type<int>()});
        CHECK_FALSE(type.get_argument_value(0));
    }
    //
    {
        meta::class_type type = meta::resolve_type<vv<42, 21u>>();
        CHECK(type.get_argument_types() == meta::any_type_list{
            meta::resolve_type<int>(),
            meta::resolve_type<unsigned>()});
        CHECK(type.get_argument_value(0).is<int>());
        CHECK(type.get_argument_value(1).is<unsigned>());
    }
    {
        meta::class_type type = meta::resolve_type<vt<42, unsigned>>();
        CHECK(type.get_argument_types() == meta::any_type_list{
            meta::resolve_type<int>(),
            meta::resolve_type<unsigned>()});
        CHECK(type.get_argument_value(0).is<int>());
        CHECK_FALSE(type.get_argument_value(1).has_value());
    }
    {
        meta::class_type type = meta::resolve_type<tv<int, 42u>>();
        CHECK(type.get_argument_types() == meta::any_type_list{
            meta::resolve_type<int>(),
            meta::resolve_type<unsigned>()});
        CHECK_FALSE(type.get_argument_value(0).has_value());
        CHECK(type.get_argument_value(1).is<unsigned>());
    }
    {
        meta::class_type type = meta::resolve_type<tt<int, unsigned>>();
        CHECK(type.get_argument_types() == meta::any_type_list{
            meta::resolve_type<int>(),
            meta::resolve_type<unsigned>()});
        CHECK_FALSE(type.get_argument_value(0).has_value());
        CHECK_FALSE(type.get_argument_value(1).has_value());
    }
    //
    {
        meta::class_type type = meta::resolve_type<vvv<std::int32_t{42}, std::uint32_t{21}, std::int64_t{10}>>();
        CHECK(type.get_argument_types() == meta::any_type_list{
            meta::resolve_type<std::int32_t>(),
            meta::resolve_type<std::uint32_t>(),
            meta::resolve_type<std::int64_t>(),
        });
        CHECK(type.get_argument_value(0).as<std::int32_t>() == 42);
        CHECK(type.get_argument_value(1).as<std::uint32_t>() == 21);
        CHECK(type.get_argument_value(2).as<std::int64_t>() == 10);
    }
    {
        meta::class_type type = meta::resolve_type<vvt<std::int32_t{42}, std::uint32_t{21}, float>>();
        CHECK(type.get_argument_types() == meta::any_type_list{
            meta::resolve_type<std::int32_t>(),
            meta::resolve_type<std::uint32_t>(),
            meta::resolve_type<float>(),
        });
        CHECK(type.get_argument_value(0).as<std::int32_t>() == 42);
        CHECK(type.get_argument_value(1).as<std::uint32_t>() == 21);
        CHECK_FALSE(type.get_argument_value(2).has_value());
    }
    {
        meta::class_type type = meta::resolve_type<vtv<std::int32_t{42}, float, std::uint32_t{21}>>();
        CHECK(type.get_argument_types() == meta::any_type_list{
            meta::resolve_type<std::int32_t>(),
            meta::resolve_type<float>(),
            meta::resolve_type<std::uint32_t>(),
        });
        CHECK(type.get_argument_value(0).as<std::int32_t>() == 42);
        CHECK_FALSE(type.get_argument_value(1).has_value());
        CHECK(type.get_argument_value(2).as<std::uint32_t>() == 21);
    }
    {
        meta::class_type type = meta::resolve_type<vtt<std::int32_t{42}, float, double>>();
        CHECK(type.get_argument_types() == meta::any_type_list{
            meta::resolve_type<std::int32_t>(),
            meta::resolve_type<float>(),
            meta::resolve_type<double>(),
        });
        CHECK(type.get_argument_value(0).as<std::int32_t>() == 42);
        CHECK_FALSE(type.get_argument_value(1).has_value());
        CHECK_FALSE(type.get_argument_value(2).has_value());
    }
    {
        meta::class_type type = meta::resolve_type<tvv<float, std::int32_t{42}, std::uint32_t{21}>>();
        CHECK(type.get_argument_types() == meta::any_type_list{
            meta::resolve_type<float>(),
            meta::resolve_type<std::int32_t>(),
            meta::resolve_type<std::uint32_t>(),
        });
        CHECK_FALSE(type.get_argument_value(0).has_value());
        CHECK(type.get_argument_value(1).as<std::int32_t>() == 42);
        CHECK(type.get_argument_value(2).as<std::uint32_t>() == 21);
    }
    {
        meta::class_type type = meta::resolve_type<tvt<float, std::int32_t{42}, double>>();
        CHECK(type.get_argument_types() == meta::any_type_list{
            meta::resolve_type<float>(),
            meta::resolve_type<std::int32_t>(),
            meta::resolve_type<double>(),
        });
        CHECK_FALSE(type.get_argument_value(0).has_value());
        CHECK(type.get_argument_value(1).as<std::int32_t>() == 42);
        CHECK_FALSE(type.get_argument_value(2).has_value());
    }
    {
        meta::class_type type = meta::resolve_type<ttv<float, double, std::int32_t{42}>>();
        CHECK(type.get_argument_types() == meta::any_type_list{
            meta::resolve_type<float>(),
            meta::resolve_type<double>(),
            meta::resolve_type<std::int32_t>(),
        });
        CHECK_FALSE(type.get_argument_value(0).has_value());
        CHECK_FALSE(type.get_argument_value(1).has_value());
        CHECK(type.get_argument_value(2).as<std::int32_t>() == 42);
    }
    {
        meta::class_type type = meta::resolve_type<ttt<int, float, double>>();
        CHECK(type.get_argument_types() == meta::any_type_list{
            meta::resolve_type<int>(),
            meta::resolve_type<float>(),
            meta::resolve_type<double>(),
        });
        CHECK_FALSE(type.get_argument_value(0).has_value());
        CHECK_FALSE(type.get_argument_value(1).has_value());
        CHECK_FALSE(type.get_argument_value(2).has_value());
    }
    //
    {
        meta::class_type type = meta::resolve_type<vvvtt<std::int32_t{42}, std::uint32_t{21}, std::uint64_t{10}, float, double>>();
        CHECK(type.get_argument_types() == meta::any_type_list{
            meta::resolve_type<std::int32_t>(),
            meta::resolve_type<std::uint32_t>(),
            meta::resolve_type<std::uint64_t>(),
            meta::resolve_type<float>(),
            meta::resolve_type<double>(),
        });
        CHECK(type.get_argument_value(0).as<std::int32_t>() == 42);
        CHECK(type.get_argument_value(1).as<std::uint32_t>() == 21);
        CHECK(type.get_argument_value(2).as<std::uint64_t>() == 10);
        CHECK_FALSE(type.get_argument_value(3).has_value());
        CHECK_FALSE(type.get_argument_value(4).has_value());
    }
    {
        meta::class_type type = meta::resolve_type<tttvv<float, double, long double, std::int32_t{42}, std::uint32_t{21}>>();
        CHECK(type.get_argument_types() == meta::any_type_list{
            meta::resolve_type<float>(),
            meta::resolve_type<double>(),
            meta::resolve_type<long double>(),
            meta::resolve_type<std::int32_t>(),
            meta::resolve_type<std::uint32_t>(),
        });
        CHECK_FALSE(type.get_argument_value(0).has_value());
        CHECK_FALSE(type.get_argument_value(1).has_value());
        CHECK_FALSE(type.get_argument_value(2).has_value());
        CHECK(type.get_argument_value(3).as<std::int32_t>() == 42);
        CHECK(type.get_argument_value(4).as<std::uint32_t>() == 21);
    }
}
