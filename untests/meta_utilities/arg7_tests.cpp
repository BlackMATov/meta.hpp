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
        int i = 2;
    };

    struct C : virtual A {
        int i = 3;
    };

    struct D : B, C {
        int i = 4;
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
    using meta::detail::arg;

    SUBCASE("int* -> void*") {
        int i{42};

        static_assert(std::is_invocable_v<void(void*), int*>);
        static_assert(std::is_invocable_v<void(const void*), int*>);

        CHECK(arg{&i}.can_cast_to<void*>());
        CHECK(arg{&i}.can_cast_to<const void*>());

        CHECK(arg{&i}.cast<void*>() == &i);
        CHECK(arg{&i}.cast<const void*>() == &i);
    }

    SUBCASE("const int* -> void*") {
        const int i{42};

        static_assert(!std::is_invocable_v<void(void*), const int*>);
        static_assert(std::is_invocable_v<void(const void*), const int*>);

        CHECK_FALSE(arg{&i}.can_cast_to<void*>());
        CHECK(arg{&i}.can_cast_to<const void*>());

        CHECK_THROWS(std::ignore = arg{&i}.cast<void*>());
        CHECK(arg{&i}.cast<const void*>() == &i);
    }

    SUBCASE("D* -> void*") {
        D d;

        static_assert(std::is_invocable_v<void(void*), D*>);
        static_assert(std::is_invocable_v<void(const void*), D*>);

        CHECK(arg{&d}.can_cast_to<void*>());
        CHECK(arg{&d}.can_cast_to<const void*>());

        CHECK(arg{&d}.cast<void*>() == &d);
        CHECK(arg{&d}.cast<const void*>() == &d);
    }

    SUBCASE("const D* -> void*") {
        const D d;

        static_assert(!std::is_invocable_v<void(void*), const D*>);
        static_assert(std::is_invocable_v<void(const void*), const D*>);

        CHECK_FALSE(arg{&d}.can_cast_to<void*>());
        CHECK(arg{&d}.can_cast_to<const void*>());

        CHECK_THROWS(std::ignore = arg{&d}.cast<void*>());
        CHECK(arg{&d}.cast<const void*>() == &d);
    }

    SUBCASE("D[2] -> void*") {
        D arr[2];

        static_assert(std::is_invocable_v<void(void*), D (&) [2]>);
        static_assert(std::is_invocable_v<void(const void*), D (&) [2]>);

        CHECK(arg{arr}.can_cast_to<void*>());
        CHECK(arg{arr}.can_cast_to<const void*>());

        CHECK(arg{arr}.cast<void*>() == &arr);
        CHECK(arg{arr}.cast<const void*>() == &arr);
    }

    SUBCASE("const D[2] -> void*") {
        const D arr[2];

        static_assert(!std::is_invocable_v<void(void*), const D (&) [2]>);
        static_assert(std::is_invocable_v<void(const void*), const D (&) [2]>);

        CHECK_FALSE(arg{arr}.can_cast_to<void*>());
        CHECK(arg{arr}.can_cast_to<const void*>());

        CHECK_THROWS(std::ignore = arg{arr}.cast<void*>());
        CHECK(arg{arr}.cast<const void*>() == &arr);
    }
}

TEST_CASE("meta/meta_utilities/arg7/cast/from_nullptr") {
    namespace meta = meta_hpp;
    using meta::detail::arg;

    SUBCASE("nullptr -> *") {
        static_assert(std::is_invocable_v<void(int*), nullptr_t>);
        static_assert(std::is_invocable_v<void(int*), nullptr_t&>);
        static_assert(std::is_invocable_v<void(int*), nullptr_t&&>);
        static_assert(std::is_invocable_v<void(int*), const nullptr_t>);
        static_assert(std::is_invocable_v<void(int*), const nullptr_t&>);
        static_assert(std::is_invocable_v<void(int*), const nullptr_t&&>);

        static_assert(std::is_invocable_v<void(const D*), nullptr_t>);
        static_assert(std::is_invocable_v<void(const D*), nullptr_t&>);
        static_assert(std::is_invocable_v<void(const D*), nullptr_t&&>);
        static_assert(std::is_invocable_v<void(const D*), const nullptr_t>);
        static_assert(std::is_invocable_v<void(const D*), const nullptr_t&>);
        static_assert(std::is_invocable_v<void(const D*), const nullptr_t&&>);

        nullptr_t n1{nullptr};
        const nullptr_t n2{nullptr};

        CHECK(arg{n1}.can_cast_to<int*>());
        CHECK(arg{std::move(n1)}.can_cast_to<int*>());
        CHECK(arg{n2}.can_cast_to<int*>());
        CHECK(arg{std::move(n2)}.can_cast_to<int*>());

        CHECK(arg{n1}.can_cast_to<const int*>());
        CHECK(arg{std::move(n1)}.can_cast_to<const int*>());
        CHECK(arg{n2}.can_cast_to<const int*>());
        CHECK(arg{std::move(n2)}.can_cast_to<const int*>());

        CHECK(arg{n1}.can_cast_to<D*>());
        CHECK(arg{std::move(n1)}.can_cast_to<D*>());
        CHECK(arg{n2}.can_cast_to<D*>());
        CHECK(arg{std::move(n2)}.can_cast_to<D*>());

        CHECK(arg{n1}.can_cast_to<const D*>());
        CHECK(arg{std::move(n1)}.can_cast_to<const D*>());
        CHECK(arg{n2}.can_cast_to<const D*>());
        CHECK(arg{std::move(n2)}.can_cast_to<const D*>());

        //

        CHECK(arg{n1}.cast<int*>() == nullptr);
        CHECK(arg{std::move(n1)}.cast<int*>() == nullptr);
        CHECK(arg{n2}.cast<int*>() == nullptr);
        CHECK(arg{std::move(n2)}.cast<int*>() == nullptr);

        CHECK(arg{n1}.cast<const int*>() == nullptr);
        CHECK(arg{std::move(n1)}.cast<const int*>() == nullptr);
        CHECK(arg{n2}.cast<const int*>() == nullptr);
        CHECK(arg{std::move(n2)}.cast<const int*>() == nullptr);

        CHECK(arg{n1}.cast<D*>() == nullptr);
        CHECK(arg{std::move(n1)}.cast<D*>() == nullptr);
        CHECK(arg{n2}.cast<D*>() == nullptr);
        CHECK(arg{std::move(n2)}.cast<D*>() == nullptr);

        CHECK(arg{n1}.cast<const D*>() == nullptr);
        CHECK(arg{std::move(n1)}.cast<const D*>() == nullptr);
        CHECK(arg{n2}.cast<const D*>() == nullptr);
        CHECK(arg{std::move(n2)}.cast<const D*>() == nullptr);
    }
}
