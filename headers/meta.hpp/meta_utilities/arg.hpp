/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_fwd.hpp"

namespace meta_hpp
{
    class arg final {
    public:
        arg() = delete;

        arg(arg&&) = delete;
        arg& operator=(arg&&) = delete;

        arg(const arg&) = delete;
        arg& operator=(const arg&) = delete;
    };
}
