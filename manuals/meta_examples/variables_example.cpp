/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_manuals.hpp"

namespace
{
    const double pi_v = 3.1415926536;
    const double sqrt2_v = 1.4142135624;
}

TEST_CASE("meta/meta_examples/variables/usage") {
    namespace meta = meta_hpp;

    // 'constants' local scope declaration
    const meta::scope constants_scope = meta::local_scope_("constants")
        .variable_("pi_v", &pi_v)
        .variable_("sqrt2_v", &sqrt2_v);

    // finds the variable by name 'pi_v'
    const meta::variable pi_variable = constants_scope.get_variable("pi_v");

    // checks the type and data type of the founded variable
    CHECK(pi_variable.get_type() == meta::resolve_type<const double*>());
    CHECK(pi_variable.get_type().get_data_type() == meta::resolve_type<double>());

    // retrieves a variable value
    const meta::value pi_variable_value = pi_variable.get();

    // checks the type of the retrieved value
    CHECK(pi_variable_value.get_type() == meta::resolve_type<double>());

    // checks the typed variable value
    CHECK(pi_variable_value.cast<double>() == doctest::Approx(3.14).epsilon(0.01));

    // we can change variable values, but only non-const
    CHECK_THROWS(pi_variable.set(6.0));

    // prints all variables in the scope
    fmt::print("* {}:\n", constants_scope.get_name());
    for ( auto&& [index, variable] : constants_scope.get_variables() ) {
        fmt::print("  - {}:{}\n", index.name, variable.get());
    }
}
