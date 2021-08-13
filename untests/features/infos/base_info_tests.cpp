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
    class base {
    public:
        virtual ~base() {}
    };

    class derived final : public base {
    public:
    };

    struct empty final {};
}

TEST_CASE("features/infos/base") {
    registry db;

    db(
        class_<base>{"base"},
        class_<derived>{"derived"}(
            base_<base>{}
        )
    );

    REQUIRE(db.get_class_by_path("derived"));
    const class_info derived_info = db.get_class_by_path("derived");

    CHECK(derived_info.is_derived_from<base>());
    CHECK_FALSE(derived_info.is_derived_from<empty>());
}
