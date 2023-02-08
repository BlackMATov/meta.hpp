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
    [[nodiscard]] typename State::state_ptr state_access(const State& state) {
        return state.state_;
    }
}
