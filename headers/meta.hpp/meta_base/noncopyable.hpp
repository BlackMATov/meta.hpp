/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"

namespace meta_hpp::detail
{
    class noncopyable {
    protected:
        noncopyable() = default;
        ~noncopyable() = default;

    public:
        noncopyable(noncopyable&&) = delete;
        noncopyable(const noncopyable&) = delete;
        noncopyable& operator=(noncopyable&&) = delete;
        noncopyable& operator=(const noncopyable&) = delete;
    };
}
