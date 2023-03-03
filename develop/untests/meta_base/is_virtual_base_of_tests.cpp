/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct A {};
    struct B : virtual A {};
    struct C : virtual A {};
    struct D : B, C {};
    struct E {};

    struct I {};
    struct J : virtual I {};
    struct K : J {};
}

TEST_CASE("meta/meta_base/is_virtual_base_of") {
    namespace meta = meta_hpp;
    using meta::detail::is_virtual_base_of;
    using meta::detail::is_virtual_base_of_v;

    {
        static_assert(!is_virtual_base_of_v<A, A>);
        static_assert(is_virtual_base_of_v<A, B>);
        static_assert(is_virtual_base_of_v<A, C>);

        static_assert(is_virtual_base_of_v<A, D>);
        static_assert(!is_virtual_base_of_v<B, D>);
        static_assert(!is_virtual_base_of_v<C, D>);

        static_assert(!is_virtual_base_of_v<A, E>);
        static_assert(!is_virtual_base_of_v<E, A>);

        static_assert(!is_virtual_base_of_v<E, D>);
        static_assert(!is_virtual_base_of_v<D, E>);
    }

    {
        static_assert(!is_virtual_base_of_v<I, I>);
        static_assert(is_virtual_base_of_v<I, J>);
        static_assert(is_virtual_base_of_v<I, K>);

        static_assert(!is_virtual_base_of_v<J, I>);
        static_assert(!is_virtual_base_of_v<J, J>);
        static_assert(!is_virtual_base_of_v<J, K>);

        static_assert(!is_virtual_base_of_v<K, I>);
        static_assert(!is_virtual_base_of_v<K, J>);
        static_assert(!is_virtual_base_of_v<K, K>);
    }

    {
        static_assert(is_virtual_base_of_v<I, J>);

        static_assert(is_virtual_base_of_v<const I, J>);
        static_assert(is_virtual_base_of_v<volatile I, J>);
        static_assert(is_virtual_base_of_v<const volatile I, J>);

        static_assert(is_virtual_base_of_v<const I, const J>);
        static_assert(is_virtual_base_of_v<volatile I, const J>);
        static_assert(is_virtual_base_of_v<const volatile I, const J>);

        static_assert(is_virtual_base_of_v<const I, volatile J>);
        static_assert(is_virtual_base_of_v<volatile I, volatile J>);
        static_assert(is_virtual_base_of_v<const volatile I, volatile J>);

        static_assert(is_virtual_base_of_v<const I, const volatile J>);
        static_assert(is_virtual_base_of_v<volatile I, const volatile J>);
        static_assert(is_virtual_base_of_v<const volatile I, const volatile J>);
    }
}
