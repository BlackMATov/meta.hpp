/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>

#include <algorithm>
#include <array>
#include <atomic>
#include <functional>
#include <initializer_list>
#include <iosfwd>
#include <map>
#include <memory>
#include <set>
#include <span>
#include <stdexcept>
#include <string_view>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include <enum.hpp/enum.hpp>
#include <enum.hpp/enum_bitflags.hpp>

namespace meta_hpp
{
    template < typename Enum >
    using bitflags = enum_hpp::bitflags::bitflags<Enum>;
}

namespace meta_hpp
{
    template < typename Signature >
    constexpr auto select(Signature* func) noexcept -> Signature* {
        return func;
    }

    template < typename Signature, typename Class >
    constexpr auto select(Signature Class::*func) noexcept -> Signature Class::* {
        return func;
    }

    namespace detail
    {
        template < typename... Types >
        struct type_list {};

        template < std::size_t Index, typename TypeList >
        struct type_list_at;

        template < std::size_t Index, typename... Types >
        struct type_list_at<Index, type_list<Types...>> {
            using type = std::tuple_element_t<Index, std::tuple<Types...>>;
        };

        template < std::size_t Index, typename TypeList >
        using type_list_at_t = typename type_list_at<Index, TypeList>::type;
    }
}

namespace meta_hpp
{
    class value;

    namespace detail
    {
        class arg_base;
        class arg;

        class inst_base;
        class inst;
    }
}

namespace meta_hpp
{
    class ctor;
    class dtor;
    class evalue;
    class function;
    class member;
    class method;
    class scope;
    class variable;

    namespace detail
    {
        struct ctor_state;
        struct dtor_state;
        struct evalue_state;
        struct function_state;
        struct member_state;
        struct method_state;
        struct scope_state;
        struct variable_state;

        using ctor_state_ptr = std::shared_ptr<ctor_state>;
        using dtor_state_ptr = std::shared_ptr<dtor_state>;
        using evalue_state_ptr = std::shared_ptr<evalue_state>;
        using function_state_ptr = std::shared_ptr<function_state>;
        using member_state_ptr = std::shared_ptr<member_state>;
        using method_state_ptr = std::shared_ptr<method_state>;
        using scope_state_ptr = std::shared_ptr<scope_state>;
        using variable_state_ptr = std::shared_ptr<variable_state>;
    }
}

namespace meta_hpp
{
    class any_type;
    class array_type;
    class class_type;
    class ctor_type;
    class dtor_type;
    class enum_type;
    class function_type;
    class member_type;
    class method_type;
    class nullptr_type;
    class number_type;
    class pointer_type;
    class reference_type;
    class void_type;

    namespace detail
    {
        struct type_data_base;
        struct array_type_data;
        struct class_type_data;
        struct ctor_type_data;
        struct dtor_type_data;
        struct enum_type_data;
        struct function_type_data;
        struct member_type_data;
        struct method_type_data;
        struct nullptr_type_data;
        struct number_type_data;
        struct pointer_type_data;
        struct reference_type_data;
        struct void_type_data;

        using type_data_base_ptr = std::shared_ptr<type_data_base>;
        using array_type_data_ptr = std::shared_ptr<array_type_data>;
        using class_type_data_ptr = std::shared_ptr<class_type_data>;
        using ctor_type_data_ptr = std::shared_ptr<ctor_type_data>;
        using dtor_type_data_ptr = std::shared_ptr<dtor_type_data>;
        using enum_type_data_ptr = std::shared_ptr<enum_type_data>;
        using function_type_data_ptr = std::shared_ptr<function_type_data>;
        using member_type_data_ptr = std::shared_ptr<member_type_data>;
        using method_type_data_ptr = std::shared_ptr<method_type_data>;
        using nullptr_type_data_ptr = std::shared_ptr<nullptr_type_data>;
        using number_type_data_ptr = std::shared_ptr<number_type_data>;
        using pointer_type_data_ptr = std::shared_ptr<pointer_type_data>;
        using reference_type_data_ptr = std::shared_ptr<reference_type_data>;
        using void_type_data_ptr = std::shared_ptr<void_type_data>;
    }
}

namespace meta_hpp
{
    struct ctor_index;
    struct dtor_index;
    struct evalue_index;
    struct function_index;
    struct member_index;
    struct method_index;
    struct scope_index;
    struct variable_index;

    using class_set = std::set<class_type, std::less<>>;
    using class_map = std::map<std::string, class_type, std::less<>>;

    using enum_set = std::set<enum_type, std::less<>>;
    using enum_map = std::map<std::string, enum_type, std::less<>>;

    using ctor_map = std::map<ctor_index, ctor, std::less<>>;
    using dtor_map = std::map<dtor_index, dtor, std::less<>>;
    using evalue_map = std::map<evalue_index, evalue, std::less<>>;
    using function_map = std::map<function_index, function, std::less<>>;
    using member_map = std::map<member_index, member, std::less<>>;
    using method_map = std::map<method_index, method, std::less<>>;
    using scope_map = std::map<scope_index, scope, std::less<>>;
    using variable_map = std::map<variable_index, variable, std::less<>>;
}

namespace meta_hpp::detail::stdex
{
    template < typename T >
    [[nodiscard]] constexpr std::underlying_type_t<T> to_underlying(T v) noexcept {
        return static_cast<std::underlying_type_t<T>>(v);
    }
}

namespace meta_hpp::detail::stdex
{
    template < typename T, typename U >
    concept same_as =
        std::is_same_v<T, U> &&
        std::is_same_v<U, T>;

    template < typename Derived, typename Base >
    concept derived_from =
        std::is_base_of_v<Base, Derived> &&
        std::is_convertible_v<const volatile Derived*, const volatile Base*>;

    template < typename From, typename To >
    concept convertible_to =
        std::is_convertible_v<From, To> &&
        requires { static_cast<To>(std::declval<From>()); };

    template < typename T >
    concept destructible =
        std::is_nothrow_destructible_v<T>;

    template < typename T, typename... Args >
    concept constructible_from =
        destructible<T> &&
        std::is_constructible_v<T, Args...>;

    template < typename T >
    concept move_constructible =
        constructible_from<T, T> &&
        convertible_to<T, T>;

    template<typename T>
    concept copy_constructible =
        move_constructible<T> &&
        constructible_from<T, T&> && convertible_to<T&, T> &&
        constructible_from<T, const T&> && convertible_to<const T&, T> &&
        constructible_from<T, const T> && convertible_to<const T, T>;
}
