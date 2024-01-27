/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "meta_shared_lib.hpp"

namespace
{
    using namespace meta_shared_lib;

    struct ivec2 {
        int x;
        int y;

        explicit ivec2(int nv) : x{nv}, y{nv} {}
        ivec2(int nx, int ny) : x{nx}, y{ny} {}

        int length2() const noexcept {
            return x * x + y * y;
        }
    };

    void bind_ivec2() {
        meta::class_<ivec2>()
            .constructor_<int>()
            .constructor_<int, int>()
            .member_("x", &ivec2::x)
            .member_("y", &ivec2::y)
            .method_("length2", &ivec2::length2);
    }

    const auto library_scope = []() -> meta::scope {
        bind_ivec2();

        return meta::local_scope_("library_scope")
            .typedef_<int>("int")
            .typedef_<ivec2>("ivec2");
    }();
}

namespace meta_shared_lib
{
    meta::scope get_library_scope() {
        return library_scope;
    }

    meta::uvalue wrap_int(int value) {
        return value;
    }

    int unwrap_int(const meta::uvalue& value) {
        return value.as<int>();
    }
}
