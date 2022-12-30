/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
    struct clazz_throw_dtor {
        int i{};

        clazz_throw_dtor(int ni) : i{ni} {}
        clazz_throw_dtor(const clazz_throw_dtor&) = default;

        ~clazz_throw_dtor() noexcept(false) = default;

        static clazz_throw_dtor make(int ni) {
            return {ni};
        }
    };
}

TEST_CASE("meta/meta_utilities/value5") {
    namespace meta = meta_hpp;

    meta::class_<clazz_throw_dtor>()
        .function_("make", &clazz_throw_dtor::make);
}

TEST_CASE("meta/meta_utilities/value5/throw_dtor") {
    namespace meta = meta_hpp;

    SUBCASE("value") {
        meta::uvalue v{clazz_throw_dtor{42}};

        CHECK(v.get_type() == meta::resolve_type<clazz_throw_dtor>());
        CHECK(v.get_as<clazz_throw_dtor>().i == 42);
    }

    SUBCASE("ptr_deref") {
        clazz_throw_dtor obj{42};

        meta::uvalue v_ptr{&obj};
        CHECK(v_ptr.get_type() == meta::resolve_type<clazz_throw_dtor*>());
        CHECK(v_ptr.get_as<clazz_throw_dtor*>() == &obj);

        meta::uvalue v = *v_ptr;
        CHECK(v.get_type() == meta::resolve_type<clazz_throw_dtor>());
        CHECK(v.get_as<clazz_throw_dtor>().i == 42);
    }

    SUBCASE("array_index") {
        clazz_throw_dtor objs[2]{42, 21};

        meta::uvalue v_ptr{objs};
        CHECK(v_ptr.get_type() == meta::resolve_type<clazz_throw_dtor*>());
        CHECK(v_ptr.get_as<clazz_throw_dtor*>() == objs);

        meta::uvalue v = v_ptr[1];
        CHECK(v.get_type() == meta::resolve_type<clazz_throw_dtor>());
        CHECK(v.get_as<clazz_throw_dtor>().i == 21);
    }

    SUBCASE("as_return_value") {
        meta::class_type clazz_throw_dtor_type = meta::resolve_type<clazz_throw_dtor>();
        meta::function clazz_throw_dtor_make_function = clazz_throw_dtor_type.get_function("make");

        meta::uvalue v{clazz_throw_dtor_make_function(42)};
        CHECK(v.get_type() == meta::resolve_type<clazz_throw_dtor>());
        CHECK(v.get_as<clazz_throw_dtor>().i == 42);
    }
}
