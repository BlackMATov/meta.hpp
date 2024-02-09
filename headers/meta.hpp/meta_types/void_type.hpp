/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_registry.hpp"
#include "../meta_types.hpp"

#include "../meta_detail/type_sharing.hpp"

namespace meta_hpp::detail
{
    template < void_kind Void >
    void_type_data::void_type_data(void_traits<Void>)
    : type_data_base{type_kind::void_, shared_traits_hash<void_traits<Void>>{}(this)} {}
}
