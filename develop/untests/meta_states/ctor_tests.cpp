/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    template < typename Tag >
    struct clazz {
        int i{};

        clazz(int ni) : i{ni} {
            ++constructor_counter;
        }

        clazz(clazz&& other) noexcept : i{other.i} {
            other.i = 0;
            ++move_constructor_counter;
        }

        clazz(const clazz& other) : i{other.i} {
            ++copy_constructor_counter;
        }

        clazz& operator=(clazz&& other) = delete;
        clazz& operator=(const clazz& other) = delete;

        ~clazz() {
            ++destructor_counter;
        }

        inline static int constructor_counter{};
        inline static int destructor_counter{};
        inline static int move_constructor_counter{};
        inline static int copy_constructor_counter{};
    };

    template < typename Tag >
    struct clazz_noncopyable {
        int i{};

        clazz_noncopyable(int ni) : i{ni} {
            ++constructor_counter;
        }

        clazz_noncopyable(clazz_noncopyable&& other) : i{other.i} {
            other.i = 0;
            ++move_constructor_counter;
        }

        clazz_noncopyable(const clazz_noncopyable& other) = delete;
        clazz_noncopyable& operator=(clazz_noncopyable&& other) = delete;
        clazz_noncopyable& operator=(const clazz_noncopyable& other) = delete;

        ~clazz_noncopyable() {
            ++destructor_counter;
        }

        inline static int constructor_counter{};
        inline static int destructor_counter{};
        inline static int move_constructor_counter{};
        inline static int copy_constructor_counter{};
    };
}

TEST_CASE("meta/meta_states/ctor/noncopyable") {
    namespace meta = meta_hpp;

    using clazz_t = clazz_noncopyable<meta::constructor_policy::as_raw_pointer_t>;

    meta::class_<clazz_t>()
        .constructor_<int>(meta::constructor_policy::as_raw_pointer)
        .constructor_<clazz_t&&>(meta::constructor_policy::as_raw_pointer);

    clazz_t::constructor_counter = 0;
    clazz_t::destructor_counter = 0;
    clazz_t::move_constructor_counter = 0;
    clazz_t::copy_constructor_counter = 0;

    const meta::class_type clazz_type = meta::resolve_type<clazz_t>();
    REQUIRE(clazz_type);

    SUBCASE("int") {
        {
            const meta::constructor ctor = clazz_type.get_constructor_with<int>();
            REQUIRE(ctor);
            CHECK(ctor.get_type() == meta::resolve_constructor_type<clazz_t, int>());
        }
        {
            const meta::uvalue v = clazz_type.create(42);
            CHECK(v.get_type() == meta::resolve_type<clazz_t*>());
            CHECK(v.get_as<clazz_t*>()->i == 42);
            CHECK(clazz_type.destroy(v));
        }
        CHECK(clazz_t::constructor_counter == 1);
        CHECK(clazz_t::destructor_counter == 1);
        CHECK(clazz_t::move_constructor_counter == 0);
        CHECK(clazz_t::copy_constructor_counter == 0);
    }

    SUBCASE("clazz_t&&") {
        {
            const meta::constructor ctor = clazz_type.get_constructor_with<clazz_t&&>();
            REQUIRE(ctor);
            CHECK(ctor.get_type() == meta::resolve_constructor_type<clazz_t, clazz_t&&>());
        }
        {
            clazz_t o{42};
            const meta::uvalue v = clazz_type.create(std::move(o));
            CHECK(v.get_type() == meta::resolve_type<clazz_t*>());
            CHECK(v.get_as<clazz_t*>()->i == 42);
            CHECK(clazz_type.destroy(v));
        }
        CHECK(clazz_t::constructor_counter == 1);
        CHECK(clazz_t::destructor_counter == 2);
        CHECK(clazz_t::move_constructor_counter == 1);
        CHECK(clazz_t::copy_constructor_counter == 0);
    }
}

