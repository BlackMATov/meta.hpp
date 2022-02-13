/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
    struct base {
        base() = default;
        base(const base&) = default;
        virtual ~base() = default;
        virtual int int_method() const = 0;
    };

    struct clazz : base {
        int int_member{42};
        int int_method() const override { return int_member; }
    };

    using int_member_t = int clazz::*;
    using int_method_t = int (clazz::*)() const;

    int func_with_member(const clazz& cl, int_member_t m) {
        return cl.*m;
    }

    int func_with_method(const clazz& cl, int_method_t m) {
        return (cl.*m)();
    }
}

TEST_CASE("meta/meta_utilities/arg6") {
    namespace meta = meta_hpp;
    using meta::detail::uarg;

    const meta::scope scope = meta::local_scope_("scope")
        .function_("func_with_member", &func_with_member)
        .function_("func_with_method", &func_with_method);

    SUBCASE("int_member") {
        const meta::function f = scope.get_function("func_with_member");
        REQUIRE(f);

        clazz cl;
        CHECK(f.is_invocable_with<clazz&, int_member_t>());

        CHECK(f.is_invocable_with(cl, &clazz::int_member));
        CHECK(f.invoke(cl, &clazz::int_member) == 42);

        CHECK(f.is_invocable_with(meta::uvalue{cl}, meta::uvalue{&clazz::int_member}));
        CHECK(f.invoke(meta::uvalue{cl}, meta::uvalue{&clazz::int_member}) == 42);
    }

    SUBCASE("int_method") {
        const meta::function f = scope.get_function("func_with_method");
        REQUIRE(f);

        clazz cl;
        CHECK(f.is_invocable_with<clazz&, int_method_t>());

        CHECK(f.is_invocable_with(cl, &clazz::int_method));
        CHECK(f.invoke(cl, &clazz::int_method) == 42);

        CHECK(f.is_invocable_with(meta::uvalue{cl}, meta::uvalue{&clazz::int_method}));
        CHECK(f.invoke(meta::uvalue{cl}, meta::uvalue{&clazz::int_method}) == 42);
    }
}
