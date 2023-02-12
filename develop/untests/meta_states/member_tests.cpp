/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct clazz_1 {
        int int_member = 1;
        const int const_int_member = 2;
        std::unique_ptr<int> unique_int_member = std::make_unique<int>(42);
    };

    struct clazz_2 {};
}

TEST_CASE("meta/meta_states/member/_") {
    namespace meta = meta_hpp;

    meta::class_<clazz_1>()
        .member_("int_member", &clazz_1::int_member)
        .member_("const_int_member", &clazz_1::const_int_member)
        // .member_("unique_int_member", &clazz_1::unique_int_member)
        .member_("unique_int_member_as_ptr", &clazz_1::unique_int_member, meta::member_policy::as_pointer)
        .member_("unique_int_member_as_ref", &clazz_1::unique_int_member, meta::member_policy::as_reference_wrapper);
}

TEST_CASE("meta/meta_states/member") {
    namespace meta = meta_hpp;

    const meta::class_type clazz_1_type = meta::resolve_type<clazz_1>();
    REQUIRE(clazz_1_type);

    SUBCASE("") {
        const meta::member member;
        CHECK_FALSE(member);
        CHECK_FALSE(member.is_valid());
        CHECK(member == clazz_1_type.get_member("non-existent-member"));
    }

    SUBCASE("operators") {
        meta::member int_member_m = clazz_1_type.get_member("int_member");
        meta::member const_int_member_m = clazz_1_type.get_member("const_int_member");
        CHECK(int_member_m == int_member_m);
        CHECK(int_member_m != const_int_member_m);
        CHECK((int_member_m < const_int_member_m || const_int_member_m < int_member_m));
    }

    SUBCASE("int") {
        meta::member vm = clazz_1_type.get_member("int_member");
        REQUIRE(vm);

        CHECK(vm.get_type() == meta::resolve_type(&clazz_1::int_member));
        CHECK(vm.get_name() == "int_member");

        clazz_1 v;
        clazz_2 v2;

        {
            CHECK(vm.is_gettable_with<clazz_1>());
            CHECK(vm.is_gettable_with<clazz_1*>());
            CHECK(vm.is_gettable_with<clazz_1&>());
            CHECK(vm.is_gettable_with<clazz_1&&>());
            CHECK(vm.is_gettable_with<const clazz_1>());
            CHECK(vm.is_gettable_with<const clazz_1*>());
            CHECK(vm.is_gettable_with<const clazz_1&>());
            CHECK(vm.is_gettable_with<const clazz_1&&>());

            CHECK(vm.is_gettable_with(v));
            CHECK(vm.is_gettable_with(&v));
            CHECK(vm.is_gettable_with(std::as_const(v)));
            CHECK(vm.is_gettable_with(&std::as_const(v)));
            CHECK(vm.is_gettable_with(std::move(v)));
            CHECK(vm.is_gettable_with(std::move(std::as_const(v))));

            CHECK_FALSE(vm.is_gettable_with<clazz_2>());
            CHECK_FALSE(vm.is_gettable_with<clazz_2*>());
            CHECK_FALSE(vm.is_gettable_with(v2));
            CHECK_FALSE(vm.is_gettable_with(&v2));
        }

        {
            CHECK(vm.get(v).get_as<int>() == 1);
            CHECK(vm.get(&v).get_as<int>() == 1);
            CHECK(vm.get(std::as_const(v)).get_as<int>() == 1);
            CHECK(vm.get(&std::as_const(v)).get_as<int>() == 1);
            CHECK(vm.get(std::move(v)).get_as<int>() == 1);
            CHECK(vm.get(std::move(std::as_const(v))).get_as<int>() == 1);

            CHECK(vm.get_as<int>(v) == 1);
            CHECK(vm.get_as<int>(&v) == 1);
            CHECK(vm.get_as<int>(std::as_const(v)) == 1);
            CHECK(vm.get_as<int>(&std::as_const(v)) == 1);
            CHECK(vm.get_as<int>(std::move(v)) == 1);
            CHECK(vm.get_as<int>(std::move(std::as_const(v))) == 1);

            CHECK(vm(v).get_as<int>() == 1);
            CHECK(vm(&v).get_as<int>() == 1);
            CHECK(vm(std::as_const(v)).get_as<int>() == 1);
            CHECK(vm(&std::as_const(v)).get_as<int>() == 1);
            CHECK(vm(std::move(v)).get_as<int>() == 1);
            CHECK(vm(std::move(std::as_const(v))).get_as<int>() == 1);

            CHECK_FALSE(vm.safe_get(v2));
            CHECK_FALSE(vm.safe_get(&v2));
        }

        {
            CHECK(vm.is_settable_with<clazz_1, int>());
            CHECK(vm.is_settable_with<clazz_1*, int>());
            CHECK(vm.is_settable_with<clazz_1&, int>());
            CHECK(vm.is_settable_with<clazz_1&&, int>());

            CHECK(vm.is_settable_with<clazz_1, int&&>());
            CHECK(vm.is_settable_with<clazz_1*, int&&>());
            CHECK(vm.is_settable_with<clazz_1&, int&&>());
            CHECK(vm.is_settable_with<clazz_1&&, int&&>());

            CHECK(vm.is_settable_with(v, 10));
            CHECK(vm.is_settable_with(&v, 10));
            CHECK(vm.is_settable_with(std::move(v), 12));

            CHECK_FALSE(vm.is_settable_with<clazz_1, float>());
            CHECK_FALSE(vm.is_settable_with<clazz_1*, float>());
            CHECK_FALSE(vm.is_settable_with<clazz_1&, float>());
            CHECK_FALSE(vm.is_settable_with<clazz_1&&, float>());

            CHECK_FALSE(vm.is_settable_with<const clazz_1, int>());
            CHECK_FALSE(vm.is_settable_with<const clazz_1*, int>());
            CHECK_FALSE(vm.is_settable_with<const clazz_1&, int>());
            CHECK_FALSE(vm.is_settable_with<const clazz_1&&, int>());

            CHECK_FALSE(vm.is_settable_with<clazz_2*, int>());
            CHECK_FALSE(vm.is_settable_with<clazz_2&, int>());

            CHECK_FALSE(vm.is_settable_with(v2, 10));
            CHECK_FALSE(vm.is_settable_with(&v2, 10));
            CHECK_FALSE(vm.is_settable_with(std::move(v2), 10));
            CHECK_FALSE(vm.is_settable_with(v, 10.0));
            CHECK_FALSE(vm.is_settable_with(&v, 10.0));
            CHECK_FALSE(vm.is_settable_with(std::move(v), 12.0));
        }

        {
            vm.set(v, 10); CHECK(vm.get(v).get_as<int>() == 10);
            vm.set(&v, 100); CHECK(vm.get(v).get_as<int>() == 100);
            CHECK_FALSE(vm.safe_set(std::as_const(v), 11)); CHECK(vm.get(v).get_as<int>() == 100);
            CHECK_FALSE(vm.safe_set(&std::as_const(v), 11)); CHECK(vm.get(v).get_as<int>() == 100);

            vm.set(std::move(v), 12); CHECK(vm.get(v).get_as<int>() == 12);
            CHECK_FALSE(vm.safe_set(std::move(std::as_const(v)), 13)); CHECK(vm.get(v).get_as<int>() == 12);

            vm(v, 13); CHECK(vm(v).get_as<int>() == 13);
            vm(&v, 130); CHECK(vm(v).get_as<int>() == 130);

            vm(std::move(v), 15); CHECK(vm(v).get_as<int>() == 15);

            CHECK_FALSE(vm.safe_set(v2, 17));
            CHECK_FALSE(vm.safe_set(&v2, 17));
            CHECK(vm(v).get_as<int>() == 15);
        }
    }

    SUBCASE("const int") {
        meta::member vm = clazz_1_type.get_member("const_int_member");
        REQUIRE(vm);

        CHECK(vm.get_type() == meta::resolve_type(&clazz_1::const_int_member));
        CHECK(vm.get_name() == "const_int_member");

        clazz_1 v;
        clazz_2 v2;

        {
            CHECK(vm.is_gettable_with<clazz_1>());
            CHECK(vm.is_gettable_with<clazz_1*>());
            CHECK(vm.is_gettable_with<clazz_1&>());
            CHECK(vm.is_gettable_with<clazz_1&&>());
            CHECK(vm.is_gettable_with<const clazz_1>());
            CHECK(vm.is_gettable_with<const clazz_1*>());
            CHECK(vm.is_gettable_with<const clazz_1&>());
            CHECK(vm.is_gettable_with<const clazz_1&&>());

            CHECK(vm.is_gettable_with(v));
            CHECK(vm.is_gettable_with(&v));
            CHECK(vm.is_gettable_with(std::as_const(v)));
            CHECK(vm.is_gettable_with(&std::as_const(v)));
            CHECK(vm.is_gettable_with(std::move(v)));
            CHECK(vm.is_gettable_with(std::move(std::as_const(v))));

            CHECK_FALSE(vm.is_gettable_with<clazz_2>());
            CHECK_FALSE(vm.is_gettable_with<clazz_2*>());
            CHECK_FALSE(vm.is_gettable_with(v2));
            CHECK_FALSE(vm.is_gettable_with(&v2));
        }

        {
            CHECK(vm.get(v).get_as<int>() == 2);
            CHECK(vm.get(&v).get_as<int>() == 2);
            CHECK(vm.get(std::as_const(v)).get_as<int>() == 2);
            CHECK(vm.get(&std::as_const(v)).get_as<int>() == 2);
            CHECK(vm.get(std::move(v)).get_as<int>() == 2);
            CHECK(vm.get(std::move(std::as_const(v))).get_as<int>() == 2);

            CHECK(vm(v).get_as<int>() == 2);
            CHECK(vm(&v).get_as<int>() == 2);
            CHECK(vm(std::as_const(v)).get_as<int>() == 2);
            CHECK(vm(&std::as_const(v)).get_as<int>() == 2);
            CHECK(vm(std::move(v)).get_as<int>() == 2);
            CHECK(vm(std::move(std::as_const(v))).get_as<int>() == 2);

            CHECK_FALSE(vm.safe_get(v2));
            CHECK_FALSE(vm.safe_get(&v2));
        }

        {
            CHECK_FALSE(vm.is_settable_with<clazz_1, int>());
            CHECK_FALSE(vm.is_settable_with<clazz_1*, int>());
            CHECK_FALSE(vm.is_settable_with<clazz_1&, int>());
            CHECK_FALSE(vm.is_settable_with<clazz_1&&, int>());

            CHECK_FALSE(vm.is_settable_with<clazz_1, int&&>());
            CHECK_FALSE(vm.is_settable_with<clazz_1*, int&&>());
            CHECK_FALSE(vm.is_settable_with<clazz_1&, int&&>());
            CHECK_FALSE(vm.is_settable_with<clazz_1&&, int&&>());

            CHECK_FALSE(vm.is_settable_with<clazz_1, float>());
            CHECK_FALSE(vm.is_settable_with<clazz_1*, float>());
            CHECK_FALSE(vm.is_settable_with<clazz_1&, float>());
            CHECK_FALSE(vm.is_settable_with<clazz_1&&, float>());

            CHECK_FALSE(vm.is_settable_with<const clazz_1, int>());
            CHECK_FALSE(vm.is_settable_with<const clazz_1*, int>());
            CHECK_FALSE(vm.is_settable_with<const clazz_1&, int>());
            CHECK_FALSE(vm.is_settable_with<const clazz_1&&, int>());

            CHECK_FALSE(vm.is_settable_with(v, 10));
            CHECK_FALSE(vm.is_settable_with(&v, 10));
            CHECK_FALSE(vm.is_settable_with(std::move(v), 12));

            CHECK_FALSE(vm.is_settable_with<clazz_2*, int>());
            CHECK_FALSE(vm.is_settable_with<clazz_2&, int>());
            CHECK_FALSE(vm.is_settable_with(v2, 10));
            CHECK_FALSE(vm.is_settable_with(&v2, 10));
            CHECK_FALSE(vm.is_settable_with(std::move(v2), 12));
        }

        {
            CHECK_FALSE(vm.safe_set(v, 10)); CHECK(vm.get(v).get_as<int>() == 2);
            CHECK_FALSE(vm.safe_set(&v, 10)); CHECK(vm.get(v).get_as<int>() == 2);
            CHECK_FALSE(vm.safe_set(std::as_const(v), 11)); CHECK(vm.get(v).get_as<int>() == 2);
            CHECK_FALSE(vm.safe_set(&std::as_const(v), 11)); CHECK(vm.get(v).get_as<int>() == 2);
            CHECK_FALSE(vm.safe_set(std::move(v), 12)); CHECK(vm.get(v).get_as<int>() == 2);
            CHECK_FALSE(vm.safe_set(std::move(std::as_const(v)), 16)); CHECK(vm.get(v).get_as<int>() == 2);

            CHECK_FALSE(vm.safe_set(v2, 17));
            CHECK_FALSE(vm.safe_set(&v2, 17));
            CHECK(vm(v).get_as<int>() == 2);
        }
    }

    SUBCASE("unique_int_member_as_ptr") {
        meta::member vm = clazz_1_type.get_member("unique_int_member_as_ptr");
        REQUIRE(vm);

        CHECK(vm.get_type() == meta::resolve_type(&clazz_1::unique_int_member));
        CHECK(vm.get_name() == "unique_int_member_as_ptr");

        {
            clazz_1 v;
            CHECK(vm.get(v).get_type() == meta::resolve_type<std::unique_ptr<int>*>());
            CHECK(vm.get_as<std::unique_ptr<int>*>(v) == std::addressof(v.unique_int_member));
        }

        {
            const clazz_1 v;
            CHECK(vm.get(v).get_type() == meta::resolve_type<const std::unique_ptr<int>*>());
            CHECK(vm.get_as<const std::unique_ptr<int>*>(v) == std::addressof(v.unique_int_member));
        }
    }

    SUBCASE("unique_int_member_as_ref") {
        meta::member vm = clazz_1_type.get_member("unique_int_member_as_ref");
        REQUIRE(vm);

        CHECK(vm.get_type() == meta::resolve_type(&clazz_1::unique_int_member));
        CHECK(vm.get_name() == "unique_int_member_as_ref");

        {
            clazz_1 v;
            using ref_t = std::reference_wrapper<std::unique_ptr<int>>;
            CHECK(vm.get(v).get_type() == meta::resolve_type<ref_t>());
            CHECK(vm.get_as<ref_t>(v).get() == v.unique_int_member);
        }

        {
            const clazz_1 v;
            using ref_t = std::reference_wrapper<const std::unique_ptr<int>>;
            CHECK(vm.get(v).get_type() == meta::resolve_type<ref_t>());
            CHECK(vm.get_as<ref_t>(v).get() == v.unique_int_member);
        }
    }

    SUBCASE("safe_get") {
        meta::member vm = clazz_1_type.get_member("int_member");
        REQUIRE(vm);

        clazz_1 v;
        clazz_2 v2;

        CHECK(vm.safe_get(v));
        CHECK(vm.safe_get(v)->get_as<int>() == 1);
        CHECK_FALSE(vm.safe_get(v2));
    }

    SUBCASE("safe_get_as") {
        meta::member vm = clazz_1_type.get_member("int_member");
        REQUIRE(vm);

        clazz_1 v;
        clazz_2 v2;

        CHECK(vm.safe_get_as<int>(v) == 1);
        CHECK_FALSE(vm.safe_get_as<double>(v));

        CHECK_FALSE(vm.safe_get_as<int>(v2));
        CHECK_FALSE(vm.safe_get_as<double>(v2));
    }

    SUBCASE("safe_set") {
        meta::member vm = clazz_1_type.get_member("int_member");
        REQUIRE(vm);

        clazz_1 v;
        clazz_2 v2;

        CHECK(vm.safe_set(v, 10));
        CHECK(vm.get_as<int>(v) == 10);

        CHECK_FALSE(vm.safe_set(v, 20.0));
        CHECK_FALSE(vm.safe_set(v2, 20));
        CHECK(vm.get_as<int>(v) == 10);
    }
}
