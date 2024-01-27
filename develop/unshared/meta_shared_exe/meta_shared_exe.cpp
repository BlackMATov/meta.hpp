/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta_shared_lib.hpp>

#include <doctest/doctest.h>

TEST_CASE("meta/meta_shared/exe") {
    namespace meta = meta_hpp;
    namespace shared = meta_shared_lib;

    const meta::scope library_scope = shared::get_library_scope();
    REQUIRE(library_scope);

    //

    const meta::number_type int_type = library_scope.get_typedef("int").as_number();
    REQUIRE(int_type);

    const meta::class_type ivec2_type = library_scope.get_typedef("ivec2").as_class();
    REQUIRE(ivec2_type);

    const meta::constructor ivec2_ctor2 = ivec2_type.get_constructor_with({int_type, int_type});
    REQUIRE(ivec2_ctor2);

    const meta::method ivec2_length2 = ivec2_type.get_method("length2");
    REQUIRE(ivec2_length2);

    //

    const meta::uvalue ivec2_inst = ivec2_ctor2.create(shared::wrap_int(3), shared::wrap_int(4));
    REQUIRE(ivec2_inst);

    const meta::uvalue ivec2_inst_length2 = ivec2_length2.invoke(ivec2_inst);
    REQUIRE(ivec2_inst_length2);

    CHECK(shared::unwrap_int(ivec2_inst_length2) == 25);
}
