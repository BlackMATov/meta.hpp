/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
    class function_type final : public base_type {
    public:
        template < typename T >
        explicit function_type(typename_arg_t<T>)
        : base_type{typename_arg<T>} {}
    };
}
