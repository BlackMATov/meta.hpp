/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    inline constexpr bool is_type_family_v =
        stdex::same_as<T, any_type> ||
        stdex::same_as<T, array_type> ||
        stdex::same_as<T, class_type> ||
        stdex::same_as<T, constructor_type> ||
        stdex::same_as<T, destructor_type> ||
        stdex::same_as<T, enum_type> ||
        stdex::same_as<T, function_type> ||
        stdex::same_as<T, member_type> ||
        stdex::same_as<T, method_type> ||
        stdex::same_as<T, nullptr_type> ||
        stdex::same_as<T, number_type> ||
        stdex::same_as<T, pointer_type> ||
        stdex::same_as<T, reference_type> ||
        stdex::same_as<T, void_type>;

    template < typename T >
    concept type_family = is_type_family_v<T>;

    template < type_family T >
    [[nodiscard]] auto type_access(const T& type) {
        return type.data_;
    }
}
