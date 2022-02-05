/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    inline constexpr bool is_type_family_v =
        std::is_same_v<T, any_type> ||
        std::is_same_v<T, array_type> ||
        std::is_same_v<T, class_type> ||
        std::is_same_v<T, ctor_type> ||
        std::is_same_v<T, dtor_type> ||
        std::is_same_v<T, enum_type> ||
        std::is_same_v<T, function_type> ||
        std::is_same_v<T, member_type> ||
        std::is_same_v<T, method_type> ||
        std::is_same_v<T, nullptr_type> ||
        std::is_same_v<T, number_type> ||
        std::is_same_v<T, pointer_type> ||
        std::is_same_v<T, reference_type> ||
        std::is_same_v<T, void_type>;

    template < typename T >
    concept type_family = is_type_family_v<T>;

    template < type_family T >
    [[nodiscard]] auto type_access(const T& type) {
        return type.data_;
    }
}
