/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"

#include "type_kinds.hpp"

#include "type_traits/array_traits.hpp"
#include "type_traits/class_traits.hpp"
#include "type_traits/constructor_traits.hpp"
#include "type_traits/destructor_traits.hpp"
#include "type_traits/enum_traits.hpp"
#include "type_traits/function_traits.hpp"
#include "type_traits/member_traits.hpp"
#include "type_traits/method_traits.hpp"
#include "type_traits/nullptr_traits.hpp"
#include "type_traits/number_traits.hpp"
#include "type_traits/pointer_traits.hpp"
#include "type_traits/reference_traits.hpp"
#include "type_traits/void_traits.hpp"

namespace meta_hpp::detail
{
    struct type_data_base;
    struct array_type_data;
    struct class_type_data;
    struct constructor_type_data;
    struct destructor_type_data;
    struct enum_type_data;
    struct function_type_data;
    struct member_type_data;
    struct method_type_data;
    struct nullptr_type_data;
    struct number_type_data;
    struct pointer_type_data;
    struct reference_type_data;
    struct void_type_data;
}

namespace meta_hpp::detail
{
    template < type_family Type >
    struct type_traits;

    template < type_family Type >
    [[nodiscard]] typename type_traits<Type>::data_ptr type_access(const Type& type) {
        return type.data_;
    }
}

namespace meta_hpp::detail
{
    template <>
    struct type_traits<any_type> {
        using data_ptr = type_data_base*;
    };

    template <>
    struct type_traits<array_type> {
        using type = array_type;
        using data_ptr = array_type_data*;
        using data_type = array_type_data;
        inline static constexpr type_kind kind{type_kind::array_};
    };

    template <>
    struct type_traits<class_type> {
        using type = class_type;
        using data_ptr = class_type_data*;
        using data_type = class_type_data;
        inline static constexpr type_kind kind{type_kind::class_};
    };

    template <>
    struct type_traits<constructor_type> {
        using type = constructor_type;
        using data_ptr = constructor_type_data*;
        using data_type = constructor_type_data;
        inline static constexpr type_kind kind{type_kind::constructor_};
    };

    template <>
    struct type_traits<destructor_type> {
        using type = destructor_type;
        using data_ptr = destructor_type_data*;
        using data_type = destructor_type_data;
        inline static constexpr type_kind kind{type_kind::destructor_};
    };

    template <>
    struct type_traits<enum_type> {
        using type = enum_type;
        using data_ptr = enum_type_data*;
        using data_type = enum_type_data;
        inline static constexpr type_kind kind{type_kind::enum_};
    };

    template <>
    struct type_traits<function_type> {
        using type = function_type;
        using data_ptr = function_type_data*;
        using data_type = function_type_data;
        inline static constexpr type_kind kind{type_kind::function_};
    };

    template <>
    struct type_traits<member_type> {
        using type = member_type;
        using data_ptr = member_type_data*;
        using data_type = member_type_data;
        inline static constexpr type_kind kind{type_kind::member_};
    };

    template <>
    struct type_traits<method_type> {
        using type = method_type;
        using data_ptr = method_type_data*;
        using data_type = method_type_data;
        inline static constexpr type_kind kind{type_kind::method_};
    };

    template <>
    struct type_traits<nullptr_type> {
        using type = nullptr_type;
        using data_ptr = nullptr_type_data*;
        using data_type = nullptr_type_data;
        inline static constexpr type_kind kind{type_kind::nullptr_};
    };

    template <>
    struct type_traits<number_type> {
        using type = number_type;
        using data_ptr = number_type_data*;
        using data_type = number_type_data;
        inline static constexpr type_kind kind{type_kind::number_};
    };

    template <>
    struct type_traits<pointer_type> {
        using type = pointer_type;
        using data_ptr = pointer_type_data*;
        using data_type = pointer_type_data;
        inline static constexpr type_kind kind{type_kind::pointer_};
    };

    template <>
    struct type_traits<reference_type> {
        using type = reference_type;
        using data_ptr = reference_type_data*;
        using data_type = reference_type_data;
        inline static constexpr type_kind kind{type_kind::reference_};
    };

    template <>
    struct type_traits<void_type> {
        using type = void_type;
        using data_ptr = void_type_data*;
        using data_type = void_type_data;
        inline static constexpr type_kind kind{type_kind::void_};
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    struct type_to_traits;

    template < typename T >
    using type_to_traits_t = typename type_to_traits<T>::type;

    template < array_kind T >
    struct type_to_traits<T> {
        using type = array_traits<T>;
    };

    template < class_kind T >
    struct type_to_traits<T> {
        using type = class_traits<T>;
    };

    template < enum_kind T >
    struct type_to_traits<T> {
        using type = enum_traits<T>;
    };

    template < function_kind T >
    struct type_to_traits<T> {
        using type = function_traits<T>;
    };

    template < member_pointer_kind T >
    struct type_to_traits<T> {
        using type = member_traits<T>;
    };

    template < method_pointer_kind T >
    struct type_to_traits<T> {
        using type = method_traits<T>;
    };

    template < nullptr_kind T >
    struct type_to_traits<T> {
        using type = nullptr_traits<T>;
    };

    template < number_kind T >
    struct type_to_traits<T> {
        using type = number_traits<T>;
    };

    template < pointer_kind T >
    struct type_to_traits<T> {
        using type = pointer_traits<T>;
    };

    template < reference_kind T >
    struct type_to_traits<T> {
        using type = reference_traits<T>;
    };

    template < void_kind T >
    struct type_to_traits<T> {
        using type = void_traits<T>;
    };
}

namespace meta_hpp::detail
{
    template < typename Traits >
    struct traits_to_type_traits;

    template < typename Traits >
    using traits_to_type_traits_t = typename traits_to_type_traits<Traits>::type;

    template < array_kind Array >
    struct traits_to_type_traits<array_traits<Array>> {
        using type = type_traits<array_type>;
    };

    template < class_kind Class >
    struct traits_to_type_traits<class_traits<Class>> {
        using type = type_traits<class_type>;
    };

    template < class_kind Class, typename... Args >
    struct traits_to_type_traits<constructor_traits<Class, Args...>> {
        using type = type_traits<constructor_type>;
    };

    template < class_kind Class >
    struct traits_to_type_traits<destructor_traits<Class>> {
        using type = type_traits<destructor_type>;
    };

    template < enum_kind Enum >
    struct traits_to_type_traits<enum_traits<Enum>> {
        using type = type_traits<enum_type>;
    };

    template < function_kind Function >
    struct traits_to_type_traits<function_traits<Function>> {
        using type = type_traits<function_type>;
    };

    template < member_pointer_kind Member >
    struct traits_to_type_traits<member_traits<Member>> {
        using type = type_traits<member_type>;
    };

    template < method_pointer_kind Method >
    struct traits_to_type_traits<method_traits<Method>> {
        using type = type_traits<method_type>;
    };

    template < nullptr_kind Nullptr >
    struct traits_to_type_traits<nullptr_traits<Nullptr>> {
        using type = type_traits<nullptr_type>;
    };

    template < number_kind Number >
    struct traits_to_type_traits<number_traits<Number>> {
        using type = type_traits<number_type>;
    };

    template < pointer_kind Pointer >
    struct traits_to_type_traits<pointer_traits<Pointer>> {
        using type = type_traits<pointer_type>;
    };

    template < reference_kind Reference >
    struct traits_to_type_traits<reference_traits<Reference>> {
        using type = type_traits<reference_type>;
    };

    template < void_kind Void >
    struct traits_to_type_traits<void_traits<Void>> {
        using type = type_traits<void_type>;
    };
}
