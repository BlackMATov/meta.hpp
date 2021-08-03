/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
    class enum_type final : public base_type {
    public:
        template < typename T >
        explicit enum_type(typename_arg_t<T>)
        : base_type{typename_arg<T>} {
            static_assert(std::is_enum_v<T>);
        }
    };
}
