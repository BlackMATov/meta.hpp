/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"

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
    concept function_pointer_kind = std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>;

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

    template < typename T >
    concept any_pointer_family = std::is_pointer_v<T> || std::is_member_pointer_v<T>;

    template < typename T >
    concept not_any_pointer_family = !std::is_pointer_v<T> && !std::is_member_pointer_v<T>;
}

namespace meta_hpp::detail
{
    template < typename T >
    struct type_to_kind;

    template < typename T >
    inline constexpr type_kind type_to_kind_v = type_to_kind<T>::value;
}

namespace meta_hpp::detail
{
    template < array_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::array_> {};

    template < class_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::class_> {};

    template < enum_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::enum_> {};

    template < function_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::function_> {};

    template < member_pointer_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::member_> {};

    template < method_pointer_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::method_> {};

    template < nullptr_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::nullptr_> {};

    template < number_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::number_> {};

    template < pointer_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::pointer_> {};

    template < reference_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::reference_> {};

    template < void_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::void_> {};
}
