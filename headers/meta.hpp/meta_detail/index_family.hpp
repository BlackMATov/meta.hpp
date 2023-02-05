/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    concept index_family = //
        std::is_same_v<T, argument_index> ||
        std::is_same_v<T, constructor_index> ||
        std::is_same_v<T, destructor_index> ||
        std::is_same_v<T, evalue_index> ||
        std::is_same_v<T, function_index> ||
        std::is_same_v<T, member_index> ||
        std::is_same_v<T, method_index> ||
        std::is_same_v<T, scope_index> ||
        std::is_same_v<T, variable_index>;
}
