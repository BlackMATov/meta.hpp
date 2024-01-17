/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"

namespace meta_hpp::detail
{
    class nonesuch {
    public:
        nonesuch() = delete;
        ~nonesuch() = delete;
        nonesuch(nonesuch&&) = delete;
        nonesuch(const nonesuch&) = delete;
        nonesuch& operator=(nonesuch&&) = delete;
        nonesuch& operator=(const nonesuch&) = delete;
    };
}
