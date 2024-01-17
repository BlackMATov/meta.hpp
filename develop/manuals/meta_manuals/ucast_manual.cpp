/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>

#include <doctest/doctest.h>
#include <fmt/core.h>

namespace
{
    // we should add META_HPP_ENABLE_POLY_INFO macro inside every class
    // to add polymorphic information to our inheritance hierarchy

    // after this we will be able to use the library's polymorphic functions
    // like `ucast` or `resolve_type(T&&)`

    struct A {
        virtual ~A() = default;
        META_HPP_ENABLE_POLY_INFO()
    };

    struct B {
        virtual ~B() = default;
        META_HPP_ENABLE_POLY_INFO()
    };

    struct C : A, B {
        META_HPP_ENABLE_POLY_INFO(A, B)
    };
}

TEST_CASE("meta/meta_manuals/ucast") {
    namespace meta = meta_hpp;

    C c{};

    A& a = c;
    B& b = c;

    // functions `ucast` and `resolve_type(T&&)` rely on polymorphic information
    // are provided in META_HPP_ENABLE_POLY_INFO macro

    // we can use polymorphic type resolving to get most derived class type
    CHECK(meta::resolve_type(a) == meta::resolve_type<C>());

    // and cast our pointers and references like dynamic_cast does
    CHECK(&c == &meta::ucast<C&>(a));

    // `ucast` supports cross casts, multiple and virtual inheritance as well
    CHECK(&a == meta::ucast<A*>(&b));
}
