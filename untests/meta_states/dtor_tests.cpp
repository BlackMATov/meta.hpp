/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
    class clazz_closed_dtor {
    public:
        ~clazz_closed_dtor() = delete;
    };

    class clazz_opened_dtor {
    public:
        ~clazz_opened_dtor() = default;
    };

    class clazz_virtual_dtor {
    public:
        virtual ~clazz_virtual_dtor() noexcept(false) = default;
    };

    class clazz_dtor_metadata {
    public:
        virtual ~clazz_dtor_metadata() = default;
    };
}

TEST_CASE("meta/meta_states/dtor") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    meta::class_<clazz_opened_dtor>();
    meta::class_<clazz_closed_dtor>();
    meta::class_<clazz_virtual_dtor>();

    meta::class_<clazz_dtor_metadata>()
        .destructor_({
            .metadata{
                {"desc", meta::uvalue{"virtual dtor"s}}
            }
        });

    SUBCASE("closed_dtor") {
        const meta::class_type clazz_type = meta::resolve_type<clazz_closed_dtor>();
        REQUIRE(clazz_type);

        CHECK(clazz_type.get_destructors().empty());
    }

    SUBCASE("opened_dtor") {
        const meta::class_type clazz_type = meta::resolve_type<clazz_opened_dtor>();
        REQUIRE(clazz_type);

        REQUIRE(clazz_type.get_destructors().size() == 1);
        auto&& [_, dtor] = *clazz_type.get_destructors().begin();

        CHECK(dtor.get_type().get_class_type() == meta::resolve_type<clazz_opened_dtor>());
        CHECK(dtor.get_type().get_flags() == meta::destructor_flags::is_noexcept);
    }

    SUBCASE("virtual_dtor") {
        const meta::class_type clazz_type = meta::resolve_type<clazz_virtual_dtor>();
        REQUIRE(clazz_type);

        REQUIRE(clazz_type.get_destructors().size() == 1);
        auto&& [_, dtor] = *clazz_type.get_destructors().begin();

        CHECK(dtor.get_type().get_class_type() == meta::resolve_type<clazz_virtual_dtor>());
        CHECK(dtor.get_type().get_flags() == meta::destructor_flags::is_virtual);
    }

    SUBCASE("virtual_dtor") {
        const meta::class_type clazz_type = meta::resolve_type<clazz_virtual_dtor>();
        REQUIRE(clazz_type);

        REQUIRE(clazz_type.get_destructors().size() == 1);
        auto&& [_, dtor] = *clazz_type.get_destructors().begin();

        CHECK(dtor.get_type().get_class_type() == meta::resolve_type<clazz_virtual_dtor>());
        CHECK(dtor.get_type().get_flags() == meta::destructor_flags::is_virtual);
    }

    SUBCASE("dtor_metadata") {
        const meta::class_type clazz_type = meta::resolve_type<clazz_dtor_metadata>();
        REQUIRE(clazz_type);

        REQUIRE(clazz_type.get_destructors().size() == 1);
        auto&& [_, dtor] = *clazz_type.get_destructors().begin();

        CHECK(dtor.get_type().get_class_type() == meta::resolve_type<clazz_dtor_metadata>());
        CHECK(dtor.get_type().get_flags() == (meta::destructor_flags::is_noexcept | meta::destructor_flags::is_virtual));

        CHECK(dtor.get_metadata().at("desc") == "virtual dtor"s);
    }
}
