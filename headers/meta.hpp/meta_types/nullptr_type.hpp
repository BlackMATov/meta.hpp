/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_registry.hpp"
#include "../meta_types.hpp"

namespace meta_hpp::detail
{
    template < nullptr_kind Nullptr >
    nullptr_type_data::nullptr_type_data(type_list<Nullptr>)
    : type_data_base{type_kind::nullptr_} {}
}
