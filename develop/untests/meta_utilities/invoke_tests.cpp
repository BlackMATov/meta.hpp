/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct ivec2 {
        int x{};
        int y{};
    };

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

    std::vector<meta::uvalue> params;
    params.push_back(3);
    std::vector<meta::uvalue> wrong_params;
    wrong_params.push_back(ivec2{});
    wrong_params.push_back(ivec2{});
    std::vector<int> typed_params;
    typed_params.push_back(3);

    {
        CHECK(meta::invoke(&clazz::function, 3).as<int>() == 3);
        CHECK(meta::invoke(&clazz::function, meta::uvalue{3}).as<int>() == 3);
        CHECK(meta::invoke(&clazz::function, meta::uresult{3}).as<int>() == 3);
        CHECK(meta::invoke(clazz_function, 3).as<int>() == 3);
        CHECK(meta::invoke(clazz_function, meta::uvalue{3}).as<int>() == 3);
        CHECK(meta::invoke(clazz_function, meta::uresult{3}).as<int>() == 3);

        CHECK(meta::try_invoke(&clazz::function, 3)->as<int>() == 3);
        CHECK(meta::try_invoke(&clazz::function, meta::uvalue{3})->as<int>() == 3);
        CHECK(meta::try_invoke(&clazz::function, meta::uresult{3})->as<int>() == 3);
        CHECK_FALSE(meta::try_invoke(&clazz::function, ivec2{}));
        CHECK_FALSE(meta::try_invoke(&clazz::function, meta::uvalue{ivec2{}}));
        CHECK(meta::try_invoke(clazz_function, 3)->as<int>() == 3);
        CHECK(meta::try_invoke(clazz_function, meta::uvalue{3})->as<int>() == 3);
        CHECK(meta::try_invoke(clazz_function, meta::uresult{3})->as<int>() == 3);
        CHECK_FALSE(meta::try_invoke(clazz_function, ivec2{}));
        CHECK_FALSE(meta::try_invoke(clazz_function, meta::uvalue{ivec2{}}));

        CHECK(meta::is_invocable_with(clazz_function, 3));
        CHECK(meta::is_invocable_with(clazz_function, meta::uvalue{3}));
        CHECK(meta::is_invocable_with(clazz_function, meta::uresult{3}));
        CHECK_FALSE(meta::is_invocable_with(clazz_function, ivec2{}));
        CHECK_FALSE(meta::is_invocable_with(clazz_function, meta::uvalue{ivec2{}}));
        CHECK(meta::is_invocable_with<int>(clazz_function));
        CHECK_FALSE(meta::is_invocable_with<ivec2>(clazz_function));

        CHECK(meta::is_invocable_with(&clazz::function, 3));
        CHECK(meta::is_invocable_with(&clazz::function, meta::uvalue{3}));
        CHECK(meta::is_invocable_with(&clazz::function, meta::uresult{3}));
        CHECK_FALSE(meta::is_invocable_with(&clazz::function, ivec2{}));
        CHECK_FALSE(meta::is_invocable_with(&clazz::function, meta::uvalue{ivec2{}}));
        CHECK(meta::is_invocable_with<int>(&clazz::function));
        CHECK_FALSE(meta::is_invocable_with<ivec2>(&clazz::function));
    }

    {
        CHECK(meta::invoke_variadic(&clazz::function, params.begin(), params.end()).as<int>() == 3);
        CHECK(meta::invoke_variadic(&clazz::function, typed_params.begin(), typed_params.end()).as<int>() == 3);
        CHECK(meta::invoke_variadic(clazz_function, params.begin(), params.end()).as<int>() == 3);
        CHECK(meta::invoke_variadic(clazz_function, typed_params.begin(), typed_params.end()).as<int>() == 3);

        CHECK((*meta::try_invoke_variadic(&clazz::function, params.begin(), params.end())).as<int>() == 3);
        CHECK((*meta::try_invoke_variadic(&clazz::function, typed_params.begin(), typed_params.end())).as<int>() == 3);
        CHECK_FALSE(meta::try_invoke_variadic(&clazz::function, wrong_params.begin(), wrong_params.end()));
        CHECK((*meta::try_invoke_variadic(clazz_function, params.begin(), params.end())).as<int>() == 3);
        CHECK((*meta::try_invoke_variadic(clazz_function, typed_params.begin(), typed_params.end())).as<int>() == 3);
        CHECK_FALSE(meta::try_invoke_variadic(clazz_function, wrong_params.begin(), wrong_params.end()));

        CHECK(meta::is_variadic_invocable_with(&clazz::function, params.begin(), params.end()));
        CHECK(meta::is_variadic_invocable_with(&clazz::function, typed_params.begin(), typed_params.end()));
        CHECK_FALSE(meta::is_variadic_invocable_with(&clazz::function, wrong_params.begin(), wrong_params.end()));
        CHECK(meta::is_variadic_invocable_with(clazz_function, params.begin(), params.end()));
        CHECK(meta::is_variadic_invocable_with(clazz_function, typed_params.begin(), typed_params.end()));
        CHECK_FALSE(meta::is_variadic_invocable_with(clazz_function, wrong_params.begin(), wrong_params.end()));
    }

    {
        clazz cl;

        CHECK(meta::invoke(&clazz::member, cl).as<int>() == 1);
        CHECK(meta::invoke(&clazz::member, meta::uvalue{cl}).as<int>() == 1);
        CHECK(meta::invoke(&clazz::member, meta::uresult{cl}).as<int>() == 1);
        CHECK(meta::invoke(clazz_member, cl).as<int>() == 1);
        CHECK(meta::invoke(clazz_member, meta::uvalue{cl}).as<int>() == 1);
        CHECK(meta::invoke(clazz_member, meta::uresult{cl}).as<int>() == 1);

        CHECK(meta::try_invoke(&clazz::member, cl)->as<int>() == 1);
        CHECK(meta::try_invoke(&clazz::member, meta::uvalue{cl})->as<int>() == 1);
        CHECK(meta::try_invoke(&clazz::member, meta::uresult{cl})->as<int>() == 1);
        CHECK_FALSE(meta::try_invoke(&clazz::member, ivec2{}));
        CHECK_FALSE(meta::try_invoke(&clazz::member, meta::uvalue{ivec2{}}));
        CHECK(meta::try_invoke(clazz_member, cl)->as<int>() == 1);
        CHECK(meta::try_invoke(clazz_member, meta::uvalue{cl})->as<int>() == 1);
        CHECK(meta::try_invoke(clazz_member, meta::uresult{cl})->as<int>() == 1);
        CHECK_FALSE(meta::try_invoke(clazz_member, ivec2{}));
        CHECK_FALSE(meta::try_invoke(clazz_member, meta::uvalue{ivec2{}}));

        CHECK(meta::is_invocable_with(clazz_member, cl));
        CHECK(meta::is_invocable_with(clazz_member, meta::uvalue{cl}));
        CHECK(meta::is_invocable_with(clazz_member, meta::uresult{cl}));
        CHECK(meta::is_invocable_with<const clazz&>(clazz_member));
        CHECK_FALSE(meta::is_invocable_with(clazz_member, ivec2{}));
        CHECK_FALSE(meta::is_invocable_with(clazz_member, meta::uvalue{ivec2{}}));
        CHECK_FALSE(meta::is_invocable_with<const ivec2&>(clazz_member));

        CHECK(meta::is_invocable_with(&clazz::member, cl));
        CHECK(meta::is_invocable_with(&clazz::member, meta::uvalue{cl}));
        CHECK(meta::is_invocable_with(&clazz::member, meta::uresult{cl}));
        CHECK(meta::is_invocable_with<const clazz&>(&clazz::member));
        CHECK_FALSE(meta::is_invocable_with(&clazz::member, ivec2{}));
        CHECK_FALSE(meta::is_invocable_with(&clazz::member, meta::uvalue{ivec2{}}));
        CHECK_FALSE(meta::is_invocable_with<const ivec2&>(&clazz::member));
    }

    {
        clazz cl;

        CHECK(meta::invoke(&clazz::method, cl, 2).as<int>() == 2);
        CHECK(meta::invoke(&clazz::method, meta::uvalue{cl}, meta::uvalue{2}).as<int>() == 2);
        CHECK(meta::invoke(&clazz::method, meta::uresult{cl}, meta::uresult{2}).as<int>() == 2);
        CHECK(meta::invoke(clazz_method, cl, 2).as<int>() == 2);
        CHECK(meta::invoke(clazz_method, meta::uvalue{cl}, meta::uvalue{2}).as<int>() == 2);
        CHECK(meta::invoke(clazz_method, meta::uresult{cl}, meta::uresult{2}).as<int>() == 2);

        CHECK(meta::try_invoke(&clazz::method, cl, 2)->as<int>() == 2);
        CHECK(meta::try_invoke(&clazz::method, meta::uvalue{cl}, meta::uvalue{2})->as<int>() == 2);
        CHECK(meta::try_invoke(&clazz::method, meta::uresult{cl}, meta::uresult{2})->as<int>() == 2);
        CHECK_FALSE(meta::try_invoke(&clazz::method, cl, ivec2{}));
        CHECK_FALSE(meta::try_invoke(&clazz::method, meta::uvalue{ivec2{}}, meta::uvalue{2}));
        CHECK(meta::try_invoke(clazz_method, cl, 2)->as<int>() == 2);
        CHECK(meta::try_invoke(clazz_method, meta::uvalue{cl}, meta::uvalue{2})->as<int>() == 2);
        CHECK(meta::try_invoke(clazz_method, meta::uresult{cl}, meta::uresult{2})->as<int>() == 2);
        CHECK_FALSE(meta::try_invoke(clazz_method, cl, ivec2{}));
        CHECK_FALSE(meta::try_invoke(clazz_method, meta::uvalue{cl}, meta::uvalue{ivec2{}}));

        CHECK(meta::is_invocable_with(clazz_method, cl, 2));
        CHECK(meta::is_invocable_with(clazz_method, meta::uvalue{cl}, meta::uvalue{2}));
        CHECK(meta::is_invocable_with(clazz_method, meta::uresult{cl}, meta::uresult{2}));
        CHECK(meta::is_invocable_with<const clazz&, int>(clazz_method));
        CHECK_FALSE(meta::is_invocable_with(clazz_method, cl, ivec2{}));
        CHECK_FALSE(meta::is_invocable_with(clazz_method, meta::uvalue{cl}, meta::uvalue{ivec2{}}));
        CHECK_FALSE(meta::is_invocable_with<const clazz&, ivec2>(clazz_method));

        CHECK(meta::is_invocable_with(&clazz::method, cl, 2));
        CHECK(meta::is_invocable_with(&clazz::method, meta::uvalue{cl}, meta::uvalue{2}));
        CHECK(meta::is_invocable_with(&clazz::method, meta::uresult{cl}, meta::uresult{2}));
        CHECK(meta::is_invocable_with<const clazz&, int>(&clazz::method));
        CHECK_FALSE(meta::is_invocable_with(&clazz::method, cl, ivec2{}));
        CHECK_FALSE(meta::is_invocable_with(&clazz::method, meta::uvalue{cl}, meta::uvalue{ivec2{}}));
        CHECK_FALSE(meta::is_invocable_with<const clazz&, ivec2>(&clazz::method));
    }

    {
        clazz cl;

        CHECK(meta::invoke_variadic(&clazz::method, meta::uvalue{cl}, params.begin(), params.end()).as<int>() == 3);
        CHECK(meta::invoke_variadic(&clazz::method, meta::uresult{cl}, typed_params.begin(), typed_params.end()).as<int>() == 3);
        CHECK(meta::invoke_variadic(clazz_method, meta::uvalue{cl}, params.begin(), params.end()).as<int>() == 3);
        CHECK(meta::invoke_variadic(clazz_method, meta::uresult{cl}, typed_params.begin(), typed_params.end()).as<int>() == 3);

        CHECK(meta::try_invoke_variadic(&clazz::method, meta::uvalue{cl}, params.begin(), params.end())->as<int>() == 3);
        CHECK(meta::try_invoke_variadic(&clazz::method, meta::uresult{cl}, typed_params.begin(), typed_params.end())->as<int>() == 3);
        CHECK_FALSE(meta::try_invoke_variadic(&clazz::method, meta::uresult{cl}, wrong_params.begin(), wrong_params.end()));
        CHECK(meta::try_invoke_variadic(clazz_method, meta::uvalue{cl}, params.begin(), params.end())->as<int>() == 3);
        CHECK(meta::try_invoke_variadic(clazz_method, meta::uresult{cl}, typed_params.begin(), typed_params.end())->as<int>() == 3);
        CHECK_FALSE(meta::try_invoke_variadic(clazz_method, meta::uresult{cl}, wrong_params.begin(), wrong_params.end()));

        CHECK(meta::is_variadic_invocable_with(&clazz::method, meta::uvalue{cl}, params.begin(), params.end()));
        CHECK(meta::is_variadic_invocable_with(&clazz::method, meta::uresult{cl}, typed_params.begin(), typed_params.end()));
        CHECK_FALSE(meta::is_variadic_invocable_with(&clazz::method, meta::uresult{cl}, wrong_params.begin(), wrong_params.end()));
        CHECK(meta::is_variadic_invocable_with(clazz_method, meta::uvalue{cl}, params.begin(), params.end()));
        CHECK(meta::is_variadic_invocable_with(clazz_method, meta::uresult{cl}, typed_params.begin(), typed_params.end()));
        CHECK_FALSE(meta::is_variadic_invocable_with(clazz_method, meta::uresult{cl}, wrong_params.begin(), wrong_params.end()));
    }
}
