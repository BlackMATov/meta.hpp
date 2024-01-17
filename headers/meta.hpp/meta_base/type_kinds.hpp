/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"

namespace meta_hpp::detail
{
    enum class type_kind : std::uint32_t {
        array_,
        class_,
        constructor_,
        destructor_,
        enum_,
        function_,
        member_,
        method_,
        nullptr_,
        number_,
        pointer_,
        reference_,
        void_,
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    concept array_kind = std::is_array_v<T>;

    template < typename T >
    concept class_kind = std::is_class_v<T>;

    template < typename T >
    concept enum_kind = std::is_enum_v<T>;

    template < typename T >
    concept function_kind = std::is_function_v<T>;

    template < typename T >
    concept member_pointer_kind = std::is_member_object_pointer_v<T>;

    template < typename T >
    concept method_pointer_kind = std::is_member_function_pointer_v<T>;

    template < typename T >
    concept nullptr_kind = std::is_null_pointer_v<T>;

    template < typename T >
    concept number_kind = std::is_arithmetic_v<T>;

    template < typename T >
    concept pointer_kind = std::is_pointer_v<T>;

    template < typename T >
    concept reference_kind = std::is_reference_v<T>;

    template < typename T >
    concept void_kind = std::is_void_v<T>;
}

namespace meta_hpp::detail
{
    template < typename T >
    concept non_pointer_kind = !std::is_pointer_v<T>;

    template < typename T >
    concept function_pointer_kind = std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>;

    template < typename T >
    concept non_function_pointer_kind = std::is_pointer_v<T> && !std::is_function_v<std::remove_pointer_t<T>>;
}