TEST_CASE("meta/meta_states/ctor/as_object") {
    namespace meta = meta_hpp;

    using clazz_t = clazz<meta::constructor_policy::as_object_t>;

    meta::class_<clazz_t>()
        .constructor_<int>(meta::constructor_policy::as_object)
        .constructor_<clazz_t&&>(meta::constructor_policy::as_object)
        .constructor_<const clazz_t&>(meta::constructor_policy::as_object);

    clazz_t::constructor_counter = 0;
    clazz_t::destructor_counter = 0;
    clazz_t::move_constructor_counter = 0;
    clazz_t::copy_constructor_counter = 0;

    const meta::class_type clazz_type = meta::resolve_type<clazz_t>();
    REQUIRE(clazz_type);

    alignas(clazz_t) std::byte clazz_mem[sizeof(clazz_t)];

    SUBCASE("int") {
        {
            const meta::constructor ctor = clazz_type.get_constructor_with<int>();
            REQUIRE(ctor);
            CHECK(ctor.get_type() == meta::resolve_constructor_type<clazz_t, int>());
            CHECK(ctor.get_type().get_owner_type() == meta::resolve_type<clazz_t>());
            CHECK(ctor.get_type().get_flags() == meta::constructor_bitflags{});
        }
        {
            const meta::uvalue v = clazz_type.create(42);
            CHECK(v.get_type() == meta::resolve_type<clazz_t>());
            CHECK(v.get_as<clazz_t>().i == 42);
            CHECK_FALSE(clazz_type.destroy(v));
        }
        CHECK(clazz_t::constructor_counter == 1);
        CHECK(clazz_t::destructor_counter == 1);
        CHECK(clazz_t::move_constructor_counter == 0);
        CHECK(clazz_t::copy_constructor_counter == 0);
    }

    SUBCASE("int_safe") {
        const meta::constructor ctor = clazz_type.get_constructor_with<int>();
        REQUIRE(ctor);
        {
            std::optional<meta::uvalue> v{ctor.safe_create(42.0)};
            CHECK_FALSE(v);

            CHECK(clazz_t::constructor_counter == 0);
        }
        {
            std::optional<meta::uvalue> v{ctor.safe_create(42)};
            CHECK(v);

            CHECK(clazz_t::constructor_counter == 1);
            CHECK(v->get_type() == meta::resolve_type<clazz_t>());
            CHECK(v->get_as<clazz_t>().i == 42);
        }
    }

    SUBCASE("int/inplace") {
        {
            const meta::uvalue v = clazz_type
                .get_constructor_with<int>()
                .create_at(clazz_mem, 42);

            CHECK(v.get_type() == meta::resolve_type<clazz_t*>());
            CHECK(v.get_as<clazz_t*>()->i == 42);

            clazz_type.get_destructor().destroy_at(clazz_mem);
        }
        CHECK(clazz_t::constructor_counter == 1);
        CHECK(clazz_t::destructor_counter == 1);
        CHECK(clazz_t::move_constructor_counter == 0);
        CHECK(clazz_t::copy_constructor_counter == 0);
    }

    SUBCASE("safe_int/inplace") {
        const meta::constructor ctor = clazz_type.get_constructor_with<int>();
        REQUIRE(ctor);
        {
            std::optional<meta::uvalue> v{ctor.safe_create_at(clazz_mem, 42.0)};
            CHECK_FALSE(v);

            CHECK(clazz_t::constructor_counter == 0);
        }
        {
            std::optional<meta::uvalue> v{ctor.safe_create_at(clazz_mem, 42)};
            CHECK(v);

            CHECK(clazz_t::constructor_counter == 1);
            CHECK(v->get_type() == meta::resolve_type<clazz_t*>());
            CHECK(v->get_as<clazz_t*>()->i == 42);
        }
    }

    SUBCASE("clazz_t&&") {
        {
            const meta::constructor ctor = clazz_type.get_constructor_with<clazz_t&&>();
            REQUIRE(ctor);
            CHECK(ctor.get_type() == meta::resolve_constructor_type<clazz_t, clazz_t&&>());
            CHECK(ctor.get_type().get_owner_type() == meta::resolve_type<clazz_t>());
            CHECK(ctor.get_type().get_flags() == meta::constructor_flags::is_noexcept);
        }
        {
            clazz_t o{42};
            const meta::uvalue v = clazz_type.create(std::move(o));
            CHECK(v.get_type() == meta::resolve_type<clazz_t>());
            CHECK(v.get_as<clazz_t>().i == 42);
            CHECK_FALSE(clazz_type.destroy(v));
        }
        CHECK(clazz_t::constructor_counter == 1);
        CHECK(clazz_t::destructor_counter == 2);
        CHECK(clazz_t::move_constructor_counter == 1);
        CHECK(clazz_t::copy_constructor_counter == 0);
    }

    SUBCASE("clazz_t&&/inplace") {
        {
            clazz_t o{42};
            const meta::uvalue v = clazz_type
                .get_constructor_with<clazz_t&&>()
                .create_at(clazz_mem, std::move(o));

            CHECK(v.get_type() == meta::resolve_type<clazz_t*>());
            CHECK(v.get_as<clazz_t*>()->i == 42);

            clazz_type.get_destructor().destroy_at(clazz_mem);
        }
        CHECK(clazz_t::constructor_counter == 1);
        CHECK(clazz_t::destructor_counter == 2);
        CHECK(clazz_t::move_constructor_counter == 1);
        CHECK(clazz_t::copy_constructor_counter == 0);
    }

    SUBCASE("const clazz_t&") {
        {
            const meta::constructor ctor = clazz_type.get_constructor_with<const clazz_t&>();
            REQUIRE(ctor);
            CHECK(ctor.get_type() == meta::resolve_constructor_type<clazz_t, const clazz_t&>());
        }
        {
            clazz_t o{42};
            const meta::uvalue v = clazz_type.create(std::as_const(o));
            CHECK(v.get_type() == meta::resolve_type<clazz_t>());
            CHECK(v.get_as<clazz_t>().i == 42);
            CHECK_FALSE(clazz_type.destroy(v));
        }
        CHECK(clazz_t::constructor_counter == 1);
        CHECK(clazz_t::destructor_counter == 2);
        CHECK(clazz_t::move_constructor_counter == 0);
        CHECK(clazz_t::copy_constructor_counter == 1);
    }

    SUBCASE("const clazz_t&/inplace") {
        {
            clazz_t o{42};
            const meta::uvalue v = clazz_type
                .get_constructor_with<const clazz_t&>()
                .create_at(clazz_mem, std::as_const(o));

            CHECK(v.get_type() == meta::resolve_type<clazz_t*>());
            CHECK(v.get_as<clazz_t*>()->i == 42);

            clazz_type.get_destructor().destroy_at(clazz_mem);
        }
        CHECK(clazz_t::constructor_counter == 1);
        CHECK(clazz_t::destructor_counter == 2);
        CHECK(clazz_t::move_constructor_counter == 0);
        CHECK(clazz_t::copy_constructor_counter == 1);
    }
}

