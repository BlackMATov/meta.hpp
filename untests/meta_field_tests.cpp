/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_field.hpp>
#include "doctest/doctest.hpp"

namespace
{
    class clazz {
    public:
        int field{1};
        const int cfield{2};
    };
}

TEST_CASE("meta/field") {
    namespace meta = meta_hpp;

    meta::field_<&clazz::field> field_{"field"};
    meta::field_<&clazz::cfield> cfield_{"cfield"};

    const meta::field_info& field_info = field_;
    const meta::field_info& cfield_info = cfield_;

    {
        clazz instance;

        CHECK(instance.field == 1);
        CHECK(field_info.get(instance).cast<int>() == 1);
        CHECK(field_info.get(std::as_const(instance)).cast<int>() == 1);

        CHECK_NOTHROW(field_info.set(instance, 3));
        CHECK_THROWS_AS(field_info.set(instance, 4.f), std::logic_error);

        CHECK(instance.field == 3);
        CHECK(field_info.get(instance).cast<int>() == 3);
        CHECK(field_info.get(std::as_const(instance)).cast<int>() == 3);
    }

    {
        clazz instance;

        CHECK(instance.cfield == 2);
        CHECK(cfield_info.get(instance).cast<int>() == 2);
        CHECK(cfield_info.get(std::as_const(instance)).cast<int>() == 2);

        CHECK_THROWS_AS(cfield_info.set(instance, 4), std::logic_error);

        CHECK(instance.cfield == 2);
        CHECK(cfield_info.get(instance).cast<int>() == 2);
        CHECK(cfield_info.get(std::as_const(instance)).cast<int>() == 2);
    }

    {
        clazz instance;

        instance.field = 5;
        CHECK(field_info.get(instance).cast<int>() == 5);
        CHECK(field_info.get(std::as_const(instance)).cast<int>() == 5);
    }
}
