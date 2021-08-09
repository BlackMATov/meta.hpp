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
    struct base {
        virtual ~base() = 0;
    };

    struct derived final : base {
    };
}

TEST_CASE("features/types/base") {
    base_type bt{typename_arg<base>, typename_arg<derived>};

    CHECK(bt.base_class_type().id() == type_db::get<base>().id());
    CHECK(bt.derived_class_type().id() == type_db::get<derived>().id());
}
