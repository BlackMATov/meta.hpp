/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct A {
        A() = default;
        virtual ~A() = default;

        A(A&&) = delete;
        A(const A&) = delete;
        A& operator=(A&&) = delete;
        A& operator=(const A&) = delete;

        int i = 1;
    };

    struct B : virtual A {
        int bi = 2;
    };

    struct C : virtual A {
        int ci = 3;
    };

    struct D : B, C {
        int di = 4;
    };
}

TEST_CASE("meta/meta_utilities/arg7") {
    namespace meta = meta_hpp;

    //  * <- B <- *
    // A           D
    //  * <- C <- *
}

TEST_CASE("meta/meta_utilities/arg7/cast/to_void") {
    namespace meta = meta_hpp;
    using meta::detail::uarg;
    using meta::detail::type_registry;
    type_registry& r{type_registry::instance()};

    SUBCASE("int* -> void*") {
        int i{42};

        static_assert(std::is_invocable_v<void(void*), int*>);
        static_assert(std::is_invocable_v<void(const void*), int*>);

        CHECK(uarg{r, &i}.can_cast_to<void*>(r));
        CHECK(uarg{r, &i}.can_cast_to<const void*>(r));

        CHECK(uarg{r, &i}.cast<void*>(r) == &i);
        CHECK(uarg{r, &i}.cast<const void*>(r) == &i);
    }

    SUBCASE("const int* -> void*") {
        const int i{42};

        static_assert(!std::is_invocable_v<void(void*), const int*>);
        static_assert(std::is_invocable_v<void(const void*), const int*>);

        CHECK_FALSE(uarg{r, &i}.can_cast_to<void*>(r));
        CHECK(uarg{r, &i}.can_cast_to<const void*>(r));

        // CHECK_THROWS(std::ignore = uarg{r, &i}.cast<void*>(r));
        CHECK(uarg{r, &i}.cast<const void*>(r) == &i);
    }

    SUBCASE("D* -> void*") {
        D d;

        static_assert(std::is_invocable_v<void(void*), D*>);
        static_assert(std::is_invocable_v<void(const void*), D*>);

        CHECK(uarg{r, &d}.can_cast_to<void*>(r));
        CHECK(uarg{r, &d}.can_cast_to<const void*>(r));

        CHECK(uarg{r, &d}.cast<void*>(r) == &d);
        CHECK(uarg{r, &d}.cast<const void*>(r) == &d);
    }

    SUBCASE("const D* -> void*") {
        const D d;

        static_assert(!std::is_invocable_v<void(void*), const D*>);
        static_assert(std::is_invocable_v<void(const void*), const D*>);

        CHECK_FALSE(uarg{r, &d}.can_cast_to<void*>(r));
        CHECK(uarg{r, &d}.can_cast_to<const void*>(r));

        // CHECK_THROWS(std::ignore = uarg{r, &d}.cast<void*>(r));
        CHECK(uarg{r, &d}.cast<const void*>(r) == &d);
    }

    SUBCASE("D[2] -> void*") {
        D arr[2];

        static_assert(std::is_invocable_v<void(void*), D (&) [2]>);
        static_assert(std::is_invocable_v<void(const void*), D (&) [2]>);

        CHECK(uarg{r, arr}.can_cast_to<void*>(r));
        CHECK(uarg{r, arr}.can_cast_to<const void*>(r));

        CHECK(uarg{r, arr}.cast<void*>(r) == &arr);
        CHECK(uarg{r, arr}.cast<const void*>(r) == &arr);
    }

    SUBCASE("const D[2] -> void*") {
        const D arr[2];

        static_assert(!std::is_invocable_v<void(void*), const D (&) [2]>);
        static_assert(std::is_invocable_v<void(const void*), const D (&) [2]>);

        CHECK_FALSE(uarg{r, arr}.can_cast_to<void*>(r));
        CHECK(uarg{r, arr}.can_cast_to<const void*>(r));

        // CHECK_THROWS(std::ignore = uarg{r, arr}.cast<void*>(r));
        CHECK(uarg{r, arr}.cast<const void*>(r) == &arr);
    }
}

