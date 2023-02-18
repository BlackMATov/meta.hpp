/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct clazz {
        int member{1};
        int method(int i) const { return i; }
        static int function(int i) { return i; }
    };
}

TEST_CASE("meta/meta_utilities/invoke") {
    namespace meta = meta_hpp;

    meta::class_<clazz>()
        .member_("member", &clazz::member)
        .method_("method", &clazz::method)
        .function_("function", &clazz::function);

    const meta::class_type& clazz_type = meta::resolve_type<clazz>();
    REQUIRE(clazz_type);

    const meta::member& clazz_member = clazz_type.get_member("member");
    const meta::method& clazz_method = clazz_type.get_method("method");
    const meta::function& clazz_function = clazz_type.get_function("function");
    REQUIRE((clazz_member && clazz_method && clazz_function));

    {
        CHECK(meta::invoke(&clazz::function, 3).as<int>() == 3);
        CHECK(meta::invoke(&clazz::function, meta::uvalue{3}).as<int>() == 3);
        CHECK(meta::invoke(&clazz::function, meta::uresult{3}).as<int>() == 3);
        CHECK(meta::invoke(clazz_function, 3).as<int>() == 3);
        CHECK(meta::invoke(clazz_function, meta::uvalue{3}).as<int>() == 3);
        CHECK(meta::invoke(clazz_function, meta::uresult{3}).as<int>() == 3);

        CHECK(meta::is_invocable_with(clazz_function, 3));
        CHECK(meta::is_invocable_with(clazz_function, meta::uvalue{3}));
        CHECK(meta::is_invocable_with(clazz_function, meta::uresult{3}));
        CHECK(meta::is_invocable_with<int>(clazz_function));

        CHECK(meta::is_invocable_with(&clazz::function, 3));
        CHECK(meta::is_invocable_with(&clazz::function, meta::uvalue{3}));
        CHECK(meta::is_invocable_with(&clazz::function, meta::uresult{3}));
        CHECK(meta::is_invocable_with<int>(&clazz::function));
    }

    {
        clazz cl;

        CHECK(meta::invoke(&clazz::member, cl).as<int>() == 1);
        CHECK(meta::invoke(&clazz::member, meta::uvalue{cl}).as<int>() == 1);
        CHECK(meta::invoke(&clazz::member, meta::uresult{cl}).as<int>() == 1);
        CHECK(meta::invoke(clazz_member, cl).as<int>() == 1);
        CHECK(meta::invoke(clazz_member, meta::uvalue{cl}).as<int>() == 1);
        CHECK(meta::invoke(clazz_member, meta::uresult{cl}).as<int>() == 1);

        CHECK(meta::is_invocable_with(clazz_member, cl));
        CHECK(meta::is_invocable_with(clazz_member, meta::uvalue{cl}));
        CHECK(meta::is_invocable_with(clazz_member, meta::uresult{cl}));
        CHECK(meta::is_invocable_with<const clazz&>(clazz_member));

        CHECK(meta::is_invocable_with(&clazz::member, cl));
        CHECK(meta::is_invocable_with(&clazz::member, meta::uvalue{cl}));
        CHECK(meta::is_invocable_with(&clazz::member, meta::uresult{cl}));
        CHECK(meta::is_invocable_with<const clazz&>(&clazz::member));
    }

    {
        clazz cl;

        CHECK(meta::invoke(&clazz::method, cl, 2).as<int>() == 2);
        CHECK(meta::invoke(&clazz::method, meta::uvalue{cl}, meta::uvalue{2}).as<int>() == 2);
        CHECK(meta::invoke(&clazz::method, meta::uresult{cl}, meta::uresult{2}).as<int>() == 2);
        CHECK(meta::invoke(clazz_method, cl, 2).as<int>() == 2);
        CHECK(meta::invoke(clazz_method, meta::uvalue{cl}, meta::uvalue{2}).as<int>() == 2);
        CHECK(meta::invoke(clazz_method, meta::uresult{cl}, meta::uresult{2}).as<int>() == 2);

        CHECK(meta::is_invocable_with(clazz_method, cl, 2));
        CHECK(meta::is_invocable_with(clazz_method, meta::uvalue{cl}, meta::uvalue{2}));
        CHECK(meta::is_invocable_with(clazz_method, meta::uresult{cl}, meta::uresult{2}));
        CHECK(meta::is_invocable_with<const clazz&, int>(clazz_method));

        CHECK(meta::is_invocable_with(&clazz::method, cl, 2));
        CHECK(meta::is_invocable_with(&clazz::method, meta::uvalue{cl}, meta::uvalue{2}));
        CHECK(meta::is_invocable_with(&clazz::method, meta::uresult{cl}, meta::uresult{2}));
        CHECK(meta::is_invocable_with<const clazz&, int>(&clazz::method));
    }
}
