/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

TEST_CASE("meta/meta_base/type_id") {
    namespace meta = meta_hpp;
    using meta::detail::type_id;
    using meta::detail::type_list;

    static_assert(std::is_nothrow_constructible_v<type_id, type_list<int>>);
    static_assert(std::is_nothrow_copy_constructible_v<type_id>);
    static_assert(std::is_nothrow_move_constructible_v<type_id>);
    static_assert(std::is_nothrow_copy_assignable_v<type_id>);
    static_assert(std::is_nothrow_move_assignable_v<type_id>);

    SUBCASE("ctor") {
        type_id int1_id{type_list<int>{}};
        type_id int2_id{type_list<int>{}};
        type_id float_id{type_list<float>{}};

        CHECK(int1_id == int2_id);
        CHECK_FALSE(int2_id == float_id);

        CHECK((int2_id < float_id || float_id < int2_id));
        CHECK_FALSE((int1_id < int2_id || int2_id < int1_id));
    }

    SUBCASE("get_hash") {
        type_id int1_id{type_list<int>{}};
        type_id int2_id{type_list<int>{}};
        type_id float_id{type_list<float>{}};

        CHECK(int1_id.get_hash() == int2_id.get_hash());
        CHECK_FALSE(int2_id.get_hash() == float_id.get_hash());
    }

    SUBCASE("swap") {
        const type_id int_id{type_list<int>{}};
        const type_id float_id{type_list<float>{}};

        type_id id1{type_list<int>{}};
        type_id id2{type_list<float>{}};

        id1.swap(id2);
        CHECK(id1 == float_id);
        CHECK(id2 == int_id);

        swap(id1, id2);
        CHECK(id1 == int_id);
        CHECK(id2 == float_id);
    }
}
