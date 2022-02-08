/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

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

    meta::class_<A>();
    meta::class_<B>().base_<A>();
    meta::class_<C>().base_<A>();
    meta::class_<D>().base_<B>().base_<C>();
}

TEST_CASE("meta/meta_utilities/arg7/cast/to_void") {
    namespace meta = meta_hpp;
    using meta::detail::uarg;

    SUBCASE("int* -> void*") {
        int i{42};

        static_assert(std::is_invocable_v<void(void*), int*>);
        static_assert(std::is_invocable_v<void(const void*), int*>);

        CHECK(uarg{&i}.can_cast_to<void*>());
        CHECK(uarg{&i}.can_cast_to<const void*>());

        CHECK(uarg{&i}.cast<void*>() == &i);
        CHECK(uarg{&i}.cast<const void*>() == &i);
    }

    SUBCASE("const int* -> void*") {
        const int i{42};

        static_assert(!std::is_invocable_v<void(void*), const int*>);
        static_assert(std::is_invocable_v<void(const void*), const int*>);

        CHECK_FALSE(uarg{&i}.can_cast_to<void*>());
        CHECK(uarg{&i}.can_cast_to<const void*>());

        CHECK_THROWS(std::ignore = uarg{&i}.cast<void*>());
        CHECK(uarg{&i}.cast<const void*>() == &i);
    }

    SUBCASE("D* -> void*") {
        D d;

        static_assert(std::is_invocable_v<void(void*), D*>);
        static_assert(std::is_invocable_v<void(const void*), D*>);

        CHECK(uarg{&d}.can_cast_to<void*>());
        CHECK(uarg{&d}.can_cast_to<const void*>());

        CHECK(uarg{&d}.cast<void*>() == &d);
        CHECK(uarg{&d}.cast<const void*>() == &d);
    }

    SUBCASE("const D* -> void*") {
        const D d;

        static_assert(!std::is_invocable_v<void(void*), const D*>);
        static_assert(std::is_invocable_v<void(const void*), const D*>);

        CHECK_FALSE(uarg{&d}.can_cast_to<void*>());
        CHECK(uarg{&d}.can_cast_to<const void*>());

        CHECK_THROWS(std::ignore = uarg{&d}.cast<void*>());
        CHECK(uarg{&d}.cast<const void*>() == &d);
    }

    SUBCASE("D[2] -> void*") {
        D arr[2];

        static_assert(std::is_invocable_v<void(void*), D (&) [2]>);
        static_assert(std::is_invocable_v<void(const void*), D (&) [2]>);

        CHECK(uarg{arr}.can_cast_to<void*>());
        CHECK(uarg{arr}.can_cast_to<const void*>());

        CHECK(uarg{arr}.cast<void*>() == &arr);
        CHECK(uarg{arr}.cast<const void*>() == &arr);
    }

    SUBCASE("const D[2] -> void*") {
        const D arr[2];

        static_assert(!std::is_invocable_v<void(void*), const D (&) [2]>);
        static_assert(std::is_invocable_v<void(const void*), const D (&) [2]>);

        CHECK_FALSE(uarg{arr}.can_cast_to<void*>());
        CHECK(uarg{arr}.can_cast_to<const void*>());

        CHECK_THROWS(std::ignore = uarg{arr}.cast<void*>());
        CHECK(uarg{arr}.cast<const void*>() == &arr);
    }
}

TEST_CASE("meta/meta_utilities/arg7/cast/from_nullptr") {
    namespace meta = meta_hpp;
    using meta::detail::uarg;

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

        CHECK(uarg{n1}.can_cast_to<int*>());
        CHECK(uarg{std::move(n1)}.can_cast_to<int*>());
        CHECK(uarg{n2}.can_cast_to<int*>());
        CHECK(uarg{std::move(n2)}.can_cast_to<int*>());

        CHECK(uarg{n1}.can_cast_to<const int*>());
        CHECK(uarg{std::move(n1)}.can_cast_to<const int*>());
        CHECK(uarg{n2}.can_cast_to<const int*>());
        CHECK(uarg{std::move(n2)}.can_cast_to<const int*>());

        CHECK(uarg{n1}.can_cast_to<D*>());
        CHECK(uarg{std::move(n1)}.can_cast_to<D*>());
        CHECK(uarg{n2}.can_cast_to<D*>());
        CHECK(uarg{std::move(n2)}.can_cast_to<D*>());

        CHECK(uarg{n1}.can_cast_to<const D*>());
        CHECK(uarg{std::move(n1)}.can_cast_to<const D*>());
        CHECK(uarg{n2}.can_cast_to<const D*>());
        CHECK(uarg{std::move(n2)}.can_cast_to<const D*>());

        //

        CHECK(uarg{n1}.cast<int*>() == nullptr);
        CHECK(uarg{std::move(n1)}.cast<int*>() == nullptr);
        CHECK(uarg{n2}.cast<int*>() == nullptr);
        CHECK(uarg{std::move(n2)}.cast<int*>() == nullptr);

        CHECK(uarg{n1}.cast<const int*>() == nullptr);
        CHECK(uarg{std::move(n1)}.cast<const int*>() == nullptr);
        CHECK(uarg{n2}.cast<const int*>() == nullptr);
        CHECK(uarg{std::move(n2)}.cast<const int*>() == nullptr);

        CHECK(uarg{n1}.cast<D*>() == nullptr);
        CHECK(uarg{std::move(n1)}.cast<D*>() == nullptr);
        CHECK(uarg{n2}.cast<D*>() == nullptr);
        CHECK(uarg{std::move(n2)}.cast<D*>() == nullptr);

        CHECK(uarg{n1}.cast<const D*>() == nullptr);
        CHECK(uarg{std::move(n1)}.cast<const D*>() == nullptr);
        CHECK(uarg{n2}.cast<const D*>() == nullptr);
        CHECK(uarg{std::move(n2)}.cast<const D*>() == nullptr);
    }
}