TEST_CASE("meta/meta_states/ctor/as_raw_pointer") {
    namespace meta = meta_hpp;

    using clazz_t = clazz<meta::constructor_policy::as_raw_pointer_t>;

    meta::class_<clazz_t>()
        .constructor_<int>(meta::constructor_policy::as_raw_pointer)
        .constructor_<clazz_t&&>(meta::constructor_policy::as_raw_pointer)
        .constructor_<const clazz_t&>(meta::constructor_policy::as_raw_pointer);

    clazz_t::constructor_counter = 0;
    clazz_t::destructor_counter = 0;
    clazz_t::move_constructor_counter = 0;
    clazz_t::copy_constructor_counter = 0;

    const meta::class_type clazz_type = meta::resolve_type<clazz_t>();
    REQUIRE(clazz_type);

    SUBCASE("int") {
        {
            const meta::constructor ctor = clazz_type.get_constructor_with<int>();
            REQUIRE(ctor);
            CHECK(ctor.get_type() == meta::resolve_constructor_type<clazz_t, int>());
        }
        {
            const meta::uvalue v = clazz_type.create(42);
            CHECK(v.get_type() == meta::resolve_type<clazz_t*>());
            CHECK(v.get_as<clazz_t*>()->i == 42);
            CHECK(clazz_type.destroy(v));
        }
        CHECK(clazz_t::constructor_counter == 1);
        CHECK(clazz_t::destructor_counter == 1);
        CHECK(clazz_t::move_constructor_counter == 0);
        CHECK(clazz_t::copy_constructor_counter == 0);
    }

    SUBCASE("clazz_t&&") {
        {
            const meta::constructor ctor = clazz_type.get_constructor_with<clazz_t&&>();
            REQUIRE(ctor);
            CHECK(ctor.get_type() == meta::resolve_constructor_type<clazz_t, clazz_t&&>());
        }
        {
            clazz_t o{42};
            const meta::uvalue v = clazz_type.create(std::move(o));
            CHECK(v.get_type() == meta::resolve_type<clazz_t*>());
            CHECK(v.get_as<clazz_t*>()->i == 42);
            CHECK(clazz_type.destroy(v));
        }
        CHECK(clazz_t::constructor_counter == 1);
        CHECK(clazz_t::destructor_counter == 2);
        CHECK(clazz_t::move_constructor_counter == 1);
        CHECK(clazz_t::copy_constructor_counter == 0);
    }

    SUBCASE("const clazz_t&") {
        {
            const meta::constructor ctor = clazz_type.get_constructor_with<const clazz_t&>();
            REQUIRE(ctor);
            CHECK(ctor.get_type() == meta::resolve_constructor_type<clazz_t, const clazz_t&>());
        }
        {
            clazz_t o{42};
            const meta::uvalue v = clazz_type.create(std::as_const(o));
            CHECK(v.get_type() == meta::resolve_type<clazz_t*>());
            CHECK(v.get_as<clazz_t*>()->i == 42);
            CHECK(clazz_type.destroy(v));
        }
        CHECK(clazz_t::constructor_counter == 1);
        CHECK(clazz_t::destructor_counter == 2);
        CHECK(clazz_t::move_constructor_counter == 0);
        CHECK(clazz_t::copy_constructor_counter == 1);
    }
}

