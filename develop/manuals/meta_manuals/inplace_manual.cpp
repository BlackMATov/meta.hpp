/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>

#include <doctest/doctest.h>
#include <fmt/core.h>

namespace
{
    struct ivec2 {
        int x;
        int y;

        explicit ivec2(int nv) : x{nv}, y{nv} {}
        ivec2(int nx, int ny) : x{nx}, y{ny} {}

        int length2() const noexcept {
            return x * x + y * y;
        }
    };
}

TEST_CASE("meta/meta_manuals/inplace") {
    namespace meta = meta_hpp;

    // 'ivec2' class type registration
    meta::class_<ivec2>()
        .constructor_<int>()
        .constructor_<int, int>()
        .member_("x", &ivec2::x)
        .member_("y", &ivec2::y)
        .method_("length2", &ivec2::length2);

    // creates local scope for finding types by name
    const meta::scope math_scope = meta::local_scope_("math")
        .typedef_<ivec2>("ivec2");

    // finds required types/members/methods
    const meta::class_type ivec2_type = math_scope.get_typedef("ivec2").as_class();
    const meta::member ivec2_x = ivec2_type.get_member("x");
    const meta::member ivec2_y = ivec2_type.get_member("y");
    const meta::method ivec2_length2 = ivec2_type.get_method("length2");

    // allocates dynamic memory buffer with required size and alignment
    meta::memory_buffer ivec2_buffer{
        ivec2_type.get_size(),
        std::align_val_t{ivec2_type.get_align()}};

    // or you can use static buffer instead (if the required type is available at compile-time)
    // alignas(ivec2) std::byte ivec2_buffer[sizeof(ivec2)];

    // creates 'ivec2' on the memory buffer ('placement new' under the hood)
    const meta::uvalue ivec2_ptr = ivec2_type.create_at(ivec2_buffer.get_data(), 2, 3);

    // 'create_at' returns a pointer to the constructed object
    CHECK(ivec2_ptr.get_type() == meta::resolve_type<ivec2*>());

    // interacts with the created object as usual
    CHECK(ivec2_x.get(ivec2_ptr).as<int>() == 2);
    CHECK(ivec2_y.get(ivec2_ptr).as<int>() == 3);
    CHECK(ivec2_length2(ivec2_ptr).as<int>() == 13);

    // you must manually call the object's destructor
    CHECK(ivec2_type.destroy_at(ivec2_buffer.get_data()));
}
