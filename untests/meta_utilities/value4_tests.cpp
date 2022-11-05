/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
    struct base0 {
        int i{21};
    };

    struct base1 : virtual base0 {
        int j{42};
    };

    struct base2 : virtual base0 {
        int k{84};
    };

    struct derived : base1, base2 {
        int l{168};
    };

    struct derived2 : base1, base2 {
        int m{336};
    };
}

TEST_CASE("meta/meta_utilities/value4") {
    namespace meta = meta_hpp;

    meta::class_<base0>();

    meta::class_<base1>()
        .base_<base0>();

    meta::class_<base2>()
        .base_<base0>();

    meta::class_<derived>()
        .base_<base1>()
        .base_<base2>();

    meta::class_<derived2>()
        .base_<base1>()
        .base_<base2>();
}

TEST_CASE("meta/meta_utilities/value4/get_type") {
    namespace meta = meta_hpp;

    SUBCASE("from ref") {
        {
            derived d{};
            CHECK(meta::uvalue{d}.get_type() == meta::resolve_type<derived>());
        }
        {
            const derived d{};
            CHECK(meta::uvalue{d}.get_type() == meta::resolve_type<derived>());
        }
        {
            derived d{};
            CHECK(meta::uvalue{std::move(d)}.get_type() == meta::resolve_type<derived>());
        }
        {
            const derived d{};
            CHECK(meta::uvalue{std::move(d)}.get_type() == meta::resolve_type<derived>());
        }
    }

    SUBCASE("from ptr") {
        {
            derived d{};
            derived* pd = &d;
            CHECK(meta::uvalue{pd}.get_type() == meta::resolve_type<derived*>());
        }
        {
            derived d{};
            derived* const pd = &d;
            CHECK(meta::uvalue{pd}.get_type() == meta::resolve_type<derived*>());
        }
        {
            derived d{};
            const derived* pd = &d;
            CHECK(meta::uvalue{pd}.get_type() == meta::resolve_type<const derived*>());
        }
        {
            derived d{};
            const derived* const pd = &d;
            CHECK(meta::uvalue{pd}.get_type() == meta::resolve_type<const derived*>());
        }
    }
}

TEST_CASE("meta/meta_utilities/value4/get_as_ref") {
    namespace meta = meta_hpp;

    static_assert(std::is_same_v<decltype(get_as_ref<derived>(std::declval<meta::uvalue&>())), derived&>);
    static_assert(std::is_same_v<decltype(get_as_ref<derived>(std::declval<meta::uvalue&&>())), derived&&>);
    static_assert(std::is_same_v<decltype(get_as_ref<derived>(std::declval<const meta::uvalue&>())), const derived&>);
    static_assert(std::is_same_v<decltype(get_as_ref<derived>(std::declval<const meta::uvalue&&>())), const derived&&>);

    static_assert(std::is_same_v<decltype(get_as_ref<const derived>(std::declval<meta::uvalue&>())), const derived&>);
    static_assert(std::is_same_v<decltype(get_as_ref<const derived>(std::declval<meta::uvalue&&>())), const derived&&>);
    static_assert(std::is_same_v<decltype(get_as_ref<const derived>(std::declval<const meta::uvalue&>())), const derived&>);
    static_assert(std::is_same_v<decltype(get_as_ref<const derived>(std::declval<const meta::uvalue&&>())), const derived&&>);

    static_assert(std::is_same_v<decltype(get_as_ref<volatile derived>(std::declval<meta::uvalue&>())), volatile derived&>);
    static_assert(std::is_same_v<decltype(get_as_ref<volatile derived>(std::declval<meta::uvalue&&>())), volatile derived&&>);
    static_assert(std::is_same_v<decltype(get_as_ref<volatile derived>(std::declval<const meta::uvalue&>())), const volatile derived&>);
    static_assert(std::is_same_v<decltype(get_as_ref<volatile derived>(std::declval<const meta::uvalue&&>())), const volatile derived&&>);
}

