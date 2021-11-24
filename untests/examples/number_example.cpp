/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_tests.hpp"

namespace
{
}

TEST_CASE("meta/examples/number") {
    namespace meta = meta_hpp;

    {
        const meta::number_type int_type = meta::resolve_type<int>();
        REQUIRE(int_type);

        CHECK(int_type.get_flags().has(meta::number_flags::is_signed));
        CHECK(int_type.get_size() == sizeof(int));
    }

    {
        const meta::number_type unsigned_type = meta::resolve_type<unsigned>();
        REQUIRE(unsigned_type);

        CHECK(unsigned_type.get_flags().has(meta::number_flags::is_unsigned));
        CHECK(unsigned_type.get_size() == sizeof(unsigned));
    }
}
