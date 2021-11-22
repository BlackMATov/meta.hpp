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
#include <any>
#include <array>
#include <atomic>
#include <concepts>
#include <functional>
#include <initializer_list>
#include <iosfwd>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <string_view>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
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

namespace meta_hpp
{
    template < typename Signature >
    constexpr auto select(Signature* func) noexcept -> Signature* {
        return func;
    }

    template < typename Signature, typename Class >
    constexpr auto select(Signature Class::*func) -> decltype(func) {
        return func;
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
    class evalue;
    class function;
    class member;
    class method;
    class scope;
    class variable;

    namespace detail
    {
        struct ctor_state;
        struct evalue_state;
        struct function_state;
        struct member_state;
        struct method_state;
        struct scope_state;
        struct variable_state;

        using ctor_state_ptr = std::shared_ptr<ctor_state>;
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
    class enum_type;
    class function_type;
    class member_type;
    class method_type;
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
        struct enum_type_data;
        struct function_type_data;
        struct member_type_data;
        struct method_type_data;
        struct number_type_data;
        struct pointer_type_data;
        struct reference_type_data;
        struct void_type_data;

        using type_data_base_ptr = std::shared_ptr<type_data_base>;
        using array_type_data_ptr = std::shared_ptr<array_type_data>;
        using class_type_data_ptr = std::shared_ptr<class_type_data>;
        using ctor_type_data_ptr = std::shared_ptr<ctor_type_data>;
        using enum_type_data_ptr = std::shared_ptr<enum_type_data>;
        using function_type_data_ptr = std::shared_ptr<function_type_data>;
        using member_type_data_ptr = std::shared_ptr<member_type_data>;
        using method_type_data_ptr = std::shared_ptr<method_type_data>;
        using number_type_data_ptr = std::shared_ptr<number_type_data>;
        using pointer_type_data_ptr = std::shared_ptr<pointer_type_data>;
        using reference_type_data_ptr = std::shared_ptr<reference_type_data>;
        using void_type_data_ptr = std::shared_ptr<void_type_data>;
    }
}

namespace meta_hpp
{
    struct ctor_index;
    struct evalue_index;
    struct function_index;
    struct member_index;
    struct method_index;
    struct variable_index;

    using class_set = std::set<class_type, std::less<>>;
    using class_map = std::map<std::string, class_type, std::less<>>;

    using enum_set = std::set<enum_type, std::less<>>;
    using enum_map = std::map<std::string, enum_type, std::less<>>;

    using ctor_map = std::map<ctor_index, ctor, std::less<>>;
    using evalue_map = std::map<evalue_index, evalue, std::less<>>;
    using function_map = std::map<function_index, function, std::less<>>;
    using member_map = std::map<member_index, member, std::less<>>;
    using method_map = std::map<method_index, method, std::less<>>;
    using variable_map = std::map<variable_index, variable, std::less<>>;
}
