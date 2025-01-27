/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2025, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>

#include <doctest/doctest.h>
#include <fmt/core.h>

namespace
{
    int add(int a, int b) {
        return a + b;
    }

    int sub(int a, int b) {
        return a - b;
    }
}

TEST_CASE("meta/meta_examples/function/type") {
    namespace meta = meta_hpp;

    // resolves a function type by reference
    const meta::function_type add_function_type = meta::resolve_type(add);

    // also, it can be resolved by static function type declaration
    CHECK(add_function_type == meta::resolve_type<int(int, int)>());

    // checks a return value type
    CHECK(add_function_type.get_return_type() == meta::resolve_type<int>());

    // allows to know an argument count
    CHECK(add_function_type.get_arity() == 2);

    // ... and their types
    CHECK(add_function_type.get_argument_type(0) == meta::resolve_type<int>());
    CHECK(add_function_type.get_argument_type(1) == meta::resolve_type<int>());
}

TEST_CASE("meta/meta_examples/function/usage") {
    namespace meta = meta_hpp;

    // 'math' local scope declaration
    const meta::scope math_scope = meta::local_scope_("math")
        .function_("add", &add)
        .function_("sub", &sub);

    // finds the function by name 'sub'
    const meta::function sub_function = math_scope.get_function("sub");

    // also, we can find it with argument types (for overloaded functions)
    CHECK(sub_function == math_scope.get_function_with<int, int>("sub"));

    // checks a type of the found function
    CHECK(sub_function.get_type() == meta::resolve_type<int(int, int)>());

    // checks the ability to call the function with specific arguments
    CHECK(sub_function.is_invocable_with(60, 18));
    CHECK(sub_function.is_invocable_with<int, int>());

    // calls the function and retrieves a returned value
    const meta::uvalue sub_result_value = sub_function.invoke(60, 18);

    // checks the type of the returned value
    CHECK(sub_result_value.get_type() == meta::resolve_type<int>());

    // casts the dynamic returned value to the typed value
    const int sub_function_typed_result = sub_result_value.as<int>();

    // here is our typed result
    CHECK(sub_function_typed_result == 42);

    // prints all functions in the scope
    fmt::print("* {}\n", math_scope.get_name());
    for ( const meta::function& function : math_scope.get_functions() ) {
        fmt::print("  + {}/{}\n",
            function.get_name(),
            function.get_type().get_arity());
    }

    // Output:
    // * math
    //   + add/2
    //   + sub/2
}
