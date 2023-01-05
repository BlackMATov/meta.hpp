/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
    class rect {
    public:
        void get_width(int* v) {
            *v = 42;
        }

        static void get_width_static(int* v) {
            *v = 84;
        }
    };
}

TEST_CASE("meta/meta_issues/15") {
    namespace meta = meta_hpp;

    meta::class_<rect>()
        .method_("get_width", &rect::get_width)
        .function_("get_width_static", &rect::get_width_static);

    int v{};
    rect r{};

    meta::resolve_type(r).get_method("get_width").invoke(r, &v);
    CHECK(v == 42);

    meta::resolve_type(r).get_function("get_width_static").invoke(&v);
    CHECK(v == 84);
}
