/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"

namespace meta_hpp
{
    enum class type_kind : std::uint32_t {
        array_,
        class_,
        ctor_,
        dtor_,
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
    concept function_kind = std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>;

    template < typename T >
    concept member_kind = std::is_member_object_pointer_v<T>;

    template < typename T >
    concept method_kind = std::is_member_function_pointer_v<T>;

    template < typename T >
    concept nullptr_kind = std::is_null_pointer_v<T>;

    template < typename T >
    concept number_kind = std::is_arithmetic_v<T>;

    template < typename T >
    concept pointer_kind = std::is_pointer_v<T> && !std::is_function_v<std::remove_pointer_t<T>>;

    template < typename T >
    concept reference_kind = std::is_reference_v<T>;

    template < typename T >
    concept void_kind = std::is_void_v<T>;

    template < typename T >
    constexpr type_kind make_type_kind() noexcept {
        if constexpr ( array_kind<T> ) { return type_kind::array_; }
        if constexpr ( class_kind<T> ) { return type_kind::class_; }
        if constexpr ( enum_kind<T> ) { return type_kind::enum_; }
        if constexpr ( function_kind<T> ) { return type_kind::function_; }
        if constexpr ( member_kind<T> ) { return type_kind::member_; }
        if constexpr ( method_kind<T> ) { return type_kind::method_; }
        if constexpr ( nullptr_kind<T> ) { return type_kind::nullptr_; }
        if constexpr ( number_kind<T> ) { return type_kind::number_; }
        if constexpr ( pointer_kind<T> ) { return type_kind::pointer_; }
        if constexpr ( reference_kind<T> ) { return type_kind::reference_; }
        if constexpr ( void_kind<T> ) { return type_kind::void_; }
    }
}

namespace meta_hpp::detail
{
    template < type_kind Kind >
    struct type_kind_traits;

    template <>
    struct type_kind_traits<type_kind::array_> {
        using kind_type = array_type;
        using kind_type_data = array_type_data;
    };

    template <>
    struct type_kind_traits<type_kind::class_> {
        using kind_type = class_type;
        using kind_type_data = class_type_data;
    };

    template <>
    struct type_kind_traits<type_kind::ctor_> {
        using kind_type = ctor_type;
        using kind_type_data = ctor_type_data;
    };

    template <>
    struct type_kind_traits<type_kind::dtor_> {
        using kind_type = dtor_type;
        using kind_type_data = dtor_type_data;
    };

    template <>
    struct type_kind_traits<type_kind::enum_> {
        using kind_type = enum_type;
        using kind_type_data = enum_type_data;
    };

    template <>
    struct type_kind_traits<type_kind::function_> {
        using kind_type = function_type;
        using kind_type_data = function_type_data;
    };

    template <>
    struct type_kind_traits<type_kind::member_> {
        using kind_type = member_type;
        using kind_type_data = member_type_data;
    };

    template <>
    struct type_kind_traits<type_kind::method_> {
        using kind_type = method_type;
        using kind_type_data = method_type_data;
    };

    template <>
    struct type_kind_traits<type_kind::nullptr_> {
        using kind_type = nullptr_type;
        using kind_type_data = nullptr_type_data;
    };

    template <>
    struct type_kind_traits<type_kind::number_> {
        using kind_type = number_type;
        using kind_type_data = number_type_data;
    };

    template <>
    struct type_kind_traits<type_kind::pointer_> {
        using kind_type = pointer_type;
        using kind_type_data = pointer_type_data;
    };

    template <>
    struct type_kind_traits<type_kind::reference_> {
        using kind_type = reference_type;
        using kind_type_data = reference_type_data;
    };

    template <>
    struct type_kind_traits<type_kind::void_> {
        using kind_type = void_type;
        using kind_type_data = void_type_data;
    };

    template < typename T >
    using kind_type = typename type_kind_traits<make_type_kind<T>()>::kind_type;

    template < typename T >
    using kind_type_data = typename type_kind_traits<make_type_kind<T>()>::kind_type_data;

    template < typename T >
    using kind_type_data_ptr = std::shared_ptr<kind_type_data<T>>;
}
