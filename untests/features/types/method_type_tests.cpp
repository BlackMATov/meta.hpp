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
    struct ivec2 {
        int x{};
        int y{};

        int& at(std::size_t i) {
            switch ( i ) {
            case 0: return x;
            case 1: return y;
            default: throw std::out_of_range("ivec2::at");
            }
        }

        int length2() const noexcept {
            return x * x + y * y;
        }
    };
}

TEST_CASE("features/types/method") {
    SUBCASE("ivec2::at") {
        using type = decltype(&ivec2::at);

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<method_type>());

        const method_type mt = type_db::get<type>().as<method_type>();
        CHECK(mt.class_type().id() == type_db::get<ivec2>().id());
        CHECK(mt.return_type().id() == type_db::get<int&>().id());
        CHECK(mt.argument_types().size() == 1);
        CHECK(mt.arity() == 1);
        CHECK_FALSE(mt.is_const());
        CHECK_FALSE(mt.is_noexcept());

        {
            REQUIRE(mt.argument_type(0));
            const any_type arg0 = mt.argument_type(0);
            CHECK(arg0.id() == type_db::get<std::size_t>().id());
        }

        {
            CHECK_FALSE(mt.argument_type(1));
        }
    }

    SUBCASE("ivec2::length2") {
        using type = decltype(&ivec2::length2);

        REQUIRE(type_db::get<type>());
        REQUIRE(type_db::get<type>().is<method_type>());

        const method_type mt = type_db::get<type>().as<method_type>();
        CHECK(mt.class_type().id() == type_db::get<ivec2>().id());
        CHECK(mt.return_type().id() == type_db::get<int>().id());
        CHECK(mt.argument_types().size() == 0);
        CHECK(mt.arity() == 0);
        CHECK(mt.is_const());
        CHECK(mt.is_noexcept());

        {
            CHECK_FALSE(mt.argument_type(0));
        }
    }
}
