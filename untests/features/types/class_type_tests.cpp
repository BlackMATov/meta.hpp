/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "_types_fwd.hpp"

namespace
{
    using namespace meta_hpp;
    using namespace std::string_literals;
}

namespace
{
    struct base {
        virtual ~base() = 0;
    };

    struct derived final : base {
    };

    struct empty final {
    };

    struct ivec2 final {
        int x{};
        int y{};
    };
}

TEST_CASE("features/types/class") {
    SUBCASE("base") {
        using type = base;

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<class_type>());

        const class_type ct = type_db::get<type>().as<class_type>();
        CHECK(ct.id() == type_id{typename_arg<base_type::tag<type>>});

        CHECK_FALSE(ct.raw_type());
        CHECK(ct.size() == sizeof(type));

        CHECK(ct.flags() == (
            class_flags::is_abstract |
            class_flags::is_polymorphic));

        CHECK_FALSE(ct.is_const());
        CHECK_FALSE(ct.is_empty());
        CHECK_FALSE(ct.is_final());
        CHECK(ct.is_abstract());
        CHECK(ct.is_polymorphic());
    }

    SUBCASE("derived") {
        using type = derived;

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<class_type>());

        const class_type ct = type_db::get<type>().as<class_type>();
        CHECK(ct.id() == type_id{typename_arg<base_type::tag<type>>});

        CHECK_FALSE(ct.raw_type());
        CHECK(ct.size() == sizeof(type));

        CHECK(ct.flags() == (
            class_flags::is_final |
            class_flags::is_polymorphic));

        CHECK_FALSE(ct.is_const());
        CHECK_FALSE(ct.is_empty());
        CHECK(ct.is_final());
        CHECK_FALSE(ct.is_abstract());
        CHECK(ct.is_polymorphic());
    }

    SUBCASE("empty") {
        using type = empty;

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<class_type>());

        const class_type ct = type_db::get<type>().as<class_type>();
        CHECK(ct.id() == type_id{typename_arg<base_type::tag<type>>});

        CHECK_FALSE(ct.raw_type());
        CHECK(ct.size() == sizeof(type));

        CHECK(ct.flags() == (
            class_flags::is_empty |
            class_flags::is_final));

        CHECK_FALSE(ct.is_const());
        CHECK(ct.is_empty());
        CHECK(ct.is_final());
        CHECK_FALSE(ct.is_abstract());
        CHECK_FALSE(ct.is_polymorphic());
    }

    SUBCASE("ivec2") {
        using type = ivec2;

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<class_type>());

        const class_type ct = type_db::get<type>().as<class_type>();
        CHECK(ct.id() == type_id{typename_arg<base_type::tag<type>>});

        CHECK_FALSE(ct.raw_type());
        CHECK(ct.size() == sizeof(type));

        CHECK(ct.flags() == (
            class_flags::is_final));

        CHECK_FALSE(ct.is_const());
        CHECK_FALSE(ct.is_empty());
        CHECK(ct.is_final());
        CHECK_FALSE(ct.is_abstract());
        CHECK_FALSE(ct.is_polymorphic());
    }

    SUBCASE("const ivec2") {
        using type = const ivec2;

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<class_type>());

        const class_type ct = type_db::get<type>().as<class_type>();
        CHECK(ct.id() == type_id{typename_arg<base_type::tag<type>>});

        REQUIRE(ct.raw_type());
        CHECK(ct.raw_type().id() == type_db::get<ivec2>().id());
        CHECK(ct.size() == sizeof(type));

        CHECK(ct.flags() == (
            class_flags::is_const |
            class_flags::is_final));

        CHECK(ct.is_const());
        CHECK_FALSE(ct.is_empty());
        CHECK(ct.is_final());
        CHECK_FALSE(ct.is_abstract());
        CHECK_FALSE(ct.is_polymorphic());
    }
}
