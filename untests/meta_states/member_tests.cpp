/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
    struct clazz_1 {
        int int_member = 1;
        const int const_int_member = 2;
    };

    struct clazz_2 {};
}

TEST_CASE("meta/meta_states/member") {
    namespace meta = meta_hpp;

    meta::class_<clazz_1>()
        .member_("int_member", &clazz_1::int_member)
        .member_("const_int_member", &clazz_1::const_int_member);

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
            CHECK(vm.is_gettable_with<clazz_1&>());
            CHECK(vm.is_gettable_with<clazz_1&&>());
            CHECK(vm.is_gettable_with<const clazz_1>());
            CHECK(vm.is_gettable_with<const clazz_1&>());
            CHECK(vm.is_gettable_with<const clazz_1&&>());

            CHECK(vm.is_gettable_with(v));
            CHECK(vm.is_gettable_with(std::as_const(v)));
            CHECK(vm.is_gettable_with(std::move(v)));
            CHECK(vm.is_gettable_with(std::move(std::as_const(v))));

            CHECK_FALSE(vm.is_gettable_with<clazz_2>());
            CHECK_FALSE(vm.is_gettable_with(v2));
        }

        {
            CHECK(vm.get(v) == 1);
            CHECK(vm.get(std::as_const(v)) == 1);
            CHECK(vm.get(std::move(v)) == 1);
            CHECK(vm.get(std::move(std::as_const(v))) == 1);

            CHECK(vm(v) == 1);
            CHECK(vm(std::as_const(v)) == 1);
            CHECK(vm(std::move(v)) == 1);
            CHECK(vm(std::move(std::as_const(v))) == 1);

            CHECK_THROWS(std::ignore = vm.get(v2));
            CHECK_THROWS(std::ignore = vm(v2));
        }

        {
            CHECK(vm.is_settable_with<clazz_1, int>());
            CHECK(vm.is_settable_with<clazz_1&, int>());
            CHECK(vm.is_settable_with<clazz_1&&, int>());

            CHECK(vm.is_settable_with<clazz_1, int&&>());
            CHECK(vm.is_settable_with<clazz_1&, int&&>());
            CHECK(vm.is_settable_with<clazz_1&&, int&&>());

            CHECK(vm.is_settable_with(v, 10));
            CHECK(vm.is_settable_with(std::move(v), 12));

            CHECK_FALSE(vm.is_settable_with<clazz_1, float>());
            CHECK_FALSE(vm.is_settable_with<clazz_1&, float>());
            CHECK_FALSE(vm.is_settable_with<clazz_1&&, float>());

            CHECK_FALSE(vm.is_settable_with<const clazz_1, int>());
            CHECK_FALSE(vm.is_settable_with<const clazz_1&, int>());
            CHECK_FALSE(vm.is_settable_with<const clazz_1&&, int>());

            CHECK_FALSE(vm.is_settable_with<clazz_2&, int>());

            CHECK_FALSE(vm.is_settable_with(v2, 10));
            CHECK_FALSE(vm.is_settable_with(std::move(v2), 10));
            CHECK_FALSE(vm.is_settable_with(v, 10.0));
            CHECK_FALSE(vm.is_settable_with(std::move(v), 12.0));
        }

        {
            CHECK_NOTHROW(vm.set(v, 10)); CHECK(vm.get(v) == 10);
            CHECK_THROWS(vm.set(std::as_const(v), 11)); CHECK(vm.get(v) == 10);
            CHECK_NOTHROW(vm.set(std::move(v), 12)); CHECK(vm.get(v) == 12);
            CHECK_THROWS(vm.set(std::move(std::as_const(v)), 13)); CHECK(vm.get(v) == 12);

            CHECK_NOTHROW(vm(v, 13)); CHECK(vm(v) == 13);
            CHECK_THROWS(vm(std::as_const(v), 14)); CHECK(vm(v) == 13);
            CHECK_NOTHROW(vm(std::move(v), 15)); CHECK(vm(v) == 15);
            CHECK_THROWS(vm(std::move(std::as_const(v)), 16)); CHECK(vm(v) == 15);

            CHECK_THROWS(vm.set(v2, 17));
            CHECK_THROWS(vm(v2, 17));
            CHECK(vm(v) == 15);
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
            CHECK(vm.is_gettable_with<clazz_1&>());
            CHECK(vm.is_gettable_with<clazz_1&&>());
            CHECK(vm.is_gettable_with<const clazz_1>());
            CHECK(vm.is_gettable_with<const clazz_1&>());
            CHECK(vm.is_gettable_with<const clazz_1&&>());

            CHECK(vm.is_gettable_with(v));
            CHECK(vm.is_gettable_with(std::as_const(v)));
            CHECK(vm.is_gettable_with(std::move(v)));
            CHECK(vm.is_gettable_with(std::move(std::as_const(v))));

            CHECK_FALSE(vm.is_gettable_with<clazz_2>());
            CHECK_FALSE(vm.is_gettable_with(v2));
        }

        {
            CHECK(vm.get(v) == 2);
            CHECK(vm.get(std::as_const(v)) == 2);
            CHECK(vm.get(std::move(v)) == 2);
            CHECK(vm.get(std::move(std::as_const(v))) == 2);

            CHECK(vm(v) == 2);
            CHECK(vm(std::as_const(v)) == 2);
            CHECK(vm(std::move(v)) == 2);
            CHECK(vm(std::move(std::as_const(v))) == 2);

            CHECK_THROWS(std::ignore = vm.get(v2));
            CHECK_THROWS(std::ignore = vm(v2));
        }

        {
            CHECK_FALSE(vm.is_settable_with<clazz_1, int>());
            CHECK_FALSE(vm.is_settable_with<clazz_1&, int>());
            CHECK_FALSE(vm.is_settable_with<clazz_1&&, int>());

            CHECK_FALSE(vm.is_settable_with<clazz_1, int&&>());
            CHECK_FALSE(vm.is_settable_with<clazz_1&, int&&>());
            CHECK_FALSE(vm.is_settable_with<clazz_1&&, int&&>());

            CHECK_FALSE(vm.is_settable_with<clazz_1, float>());
            CHECK_FALSE(vm.is_settable_with<clazz_1&, float>());
            CHECK_FALSE(vm.is_settable_with<clazz_1&&, float>());

            CHECK_FALSE(vm.is_settable_with<const clazz_1, int>());
            CHECK_FALSE(vm.is_settable_with<const clazz_1&, int>());
            CHECK_FALSE(vm.is_settable_with<const clazz_1&&, int>());

            CHECK_FALSE(vm.is_settable_with(v, 10));
            CHECK_FALSE(vm.is_settable_with(std::move(v), 12));

            CHECK_FALSE(vm.is_settable_with<clazz_2&, int>());
            CHECK_FALSE(vm.is_settable_with(v2, 10));
            CHECK_FALSE(vm.is_settable_with(std::move(v2), 12));
        }

        {
            CHECK_THROWS(vm.set(v, 10)); CHECK(vm.get(v) == 2);
            CHECK_THROWS(vm.set(std::as_const(v), 11)); CHECK(vm.get(v) == 2);
            CHECK_THROWS(vm.set(std::move(v), 12)); CHECK(vm.get(v) == 2);
            CHECK_THROWS(vm.set(std::move(std::as_const(v)), 16)); CHECK(vm.get(v) == 2);

            CHECK_THROWS(vm(v, 13)); CHECK(vm(v) == 2);
            CHECK_THROWS(vm(std::as_const(v), 14)); CHECK(vm(v) == 2);
            CHECK_THROWS(vm(std::move(v), 15)); CHECK(vm(v) == 2);
            CHECK_THROWS(vm(std::move(std::as_const(v)), 16)); CHECK(vm(v) == 2);

            CHECK_THROWS(vm.set(v2, 17));
            CHECK_THROWS(vm(v2, 17));
            CHECK(vm(v) == 2);
        }
    }
}