TEST_CASE("meta/meta_utilities/value4/get_as") {
    namespace meta = meta_hpp;

    static_assert(std::is_same_v<decltype(get_as<derived>(std::declval<meta::uvalue&>())), derived>);
    static_assert(std::is_same_v<decltype(get_as<derived>(std::declval<meta::uvalue&&>())), derived>);
    static_assert(std::is_same_v<decltype(get_as<derived>(std::declval<const meta::uvalue&>())), derived>);
    static_assert(std::is_same_v<decltype(get_as<derived>(std::declval<const meta::uvalue&&>())), derived>);

    static_assert(std::is_same_v<decltype(get_as<derived&>(std::declval<meta::uvalue&>())), derived&>);
    static_assert(std::is_same_v<decltype(get_as<derived&>(std::declval<meta::uvalue&&>())), derived&>);
    static_assert(std::is_same_v<decltype(get_as<derived&>(std::declval<const meta::uvalue&>())), derived&>);
    static_assert(std::is_same_v<decltype(get_as<derived&>(std::declval<const meta::uvalue&&>())), derived&>);

    static_assert(std::is_same_v<decltype(get_as<const derived&>(std::declval<meta::uvalue&>())), const derived&>);
    static_assert(std::is_same_v<decltype(get_as<const derived&>(std::declval<meta::uvalue&&>())), const derived&>);
    static_assert(std::is_same_v<decltype(get_as<const derived&>(std::declval<const meta::uvalue&>())), const derived&>);
    static_assert(std::is_same_v<decltype(get_as<const derived&>(std::declval<const meta::uvalue&&>())), const derived&>);

    SUBCASE("derived to derived") {
        {
            meta::uvalue v{derived{}};
            CHECK(get_as<derived>(v).l == 168);
            CHECK(get_as<derived&>(v).l == 168);
            // CHECK(get_as<derived&&>(v).l == 168);
            CHECK(get_as<const derived&>(v).l == 168);
            // CHECK(get_as<const derived&&>(v).l == 168);

            CHECK_THROWS(std::ignore = get_as<derived2>(v));
            CHECK_THROWS(std::ignore = get_as<derived2&>(v));
            CHECK_THROWS(std::ignore = get_as<const derived2&>(v));
        }
        {
            meta::uvalue v{derived{}};
            CHECK(get_as<derived>(std::move(v)).l == 168);
            // CHECK(get_as<derived&>(std::move(v)).l == 168);
            CHECK(get_as<derived&&>(std::move(v)).l == 168);
            CHECK(get_as<const derived&>(std::move(v)).l == 168);
            CHECK(get_as<const derived&&>(std::move(v)).l == 168);

            CHECK_THROWS(std::ignore = get_as<derived2>(std::move(v)));
            CHECK_THROWS(std::ignore = get_as<derived2&&>(std::move(v)));
            CHECK_THROWS(std::ignore = get_as<const derived2&>(std::move(v)));
            CHECK_THROWS(std::ignore = get_as<const derived2&&>(std::move(v)));
        }
        {
            const meta::uvalue v{derived{}};
            CHECK(get_as<derived>(v).l == 168);
            // CHECK(get_as<derived&>(v).l == 168);
            // CHECK(get_as<derived&&>(v).l == 168);
            CHECK(get_as<const derived&>(v).l == 168);
            // CHECK(get_as<const derived&&>(v).l == 168);

            CHECK_THROWS(std::ignore = get_as<derived2>(v));
            CHECK_THROWS(std::ignore = get_as<const derived2&>(v));
        }
        {
            const meta::uvalue v{derived{}};
            CHECK(get_as<derived>(std::move(v)).l == 168);
            // CHECK(get_as<derived&>(std::move(v)).l == 168);
            // CHECK(get_as<derived&&>(std::move(v)).l == 168);
            CHECK(get_as<const derived&>(std::move(v)).l == 168);
            CHECK(get_as<const derived&&>(std::move(v)).l == 168);

            CHECK_THROWS(std::ignore = get_as<derived2>(std::move(v)));
            CHECK_THROWS(std::ignore = get_as<const derived2&>(std::move(v)));
            CHECK_THROWS(std::ignore = get_as<const derived2&&>(std::move(v)));
        }
    }

    SUBCASE("derived to base") {
        {
            meta::uvalue v{derived{}};
            CHECK(get_as<base2>(v).k == 84);
            CHECK(get_as<base2&>(v).k == 84);
            // CHECK(get_as<base2&&>(v).k == 84);
            CHECK(get_as<const base2&>(v).k == 84);
            // CHECK(get_as<const base2&&>(v).k == 84);
        }
        {
            meta::uvalue v{derived{}};
            CHECK(get_as<base2>(std::move(v)).k == 84);
            // CHECK(get_as<base2&>(std::move(v)).k == 84);
            CHECK(get_as<base2&&>(std::move(v)).k == 84);
            CHECK(get_as<const base2&>(std::move(v)).k == 84);
            CHECK(get_as<const base2&&>(std::move(v)).k == 84);
        }
        {
            const meta::uvalue v{derived{}};
            CHECK(get_as<base2>(v).k == 84);
            // CHECK(get_as<base2&>(v).k == 84);
            // CHECK(get_as<base2&&>(v).k == 84);
            CHECK(get_as<const base2&>(v).k == 84);
            // CHECK(get_as<const base2&&>(v).k == 84);
        }
        {
            const meta::uvalue v{derived{}};
            CHECK(get_as<base2>(std::move(v)).k == 84);
            // CHECK(get_as<base2&>(std::move(v)).k == 84);
            // CHECK(get_as<base2&&>(std::move(v)).k == 84);
            CHECK(get_as<const base2&>(std::move(v)).k == 84);
            CHECK(get_as<const base2&&>(std::move(v)).k == 84);
        }
    }

    SUBCASE("voidptr") {
        {
            derived d{};
            meta::uvalue v{&d};
            CHECK(get_as<void*>(v) == &d);
            CHECK(get_as<const void*>(v) == &d);
            CHECK(get_as<void*>(std::move(v)) == &d);
            CHECK(get_as<const void*>(std::move(v)) == &d);
        }
        {
            const derived d{};
            meta::uvalue v{&d};
            CHECK_THROWS(std::ignore = get_as<void*>(v));
            CHECK(get_as<const void*>(v) == &d);
            CHECK_THROWS(std::ignore = get_as<void*>(std::move(v)));
            CHECK(get_as<const void*>(std::move(v)) == &d);
        }
    }

    SUBCASE("nullptr") {
        {
            meta::uvalue v{nullptr};
            CHECK(get_as<void*>(v) == nullptr);
            CHECK(get_as<const void*>(v) == nullptr);
            CHECK(get_as<derived*>(v) == nullptr);
            CHECK(get_as<const derived*>(v) == nullptr);
            CHECK(get_as<void*>(std::move(v)) == nullptr);
            CHECK(get_as<const void*>(std::move(v)) == nullptr);
            CHECK(get_as<derived*>(std::move(v)) == nullptr);
            CHECK(get_as<const derived*>(std::move(v)) == nullptr);
        }
        {
            const meta::uvalue v{nullptr};
            CHECK(get_as<void*>(v) == nullptr);
            CHECK(get_as<const void*>(v) == nullptr);
            CHECK(get_as<derived*>(v) == nullptr);
            CHECK(get_as<const derived*>(v) == nullptr);
            CHECK(get_as<void*>(std::move(v)) == nullptr);
            CHECK(get_as<const void*>(std::move(v)) == nullptr);
            CHECK(get_as<derived*>(std::move(v)) == nullptr);
            CHECK(get_as<const derived*>(std::move(v)) == nullptr);
        }
    }

    SUBCASE("derived* to derived*") {
        {
            derived d{};
            meta::uvalue v{&d};
            CHECK(get_as<derived*>(v)->l == 168);
            CHECK(get_as<const derived*>(v)->l == 168);

            CHECK_THROWS(std::ignore = get_as<derived2*>(v));
            CHECK_THROWS(std::ignore = get_as<const derived2*>(v));
        }
        {
            derived d{};
            meta::uvalue v{&d};
            CHECK(get_as<derived*>(std::move(v))->l == 168);
            CHECK(get_as<const derived*>(std::move(v))->l == 168);

            CHECK_THROWS(std::ignore = get_as<derived2*>(std::move(v)));
            CHECK_THROWS(std::ignore = get_as<const derived2*>(std::move(v)));
        }
        {
            const derived d{};
            meta::uvalue v{&d};
            CHECK(get_as<const derived*>(v)->l == 168);

            CHECK_THROWS(std::ignore = get_as<const derived2*>(v));
        }
        {
            const derived d{};
            meta::uvalue v{&d};
            CHECK(get_as<const derived*>(std::move(v))->l == 168);

            CHECK_THROWS(std::ignore = get_as<const derived2*>(std::move(v)));
        }
    }

    SUBCASE("derived* to base*") {
        {
            derived d{};
            meta::uvalue v{&d};
            CHECK(get_as<base2*>(v)->k == 84);
            CHECK(get_as<const base2*>(v)->k == 84);
        }

        {
            const derived d{};
            meta::uvalue v{&d};
            CHECK_THROWS(std::ignore = get_as<base2*>(v));
            CHECK(get_as<const base2*>(v)->k == 84);
        }
    }
}