TEST_CASE("meta/meta_states/ctor/as_shared_pointer") {
    namespace meta = meta_hpp;

    using clazz_t = clazz<meta::constructor_policy::as_shared_pointer_t>;

    meta::class_<clazz_t>()
        .constructor_<int>(meta::constructor_policy::as_shared_pointer)
        .constructor_<clazz_t&&>(meta::constructor_policy::as_shared_pointer)
        .constructor_<const clazz_t&>(meta::constructor_policy::as_shared_pointer);

    clazz_t::constructor_counter = 0;
    clazz_t::destructor_counter = 0;
    clazz_t::move_constructor_counter = 0;
    clazz_t::copy_constructor_counter = 0;

    const meta::class_type clazz_type = meta::resolve_type<clazz_t>();
    REQUIRE(clazz_type);

    SUBCASE("int") {
        {
            const meta::constructor ctor = clazz_type.get_constructor_with<int>();
            REQUIRE(ctor);
            CHECK(ctor.get_type() == meta::resolve_constructor_type<clazz_t, int>());
        }
        {
            const meta::uvalue v = clazz_type.create(42);
            CHECK(v.get_type() == meta::resolve_type<std::shared_ptr<clazz_t>>());
            CHECK(v.get_as<std::shared_ptr<clazz_t>>()->i == 42);
            CHECK_FALSE(clazz_type.destroy(v));
        }
        CHECK(clazz_t::constructor_counter == 1);
        CHECK(clazz_t::destructor_counter == 1);
        CHECK(clazz_t::move_constructor_counter == 0);
        CHECK(clazz_t::copy_constructor_counter == 0);
    }

    SUBCASE("clazz_t&&") {
        {
            const meta::constructor ctor = clazz_type.get_constructor_with<clazz_t&&>();
            REQUIRE(ctor);
            CHECK(ctor.get_type() == meta::resolve_constructor_type<clazz_t, clazz_t&&>());
        }
        {
            clazz_t o{42};
            const meta::uvalue v = clazz_type.create(std::move(o));
            CHECK(v.get_type() == meta::resolve_type<std::shared_ptr<clazz_t>>());
            CHECK(v.get_as<std::shared_ptr<clazz_t>>()->i == 42);
            CHECK_FALSE(clazz_type.destroy(v));
        }
        CHECK(clazz_t::constructor_counter == 1);
        CHECK(clazz_t::destructor_counter == 2);
        CHECK(clazz_t::move_constructor_counter == 1);
        CHECK(clazz_t::copy_constructor_counter == 0);
    }

    SUBCASE("const clazz_t&") {
        {
            const meta::constructor ctor = clazz_type.get_constructor_with<const clazz_t&>();
            REQUIRE(ctor);
            CHECK(ctor.get_type() == meta::resolve_constructor_type<clazz_t, const clazz_t&>());
        }
        {
            clazz_t o{42};
            const meta::uvalue v = clazz_type.create(std::as_const(o));
            CHECK(v.get_type() == meta::resolve_type<std::shared_ptr<clazz_t>>());
            CHECK(v.get_as<std::shared_ptr<clazz_t>>()->i == 42);
            CHECK_FALSE(clazz_type.destroy(v));
        }
        CHECK(clazz_t::constructor_counter == 1);
        CHECK(clazz_t::destructor_counter == 2);
        CHECK(clazz_t::move_constructor_counter == 0);
        CHECK(clazz_t::copy_constructor_counter == 1);
    }
}
