/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"

namespace meta_hpp::detail
{
    struct argument_state;
    struct constructor_state;
    struct destructor_state;
    struct evalue_state;
    struct function_state;
    struct member_state;
    struct method_state;
    struct scope_state;
    struct variable_state;
}

namespace meta_hpp::detail
{
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
        using state_ptr = std::shared_ptr<argument_state>;
    };

    template <>
    struct state_traits<constructor> {
        using index_type = constructor_index;
        using state_ptr = std::shared_ptr<constructor_state>;
    };

    template <>
    struct state_traits<destructor> {
        using index_type = destructor_index;
        using state_ptr = std::shared_ptr<destructor_state>;
    };

    template <>
    struct state_traits<evalue> {
        using index_type = evalue_index;
        using state_ptr = std::shared_ptr<evalue_state>;
    };

    template <>
    struct state_traits<function> {
        using index_type = function_index;
        using state_ptr = std::shared_ptr<function_state>;
    };

    template <>
    struct state_traits<member> {
        using index_type = member_index;
        using state_ptr = std::shared_ptr<member_state>;
    };

    template <>
    struct state_traits<method> {
        using index_type = method_index;
        using state_ptr = std::shared_ptr<method_state>;
    };

    template <>
    struct state_traits<scope> {
        using index_type = scope_index;
        using state_ptr = std::shared_ptr<scope_state>;
    };

    template <>
    struct state_traits<variable> {
        using index_type = variable_index;
        using state_ptr = std::shared_ptr<variable_state>;
    };
}