TEST_CASE("meta/meta_utilities/value4/can_get_as") {
    namespace meta = meta_hpp;

    SUBCASE("derived to derived") {
        {
            meta::uvalue v{derived{}};
            CHECK(can_get_as<derived>(v));
            CHECK(can_get_as<derived&>(v));
            // CHECK_FALSE(can_get_as<derived&&>(v));
            CHECK(can_get_as<const derived&>(v));
            // CHECK_FALSE(can_get_as<const derived&&>(v));

            CHECK_FALSE(can_get_as<derived2>(v));
            CHECK_FALSE(can_get_as<derived2&>(v));
            CHECK_FALSE(can_get_as<const derived2&>(v));
        }
        {
            meta::uvalue v{derived{}};
            CHECK(can_get_as<derived>(std::move(v)));
            // CHECK_FALSE(can_get_as<derived&>(std::move(v)));
            CHECK(can_get_as<derived&&>(std::move(v)));
            CHECK(can_get_as<const derived&>(std::move(v)));
            CHECK(can_get_as<const derived&&>(std::move(v)));

            CHECK_FALSE(can_get_as<derived2>(std::move(v)));
            CHECK_FALSE(can_get_as<derived2&&>(std::move(v)));
            CHECK_FALSE(can_get_as<const derived2&>(std::move(v)));
            CHECK_FALSE(can_get_as<const derived2&&>(std::move(v)));
        }
        {
            const meta::uvalue v{derived{}};
            CHECK(can_get_as<derived>(v));
            // CHECK_FALSE(can_get_as<derived&>(v));
            // CHECK_FALSE(can_get_as<derived&&>(v));
            CHECK(can_get_as<const derived&>(v));
            // CHECK_FALSE(can_get_as<const derived&&>(v));

            CHECK_FALSE(can_get_as<derived2>(v));
            CHECK_FALSE(can_get_as<const derived2&>(v));
        }
        {
            const meta::uvalue v{derived{}};
            CHECK(can_get_as<derived>(std::move(v)));
            // CHECK_FALSE(can_get_as<derived&>(std::move(v)));
            // CHECK_FALSE(can_get_as<derived&&>(std::move(v)));
            CHECK(can_get_as<const derived&>(std::move(v)));
            CHECK(can_get_as<const derived&&>(std::move(v)));

            CHECK_FALSE(can_get_as<derived2>(std::move(v)));
            CHECK_FALSE(can_get_as<const derived2&>(std::move(v)));
            CHECK_FALSE(can_get_as<const derived2&&>(std::move(v)));
        }
    }
}
