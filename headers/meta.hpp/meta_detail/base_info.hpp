/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_types.hpp"

#include "type_registry.hpp"

namespace meta_hpp::detail
{
    struct poly_info final {
        const void* ptr{};
        class_type type{};
    };

    template < typename T >
    concept check_base_info_enabled //
        = requires { typename T::meta_base_info; };

    template < typename T >
    concept check_poly_info_enabled //
        = requires(type_registry& r, const T& v) {
              { v.get_most_derived_poly_info(r) } -> std::convertible_to<poly_info>;
          };
}

#define META_HPP_ENABLE_BASE_INFO(...) \
public: \
    using meta_base_info = ::meta_hpp::detail::type_list<__VA_ARGS__>; \
\
private:

#define META_HPP_ENABLE_POLY_INFO(...) \
    META_HPP_ENABLE_BASE_INFO(__VA_ARGS__) \
public: \
    META_HPP_IGNORE_OVERRIDE_WARNINGS_PUSH() \
    virtual ::meta_hpp::detail::poly_info get_most_derived_poly_info(::meta_hpp::detail::type_registry& registry) const { \
        using self_type = std::remove_cvref_t<decltype(*this)>; \
        return ::meta_hpp::detail::poly_info{.ptr = this, .type = registry.resolve_class_type<self_type>()}; \
    } \
    META_HPP_IGNORE_OVERRIDE_WARNINGS_POP() \
private:
