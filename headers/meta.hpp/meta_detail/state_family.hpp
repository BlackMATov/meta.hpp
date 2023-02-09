/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    concept state_family                 //
        = std::is_same_v<T, argument>    //
       || std::is_same_v<T, constructor> //
       || std::is_same_v<T, destructor>  //
       || std::is_same_v<T, evalue>      //
       || std::is_same_v<T, function>    //
       || std::is_same_v<T, member>      //
       || std::is_same_v<T, method>      //
       || std::is_same_v<T, scope>       //
       || std::is_same_v<T, variable>;   //

    template < state_family State >
    struct state_traits;

    template < state_family State >
    [[nodiscard]] typename state_traits<State>::state_ptr state_access(const State& state) {
        return state.state_;
    }
}

namespace meta_hpp::detail
{
    template <>
    struct state_traits<argument> {
        using index_type = argument_index;
        using state_ptr = argument_state_ptr;
    };

    template <>
    struct state_traits<constructor> {
        using index_type = constructor_index;
        using state_ptr = constructor_state_ptr;
    };

    template <>
    struct state_traits<destructor> {
        using index_type = destructor_index;
        using state_ptr = destructor_state_ptr;
    };

    template <>
    struct state_traits<evalue> {
        using index_type = evalue_index;
        using state_ptr = evalue_state_ptr;
    };

    template <>
    struct state_traits<function> {
        using index_type = function_index;
        using state_ptr = function_state_ptr;
    };

    template <>
    struct state_traits<member> {
        using index_type = member_index;
        using state_ptr = member_state_ptr;
    };

    template <>
    struct state_traits<method> {
        using index_type = method_index;
        using state_ptr = method_state_ptr;
    };

    template <>
    struct state_traits<scope> {
        using index_type = scope_index;
        using state_ptr = scope_state_ptr;
    };

    template <>
    struct state_traits<variable> {
        using index_type = variable_index;
        using state_ptr = variable_state_ptr;
    };
}
