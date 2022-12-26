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

TEST_CASE("meta/meta_utilities/value4/get_as") {
    namespace meta = meta_hpp;

    static_assert(std::same_as<decltype(std::declval<meta::uvalue&>().get_as<derived>()), derived&>);
    static_assert(std::same_as<decltype(std::declval<meta::uvalue&&>().get_as<derived>()), derived&>);
    static_assert(std::same_as<decltype(std::declval<const meta::uvalue&>().get_as<derived>()), const derived&>);
    static_assert(std::same_as<decltype(std::declval<const meta::uvalue&&>().get_as<derived>()), const derived&>);

    static_assert(std::same_as<decltype(std::declval<meta::uvalue&>().get_as<derived*>()), derived*>);
    static_assert(std::same_as<decltype(std::declval<meta::uvalue&&>().get_as<derived*>()), derived*>);
    static_assert(std::same_as<decltype(std::declval<const meta::uvalue&>().get_as<derived*>()), derived*>);
    static_assert(std::same_as<decltype(std::declval<const meta::uvalue&&>().get_as<derived*>()), derived*>);

    static_assert(std::same_as<decltype(std::declval<meta::uvalue&>().get_as<const derived*>()), const derived*>);
    static_assert(std::same_as<decltype(std::declval<meta::uvalue&&>().get_as<const derived*>()), const derived*>);
    static_assert(std::same_as<decltype(std::declval<const meta::uvalue&>().get_as<const derived*>()), const derived*>);
    static_assert(std::same_as<decltype(std::declval<const meta::uvalue&&>().get_as<const derived*>()), const derived*>);

    SUBCASE("derived to derived") {
        {
            meta::uvalue v{derived{}};
            CHECK(v.get_as<derived>().l == 168);
            CHECK_THROWS(std::ignore = v.get_as<derived2>());
        }
        {
            const meta::uvalue v{derived{}};
            CHECK(v.get_as<derived>().l == 168);
            CHECK_THROWS(std::ignore = v.get_as<derived2>());
        }
    }

    SUBCASE("derived to base") {
        {
            meta::uvalue v{derived{}};
            CHECK(v.get_as<base2>().k == 84);
        }
        {
            const meta::uvalue v{derived{}};
            CHECK(v.get_as<base2>().k == 84);
        }
    }

    SUBCASE("voidptr") {
        {
            derived d{};
            meta::uvalue v{&d};
            CHECK(v.get_as<void*>() == &d);
            CHECK(v.get_as<const void*>() == &d);
        }
        {
            const derived d{};
            meta::uvalue v{&d};
            CHECK_THROWS(std::ignore = v.get_as<void*>());
            CHECK(v.get_as<const void*>() == &d);
        }
        {
            meta::uvalue v{derived{}};
            CHECK_THROWS(std::ignore = v.get_as<void*>());
            CHECK_THROWS(std::ignore = v.get_as<const void*>());
        }
    }

    SUBCASE("nullptr") {
        {
            meta::uvalue v{nullptr};
            CHECK(v.get_as<void*>() == nullptr);
            CHECK(v.get_as<const void*>() == nullptr);
            CHECK(v.get_as<derived*>() == nullptr);
            CHECK(v.get_as<const derived*>() == nullptr);

            CHECK_THROWS(std::ignore = v.get_as<derived>());
        }
        {
            const meta::uvalue v{nullptr};
            CHECK(v.get_as<void*>() == nullptr);
            CHECK(v.get_as<const void*>() == nullptr);
            CHECK(v.get_as<derived*>() == nullptr);
            CHECK(v.get_as<const derived*>() == nullptr);

            CHECK_THROWS(std::ignore = v.get_as<derived>());
        }
    }

    SUBCASE("derived* to derived*") {
        {
            derived d{};
            meta::uvalue v{&d};
            CHECK(v.get_as<derived*>()->l == 168);
            CHECK(v.get_as<const derived*>()->l == 168);

            CHECK_THROWS(std::ignore = v.get_as<derived2*>());
            CHECK_THROWS(std::ignore = v.get_as<const derived2*>());
        }
        {
            const derived d{};
            meta::uvalue v{&d};
            CHECK(v.get_as<const derived*>()->l == 168);

            CHECK_THROWS(std::ignore = v.get_as<const derived2*>());
        }
    }

    SUBCASE("derived* to base*") {
        {
            derived d{};
            meta::uvalue v{&d};
            CHECK(v.get_as<base2*>()->k == 84);
            CHECK(v.get_as<const base2*>()->k == 84);
        }

        {
            const derived d{};
            meta::uvalue v{&d};
            CHECK_THROWS(std::ignore = v.get_as<base2*>());
            CHECK(v.get_as<const base2*>()->k == 84);
        }
    }
}