TEST_CASE("meta/meta_utilities/arg7/cast/from_nullptr") {
    namespace meta = meta_hpp;
    using meta::detail::uarg;
    using meta::detail::type_registry;
    type_registry& r{type_registry::instance()};

    SUBCASE("nullptr -> *") {
        static_assert(std::is_invocable_v<void(int*), std::nullptr_t>);
        static_assert(std::is_invocable_v<void(int*), std::nullptr_t&>);
        static_assert(std::is_invocable_v<void(int*), std::nullptr_t&&>);
        static_assert(std::is_invocable_v<void(int*), const std::nullptr_t>);
        static_assert(std::is_invocable_v<void(int*), const std::nullptr_t&>);
        static_assert(std::is_invocable_v<void(int*), const std::nullptr_t&&>);

        static_assert(std::is_invocable_v<void(const D*), std::nullptr_t>);
        static_assert(std::is_invocable_v<void(const D*), std::nullptr_t&>);
        static_assert(std::is_invocable_v<void(const D*), std::nullptr_t&&>);
        static_assert(std::is_invocable_v<void(const D*), const std::nullptr_t>);
        static_assert(std::is_invocable_v<void(const D*), const std::nullptr_t&>);
        static_assert(std::is_invocable_v<void(const D*), const std::nullptr_t&&>);

        std::nullptr_t n1{nullptr};
        const std::nullptr_t n2{nullptr};

        CHECK(uarg{r, n1}.can_cast_to<int*>(r));
        CHECK(uarg{r, std::move(n1)}.can_cast_to<int*>(r));
        CHECK(uarg{r, n2}.can_cast_to<int*>(r));
        CHECK(uarg{r, std::move(n2)}.can_cast_to<int*>(r));

        CHECK(uarg{r, n1}.can_cast_to<const int*>(r));
        CHECK(uarg{r, std::move(n1)}.can_cast_to<const int*>(r));
        CHECK(uarg{r, n2}.can_cast_to<const int*>(r));
        CHECK(uarg{r, std::move(n2)}.can_cast_to<const int*>(r));

        CHECK(uarg{r, n1}.can_cast_to<D*>(r));
        CHECK(uarg{r, std::move(n1)}.can_cast_to<D*>(r));
        CHECK(uarg{r, n2}.can_cast_to<D*>(r));
        CHECK(uarg{r, std::move(n2)}.can_cast_to<D*>(r));

        CHECK(uarg{r, n1}.can_cast_to<const D*>(r));
        CHECK(uarg{r, std::move(n1)}.can_cast_to<const D*>(r));
        CHECK(uarg{r, n2}.can_cast_to<const D*>(r));
        CHECK(uarg{r, std::move(n2)}.can_cast_to<const D*>(r));

        //

        CHECK(uarg{r, n1}.cast<int*>(r) == nullptr);
        CHECK(uarg{r, std::move(n1)}.cast<int*>(r) == nullptr);
        CHECK(uarg{r, n2}.cast<int*>(r) == nullptr);
        CHECK(uarg{r, std::move(n2)}.cast<int*>(r) == nullptr);

        CHECK(uarg{r, n1}.cast<const int*>(r) == nullptr);
        CHECK(uarg{r, std::move(n1)}.cast<const int*>(r) == nullptr);
        CHECK(uarg{r, n2}.cast<const int*>(r) == nullptr);
        CHECK(uarg{r, std::move(n2)}.cast<const int*>(r) == nullptr);

        CHECK(uarg{r, n1}.cast<D*>(r) == nullptr);
        CHECK(uarg{r, std::move(n1)}.cast<D*>(r) == nullptr);
        CHECK(uarg{r, n2}.cast<D*>(r) == nullptr);
        CHECK(uarg{r, std::move(n2)}.cast<D*>(r) == nullptr);

        CHECK(uarg{r, n1}.cast<const D*>(r) == nullptr);
        CHECK(uarg{r, std::move(n1)}.cast<const D*>(r) == nullptr);
        CHECK(uarg{r, n2}.cast<const D*>(r) == nullptr);
        CHECK(uarg{r, std::move(n2)}.cast<const D*>(r) == nullptr);
    }
}
