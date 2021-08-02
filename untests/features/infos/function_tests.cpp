/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "_infos_fwd.hpp"

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
    };

    void arg_copy(ivec2) {}

    void arg_ref(ivec2&) {}
    void arg_cref(const ivec2&) {}

    void arg_rref(ivec2&&) {}
    void arg_crref(const ivec2&&) {}
}

TEST_CASE("features/infos/function") {
    auto db = registry{}(
        function_{"arg_copy", &arg_copy},
        function_{"arg_ref", &arg_ref},
        function_{"arg_cref", &arg_cref},
        function_{"arg_rref", &arg_rref},
        function_{"arg_crref", &arg_crref}
    );

    const function_info arg_copy_info = db.get_function_by_name("arg_copy");
    const function_info arg_ref_info = db.get_function_by_name("arg_ref");
    const function_info arg_cref_info = db.get_function_by_name("arg_cref");
    const function_info arg_rref_info = db.get_function_by_name("arg_rref");
    const function_info arg_crref_info = db.get_function_by_name("arg_crref");
    const function_info arg_not_exist_info = db.get_function_by_name("arg_not_exist");

    REQUIRE(arg_copy_info);
    REQUIRE(arg_ref_info);
    REQUIRE(arg_cref_info);
    REQUIRE(arg_rref_info);
    REQUIRE(arg_crref_info);
    REQUIRE_FALSE(arg_not_exist_info);
}
