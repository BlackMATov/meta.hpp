/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_variable.hpp>
#include "doctest/doctest.hpp"

namespace
{
    int variable{1};
    const int cvariable{2};

    class clazz {
    public:
        static int variable;
        static const int cvariable;
    };

    int clazz::variable{1};
    const int clazz::cvariable{2};
}

TEST_CASE("meta/variable") {
    namespace meta = meta_hpp;

    SUBCASE("in_namespace") {
        meta::variable_ variable_{"variable", &variable};
        meta::variable_ cvariable_{"cvariable", &cvariable};

        const meta::variable_info& variable_info = variable_.make_info();
        const meta::variable_info& cvariable_info = cvariable_.make_info();

        {
            CHECK(variable_info.value_type() == meta::get_family_id<int>());
            CHECK(cvariable_info.value_type() == meta::get_family_id<int>());
        }

        {
            CHECK(variable == 1);
            CHECK(variable_info().cast<int>() == 1);
            CHECK(variable_info.get().cast<int>() == 1);
            CHECK(variable_info.get_r<int>() == 1);

            CHECK_NOTHROW(variable_info(3));
            CHECK_NOTHROW(variable_info.set(3));
            CHECK_THROWS_AS(variable_info(4.f), std::logic_error);
            CHECK_THROWS_AS(variable_info.set(4.f), std::logic_error);

            CHECK(variable == 3);
            CHECK(variable_info().cast<int>() == 3);
            CHECK(variable_info.get().cast<int>() == 3);
            CHECK(variable_info.get_r<int>() == 3);
        }

        {
            CHECK(cvariable == 2);
            CHECK(cvariable_info().cast<int>() == 2);
            CHECK(cvariable_info.get().cast<int>() == 2);
            CHECK(cvariable_info.get_r<int>() == 2);

            CHECK_THROWS_AS(cvariable_info(4), std::logic_error);
            CHECK_THROWS_AS(cvariable_info.set(4), std::logic_error);

            CHECK(cvariable == 2);
            CHECK(cvariable_info().cast<int>() == 2);
            CHECK(cvariable_info.get().cast<int>() == 2);
            CHECK(cvariable_info.get_r<int>() == 2);
        }

        {
            variable = 5;
            CHECK(variable_info().cast<int>() == 5);
            CHECK(variable_info.get().cast<int>() == 5);
            CHECK(variable_info.get_r<int>() == 5);
        }
    }

    SUBCASE("in_class") {
        meta::variable_ variable_{"variable", &clazz::variable};
        meta::variable_ cvariable_{"cvariable", &clazz::cvariable};

        const meta::variable_info& variable_info = variable_.make_info();
        const meta::variable_info& cvariable_info = cvariable_.make_info();

        {
            CHECK(variable_info.value_type() == meta::get_family_id<int>());
            CHECK(cvariable_info.value_type() == meta::get_family_id<int>());
        }

        {
            CHECK(clazz::variable == 1);
            CHECK(variable_info().cast<int>() == 1);
            CHECK(variable_info.get().cast<int>() == 1);
            CHECK(variable_info.get_r<int>() == 1);

            CHECK_NOTHROW(variable_info(3));
            CHECK_NOTHROW(variable_info.set(3));

            CHECK(clazz::variable == 3);
            CHECK(variable_info().cast<int>() == 3);
            CHECK(variable_info.get().cast<int>() == 3);
            CHECK(variable_info.get_r<int>() == 3);
        }

        {
            CHECK(clazz::cvariable == 2);
            CHECK(cvariable_info().cast<int>() == 2);
            CHECK(cvariable_info.get().cast<int>() == 2);
            CHECK(cvariable_info.get_r<int>() == 2);

            CHECK_THROWS_AS(cvariable_info(4), std::logic_error);
            CHECK_THROWS_AS(cvariable_info.set(4), std::logic_error);

            CHECK(clazz::cvariable == 2);
            CHECK(cvariable_info().cast<int>() == 2);
            CHECK(cvariable_info.get().cast<int>() == 2);
            CHECK(cvariable_info.get_r<int>() == 2);
        }

        {
            clazz::variable = 5;
            CHECK(variable_info().cast<int>() == 5);
            CHECK(variable_info.get().cast<int>() == 5);
            CHECK(variable_info.get_r<int>() == 5);
        }
    }
}
