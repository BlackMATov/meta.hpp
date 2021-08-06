/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "_types_fwd.hpp"

namespace
{
    using namespace meta_hpp;
    using namespace std::string_literals;
}

namespace
{
    struct ivec2 final {
        int x{};
        int y{};

        [[maybe_unused]] ivec2() = default;
        [[maybe_unused]] explicit ivec2(int v): x{v}, y{v} {}
        [[maybe_unused]] ivec2(int x, int y): x{x}, y{y} {}
    };
}

TEST_CASE("features/types/ctor") {
    SUBCASE("ivec2_void") {
        ctor_type ct{typename_arg<ivec2>, typename_arg<>};

        CHECK(ct.class_type().id() == type_db::get<ivec2>().id());
        CHECK(ct.argument_types().size() == 0);
        CHECK(ct.arity() == 0);

        CHECK(ct.flags() == (ctor_flags::is_noexcept));
        CHECK(ct.is_noexcept());

        {
            CHECK_FALSE(ct.argument_type(0));
        }
    }

    SUBCASE("ivec2_int") {
        ctor_type ct{typename_arg<ivec2>, typename_arg<int>};

        CHECK(ct.arity() == 1);
        CHECK(ct.class_type().id() == type_db::get<ivec2>().id());
        CHECK(ct.argument_types().size() == 1);

        CHECK(ct.flags() == (ctor_flags{}));
        CHECK_FALSE(ct.is_noexcept());

        {
            REQUIRE(ct.argument_type(0));
            const any_type arg0 = ct.argument_type(0);
            CHECK(arg0.id() == type_db::get<int>().id());
        }

        {
            CHECK_FALSE(ct.argument_type(1));
        }
    }

    SUBCASE("ivec2_int_int") {
        ctor_type ct{typename_arg<ivec2>, typename_arg<int, int>};

        CHECK(ct.class_type().id() == type_db::get<ivec2>().id());
        CHECK(ct.argument_types().size() == 2);
        CHECK(ct.arity() == 2);

        CHECK(ct.flags() == (ctor_flags{}));
        CHECK_FALSE(ct.is_noexcept());

        {
            REQUIRE(ct.argument_type(0));
            const any_type arg0 = ct.argument_type(0);
            CHECK(arg0.id() == type_db::get<int>().id());
        }

        {
            REQUIRE(ct.argument_type(1));
            const any_type arg1 = ct.argument_type(1);
            CHECK(arg1.id() == type_db::get<int>().id());
        }

        {
            CHECK_FALSE(ct.argument_type(2));
        }
    }
}
