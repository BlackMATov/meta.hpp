/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_kinds.hpp"

namespace meta_hpp::detail
{
    template < array_kind Array >
    struct array_traits;

    template < class_kind Class >
    struct class_traits;

    template < class_kind Class, typename... Args >
    struct ctor_traits;

    template < enum_kind Enum >
    struct enum_traits;

    template < function_kind Function >
    struct function_traits;

    template < member_kind Member >
    struct member_traits;

    template < method_kind Method >
    struct method_traits;

    template < number_kind Number >
    struct number_traits;

    template < pointer_kind Pointer >
    struct pointer_traits;

    template < reference_kind Reference >
    struct reference_traits;

    template < void_kind Void >
    struct void_traits;
}

namespace meta_hpp
{
    enum class array_flags : std::uint32_t {
        is_bounded = 1 << 0,
        is_unbounded = 1 << 1,
    };

    enum class class_flags : std::uint32_t {
        is_empty = 1 << 1,
        is_final = 1 << 2,
        is_abstract = 1 << 3,
        is_polymorphic = 1 << 4,
        is_template_instantiation = 1 << 5,
    };

    enum class ctor_flags : std::uint32_t {
        is_noexcept = 1 << 0,
    };

    enum class enum_flags : std::uint32_t {
        is_scoped = 1 << 0,
    };

    enum class function_flags : std::uint32_t {
        is_noexcept = 1 << 0,
    };

    enum class member_flags : std::uint32_t {
        is_readonly = 1 << 0,
    };

    enum class method_flags : std::uint32_t {
        is_const = 1 << 0,
        is_noexcept = 1 << 1,
        is_lvalue_qualified = 1 << 3,
        is_rvalue_qualified = 1 << 4,
    };

    enum class number_flags : std::uint32_t {
        is_signed = 1 << 1,
        is_unsigned = 1 << 2,
        is_integral = 1 << 3,
        is_floating_point = 1 << 4,
    };

    enum class pointer_flags : std::uint32_t {
        is_readonly = 1 << 0,
    };

    enum class reference_flags : std::uint32_t {
        is_readonly = 1 << 0,
        is_lvalue = 1 << 1,
        is_rvalue = 1 << 2,
    };

    enum class void_flags : std::uint32_t {
    };

    ENUM_HPP_OPERATORS_DECL(array_flags)
    ENUM_HPP_OPERATORS_DECL(class_flags)
    ENUM_HPP_OPERATORS_DECL(ctor_flags)
    ENUM_HPP_OPERATORS_DECL(enum_flags)
    ENUM_HPP_OPERATORS_DECL(function_flags)
    ENUM_HPP_OPERATORS_DECL(member_flags)
    ENUM_HPP_OPERATORS_DECL(method_flags)
    ENUM_HPP_OPERATORS_DECL(number_flags)
    ENUM_HPP_OPERATORS_DECL(pointer_flags)
    ENUM_HPP_OPERATORS_DECL(reference_flags)
    ENUM_HPP_OPERATORS_DECL(void_flags)
}
