/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

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
        CHECK(meta::invoke(&clazz::function, 3) == 3);
        CHECK(meta::invoke(&clazz::function, meta::uvalue(3)) == 3);
        CHECK(meta::invoke(clazz_function, 3) == 3);
        CHECK(meta::invoke(clazz_function, meta::uvalue(3)) == 3);

        CHECK(meta::is_invocable_with(clazz_function, 3));
        CHECK(meta::is_invocable_with(clazz_function, meta::uvalue(3)));
        CHECK(meta::is_invocable_with<int>(clazz_function));

        using function_t = decltype(&clazz::function);
        CHECK(meta::is_invocable_with<function_t>(3));
        CHECK(meta::is_invocable_with<function_t>(meta::uvalue(3)));
        CHECK(meta::is_invocable_with<function_t, int>());
    }

    {
        clazz cl;

        CHECK(meta::invoke(&clazz::member, cl) == 1);
        CHECK(meta::invoke(&clazz::member, meta::uvalue{cl}) == 1);
        CHECK(meta::invoke(clazz_member, cl) == 1);
        CHECK(meta::invoke(clazz_member, meta::uvalue{cl}) == 1);

        CHECK(meta::is_invocable_with(clazz_member, cl));
        CHECK(meta::is_invocable_with(clazz_member, meta::uvalue{cl}));
        CHECK(meta::is_invocable_with<const clazz&>(clazz_member));

        using member_t = decltype(&clazz::member);
        CHECK(meta::is_invocable_with<member_t>(cl));
        CHECK(meta::is_invocable_with<member_t>(meta::uvalue{cl}));
        CHECK(meta::is_invocable_with<member_t, const clazz&>());
    }

    {
        clazz cl;

        CHECK(meta::invoke(&clazz::method, cl, 2) == 2);
        CHECK(meta::invoke(&clazz::method, meta::uvalue{cl}, meta::uvalue(2)) == 2);
        CHECK(meta::invoke(clazz_method, cl, 2) == 2);
        CHECK(meta::invoke(clazz_method, meta::uvalue{cl}, meta::uvalue(2)) == 2);

        CHECK(meta::is_invocable_with(clazz_method, cl, 2));
        CHECK(meta::is_invocable_with(clazz_method, meta::uvalue{cl}, meta::uvalue(2)));
        CHECK(meta::is_invocable_with<const clazz&, int>(clazz_method));

        using method_t = decltype(&clazz::method);
        CHECK(meta::is_invocable_with<method_t>(cl, 2));
        CHECK(meta::is_invocable_with<method_t>(meta::uvalue{cl}, meta::uvalue(2)));
        CHECK(meta::is_invocable_with<method_t, const clazz&, int>());
    }
}
