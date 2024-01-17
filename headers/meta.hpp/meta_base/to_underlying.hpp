/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"

namespace meta_hpp::detail
{
    template < typename Enum >
    [[nodiscard]] constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept {
        return static_cast<std::underlying_type_t<Enum>>(e);
    }
}