TEST_CASE("meta/meta_utilities/value4/try_get_as") {
    namespace meta = meta_hpp;

    static_assert(std::same_as<decltype(std::declval<meta::uvalue&>().try_get_as<derived>()), derived*>);
    static_assert(std::same_as<decltype(std::declval<meta::uvalue&&>().try_get_as<derived>()), derived*>);
    static_assert(std::same_as<decltype(std::declval<const meta::uvalue&>().try_get_as<derived>()), const derived*>);
    static_assert(std::same_as<decltype(std::declval<const meta::uvalue&&>().try_get_as<derived>()), const derived*>);

    static_assert(std::same_as<decltype(std::declval<meta::uvalue&>().try_get_as<derived*>()), derived*>);
    static_assert(std::same_as<decltype(std::declval<meta::uvalue&&>().try_get_as<derived*>()), derived*>);
    static_assert(std::same_as<decltype(std::declval<const meta::uvalue&>().try_get_as<derived*>()), derived*>);
    static_assert(std::same_as<decltype(std::declval<const meta::uvalue&&>().try_get_as<derived*>()), derived*>);

    static_assert(std::same_as<decltype(std::declval<meta::uvalue&>().try_get_as<const derived*>()), const derived*>);
    static_assert(std::same_as<decltype(std::declval<meta::uvalue&&>().try_get_as<const derived*>()), const derived*>);
    static_assert(std::same_as<decltype(std::declval<const meta::uvalue&>().try_get_as<const derived*>()), const derived*>);
    static_assert(std::same_as<decltype(std::declval<const meta::uvalue&&>().try_get_as<const derived*>()), const derived*>);

    SUBCASE("derived to derived") {
        {
            meta::uvalue v{derived{}};
            CHECK(v.try_get_as<derived>()->l == 168);
            CHECK_FALSE(v.try_get_as<derived2>());
        }
        {
            const meta::uvalue v{derived{}};
            CHECK(v.try_get_as<derived>()->l == 168);
            CHECK_FALSE(v.try_get_as<derived2>());
        }
    }

    SUBCASE("derived to base") {
        {
            meta::uvalue v{derived{}};
            CHECK(v.try_get_as<base2>()->k == 84);
        }
        {
            const meta::uvalue v{derived{}};
            CHECK(v.try_get_as<base2>()->k == 84);
        }
    }

    SUBCASE("voidptr") {
        {
            derived d{};
            meta::uvalue v{&d};
            CHECK(v.try_get_as<void*>() == &d);
            CHECK(v.try_get_as<const void*>() == &d);
        }
        {
            const derived d{};
            meta::uvalue v{&d};
            CHECK_FALSE(v.try_get_as<void*>());
            CHECK(v.try_get_as<const void*>() == &d);
        }
        {
            meta::uvalue v{derived{}};
            CHECK_FALSE(v.try_get_as<void*>());
            CHECK_FALSE(v.try_get_as<const void*>());
        }
    }

    SUBCASE("nullptr") {
        {
            meta::uvalue v{nullptr};
            CHECK(v.try_get_as<void*>() == nullptr);
            CHECK(v.try_get_as<const void*>() == nullptr);
            CHECK(v.try_get_as<derived*>() == nullptr);
            CHECK(v.try_get_as<const derived*>() == nullptr);

            CHECK_FALSE(v.try_get_as<derived>());
        }
        {
            const meta::uvalue v{nullptr};
            CHECK(v.try_get_as<void*>() == nullptr);
            CHECK(v.try_get_as<const void*>() == nullptr);
            CHECK(v.try_get_as<derived*>() == nullptr);
            CHECK(v.try_get_as<const derived*>() == nullptr);

            CHECK_FALSE(v.try_get_as<derived>());
        }
    }

    SUBCASE("derived* to derived*") {
        {
            derived d{};
            meta::uvalue v{&d};
            CHECK(v.try_get_as<derived*>()->l == 168);
            CHECK(v.try_get_as<const derived*>()->l == 168);

            CHECK_FALSE(v.try_get_as<derived2*>());
            CHECK_FALSE(v.try_get_as<const derived2*>());
        }
        {
            const derived d{};
            meta::uvalue v{&d};
            CHECK(v.try_get_as<const derived*>()->l == 168);

            CHECK_FALSE(v.try_get_as<const derived2*>());
        }
    }

    SUBCASE("derived* to base*") {
        {
            derived d{};
            meta::uvalue v{&d};
            CHECK(v.try_get_as<base2*>()->k == 84);
            CHECK(v.try_get_as<const base2*>()->k == 84);
        }

        {
            const derived d{};
            meta::uvalue v{&d};
            CHECK_FALSE(v.try_get_as<base2*>());
            CHECK(v.try_get_as<const base2*>()->k == 84);
        }
    }
}
