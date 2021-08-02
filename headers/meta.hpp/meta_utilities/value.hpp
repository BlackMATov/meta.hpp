/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_fwd.hpp"

namespace meta_hpp
{
    class value final {
    public:
        value() = delete;

        value(value&&) = default;
        value& operator=(value&&) = default;

        value(const value&) = default;
        value& operator=(const value&) = default;
    };
}
