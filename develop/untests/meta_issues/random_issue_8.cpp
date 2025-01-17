/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2025, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    namespace meta = meta_hpp;

    meta::scope sc;

    int inner(int a, int b) {
        return a + b;
    }

    struct A {
        A() = default;
        ~A() = default;

        A(const A&) {
            std::array<int, 2> args{1, 2};
            v = sc.get_function("inner").invoke_variadic(args.begin(), args.end()).as<int>();
        }

        int v{};
    };

    int outer(A l, A r) {
        return l.v + r.v;
    }
}

TEST_CASE("meta/meta_issues/random/8") {
    sc = meta::local_scope_("")
        .function_("inner", &inner)
        .function_("outer", &outer);

    std::array<A, 2> args{};
    CHECK(sc.get_function("outer").invoke_variadic(args.begin(), args.end()).as<int>() == 6);
}
