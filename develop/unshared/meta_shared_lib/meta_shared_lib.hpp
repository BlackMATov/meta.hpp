/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include <meta.hpp.shared.lib_export.h>

#include <meta.hpp/meta_all.hpp>

namespace meta_shared_lib
{
    namespace meta = meta_hpp;

    META_HPP_SHARED_LIB_EXPORT meta::scope get_library_scope();

    META_HPP_SHARED_LIB_EXPORT meta::uvalue wrap_int(int value);
    META_HPP_SHARED_LIB_EXPORT int unwrap_int(const meta::uvalue& value);
}
