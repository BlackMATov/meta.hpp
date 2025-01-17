/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2025, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_registry.hpp"
#include "../meta_types.hpp"

#include "../meta_detail/type_sharing.hpp"

namespace meta_hpp::detail
{
    template < nullptr_kind Nullptr >
    nullptr_type_data::nullptr_type_data(nullptr_traits<Nullptr>)
    : type_data_base{type_kind::nullptr_, shared_traits_hash<nullptr_traits<Nullptr>>{}(this)} {}

    inline void nullptr_type_data::purge_binds() {
        // nothing
    }

    inline void nullptr_type_data::purge_metadata() {
        metadata.clear();
    }
}
